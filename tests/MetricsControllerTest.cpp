#include <QtTest>
#include <QSignalSpy>
#include "controllers/MetricsController.h"
#include "network/diagnostics/MetricsAggregator.h"
#include "database/DeviceRepository.h"
#include "models/NetworkMetrics.h"
#include "models/Device.h"
#include "database/DatabaseManager.h"

/**
 * @brief Mock MetricsAggregator for testing
 *
 * This mock allows us to test MetricsController without actually performing network operations.
 * We can simulate metrics collection and verify that MetricsController handles them correctly.
 */
class MockMetricsAggregator : public MetricsAggregator {
    Q_OBJECT

public:
    explicit MockMetricsAggregator(QObject* parent = nullptr)
        : MetricsAggregator(nullptr, nullptr, nullptr, nullptr, nullptr, parent)
        , mockCollecting(false)
    {}

    void startContinuousCollection(const QString& targetIp, int intervalMs = 1000) {
        Q_UNUSED(intervalMs);
        mockCollecting = true;
        currentTargetIp = targetIp;
    }

    void stopContinuousCollection() {
        mockCollecting = false;
        currentTargetIp.clear();
    }

    bool isCollecting() const {
        return mockCollecting;
    }

    // Helper methods for testing
    void simulateMetricsUpdate(const NetworkMetrics& metrics) {
        emit metricsUpdated(metrics);
    }

    QString getCurrentTargetIp() const {
        return currentTargetIp;
    }

private:
    bool mockCollecting;
    QString currentTargetIp;
};

/**
 * @brief Unit tests for MetricsController
 *
 * Tests cover:
 * - Continuous monitoring (start/stop)
 * - One-time metrics collection
 * - Monitoring all devices
 * - Signal emissions
 * - Timer management
 * - Error handling
 * - Device tracking
 */
class MetricsControllerTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Construction tests
    void testControllerInitialization();

    // Continuous monitoring tests
    void testStartContinuousMonitoring();
    void testStartContinuousMonitoring_EmptyDeviceId();
    void testStartContinuousMonitoring_AlreadyMonitoring();
    void testStopContinuousMonitoring();
    void testStopContinuousMonitoring_NotMonitoring();

    // One-time collection tests
    void testCollectMetricsOnce();
    void testCollectMetricsOnce_EmptyDeviceId();

    // Monitor all devices tests
    void testStartMonitoringAll();
    void testStopMonitoringAll();

    // Device tracking tests
    void testIsMonitoring();
    void testGetMonitoredDeviceCount();

    // Signal emission tests
    void testSignal_MonitoringStarted();
    void testSignal_MonitoringStopped();
    void testSignal_MetricsCollected();
    void testSignal_MetricsError();

    // Timer tests
    void testTimerCleanup();

private:
    MetricsController* controller;
    MockMetricsAggregator* mockAggregator;
    DeviceRepository* repository;
    DatabaseManager* dbManager;

    Device createTestDevice(const QString& ip, const QString& hostname, bool online = true);
    NetworkMetrics createTestMetrics(double latency, double jitter, double packetLoss);
};

void MetricsControllerTest::initTestCase() {
    // Initialize database manager with in-memory database
    dbManager = DatabaseManager::instance();
    dbManager->open(":memory:");
}

void MetricsControllerTest::cleanupTestCase() {
    if (dbManager) {
        dbManager->close();
    }
}

void MetricsControllerTest::init() {
    // Create fresh instances for each test
    mockAggregator = new MockMetricsAggregator();
    repository = new DeviceRepository(dbManager);
    controller = new MetricsController(mockAggregator, repository);
}

void MetricsControllerTest::cleanup() {
    // Clean up after each test
    delete controller;
    delete repository;
    delete mockAggregator;

    controller = nullptr;
    repository = nullptr;
    mockAggregator = nullptr;
}

// ============================================================================
// Construction Tests
// ============================================================================

void MetricsControllerTest::testControllerInitialization() {
    QVERIFY(controller != nullptr);

    // Verify initial state
    QCOMPARE(controller->getMonitoredDeviceCount(), 0);
    QVERIFY(!controller->isMonitoring("192.168.1.100"));
}

