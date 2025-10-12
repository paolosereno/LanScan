#include <QtTest>
#include <QSignalSpy>
#include "viewmodels/MetricsViewModel.h"
#include "controllers/MetricsController.h"
#include "interfaces/IDeviceRepository.h"
#include "models/Device.h"
#include "models/NetworkMetrics.h"

/**
 * @brief Mock MetricsController for testing MetricsViewModel
 */
class MockMetricsController : public MetricsController {
    Q_OBJECT

public:
    explicit MockMetricsController(QObject* parent = nullptr)
        : MetricsController(nullptr, nullptr, parent)
    {}

    void collectMetricsOnce(const QString& deviceId) {
        lastDeviceId = deviceId;
        collectionCount++;
    }

    // Helper methods for testing
    void simulateMetricsCollected(const QString& deviceId, const NetworkMetrics& metrics) {
        emit metricsCollected(deviceId, metrics);
    }

    QString getLastDeviceId() const { return lastDeviceId; }
    int getCollectionCount() const { return collectionCount; }
    void resetCollectionCount() { collectionCount = 0; }

private:
    QString lastDeviceId;
    int collectionCount = 0;
};

/**
 * @brief Mock DeviceRepository for testing
 */
class MockDeviceRepository : public IDeviceRepository {
public:
    Device findById(const QString& id) const override { Q_UNUSED(id); return Device(); }
    Device findByIp(const QString& ip) const override { Q_UNUSED(ip); return Device(); }
    QList<Device> findAll() const override { return QList<Device>(); }
    bool save(const Device& device) override { Q_UNUSED(device); return true; }
    bool update(const Device& device) override { Q_UNUSED(device); return true; }
    bool remove(const QString& id) override { Q_UNUSED(id); return true; }
    void clear() override {}
};

/**
 * @brief Unit tests for MetricsViewModel
 *
 * Tests cover:
 * - Device management (set, get)
 * - Monitoring control (start, stop, isMonitoring)
 * - Metrics collection and history
 * - Signal emissions
 * - History management (clear, prune, max size)
 * - Timer functionality
 */
class MetricsViewModelTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Construction tests
    void testViewModelInitialization();
    void testViewModelInitialization_NullController();

    // Device management tests
    void testSetDevice();
    void testSetDevice_SameDevice();
    void testGetCurrentDevice();

    // Monitoring control tests
    void testStartMonitoring();
    void testStartMonitoring_AlreadyStarted();
    void testStartMonitoring_NoDevice();
    void testStartMonitoring_InvalidInterval();
    void testStopMonitoring();
    void testIsMonitoring();

    // Metrics collection tests
    void testMetricsCollection();
    void testMetricsCollection_WrongDevice();
    void testGetLatestMetrics();
    void testGetMetricsHistory();

    // History management tests
    void testClearHistory();
    void testSetMaxHistorySize();
    void testHistoryPruning();

    // Signal emission tests
    void testSignal_MonitoringStarted();
    void testSignal_MonitoringStopped();
    void testSignal_MetricsUpdated();
    void testSignal_DeviceChanged();
    void testSignal_HistoryCleared();

private:
    MetricsViewModel* viewModel;
    MockMetricsController* mockController;
    MockDeviceRepository* mockRepository;

    Device createTestDevice(const QString& ip, const QString& hostname);
    NetworkMetrics createTestMetrics(double latency, double jitter, double packetLoss);
};

void MetricsViewModelTest::initTestCase() {
    // Nothing to initialize
}

void MetricsViewModelTest::cleanupTestCase() {
    // Nothing to cleanup
}

void MetricsViewModelTest::init() {
    // Create fresh instances for each test
    mockController = new MockMetricsController();
    mockRepository = new MockDeviceRepository();
    viewModel = new MetricsViewModel(mockController, mockRepository);
}

void MetricsViewModelTest::cleanup() {
    // Clean up after each test
    delete viewModel;
    delete mockRepository;
    delete mockController;

    viewModel = nullptr;
    mockRepository = nullptr;
    mockController = nullptr;
}

// ============================================================================
// Construction Tests
// ============================================================================

void MetricsViewModelTest::testViewModelInitialization() {
    QVERIFY(viewModel != nullptr);
    QVERIFY(!viewModel->isMonitoring());
    QCOMPARE(viewModel->getMaxHistorySize(), 1000); // Default size
    QVERIFY(viewModel->getMetricsHistory().isEmpty());
}

void MetricsViewModelTest::testViewModelInitialization_NullController() {
    // Create view model with null controller (should handle gracefully)
    MetricsViewModel* nullControllerViewModel = new MetricsViewModel(nullptr, mockRepository);

    QVERIFY(nullControllerViewModel != nullptr);
    QVERIFY(!nullControllerViewModel->isMonitoring());

    delete nullControllerViewModel;
}

