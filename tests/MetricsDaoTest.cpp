#include <QtTest/QtTest>
#include "database/MetricsDao.h"
#include "database/DatabaseManager.h"
#include "models/NetworkMetrics.h"
#include "utils/Logger.h"
#include <QTemporaryDir>
#include <QSqlDatabase>

class MetricsDaoTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Test cases
    void testInsertMetrics();
    void testInsertInvalidMetrics();
    void testBatchInsert();
    void testFindByDevice();
    void testFindByDateRange();
    void testGetAverageMetrics();
    void testGetMaxLatency();
    void testGetMinLatency();
    void testGetAveragePacketLoss();
    void testGetAverageJitter();
    void testDeleteOlderThan();
    void testDeleteByDevice();
    void testGetMetricsCount();

private:
    DatabaseManager* dbManager;
    MetricsDao* metricsDao;
    QTemporaryDir* tempDir;
    QString dbPath;

    NetworkMetrics createTestMetrics(double latencyAvg = 25.0,
                                     double packetLoss = 0.5,
                                     double jitter = 2.0);
};

void MetricsDaoTest::initTestCase() {
    Logger::setLogLevel(Logger::ERROR);
    tempDir = new QTemporaryDir();
    QVERIFY(tempDir->isValid());
}

void MetricsDaoTest::cleanupTestCase() {
    delete tempDir;
}

void MetricsDaoTest::init() {
    // Create a new database for each test
    dbPath = tempDir->path() + "/test_metrics.db";
    QFile::remove(dbPath);

    dbManager = DatabaseManager::instance();
    QVERIFY(dbManager->open(dbPath));

    metricsDao = new MetricsDao(dbManager);
}

void MetricsDaoTest::cleanup() {
    delete metricsDao;
    dbManager->close();
    QFile::remove(dbPath);
}

NetworkMetrics MetricsDaoTest::createTestMetrics(double latencyAvg,
                                                 double packetLoss,
                                                 double jitter) {
    NetworkMetrics metrics;
    metrics.setLatencyMin(latencyAvg - 5.0);
    metrics.setLatencyAvg(latencyAvg);
    metrics.setLatencyMax(latencyAvg + 10.0);
    metrics.setLatencyMedian(latencyAvg);
    metrics.setJitter(jitter);
    metrics.setPacketLoss(packetLoss);
    metrics.setTimestamp(QDateTime::currentDateTime());
    metrics.calculateQualityScore();

    return metrics;
}

void MetricsDaoTest::testInsertMetrics() {
    NetworkMetrics metrics = createTestMetrics();
    QString deviceId = "test-device-1";

    bool result = metricsDao->insert(deviceId, metrics);
    QVERIFY(result);

    // Verify metrics were inserted
    QList<NetworkMetrics> retrieved = metricsDao->findByDevice(deviceId);
    QCOMPARE(retrieved.size(), 1);
    QCOMPARE(retrieved[0].getLatencyAvg(), metrics.getLatencyAvg());
    QCOMPARE(retrieved[0].getJitter(), metrics.getJitter());
    QCOMPARE(retrieved[0].getPacketLoss(), metrics.getPacketLoss());
}

void MetricsDaoTest::testInsertInvalidMetrics() {
    NetworkMetrics metrics = createTestMetrics();
    QString emptyDeviceId = "";

    bool result = metricsDao->insert(emptyDeviceId, metrics);
    QVERIFY(!result);
}

void MetricsDaoTest::testBatchInsert() {
    QList<NetworkMetrics> metricsList;
    for (int i = 0; i < 5; i++) {
        NetworkMetrics m = createTestMetrics(20.0 + i, 0.5 * i, 1.5 + i * 0.5);
        metricsList.append(m);
    }

    QString deviceId = "device-1";
    int count = metricsDao->insertBatch(deviceId, metricsList);
    QCOMPARE(count, 5);

    // Verify all metrics were inserted
    int totalCount = metricsDao->getMetricsCountByDevice(deviceId);
    QCOMPARE(totalCount, 5);
}

