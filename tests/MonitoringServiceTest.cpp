#include <QtTest/QtTest>
#include "services/MonitoringService.h"
#include "controllers/MetricsController.h"
#include "services/AlertService.h"
#include "services/HistoryService.h"
#include "database/DatabaseManager.h"
#include "models/NetworkMetrics.h"

// Mock MetricsAggregator for testing
class MockMetricsAggregator : public QObject {
    Q_OBJECT
public:
    MockMetricsAggregator(QObject* parent = nullptr) : QObject(parent) {}
signals:
    void metricsUpdated(const NetworkMetrics& metrics);
};

class MonitoringServiceTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // MonitoringConfig tests
    void testMonitoringConfigConstruction();

    // MonitoringService tests
    void testServiceConstruction();
    void testStartMonitoring();
    void testStartMonitoringWithConfig();
    void testStopMonitoring();
    void testStopAllMonitoring();
    void testIsMonitoring();
    void testGetMonitoredDevices();
    void testGetConfig();
    void testUpdateConfig();
    void testSetAlertsEnabled();
    void testSetHistoryEnabled();
    void testMonitoringSignals();

private:
    DatabaseManager* dbManager;
    MockMetricsAggregator* mockAggregator;
    MetricsController* metricsController;
    AlertService* alertService;
    HistoryService* historyService;
    MonitoringService* service;

    NetworkMetrics createTestMetrics(double latency, double jitter, double packetLoss);
};

void MonitoringServiceTest::initTestCase()
{
    qDebug() << "Starting MonitoringService tests";

    dbManager = DatabaseManager::instance();
    QVERIFY(dbManager != nullptr);
}

void MonitoringServiceTest::cleanupTestCase()
{
    if (dbManager) {
        dbManager->close();
    }
    qDebug() << "MonitoringService tests completed";
}

void MonitoringServiceTest::init()
{
    // Setup database
    QVERIFY(dbManager->open(":memory:"));
    QVERIFY(dbManager->createSchema());

    // Create services (using nullptr for MetricsController to avoid dependencies)
    mockAggregator = nullptr;
    metricsController = nullptr;
    alertService = new AlertService(this);
    historyService = new HistoryService(dbManager, this);
    QVERIFY(historyService->initialize());

    // Create monitoring service with null metrics controller
    // This limits testing but avoids complex dependencies
    service = new MonitoringService(
        nullptr,  // MetricsController not needed for basic tests
        alertService,
        historyService,
        this
    );
}

void MonitoringServiceTest::cleanup()
{
    delete service;
    delete historyService;
    delete alertService;
    delete metricsController;
    delete mockAggregator;

    service = nullptr;
    historyService = nullptr;
    alertService = nullptr;
    metricsController = nullptr;
    mockAggregator = nullptr;

    if (dbManager) {
        dbManager->close();
    }
}

NetworkMetrics MonitoringServiceTest::createTestMetrics(double latency, double jitter, double packetLoss)
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

void MonitoringServiceTest::testMonitoringConfigConstruction()
{
    MonitoringConfig config1;
    QCOMPARE(config1.intervalMs, 1000);
    QCOMPARE(config1.enableAlerts, true);
    QCOMPARE(config1.alertLatencyThreshold, 100.0);
    QCOMPARE(config1.alertPacketLossThreshold, 5.0);
    QCOMPARE(config1.alertJitterThreshold, 30.0);

    MonitoringConfig config2("192.168.1.1", 2000);
    QCOMPARE(config2.deviceId, QString("192.168.1.1"));
    QCOMPARE(config2.intervalMs, 2000);
}

void MonitoringServiceTest::testServiceConstruction()
{
    QVERIFY(service != nullptr);
    QCOMPARE(service->getMonitoredDeviceCount(), 0);
}

void MonitoringServiceTest::testStartMonitoring()
{
    QSignalSpy spy(service, &MonitoringService::monitoringStarted);

    service->startMonitoring("192.168.1.1", 1000);

    QVERIFY(service->isMonitoring("192.168.1.1"));
    QCOMPARE(service->getMonitoredDeviceCount(), 1);
    QCOMPARE(spy.count(), 1);
}

void MonitoringServiceTest::testStartMonitoringWithConfig()
{
    MonitoringConfig config;
    config.deviceId = "192.168.1.1";
    config.intervalMs = 2000;
    config.enableAlerts = true;
    config.alertLatencyThreshold = 150.0;

    service->startMonitoring(config);

    QVERIFY(service->isMonitoring("192.168.1.1"));

    MonitoringConfig retrieved = service->getConfig("192.168.1.1");
    QCOMPARE(retrieved.deviceId, QString("192.168.1.1"));
    QCOMPARE(retrieved.intervalMs, 2000);
    QCOMPARE(retrieved.alertLatencyThreshold, 150.0);
}