// ============================================================================
// Continuous Monitoring Tests
// ============================================================================

void MetricsControllerTest::testStartContinuousMonitoring() {
    QSignalSpy monitoringStartedSpy(controller, &MetricsController::monitoringStarted);

    QString deviceId = "192.168.1.100";
    controller->startContinuousMonitoring(deviceId, 1000);

    // Verify monitoring started
    QVERIFY(controller->isMonitoring(deviceId));
    QCOMPARE(controller->getMonitoredDeviceCount(), 1);

    // Verify signal was emitted
    QCOMPARE(monitoringStartedSpy.count(), 1);
    QCOMPARE(monitoringStartedSpy.takeFirst().at(0).toString(), deviceId);

    // Verify aggregator was called
    QVERIFY(mockAggregator->isCollecting());
}

void MetricsControllerTest::testStartContinuousMonitoring_EmptyDeviceId() {
    QSignalSpy monitoringStartedSpy(controller, &MetricsController::monitoringStarted);

    QString emptyDeviceId = "";
    controller->startContinuousMonitoring(emptyDeviceId);

    // Verify monitoring did NOT start
    QVERIFY(!controller->isMonitoring(emptyDeviceId));
    QCOMPARE(controller->getMonitoredDeviceCount(), 0);

    // Verify no signal was emitted
    QCOMPARE(monitoringStartedSpy.count(), 0);
}

void MetricsControllerTest::testStartContinuousMonitoring_AlreadyMonitoring() {
    QString deviceId = "192.168.1.100";

    // Start monitoring first time
    controller->startContinuousMonitoring(deviceId);
    QVERIFY(controller->isMonitoring(deviceId));

    QSignalSpy monitoringStartedSpy(controller, &MetricsController::monitoringStarted);

    // Try to start monitoring again (should be ignored)
    controller->startContinuousMonitoring(deviceId);

    // Verify no additional signal was emitted
    QCOMPARE(monitoringStartedSpy.count(), 0);

    // Verify still only one device being monitored
    QCOMPARE(controller->getMonitoredDeviceCount(), 1);
}

void MetricsControllerTest::testStopContinuousMonitoring() {
    QString deviceId = "192.168.1.100";

    // Start monitoring
    controller->startContinuousMonitoring(deviceId);
    QVERIFY(controller->isMonitoring(deviceId));

    QSignalSpy monitoringStoppedSpy(controller, &MetricsController::monitoringStopped);

    // Stop monitoring
    controller->stopContinuousMonitoring(deviceId);

    // Verify monitoring stopped
    QVERIFY(!controller->isMonitoring(deviceId));
    QCOMPARE(controller->getMonitoredDeviceCount(), 0);

    // Verify signal was emitted
    QCOMPARE(monitoringStoppedSpy.count(), 1);
    QCOMPARE(monitoringStoppedSpy.takeFirst().at(0).toString(), deviceId);
}

void MetricsControllerTest::testStopContinuousMonitoring_NotMonitoring() {
    QSignalSpy monitoringStoppedSpy(controller, &MetricsController::monitoringStopped);

    QString deviceId = "192.168.1.100";

    // Try to stop monitoring for device that's not being monitored
    controller->stopContinuousMonitoring(deviceId);

    // Verify no signal was emitted (since device wasn't being monitored)
    QCOMPARE(monitoringStoppedSpy.count(), 0);
}

// ============================================================================
// One-time Collection Tests
// ============================================================================

void MetricsControllerTest::testCollectMetricsOnce() {
    QString deviceId = "192.168.1.100";

    // Collect metrics once
    controller->collectMetricsOnce(deviceId);

    // Wait for processing
    QTest::qWait(100);

    // Verify aggregator was called
    QVERIFY(mockAggregator->isCollecting());
    QCOMPARE(mockAggregator->getCurrentTargetIp(), deviceId);

    // Verify no continuous monitoring was started
    QVERIFY(!controller->isMonitoring(deviceId));
}