void MetricsDaoTest::testFindByDevice() {
    QString device1 = "device-1";
    QString device2 = "device-2";

    // Insert metrics for different devices
    metricsDao->insert(device1, createTestMetrics());
    metricsDao->insert(device1, createTestMetrics());
    metricsDao->insert(device2, createTestMetrics());

    QList<NetworkMetrics> device1Metrics = metricsDao->findByDevice(device1);
    QCOMPARE(device1Metrics.size(), 2);

    QList<NetworkMetrics> device2Metrics = metricsDao->findByDevice(device2);
    QCOMPARE(device2Metrics.size(), 1);

    // Test with limit
    QList<NetworkMetrics> limitedMetrics = metricsDao->findByDevice(device1, 1);
    QCOMPARE(limitedMetrics.size(), 1);
}

void MetricsDaoTest::testFindByDateRange() {
    QDateTime now = QDateTime::currentDateTime();
    QDateTime yesterday = now.addDays(-1);
    QDateTime tomorrow = now.addDays(1);
    QString deviceId = "device-1";

    // Insert metrics with different timestamps
    NetworkMetrics oldMetrics = createTestMetrics();
    oldMetrics.setTimestamp(yesterday.addSecs(-3600));
    metricsDao->insert(deviceId, oldMetrics);

    NetworkMetrics recentMetrics = createTestMetrics();
    recentMetrics.setTimestamp(now);
    metricsDao->insert(deviceId, recentMetrics);

    // Find metrics in range
    QList<NetworkMetrics> metrics = metricsDao->findByDateRange(deviceId, yesterday, tomorrow);
    QCOMPARE(metrics.size(), 2);
}

void MetricsDaoTest::testGetAverageMetrics() {
    QString deviceId = "device-1";
    QDateTime start = QDateTime::currentDateTime().addDays(-1);
    QDateTime end = QDateTime::currentDateTime().addDays(1);

    // Insert multiple metrics
    metricsDao->insert(deviceId, createTestMetrics(20.0, 1.0, 2.0));
    metricsDao->insert(deviceId, createTestMetrics(30.0, 2.0, 3.0));
    metricsDao->insert(deviceId, createTestMetrics(40.0, 3.0, 4.0));

    // Calculate average
    NetworkMetrics avgMetrics = metricsDao->getAverageMetrics(deviceId, start, end);

    // Average of 20, 30, 40 = 30
    QCOMPARE(avgMetrics.getLatencyAvg(), 30.0);

    // Average of 1.0, 2.0, 3.0 = 2.0
    QCOMPARE(avgMetrics.getPacketLoss(), 2.0);

    // Average of 2.0, 3.0, 4.0 = 3.0
    QCOMPARE(avgMetrics.getJitter(), 3.0);
}

void MetricsDaoTest::testGetMaxLatency() {
    QString deviceId = "device-1";
    QDateTime start = QDateTime::currentDateTime().addDays(-1);
    QDateTime end = QDateTime::currentDateTime().addDays(1);

    // Insert metrics with different latencies
    metricsDao->insert(deviceId, createTestMetrics(20.0, 1.0, 2.0));
    metricsDao->insert(deviceId, createTestMetrics(35.0, 1.0, 2.0));  // Max will be 45.0 (35+10)
    metricsDao->insert(deviceId, createTestMetrics(25.0, 1.0, 2.0));

    double maxLatency = metricsDao->getMaxLatency(deviceId, start, end);
    QCOMPARE(maxLatency, 45.0);  // Max from middle entry (35 + 10)
}

void MetricsDaoTest::testGetMinLatency() {
    QString deviceId = "device-1";
    QDateTime start = QDateTime::currentDateTime().addDays(-1);
    QDateTime end = QDateTime::currentDateTime().addDays(1);

    // Insert metrics with different latencies
    metricsDao->insert(deviceId, createTestMetrics(20.0, 1.0, 2.0));  // Min will be 15.0 (20-5)
    metricsDao->insert(deviceId, createTestMetrics(35.0, 1.0, 2.0));
    metricsDao->insert(deviceId, createTestMetrics(25.0, 1.0, 2.0));

    double minLatency = metricsDao->getMinLatency(deviceId, start, end);
    QCOMPARE(minLatency, 15.0);  // Min from first entry (20 - 5)
}

