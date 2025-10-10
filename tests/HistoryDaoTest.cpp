#include <QtTest/QtTest>
#include "database/HistoryDao.h"
#include "database/DatabaseManager.h"
#include "utils/Logger.h"
#include <QTemporaryDir>
#include <QSqlDatabase>
#include <QJsonObject>

class HistoryDaoTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Test cases
    void testInsertEvent();
    void testInsertInvalidEvent();
    void testBatchInsert();
    void testFindByDevice();
    void testFindByType();
    void testFindByDateRange();
    void testFindByDeviceAndDateRange();
    void testDeleteOlderThan();
    void testDeleteByDevice();
    void testGetEventCount();

private:
    DatabaseManager* dbManager;
    HistoryDao* historyDao;
    QTemporaryDir* tempDir;
    QString dbPath;

    HistoryEvent createTestEvent(const QString& deviceId = "test-device-1",
                                  const QString& eventType = "scan",
                                  const QString& description = "Test scan event");
};

void HistoryDaoTest::initTestCase() {
    Logger::setLogLevel(Logger::ERROR);
    tempDir = new QTemporaryDir();
    QVERIFY(tempDir->isValid());
}

void HistoryDaoTest::cleanupTestCase() {
    delete tempDir;
}

void HistoryDaoTest::init() {
    // Create a new database for each test
    dbPath = tempDir->path() + "/test_history.db";
    QFile::remove(dbPath);

    dbManager = DatabaseManager::instance();
    QVERIFY(dbManager->open(dbPath));

    historyDao = new HistoryDao(dbManager);
}

void HistoryDaoTest::cleanup() {
    delete historyDao;
    dbManager->close();
    QFile::remove(dbPath);
}

HistoryEvent HistoryDaoTest::createTestEvent(const QString& deviceId,
                                             const QString& eventType,
                                             const QString& description) {
    HistoryEvent event;
    event.deviceId = deviceId;
    event.eventType = eventType;
    event.description = description;
    event.timestamp = QDateTime::currentDateTime();

    QJsonObject metadata;
    metadata["user"] = "test_user";
    metadata["action"] = "manual_scan";
    event.metadata = metadata;

    return event;
}

void HistoryDaoTest::testInsertEvent() {
    HistoryEvent event = createTestEvent();

    bool result = historyDao->insert(event);
    QVERIFY(result);

    // Verify event was inserted
    QList<HistoryEvent> events = historyDao->findByDevice(event.deviceId);
    QCOMPARE(events.size(), 1);
    QCOMPARE(events[0].deviceId, event.deviceId);
    QCOMPARE(events[0].eventType, event.eventType);
    QCOMPARE(events[0].description, event.description);
}

void HistoryDaoTest::testInsertInvalidEvent() {
    HistoryEvent invalidEvent;
    // Event is invalid because deviceId is empty

    bool result = historyDao->insert(invalidEvent);
    QVERIFY(!result);
}

void HistoryDaoTest::testBatchInsert() {
    QList<HistoryEvent> events;
    for (int i = 0; i < 5; i++) {
        events.append(createTestEvent("device-" + QString::number(i), "scan", "Event " + QString::number(i)));
    }

    int count = historyDao->insertBatch(events);
    QCOMPARE(count, 5);

    // Verify all events were inserted
    int totalCount = historyDao->getEventCount();
    QCOMPARE(totalCount, 5);
}

void HistoryDaoTest::testFindByDevice() {
    // Insert events for different devices
    historyDao->insert(createTestEvent("device-1", "scan"));
    historyDao->insert(createTestEvent("device-1", "alert"));
    historyDao->insert(createTestEvent("device-2", "scan"));

    QList<HistoryEvent> device1Events = historyDao->findByDevice("device-1");
    QCOMPARE(device1Events.size(), 2);

    QList<HistoryEvent> device2Events = historyDao->findByDevice("device-2");
    QCOMPARE(device2Events.size(), 1);
}