void MetricsControllerTest::testCollectMetricsOnce_EmptyDeviceId() {
    QSignalSpy metricsErrorSpy(controller, &MetricsController::metricsError);

    QString emptyDeviceId = "";
    controller->collectMetricsOnce(emptyDeviceId);

    // Wait for processing
    QTest::qWait(100);

    // Verify error signal was emitted
    QCOMPARE(metricsErrorSpy.count(), 1);
    QCOMPARE(metricsErrorSpy.takeFirst().at(0).toString(), emptyDeviceId);
}

// ============================================================================
// Monitor All Devices Tests
// ============================================================================

void MetricsControllerTest::testStartMonitoringAll() {
    // Add test devices to repository
    repository->save(createTestDevice("192.168.1.100", "device1", true));
    repository->save(createTestDevice("192.168.1.101", "device2", true));
    repository->save(createTestDevice("192.168.1.102", "device3", false)); // Offline device

    // Start monitoring all devices
    controller->startMonitoringAll(1000);

    // Wait for processing
    QTest::qWait(100);

    // Verify only online devices are being monitored (2 out of 3)
    QCOMPARE(controller->getMonitoredDeviceCount(), 2);
    QVERIFY(controller->isMonitoring("192.168.1.100"));
    QVERIFY(controller->isMonitoring("192.168.1.101"));
    QVERIFY(!controller->isMonitoring("192.168.1.102")); // Offline device
}

void MetricsControllerTest::testStopMonitoringAll() {
    // Add and start monitoring devices
    repository->save(createTestDevice("192.168.1.100", "device1", true));
    repository->save(createTestDevice("192.168.1.101", "device2", true));
    controller->startMonitoringAll();

    // Verify devices are being monitored
    QVERIFY(controller->getMonitoredDeviceCount() > 0);

    QSignalSpy monitoringStoppedSpy(controller, &MetricsController::monitoringStopped);

    // Stop monitoring all
    controller->stopMonitoringAll();

    // Verify all monitoring stopped
    QCOMPARE(controller->getMonitoredDeviceCount(), 0);
    QVERIFY(!controller->isMonitoring("192.168.1.100"));
    QVERIFY(!controller->isMonitoring("192.168.1.101"));

    // Verify signals were emitted for each device
    QVERIFY(monitoringStoppedSpy.count() >= 2);
}

// ============================================================================
// Device Tracking Tests
// ============================================================================

void MetricsControllerTest::testIsMonitoring() {
    QString deviceId = "192.168.1.100";

    // Initially not monitoring
    QVERIFY(!controller->isMonitoring(deviceId));

    // Start monitoring
    controller->startContinuousMonitoring(deviceId);

    // Now should be monitoring
    QVERIFY(controller->isMonitoring(deviceId));

    // Stop monitoring
    controller->stopContinuousMonitoring(deviceId);

    // Should not be monitoring anymore
    QVERIFY(!controller->isMonitoring(deviceId));
}

void MetricsControllerTest::testGetMonitoredDeviceCount() {
    QCOMPARE(controller->getMonitoredDeviceCount(), 0);

    // Start monitoring first device
    controller->startContinuousMonitoring("192.168.1.100");
    QCOMPARE(controller->getMonitoredDeviceCount(), 1);

    // Start monitoring second device
    controller->startContinuousMonitoring("192.168.1.101");
    QCOMPARE(controller->getMonitoredDeviceCount(), 2);

    // Stop monitoring first device
    controller->stopContinuousMonitoring("192.168.1.100");
    QCOMPARE(controller->getMonitoredDeviceCount(), 1);

    // Stop monitoring second device
    controller->stopContinuousMonitoring("192.168.1.101");
    QCOMPARE(controller->getMonitoredDeviceCount(), 0);
}

// ============================================================================
// Signal Emission Tests
// ============================================================================

void MetricsControllerTest::testSignal_MonitoringStarted() {
    QSignalSpy monitoringStartedSpy(controller, &MetricsController::monitoringStarted);

    QString deviceId = "192.168.1.100";
    controller->startContinuousMonitoring(deviceId);

    QCOMPARE(monitoringStartedSpy.count(), 1);
    QList<QVariant> arguments = monitoringStartedSpy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), deviceId);
}

