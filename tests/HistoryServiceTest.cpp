#include <QtTest/QtTest>
#include "services/HistoryService.h"
#include "database/DatabaseManager.h"
#include "models/NetworkMetrics.h"

class HistoryServiceTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // HistoryService tests
    void testServiceConstruction();
    void testInitialize();
    void testSaveMetrics();
    void testSaveEvent();
    void testGetMetricsHistory();
    void testGetAllMetricsForDevice();
    void testGetEventHistory();
    void testGetAllEventsForDevice();
    void testGetLatestMetrics();
    void testPruneOldData();
    void testDeleteDeviceHistory();
    void testGetMetricsCount();
    void testGetEventCount();
    void testHistoryEventToString();

private:
    DatabaseManager* dbManager;
    HistoryService* service;

    NetworkMetrics createTestMetrics(double latency, double jitter, double packetLoss);
};

void HistoryServiceTest::initTestCase()
{
    qDebug() << "Starting HistoryService tests";

    // Initialize database manager with in-memory database
    dbManager = DatabaseManager::instance();
    QVERIFY(dbManager != nullptr);
}

void HistoryServiceTest::cleanupTestCase()
{
    if (dbManager) {
        dbManager->close();
    }
    qDebug() << "HistoryService tests completed";
}

void HistoryServiceTest::init()
{
    // Open in-memory database for each test
    QVERIFY(dbManager->open(":memory:"));
    QVERIFY(dbManager->createSchema());

    service = new HistoryService(dbManager, this);
    QVERIFY(service->initialize());
}

void HistoryServiceTest::cleanup()
{
    delete service;
    service = nullptr;

    if (dbManager) {
        dbManager->close();
    }
}

NetworkMetrics HistoryServiceTest::createTestMetrics(double latency, double jitter, double packetLoss)
{
    NetworkMetrics metrics;
    metrics.setLatencyAvg(latency);
    metrics.setLatencyMin(latency - 5.0);
    metrics.setLatencyMax(latency + 5.0);
    metrics.setLatencyMedian(latency);
    metrics.setJitter(jitter);
    metrics.setPacketLoss(packetLoss);
    metrics.setQualityScore(NetworkMetrics::QualityScore::Excellent);
    return metrics;
}

void HistoryServiceTest::testServiceConstruction()
{
    QVERIFY(service != nullptr);
}

void HistoryServiceTest::testInitialize()
{
    HistoryService* newService = new HistoryService(dbManager, this);
    bool result = newService->initialize();
    QVERIFY(result);
    delete newService;
}

void HistoryServiceTest::testSaveMetrics()
{
    QSignalSpy spy(service, &HistoryService::metricsStored);

    NetworkMetrics metrics = createTestMetrics(25.5, 5.2, 0.0);

    bool result = service->saveMetrics("192.168.1.1", metrics);
    QVERIFY(result);
    QCOMPARE(spy.count(), 1);

    int count = service->getMetricsCount("192.168.1.1");
    QCOMPARE(count, 1);
}

void HistoryServiceTest::testSaveEvent()
{
    QSignalSpy spy(service, &HistoryService::eventStored);

    bool result = service->saveEvent("192.168.1.1", "device_online", "Device came online");
    QVERIFY(result);
    QCOMPARE(spy.count(), 1);

    int count = service->getEventCount("192.168.1.1");
    QCOMPARE(count, 1);
}

void HistoryServiceTest::testGetMetricsHistory()
{
    // Save multiple metrics
    service->saveMetrics("192.168.1.1", createTestMetrics(25.0, 5.0, 0.0));
    QTest::qWait(100);
    service->saveMetrics("192.168.1.1", createTestMetrics(30.0, 6.0, 1.0));
    QTest::qWait(100);
    service->saveMetrics("192.168.1.1", createTestMetrics(28.0, 5.5, 0.5));

    QDateTime start = QDateTime::currentDateTime().addSecs(-10);
    QDateTime end = QDateTime::currentDateTime().addSecs(10);

    QList<NetworkMetrics> history = service->getMetricsHistory("192.168.1.1", start, end);
    QCOMPARE(history.size(), 3);
}

void HistoryServiceTest::testGetAllMetricsForDevice()
{
    // Save metrics for different devices
    service->saveMetrics("192.168.1.1", createTestMetrics(25.0, 5.0, 0.0));
    service->saveMetrics("192.168.1.1", createTestMetrics(30.0, 6.0, 1.0));
    service->saveMetrics("192.168.1.2", createTestMetrics(20.0, 4.0, 0.0));

    QList<NetworkMetrics> device1Metrics = service->getAllMetricsForDevice("192.168.1.1");
    QCOMPARE(device1Metrics.size(), 2);

    QList<NetworkMetrics> device2Metrics = service->getAllMetricsForDevice("192.168.1.2");
    QCOMPARE(device2Metrics.size(), 1);

    // Test with limit
    service->saveMetrics("192.168.1.1", createTestMetrics(35.0, 7.0, 2.0));
    QList<NetworkMetrics> limitedMetrics = service->getAllMetricsForDevice("192.168.1.1", 2);
    QCOMPARE(limitedMetrics.size(), 2);
}

void HistoryServiceTest::testGetEventHistory()
{
    // Save multiple events
    service->saveEvent("192.168.1.1", "device_online", "Device online");
    QTest::qWait(100);
    service->saveEvent("192.168.1.1", "alert", "High latency");
    QTest::qWait(100);
    service->saveEvent("192.168.1.1", "device_offline", "Device offline");

    QDateTime start = QDateTime::currentDateTime().addSecs(-10);
    QDateTime end = QDateTime::currentDateTime().addSecs(10);

    QList<HistoryEvent> history = service->getEventHistory("192.168.1.1", start, end);
    QCOMPARE(history.size(), 3);
}