void HistoryDaoTest::testFindByType() {
    // Insert events of different types
    historyDao->insert(createTestEvent("device-1", "scan"));
    historyDao->insert(createTestEvent("device-2", "scan"));
    historyDao->insert(createTestEvent("device-3", "alert"));

    QList<HistoryEvent> scanEvents = historyDao->findByType("scan");
    QCOMPARE(scanEvents.size(), 2);

    QList<HistoryEvent> alertEvents = historyDao->findByType("alert");
    QCOMPARE(alertEvents.size(), 1);
}

void HistoryDaoTest::testFindByDateRange() {
    QDateTime now = QDateTime::currentDateTime();
    QDateTime yesterday = now.addDays(-1);
    QDateTime tomorrow = now.addDays(1);

    // Insert events with different timestamps
    HistoryEvent oldEvent = createTestEvent("device-1", "scan", "Old event");
    oldEvent.timestamp = yesterday.addSecs(-3600);
    historyDao->insert(oldEvent);

    HistoryEvent recentEvent = createTestEvent("device-1", "scan", "Recent event");
    recentEvent.timestamp = now;
    historyDao->insert(recentEvent);

    // Find events in range (yesterday to tomorrow)
    QList<HistoryEvent> events = historyDao->findByDateRange(yesterday, tomorrow);
    QCOMPARE(events.size(), 2);
}

void HistoryDaoTest::testFindByDeviceAndDateRange() {
    QDateTime now = QDateTime::currentDateTime();
    QDateTime yesterday = now.addDays(-1);
    QDateTime tomorrow = now.addDays(1);

    // Insert events for different devices and times
    HistoryEvent event1 = createTestEvent("device-1", "scan");
    event1.timestamp = now;
    historyDao->insert(event1);

    HistoryEvent event2 = createTestEvent("device-1", "alert");
    event2.timestamp = yesterday.addSecs(-3600);
    historyDao->insert(event2);

    HistoryEvent event3 = createTestEvent("device-2", "scan");
    event3.timestamp = now;
    historyDao->insert(event3);

    // Find device-1 events in range
    QList<HistoryEvent> events = historyDao->findByDeviceAndDateRange("device-1", yesterday, tomorrow);
    QCOMPARE(events.size(), 2);
}

void HistoryDaoTest::testDeleteOlderThan() {
    QDateTime now = QDateTime::currentDateTime();
    QDateTime yesterday = now.addDays(-1);
    QDateTime lastWeek = now.addDays(-7);

    // Insert old and recent events
    HistoryEvent oldEvent = createTestEvent("device-1", "scan");
    oldEvent.timestamp = lastWeek;
    historyDao->insert(oldEvent);

    HistoryEvent recentEvent = createTestEvent("device-1", "scan");
    recentEvent.timestamp = now;
    historyDao->insert(recentEvent);

    // Delete events older than yesterday
    int deleted = historyDao->deleteOlderThan(yesterday);
    QCOMPARE(deleted, 1);

    // Verify only recent event remains
    int remaining = historyDao->getEventCount();
    QCOMPARE(remaining, 1);
}

void HistoryDaoTest::testDeleteByDevice() {
    // Insert events for different devices
    historyDao->insert(createTestEvent("device-1", "scan"));
    historyDao->insert(createTestEvent("device-1", "alert"));
    historyDao->insert(createTestEvent("device-2", "scan"));

    // Delete device-1 events
    int deleted = historyDao->deleteByDevice("device-1");
    QCOMPARE(deleted, 2);

    // Verify only device-2 event remains
    int remaining = historyDao->getEventCount();
    QCOMPARE(remaining, 1);

    QList<HistoryEvent> device2Events = historyDao->findByDevice("device-2");
    QCOMPARE(device2Events.size(), 1);
}

void HistoryDaoTest::testGetEventCount() {
    QCOMPARE(historyDao->getEventCount(), 0);

    historyDao->insert(createTestEvent("device-1", "scan"));
    QCOMPARE(historyDao->getEventCount(), 1);

    historyDao->insert(createTestEvent("device-2", "alert"));
    QCOMPARE(historyDao->getEventCount(), 2);

    // Verify device-1 has 1 event
    QList<HistoryEvent> device1Events = historyDao->findByDevice("device-1");
    QCOMPARE(device1Events.size(), 1);
}

QTEST_MAIN(HistoryDaoTest)
#include "HistoryDaoTest.moc"