void MetricsControllerTest::testSignal_MonitoringStopped() {
    QString deviceId = "192.168.1.100";
    controller->startContinuousMonitoring(deviceId);

    QSignalSpy monitoringStoppedSpy(controller, &MetricsController::monitoringStopped);

    controller->stopContinuousMonitoring(deviceId);

    QCOMPARE(monitoringStoppedSpy.count(), 1);
    QList<QVariant> arguments = monitoringStoppedSpy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), deviceId);
}

void MetricsControllerTest::testSignal_MetricsCollected() {
    QSignalSpy metricsCollectedSpy(controller, &MetricsController::metricsCollected);

    QString deviceId = "192.168.1.100";
    controller->collectMetricsOnce(deviceId);

    // Wait for collection to start
    QTest::qWait(100);

    // Simulate metrics update from aggregator
    NetworkMetrics testMetrics = createTestMetrics(10.5, 1.2, 0.0);
    mockAggregator->simulateMetricsUpdate(testMetrics);

    // Wait for signal processing
    QTest::qWait(100);

    // Verify metricsCollected signal was emitted
    QCOMPARE(metricsCollectedSpy.count(), 1);
    QList<QVariant> arguments = metricsCollectedSpy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), deviceId);

    NetworkMetrics collectedMetrics = arguments.at(1).value<NetworkMetrics>();
    QCOMPARE(collectedMetrics.latencyAvg, testMetrics.latencyAvg);
    QCOMPARE(collectedMetrics.jitter, testMetrics.jitter);
    QCOMPARE(collectedMetrics.packetLoss, testMetrics.packetLoss);
}

void MetricsControllerTest::testSignal_MetricsError() {
    QSignalSpy metricsErrorSpy(controller, &MetricsController::metricsError);

    // Try to collect metrics with empty device ID
    controller->collectMetricsOnce("");

    // Wait for processing
    QTest::qWait(100);

    // Verify error signal was emitted
    QCOMPARE(metricsErrorSpy.count(), 1);
    QList<QVariant> arguments = metricsErrorSpy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), QString(""));
    QVERIFY(!arguments.at(1).toString().isEmpty()); // Error message should not be empty
}

// ============================================================================
// Timer Tests
// ============================================================================

void MetricsControllerTest::testTimerCleanup() {
    QString deviceId = "192.168.1.100";

    // Start monitoring (creates timer)
    controller->startContinuousMonitoring(deviceId, 500); // 500ms interval
    QVERIFY(controller->isMonitoring(deviceId));

    // Stop monitoring (should cleanup timer)
    controller->stopContinuousMonitoring(deviceId);
    QVERIFY(!controller->isMonitoring(deviceId));

    // Verify no more monitoring happening after cleanup
    QTest::qWait(600); // Wait longer than interval

    // If timer wasn't cleaned up properly, we might still get updates
    // Since we can't directly verify timer deletion, we verify state consistency
    QCOMPARE(controller->getMonitoredDeviceCount(), 0);
}

// ============================================================================
// Helper Methods
// ============================================================================

Device MetricsControllerTest::createTestDevice(const QString& ip, const QString& hostname, bool online) {
    Device device;
    device.setIp(ip);
    device.setHostname(hostname);
    device.setMacAddress("00:11:22:33:44:55");
    device.setOnline(online);
    device.setLastSeen(QDateTime::currentDateTime());
    return device;
}

NetworkMetrics MetricsControllerTest::createTestMetrics(double latency, double jitter, double packetLoss) {
    NetworkMetrics metrics;
    metrics.latencyAvg = latency;
    metrics.latencyMin = latency - 2.0;
    metrics.latencyMax = latency + 2.0;
    metrics.jitter = jitter;
    metrics.packetLoss = packetLoss;
    metrics.qualityScore = 95.0;
    metrics.timestamp = QDateTime::currentDateTime();
    return metrics;
}

QTEST_MAIN(MetricsControllerTest)
#include "MetricsControllerTest.moc"