void HistoryServiceTest::testGetAllEventsForDevice()
{
    // Save events for different devices
    service->saveEvent("192.168.1.1", "device_online", "Device 1 online");
    service->saveEvent("192.168.1.1", "alert", "Device 1 alert");
    service->saveEvent("192.168.1.2", "device_online", "Device 2 online");

    QList<HistoryEvent> device1Events = service->getAllEventsForDevice("192.168.1.1");
    QCOMPARE(device1Events.size(), 2);

    QList<HistoryEvent> device2Events = service->getAllEventsForDevice("192.168.1.2");
    QCOMPARE(device2Events.size(), 1);

    // Test with limit
    service->saveEvent("192.168.1.1", "device_offline", "Device 1 offline");
    QList<HistoryEvent> limitedEvents = service->getAllEventsForDevice("192.168.1.1", 2);
    QCOMPARE(limitedEvents.size(), 2);
}

void HistoryServiceTest::testGetLatestMetrics()
{
    service->saveMetrics("192.168.1.1", createTestMetrics(25.0, 5.0, 0.0));
    QTest::qWait(100);
    service->saveMetrics("192.168.1.1", createTestMetrics(30.0, 6.0, 1.0));
    QTest::qWait(100);
    NetworkMetrics latest = createTestMetrics(28.0, 5.5, 0.5);
    service->saveMetrics("192.168.1.1", latest);

    NetworkMetrics retrieved = service->getLatestMetrics("192.168.1.1");

    QCOMPARE(retrieved.getLatencyAvg(), 28.0);
    QCOMPARE(retrieved.getJitter(), 5.5);
    QCOMPARE(retrieved.getPacketLoss(), 0.5);
}

void HistoryServiceTest::testPruneOldData()
{
    QSignalSpy spy(service, &HistoryService::dataPruned);

    // Save some metrics and events
    service->saveMetrics("192.168.1.1", createTestMetrics(25.0, 5.0, 0.0));
    service->saveEvent("192.168.1.1", "device_online", "Device online");

    // Prune data older than 30 days (should not delete recent data)
    int deleted = service->pruneOldData(30);
    QCOMPARE(deleted, 0);  // No old data to delete

    // Verify data still exists
    QCOMPARE(service->getMetricsCount("192.168.1.1"), 1);
    QCOMPARE(service->getEventCount("192.168.1.1"), 1);
}

void HistoryServiceTest::testDeleteDeviceHistory()
{
    // Save data for multiple devices
    service->saveMetrics("192.168.1.1", createTestMetrics(25.0, 5.0, 0.0));
    service->saveMetrics("192.168.1.2", createTestMetrics(30.0, 6.0, 1.0));
    service->saveEvent("192.168.1.1", "device_online", "Device 1 online");
    service->saveEvent("192.168.1.2", "device_online", "Device 2 online");

    QCOMPARE(service->getMetricsCount("192.168.1.1"), 1);
    QCOMPARE(service->getEventCount("192.168.1.1"), 1);

    bool result = service->deleteDeviceHistory("192.168.1.1");
    QVERIFY(result);

    QCOMPARE(service->getMetricsCount("192.168.1.1"), 0);
    QCOMPARE(service->getEventCount("192.168.1.1"), 0);

    // Device 2 should still have data
    QCOMPARE(service->getMetricsCount("192.168.1.2"), 1);
    QCOMPARE(service->getEventCount("192.168.1.2"), 1);
}

void HistoryServiceTest::testGetMetricsCount()
{
    QCOMPARE(service->getMetricsCount("192.168.1.1"), 0);

    service->saveMetrics("192.168.1.1", createTestMetrics(25.0, 5.0, 0.0));
    QCOMPARE(service->getMetricsCount("192.168.1.1"), 1);

    service->saveMetrics("192.168.1.1", createTestMetrics(30.0, 6.0, 1.0));
    QCOMPARE(service->getMetricsCount("192.168.1.1"), 2);

    service->saveMetrics("192.168.1.2", createTestMetrics(20.0, 4.0, 0.0));
    QCOMPARE(service->getMetricsCount("192.168.1.1"), 2);
    QCOMPARE(service->getMetricsCount("192.168.1.2"), 1);
}

void HistoryServiceTest::testGetEventCount()
{
    QCOMPARE(service->getEventCount("192.168.1.1"), 0);

    service->saveEvent("192.168.1.1", "device_online", "Device online");
    QCOMPARE(service->getEventCount("192.168.1.1"), 1);

    service->saveEvent("192.168.1.1", "alert", "High latency");
    QCOMPARE(service->getEventCount("192.168.1.1"), 2);

    service->saveEvent("192.168.1.2", "device_online", "Device 2 online");
    QCOMPARE(service->getEventCount("192.168.1.1"), 2);
    QCOMPARE(service->getEventCount("192.168.1.2"), 1);
}

void HistoryServiceTest::testHistoryEventToString()
{
    service->saveEvent("192.168.1.1", "device_online", "Device came online");

    QList<HistoryEvent> events = service->getAllEventsForDevice("192.168.1.1");
    QCOMPARE(events.size(), 1);

    QString eventStr = events[0].toString();
    QVERIFY(eventStr.contains("192.168.1.1"));
    QVERIFY(eventStr.contains("device_online"));
    QVERIFY(eventStr.contains("Device came online"));
}

QTEST_MAIN(HistoryServiceTest)
#include "HistoryServiceTest.moc"