// ============================================================================
// Device Management Tests
// ============================================================================

void MetricsViewModelTest::testSetDevice() {
    QSignalSpy deviceChangedSpy(viewModel, &MetricsViewModel::deviceChanged);

    Device device = createTestDevice("192.168.1.100", "test-device");
    viewModel->setDevice(device);

    // Verify device was set
    Device currentDevice = viewModel->getCurrentDevice();
    QCOMPARE(currentDevice.getIp(), device.getIp());
    QCOMPARE(currentDevice.getHostname(), device.getHostname());

    // Verify signal was emitted
    QCOMPARE(deviceChangedSpy.count(), 1);
}

void MetricsViewModelTest::testSetDevice_SameDevice() {
    Device device = createTestDevice("192.168.1.100", "test-device");
    viewModel->setDevice(device);

    QSignalSpy deviceChangedSpy(viewModel, &MetricsViewModel::deviceChanged);

    // Set same device again
    viewModel->setDevice(device);

    // No signal should be emitted for same device
    QCOMPARE(deviceChangedSpy.count(), 0);
}

void MetricsViewModelTest::testGetCurrentDevice() {
    Device device = createTestDevice("192.168.1.100", "test-device");
    viewModel->setDevice(device);

    Device retrieved = viewModel->getCurrentDevice();

    QCOMPARE(retrieved.getIp(), device.getIp());
    QCOMPARE(retrieved.getHostname(), device.getHostname());
}

// ============================================================================
// Monitoring Control Tests
// ============================================================================

void MetricsViewModelTest::testStartMonitoring() {
    Device device = createTestDevice("192.168.1.100", "test-device");
    viewModel->setDevice(device);

    QSignalSpy monitoringStartedSpy(viewModel, &MetricsViewModel::monitoringStarted);

    viewModel->startMonitoring(500);

    // Verify monitoring started
    QVERIFY(viewModel->isMonitoring());

    // Verify signal was emitted
    QCOMPARE(monitoringStartedSpy.count(), 1);

    // Verify timer triggered collection immediately
    QTest::qWait(100);
    QVERIFY(mockController->getCollectionCount() > 0);
}

void MetricsViewModelTest::testStartMonitoring_AlreadyStarted() {
    Device device = createTestDevice("192.168.1.100", "test-device");
    viewModel->setDevice(device);

    viewModel->startMonitoring(500);
    QVERIFY(viewModel->isMonitoring());

    QSignalSpy monitoringStartedSpy(viewModel, &MetricsViewModel::monitoringStarted);

    // Try to start again (should be ignored)
    viewModel->startMonitoring(500);

    // No additional signal should be emitted
    QCOMPARE(monitoringStartedSpy.count(), 0);
}

void MetricsViewModelTest::testStartMonitoring_NoDevice() {
    QSignalSpy monitoringStartedSpy(viewModel, &MetricsViewModel::monitoringStarted);

    // Try to start monitoring without setting device
    viewModel->startMonitoring(500);

    // Monitoring should NOT start
    QVERIFY(!viewModel->isMonitoring());
    QCOMPARE(monitoringStartedSpy.count(), 0);
}

void MetricsViewModelTest::testStartMonitoring_InvalidInterval() {
    Device device = createTestDevice("192.168.1.100", "test-device");
    viewModel->setDevice(device);

    // Try with too small interval (< 100ms)
    viewModel->startMonitoring(50);

    // Should still start (interval clamped to minimum)
    QVERIFY(viewModel->isMonitoring());

    viewModel->stopMonitoring();

    // Try with too large interval (> 60000ms)
    viewModel->startMonitoring(100000);

    // Should still start (interval capped to maximum)
    QVERIFY(viewModel->isMonitoring());
}

void MetricsViewModelTest::testStopMonitoring() {
    Device device = createTestDevice("192.168.1.100", "test-device");
    viewModel->setDevice(device);
    viewModel->startMonitoring(500);

    QVERIFY(viewModel->isMonitoring());

    QSignalSpy monitoringStoppedSpy(viewModel, &MetricsViewModel::monitoringStopped);

    viewModel->stopMonitoring();

    // Verify monitoring stopped
    QVERIFY(!viewModel->isMonitoring());

    // Verify signal was emitted
    QCOMPARE(monitoringStoppedSpy.count(), 1);
}