void MonitoringServiceTest::testStopMonitoring()
{
    QSignalSpy startSpy(service, &MonitoringService::monitoringStarted);
    QSignalSpy stopSpy(service, &MonitoringService::monitoringStopped);

    service->startMonitoring("192.168.1.1");
    QVERIFY(service->isMonitoring("192.168.1.1"));
    QCOMPARE(startSpy.count(), 1);

    service->stopMonitoring("192.168.1.1");
    QVERIFY(!service->isMonitoring("192.168.1.1"));
    QCOMPARE(service->getMonitoredDeviceCount(), 0);
    QCOMPARE(stopSpy.count(), 1);
}

void MonitoringServiceTest::testStopAllMonitoring()
{
    service->startMonitoring("192.168.1.1");
    service->startMonitoring("192.168.1.2");
    service->startMonitoring("192.168.1.3");

    QCOMPARE(service->getMonitoredDeviceCount(), 3);

    service->stopAllMonitoring();

    QCOMPARE(service->getMonitoredDeviceCount(), 0);
    QVERIFY(!service->isMonitoring("192.168.1.1"));
    QVERIFY(!service->isMonitoring("192.168.1.2"));
    QVERIFY(!service->isMonitoring("192.168.1.3"));
}

void MonitoringServiceTest::testIsMonitoring()
{
    QVERIFY(!service->isMonitoring("192.168.1.1"));

    service->startMonitoring("192.168.1.1");
    QVERIFY(service->isMonitoring("192.168.1.1"));

    service->stopMonitoring("192.168.1.1");
    QVERIFY(!service->isMonitoring("192.168.1.1"));
}

void MonitoringServiceTest::testGetMonitoredDevices()
{
    QCOMPARE(service->getMonitoredDevices().size(), 0);

    service->startMonitoring("192.168.1.1");
    service->startMonitoring("192.168.1.2");

    QList<QString> devices = service->getMonitoredDevices();
    QCOMPARE(devices.size(), 2);
    QVERIFY(devices.contains("192.168.1.1"));
    QVERIFY(devices.contains("192.168.1.2"));
}

void MonitoringServiceTest::testGetConfig()
{
    MonitoringConfig config;
    config.deviceId = "192.168.1.1";
    config.intervalMs = 3000;
    config.alertLatencyThreshold = 200.0;

    service->startMonitoring(config);

    MonitoringConfig retrieved = service->getConfig("192.168.1.1");
    QCOMPARE(retrieved.deviceId, QString("192.168.1.1"));
    QCOMPARE(retrieved.intervalMs, 3000);
    QCOMPARE(retrieved.alertLatencyThreshold, 200.0);

    // Non-existent device should return default config
    MonitoringConfig defaultConfig = service->getConfig("192.168.1.99");
    QVERIFY(defaultConfig.deviceId.isEmpty());
}

void MonitoringServiceTest::testUpdateConfig()
{
    service->startMonitoring("192.168.1.1", 1000);

    MonitoringConfig newConfig;
    newConfig.deviceId = "192.168.1.1";
    newConfig.intervalMs = 5000;
    newConfig.alertLatencyThreshold = 250.0;

    service->updateConfig(newConfig);

    MonitoringConfig retrieved = service->getConfig("192.168.1.1");
    QCOMPARE(retrieved.intervalMs, 5000);
    QCOMPARE(retrieved.alertLatencyThreshold, 250.0);
}

void MonitoringServiceTest::testSetAlertsEnabled()
{
    service->startMonitoring("192.168.1.1");

    MonitoringConfig config = service->getConfig("192.168.1.1");
    QVERIFY(config.enableAlerts);

    service->setAlertsEnabled("192.168.1.1", false);

    config = service->getConfig("192.168.1.1");
    QVERIFY(!config.enableAlerts);

    service->setAlertsEnabled("192.168.1.1", true);

    config = service->getConfig("192.168.1.1");
    QVERIFY(config.enableAlerts);
}

void MonitoringServiceTest::testSetHistoryEnabled()
{
    service->startMonitoring("192.168.1.1");

    MonitoringConfig config = service->getConfig("192.168.1.1");
    QVERIFY(config.enableHistory);

    service->setHistoryEnabled("192.168.1.1", false);

    config = service->getConfig("192.168.1.1");
    QVERIFY(!config.enableHistory);

    service->setHistoryEnabled("192.168.1.1", true);

    config = service->getConfig("192.168.1.1");
    QVERIFY(config.enableHistory);
}

void MonitoringServiceTest::testMonitoringSignals()
{
    QSignalSpy startSpy(service, &MonitoringService::monitoringStarted);
    QSignalSpy stopSpy(service, &MonitoringService::monitoringStopped);

    service->startMonitoring("192.168.1.1");
    QCOMPARE(startSpy.count(), 1);

    QList<QVariant> startArgs = startSpy.takeFirst();
    QCOMPARE(startArgs.at(0).toString(), QString("192.168.1.1"));

    service->stopMonitoring("192.168.1.1");
    QCOMPARE(stopSpy.count(), 1);

    QList<QVariant> stopArgs = stopSpy.takeFirst();
    QCOMPARE(stopArgs.at(0).toString(), QString("192.168.1.1"));
}

QTEST_MAIN(MonitoringServiceTest)
#include "MonitoringServiceTest.moc"