void MetricsDaoTest::testGetAveragePacketLoss() {
    QString deviceId = "device-1";
    QDateTime start = QDateTime::currentDateTime().addDays(-1);
    QDateTime end = QDateTime::currentDateTime().addDays(1);

    // Insert metrics with different packet loss values
    metricsDao->insert(deviceId, createTestMetrics(20.0, 1.0, 2.0));
    metricsDao->insert(deviceId, createTestMetrics(20.0, 2.0, 2.0));
    metricsDao->insert(deviceId, createTestMetrics(20.0, 3.0, 2.0));

    double avgPacketLoss = metricsDao->getAveragePacketLoss(deviceId, start, end);
    QCOMPARE(avgPacketLoss, 2.0);  // Average of 1.0, 2.0, 3.0
}

void MetricsDaoTest::testGetAverageJitter() {
    QString deviceId = "device-1";
    QDateTime start = QDateTime::currentDateTime().addDays(-1);
    QDateTime end = QDateTime::currentDateTime().addDays(1);

    // Insert metrics with different jitter values
    metricsDao->insert(deviceId, createTestMetrics(20.0, 1.0, 2.0));
    metricsDao->insert(deviceId, createTestMetrics(20.0, 1.0, 4.0));
    metricsDao->insert(deviceId, createTestMetrics(20.0, 1.0, 6.0));

    double avgJitter = metricsDao->getAverageJitter(deviceId, start, end);
    QCOMPARE(avgJitter, 4.0);  // Average of 2.0, 4.0, 6.0
}

void MetricsDaoTest::testDeleteOlderThan() {
    QDateTime now = QDateTime::currentDateTime();
    QDateTime yesterday = now.addDays(-1);
    QDateTime lastWeek = now.addDays(-7);
    QString deviceId = "device-1";

    // Insert old and recent metrics
    NetworkMetrics oldMetrics = createTestMetrics();
    oldMetrics.setTimestamp(lastWeek);
    metricsDao->insert(deviceId, oldMetrics);

    NetworkMetrics recentMetrics = createTestMetrics();
    recentMetrics.setTimestamp(now);
    metricsDao->insert(deviceId, recentMetrics);

    // Delete metrics older than yesterday
    int deleted = metricsDao->deleteOlderThan(yesterday);
    QCOMPARE(deleted, 1);

    // Verify only recent metrics remain
    int remaining = metricsDao->getMetricsCount();
    QCOMPARE(remaining, 1);
}

void MetricsDaoTest::testDeleteByDevice() {
    QString device1 = "device-1";
    QString device2 = "device-2";

    // Insert metrics for different devices
    metricsDao->insert(device1, createTestMetrics());
    metricsDao->insert(device1, createTestMetrics());
    metricsDao->insert(device2, createTestMetrics());

    // Delete device-1 metrics
    int deleted = metricsDao->deleteByDevice(device1);
    QCOMPARE(deleted, 2);

    // Verify only device-2 metrics remain
    int remaining = metricsDao->getMetricsCount();
    QCOMPARE(remaining, 1);

    QList<NetworkMetrics> device2Metrics = metricsDao->findByDevice(device2);
    QCOMPARE(device2Metrics.size(), 1);
}

void MetricsDaoTest::testGetMetricsCount() {
    QCOMPARE(metricsDao->getMetricsCount(), 0);

    QString device1 = "device-1";
    metricsDao->insert(device1, createTestMetrics());
    QCOMPARE(metricsDao->getMetricsCount(), 1);

    QString device2 = "device-2";
    metricsDao->insert(device2, createTestMetrics());
    QCOMPARE(metricsDao->getMetricsCount(), 2);

    int device1Count = metricsDao->getMetricsCountByDevice(device1);
    QCOMPARE(device1Count, 1);
}

QTEST_MAIN(MetricsDaoTest)
#include "MetricsDaoTest.moc"