void MetricsViewModelTest::testIsMonitoring() {
    Device device = createTestDevice("192.168.1.100", "test-device");
    viewModel->setDevice(device);

    // Initially not monitoring
    QVERIFY(!viewModel->isMonitoring());

    // Start monitoring
    viewModel->startMonitoring(500);
    QVERIFY(viewModel->isMonitoring());

    // Stop monitoring
    viewModel->stopMonitoring();
    QVERIFY(!viewModel->isMonitoring());
}

// ============================================================================
// Metrics Collection Tests
// ============================================================================

void MetricsViewModelTest::testMetricsCollection() {
    Device device = createTestDevice("192.168.1.100", "test-device");
    viewModel->setDevice(device);

    QSignalSpy metricsUpdatedSpy(viewModel, &MetricsViewModel::metricsUpdated);

    // Simulate metrics collection
    NetworkMetrics metrics = createTestMetrics(10.5, 1.2, 0.5);
    mockController->simulateMetricsCollected(device.getIp(), metrics);

    // Wait for signal processing
    QTest::qWait(100);

    // Verify metricsUpdated signal was emitted
    QCOMPARE(metricsUpdatedSpy.count(), 1);

    // Verify metrics were stored
    NetworkMetrics latest = viewModel->getLatestMetrics();
    QCOMPARE(latest.latencyAvg(), metrics.latencyAvg());
    QCOMPARE(latest.jitter(), metrics.jitter());
    QCOMPARE(latest.packetLoss(), metrics.packetLoss());

    // Verify history was updated
    QCOMPARE(viewModel->getMetricsHistory().size(), 1);
}

void MetricsViewModelTest::testMetricsCollection_WrongDevice() {
    Device device = createTestDevice("192.168.1.100", "test-device");
    viewModel->setDevice(device);

    QSignalSpy metricsUpdatedSpy(viewModel, &MetricsViewModel::metricsUpdated);

    // Simulate metrics collection for different device
    NetworkMetrics metrics = createTestMetrics(10.5, 1.2, 0.5);
    mockController->simulateMetricsCollected("192.168.1.200", metrics);

    // Wait for signal processing
    QTest::qWait(100);

    // No signal should be emitted (wrong device)
    QCOMPARE(metricsUpdatedSpy.count(), 0);

    // History should remain empty
    QVERIFY(viewModel->getMetricsHistory().isEmpty());
}

void MetricsViewModelTest::testGetLatestMetrics() {
    Device device = createTestDevice("192.168.1.100", "test-device");
    viewModel->setDevice(device);

    // Simulate multiple metrics collections
    NetworkMetrics metrics1 = createTestMetrics(10.0, 1.0, 0.0);
    NetworkMetrics metrics2 = createTestMetrics(15.0, 2.0, 1.0);

    mockController->simulateMetricsCollected(device.getIp(), metrics1);
    QTest::qWait(50);
    mockController->simulateMetricsCollected(device.getIp(), metrics2);
    QTest::qWait(50);

    // Latest should be metrics2
    NetworkMetrics latest = viewModel->getLatestMetrics();
    QCOMPARE(latest.latencyAvg(), 15.0);
    QCOMPARE(latest.jitter(), 2.0);
    QCOMPARE(latest.packetLoss(), 1.0);
}

void MetricsViewModelTest::testGetMetricsHistory() {
    Device device = createTestDevice("192.168.1.100", "test-device");
    viewModel->setDevice(device);

    // Simulate multiple metrics collections
    for (int i = 0; i < 5; i++) {
        NetworkMetrics metrics = createTestMetrics(10.0 + i, 1.0, 0.0);
        mockController->simulateMetricsCollected(device.getIp(), metrics);
        QTest::qWait(50);
    }

    // Verify history contains all metrics
    QList<NetworkMetrics> history = viewModel->getMetricsHistory();
    QCOMPARE(history.size(), 5);

    // Verify order (oldest to newest)
    QCOMPARE(history.first().latencyAvg(), 10.0);
    QCOMPARE(history.last().latencyAvg(), 14.0);
}

// ============================================================================
// History Management Tests
// ============================================================================

void MetricsViewModelTest::testClearHistory() {
    Device device = createTestDevice("192.168.1.100", "test-device");
    viewModel->setDevice(device);

    // Add some metrics
    NetworkMetrics metrics = createTestMetrics(10.0, 1.0, 0.0);
    mockController->simulateMetricsCollected(device.getIp(), metrics);
    QTest::qWait(50);

    QVERIFY(!viewModel->getMetricsHistory().isEmpty());

    QSignalSpy historyClearedSpy(viewModel, &MetricsViewModel::historyCleared);

    // Clear history
    viewModel->clearHistory();

    // Verify history is empty
    QVERIFY(viewModel->getMetricsHistory().isEmpty());

    // Verify signal was emitted
    QCOMPARE(historyClearedSpy.count(), 1);
}

void MetricsViewModelTest::testSetMaxHistorySize() {
    viewModel->setMaxHistorySize(500);
    QCOMPARE(viewModel->getMaxHistorySize(), 500);

    // Test invalid values
    viewModel->setMaxHistorySize(0);
    QCOMPARE(viewModel->getMaxHistorySize(), 1000); // Should use default

    viewModel->setMaxHistorySize(200000);
    QCOMPARE(viewModel->getMaxHistorySize(), 100000); // Should cap at max
}

void MetricsViewModelTest::testHistoryPruning() {
    Device device = createTestDevice("192.168.1.100", "test-device");
    viewModel->setDevice(device);

    // Set small max history size
    viewModel->setMaxHistorySize(3);

    // Add more metrics than max size
    for (int i = 0; i < 5; i++) {
        NetworkMetrics metrics = createTestMetrics(10.0 + i, 1.0, 0.0);
        mockController->simulateMetricsCollected(device.getIp(), metrics);
        QTest::qWait(50);
    }

    // Verify history was pruned to max size
    QList<NetworkMetrics> history = viewModel->getMetricsHistory();
    QCOMPARE(history.size(), 3);

    // Verify oldest metrics were removed (should have last 3)
    QCOMPARE(history.first().latencyAvg(), 12.0); // 3rd metric
    QCOMPARE(history.last().latencyAvg(), 14.0);  // 5th metric
}

// ============================================================================
// Signal Emission Tests
// ============================================================================

void MetricsViewModelTest::testSignal_MonitoringStarted() {
    Device device = createTestDevice("192.168.1.100", "test-device");
    viewModel->setDevice(device);

    QSignalSpy monitoringStartedSpy(viewModel, &MetricsViewModel::monitoringStarted);

    viewModel->startMonitoring(500);

    QCOMPARE(monitoringStartedSpy.count(), 1);
}

void MetricsViewModelTest::testSignal_MonitoringStopped() {
    Device device = createTestDevice("192.168.1.100", "test-device");
    viewModel->setDevice(device);
    viewModel->startMonitoring(500);

    QSignalSpy monitoringStoppedSpy(viewModel, &MetricsViewModel::monitoringStopped);

    viewModel->stopMonitoring();

    QCOMPARE(monitoringStoppedSpy.count(), 1);
}

void MetricsViewModelTest::testSignal_MetricsUpdated() {
    Device device = createTestDevice("192.168.1.100", "test-device");
    viewModel->setDevice(device);

    QSignalSpy metricsUpdatedSpy(viewModel, &MetricsViewModel::metricsUpdated);

    NetworkMetrics metrics = createTestMetrics(10.5, 1.2, 0.5);
    mockController->simulateMetricsCollected(device.getIp(), metrics);

    QTest::qWait(100);

    QCOMPARE(metricsUpdatedSpy.count(), 1);
}

void MetricsViewModelTest::testSignal_DeviceChanged() {
    QSignalSpy deviceChangedSpy(viewModel, &MetricsViewModel::deviceChanged);

    Device device = createTestDevice("192.168.1.100", "test-device");
    viewModel->setDevice(device);

    QCOMPARE(deviceChangedSpy.count(), 1);

    // Verify signal argument
    QList<QVariant> arguments = deviceChangedSpy.takeFirst();
    Device emittedDevice = arguments.at(0).value<Device>();
    QCOMPARE(emittedDevice.getIp(), device.getIp());
}

void MetricsViewModelTest::testSignal_HistoryCleared() {
    Device device = createTestDevice("192.168.1.100", "test-device");
    viewModel->setDevice(device);

    // Add some metrics
    NetworkMetrics metrics = createTestMetrics(10.0, 1.0, 0.0);
    mockController->simulateMetricsCollected(device.getIp(), metrics);
    QTest::qWait(50);

    QSignalSpy historyClearedSpy(viewModel, &MetricsViewModel::historyCleared);

    viewModel->clearHistory();

    QCOMPARE(historyClearedSpy.count(), 1);
}

// ============================================================================
// Helper Methods
// ============================================================================

Device MetricsViewModelTest::createTestDevice(const QString& ip, const QString& hostname) {
    Device device;
    device.setIp(ip);
    device.setHostname(hostname);
    device.setMacAddress("00:11:22:33:44:55");
    device.setOnline(true);
    device.setLastSeen(QDateTime::currentDateTime());
    return device;
}

NetworkMetrics MetricsViewModelTest::createTestMetrics(double latency, double jitter, double packetLoss) {
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

QTEST_MAIN(MetricsViewModelTest)
#include "MetricsViewModelTest.moc"
