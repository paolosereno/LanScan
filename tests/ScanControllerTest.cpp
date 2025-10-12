#include <QtTest>
#include <QSignalSpy>
#include "controllers/ScanController.h"
#include "coordinators/ScanCoordinator.h"
#include "database/DeviceRepository.h"
#include "database/DeviceCache.h"
#include "models/Device.h"
#include "database/DatabaseManager.h"

/**
 * @brief Mock ScanCoordinator for testing
 *
 * This mock allows us to test ScanController without actually performing network scans.
 * We can simulate scan operations and verify that ScanController handles them correctly.
 */
class MockScanCoordinator : public ScanCoordinator {
    Q_OBJECT

public:
    explicit MockScanCoordinator(QObject* parent = nullptr)
        : ScanCoordinator(nullptr, nullptr, nullptr, parent)
        , mockScanning(false)
        , mockPaused(false)
        , lastConfig()
    {}

    void startScan(const ScanConfig& config) {
        lastConfig = config;
        mockScanning = true;
        mockPaused = false;

        // Emit scanStarted with a mock host count (small subnet for testing)
        int totalHosts = 8; // Simulate /29 subnet
        emit scanStarted(totalHosts);
    }

    void stopScan() {
        mockScanning = false;
        mockPaused = false;
        emit scanCompleted(0, 0);
    }

    void pauseScan() {
        mockPaused = true;
        emit scanPaused();
    }

    void resumeScan() {
        mockPaused = false;
        emit scanResumed();
    }

    bool isScanning() const { return mockScanning; }
    bool isPaused() const { return mockPaused; }

    // Helper methods for testing
    void simulateDeviceDiscovery(const Device& device) {
        emit deviceDiscovered(device);
    }

    void simulateProgress(int current, int total, const QString& ip) {
        emit scanProgress(current, total, ip);
    }

    void simulateCompletion(int count, qint64 duration) {
        mockScanning = false;
        emit scanCompleted(count, duration);
    }

    void simulateError(const QString& error) {
        mockScanning = false;
        emit scanError(error);
    }

    ScanConfig getLastConfig() const { return lastConfig; }

private:
    bool mockScanning;
    bool mockPaused;
    ScanConfig lastConfig;
};

/**
 * @brief Unit tests for ScanController
 *
 * Tests cover:
 * - Quick scan execution
 * - Deep scan execution
 * - Custom scan execution
 * - Scan control (stop, pause, resume)
 * - Device management (get all, clear)
 * - Signal emissions
 * - Error handling
 * - Cache and repository integration
 */
class ScanControllerTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Construction tests
    void testControllerInitialization();

    // Scan execution tests
    void testExecuteQuickScan();
    void testExecuteQuickScan_Config();
    void testExecuteDeepScan();
    void testExecuteDeepScan_Config();
    void testExecuteCustomScan();

    // Scan control tests
    void testStopCurrentScan();
    void testPauseCurrentScan();
    void testResumeCurrentScan();

    // Device management tests
    void testGetAllDevices_FromCache();
    void testGetAllDevices_FromRepository();
    void testClearAllDevices();

    // Signal emission tests
    void testSignal_ScanStarted();
    void testSignal_DeviceDiscovered();
    void testSignal_ScanProgress();
    void testSignal_ScanStatusChanged();
    void testSignal_DevicesUpdated();

    // Error handling tests
    void testScanError();

    // Integration tests
    void testDevicePersistence();
    void testMultipleDeviceDiscovery();

private:
    ScanController* controller;
    MockScanCoordinator* mockCoordinator;
    DeviceRepository* repository;
    DeviceCache* cache;
    DatabaseManager* dbManager;

    Device createTestDevice(const QString& ip, const QString& hostname);
};

void ScanControllerTest::initTestCase() {
    // Initialize database manager with in-memory database
    dbManager = DatabaseManager::instance();
    dbManager->open(":memory:");
}

void ScanControllerTest::cleanupTestCase() {
    if (dbManager) {
        dbManager->close();
    }
}

void ScanControllerTest::init() {
    // Create fresh instances for each test
    mockCoordinator = new MockScanCoordinator();
    repository = new DeviceRepository(dbManager);
    cache = new DeviceCache();
    controller = new ScanController(mockCoordinator, repository, cache);
}

void ScanControllerTest::cleanup() {
    // Clean up after each test
    delete controller;
    delete cache;
    delete repository;
    delete mockCoordinator;

    controller = nullptr;
    cache = nullptr;
    repository = nullptr;
    mockCoordinator = nullptr;
}

// ============================================================================
// Construction Tests
// ============================================================================

void ScanControllerTest::testControllerInitialization() {
    QVERIFY(controller != nullptr);

    // Verify initial state
    QList<Device> devices = controller->getAllDevices();
    QVERIFY(devices.isEmpty());
}

// ============================================================================
// Scan Execution Tests
// ============================================================================

void ScanControllerTest::testExecuteQuickScan() {
    QSignalSpy scanStartedSpy(controller, &ScanController::scanStarted);

    QString subnet = "192.168.1.0/29";
    controller->executeQuickScan(subnet);

    // Verify scanStarted signal was emitted
    QCOMPARE(scanStartedSpy.count(), 1);

    // Verify coordinator was called
    QVERIFY(mockCoordinator->isScanning());
}

void ScanControllerTest::testExecuteQuickScan_Config() {
    QString subnet = "192.168.1.0/24";
    controller->executeQuickScan(subnet);

    // Get the config that was passed to coordinator
    ScanCoordinator::ScanConfig config = mockCoordinator->getLastConfig();

    // Verify quick scan configuration
    QCOMPARE(config.subnet, subnet);
    QVERIFY(config.resolveDns);
    QVERIFY(!config.resolveArp);
    QVERIFY(!config.scanPorts);
    QCOMPARE(config.timeout, 1000);  // Quick scan: 1 second timeout
}

void ScanControllerTest::testExecuteDeepScan() {
    QSignalSpy scanStartedSpy(controller, &ScanController::scanStarted);

    QString subnet = "192.168.1.0/29";
    controller->executeDeepScan(subnet);

    // Verify scanStarted signal was emitted
    QCOMPARE(scanStartedSpy.count(), 1);

    // Verify coordinator was called
    QVERIFY(mockCoordinator->isScanning());
}

void ScanControllerTest::testExecuteDeepScan_Config() {
    QString subnet = "10.0.0.0/28";
    controller->executeDeepScan(subnet);

    // Get the config that was passed to coordinator
    ScanCoordinator::ScanConfig config = mockCoordinator->getLastConfig();

    // Verify deep scan configuration
    QCOMPARE(config.subnet, subnet);
    QVERIFY(config.resolveDns);
    QVERIFY(config.resolveArp);
    QVERIFY(config.scanPorts);
    QVERIFY(!config.portsToScan.isEmpty());  // Should have common ports
    QCOMPARE(config.timeout, 3000);  // Deep scan: 3 second timeout

    // Verify common ports are included
    QVERIFY(config.portsToScan.contains(22));   // SSH
    QVERIFY(config.portsToScan.contains(80));   // HTTP
    QVERIFY(config.portsToScan.contains(443));  // HTTPS
}

void ScanControllerTest::testExecuteCustomScan() {
    QSignalSpy scanStartedSpy(controller, &ScanController::scanStarted);

    // Create custom configuration
    ScanCoordinator::ScanConfig customConfig;
    customConfig.subnet = "172.16.0.0/24";
    customConfig.resolveDns = true;
    customConfig.resolveArp = false;
    customConfig.scanPorts = true;
    customConfig.portsToScan = {8080, 8443, 9000};
    customConfig.timeout = 2000;
    customConfig.maxThreads = 4;

    controller->executeCustomScan(customConfig);

    // Verify scanStarted signal was emitted
    QCOMPARE(scanStartedSpy.count(), 1);

    // Verify custom configuration was passed to coordinator
    ScanCoordinator::ScanConfig config = mockCoordinator->getLastConfig();
    QCOMPARE(config.subnet, customConfig.subnet);
    QCOMPARE(config.resolveDns, customConfig.resolveDns);
    QCOMPARE(config.resolveArp, customConfig.resolveArp);
    QCOMPARE(config.scanPorts, customConfig.scanPorts);
    QCOMPARE(config.portsToScan, customConfig.portsToScan);
    QCOMPARE(config.timeout, customConfig.timeout);
    QCOMPARE(config.maxThreads, customConfig.maxThreads);
}

// ============================================================================
// Scan Control Tests
// ============================================================================

void ScanControllerTest::testStopCurrentScan() {
    // Start a scan
    controller->executeQuickScan("192.168.1.0/29");
    QVERIFY(mockCoordinator->isScanning());

    // Stop the scan
    controller->stopCurrentScan();

    // Verify scan was stopped
    QVERIFY(!mockCoordinator->isScanning());
}

void ScanControllerTest::testPauseCurrentScan() {
    QSignalSpy pausedSpy(controller, &ScanController::scanStatusChanged);

    // Start a scan
    controller->executeQuickScan("192.168.1.0/29");

    // Pause the scan
    controller->pauseCurrentScan();

    // Verify scan was paused
    QVERIFY(mockCoordinator->isPaused());
    QVERIFY(pausedSpy.count() > 0);
}

void ScanControllerTest::testResumeCurrentScan() {
    QSignalSpy resumedSpy(controller, &ScanController::scanStatusChanged);

    // Start and pause a scan
    controller->executeQuickScan("192.168.1.0/29");
    controller->pauseCurrentScan();
    QVERIFY(mockCoordinator->isPaused());

    // Resume the scan
    controller->resumeCurrentScan();

    // Verify scan was resumed
    QVERIFY(!mockCoordinator->isPaused());
    QVERIFY(resumedSpy.count() > 0);
}

// ============================================================================
// Device Management Tests
// ============================================================================

void ScanControllerTest::testGetAllDevices_FromCache() {
    // Add devices to cache
    Device device1 = createTestDevice("192.168.1.100", "device1");
    Device device2 = createTestDevice("192.168.1.101", "device2");

    cache->put(device1.getIp(), device1);
    cache->put(device2.getIp(), device2);

    // Get all devices (should come from cache)
    QList<Device> devices = controller->getAllDevices();

    QCOMPARE(devices.size(), 2);
    // Verify devices by IP since Device doesn't have operator==
    bool hasDevice1 = false;
    bool hasDevice2 = false;
    for (const Device& d : devices) {
        if (d.getIp() == device1.getIp()) hasDevice1 = true;
        if (d.getIp() == device2.getIp()) hasDevice2 = true;
    }
    QVERIFY(hasDevice1);
    QVERIFY(hasDevice2);
}

void ScanControllerTest::testGetAllDevices_FromRepository() {
    // Add devices to repository only (not cache)
    Device device1 = createTestDevice("192.168.1.100", "device1");
    Device device2 = createTestDevice("192.168.1.101", "device2");

    repository->save(device1);
    repository->save(device2);

    // Cache should be empty initially
    QVERIFY(cache->getAll().isEmpty());

    // Get all devices (should load from repository and populate cache)
    QList<Device> devices = controller->getAllDevices();

    QCOMPARE(devices.size(), 2);

    // Verify cache was populated
    QCOMPARE(cache->getAll().size(), 2);
}

void ScanControllerTest::testClearAllDevices() {
    QSignalSpy devicesUpdatedSpy(controller, &ScanController::devicesUpdated);

    // Add devices to cache
    Device device1 = createTestDevice("192.168.1.100", "device1");
    cache->put(device1.getIp(), device1);

    // Clear all devices
    controller->clearAllDevices();

    // Verify cache was cleared
    QVERIFY(cache->getAll().isEmpty());

    // Verify signal was emitted
    QCOMPARE(devicesUpdatedSpy.count(), 1);
}

// ============================================================================
// Signal Emission Tests
// ============================================================================

void ScanControllerTest::testSignal_ScanStarted() {
    QSignalSpy scanStartedSpy(controller, &ScanController::scanStarted);
    QSignalSpy statusSpy(controller, &ScanController::scanStatusChanged);

    controller->executeQuickScan("192.168.1.0/29");

    // Verify scanStarted signal
    QCOMPARE(scanStartedSpy.count(), 1);
    QList<QVariant> arguments = scanStartedSpy.takeFirst();
    int totalHosts = arguments.at(0).toInt();
    QVERIFY(totalHosts > 0);

    // Verify status changed signal
    QVERIFY(statusSpy.count() > 0);
}

void ScanControllerTest::testSignal_DeviceDiscovered() {
    QSignalSpy deviceDiscoveredSpy(controller, &ScanController::deviceDiscovered);

    // Start a scan
    controller->executeQuickScan("192.168.1.0/29");

    // Simulate device discovery
    Device testDevice = createTestDevice("192.168.1.100", "test-device");
    mockCoordinator->simulateDeviceDiscovery(testDevice);

    // Wait for signal processing
    QTest::qWait(100);

    // Verify deviceDiscovered signal was emitted
    QCOMPARE(deviceDiscoveredSpy.count(), 1);
    QList<QVariant> arguments = deviceDiscoveredSpy.takeFirst();
    Device discoveredDevice = arguments.at(0).value<Device>();
    QCOMPARE(discoveredDevice.getIp(), testDevice.getIp());
    QCOMPARE(discoveredDevice.getHostname(), testDevice.getHostname());
}

void ScanControllerTest::testSignal_ScanProgress() {
    QSignalSpy progressSpy(controller, &ScanController::scanProgressUpdated);

    // Start a scan
    controller->executeQuickScan("192.168.1.0/29");

    // Simulate progress updates
    mockCoordinator->simulateProgress(5, 10, "192.168.1.5");

    // Wait for signal processing
    QTest::qWait(100);

    // Verify progress signal was emitted
    QCOMPARE(progressSpy.count(), 1);
    QList<QVariant> arguments = progressSpy.takeFirst();
    QCOMPARE(arguments.at(0).toInt(), 5);      // current
    QCOMPARE(arguments.at(1).toInt(), 10);     // total
    QCOMPARE(arguments.at(2).toDouble(), 50.0); // percentage
}

void ScanControllerTest::testSignal_ScanStatusChanged() {
    QSignalSpy statusSpy(controller, &ScanController::scanStatusChanged);

    // Start a scan (triggers status change)
    controller->executeQuickScan("192.168.1.0/29");

    // Simulate completion (triggers status change)
    mockCoordinator->simulateCompletion(5, 1000);

    // Wait for signal processing
    QTest::qWait(100);

    // Verify status changed signals were emitted (at least 2: start + complete)
    QVERIFY(statusSpy.count() >= 2);
}

void ScanControllerTest::testSignal_DevicesUpdated() {
    QSignalSpy devicesUpdatedSpy(controller, &ScanController::devicesUpdated);

    // Clear devices should emit devicesUpdated
    controller->clearAllDevices();

    QCOMPARE(devicesUpdatedSpy.count(), 1);
}

// ============================================================================
// Error Handling Tests
// ============================================================================

void ScanControllerTest::testScanError() {
    QSignalSpy statusSpy(controller, &ScanController::scanStatusChanged);

    // Start a scan
    controller->executeQuickScan("192.168.1.0/29");

    // Simulate an error
    QString errorMessage = "Network timeout";
    mockCoordinator->simulateError(errorMessage);

    // Wait for signal processing
    QTest::qWait(100);

    // Verify error status was reported
    QVERIFY(statusSpy.count() > 0);

    // Check that last status contains error message
    QString lastStatus = statusSpy.last().at(0).toString();
    QVERIFY(lastStatus.contains("Error"));
}

// ============================================================================
// Integration Tests
// ============================================================================

void ScanControllerTest::testDevicePersistence() {
    // Start a scan
    controller->executeQuickScan("192.168.1.0/29");

    // Simulate device discovery
    Device testDevice = createTestDevice("192.168.1.100", "test-device");
    mockCoordinator->simulateDeviceDiscovery(testDevice);

    // Wait for processing
    QTest::qWait(100);

    // Verify device was saved to cache
    Device cachedDevice = cache->get(testDevice.getIp());
    QVERIFY(!cachedDevice.getIp().isEmpty());
    QCOMPARE(cachedDevice.getIp(), testDevice.getIp());

    // Verify device was saved to repository
    Device repoDevice = repository->findById(testDevice.getIp());
    QVERIFY(!repoDevice.getIp().isEmpty());
    QCOMPARE(repoDevice.getIp(), testDevice.getIp());
}

void ScanControllerTest::testMultipleDeviceDiscovery() {
    QSignalSpy deviceDiscoveredSpy(controller, &ScanController::deviceDiscovered);

    // Start a scan
    controller->executeQuickScan("192.168.1.0/29");

    // Simulate multiple device discoveries
    for (int i = 0; i < 5; i++) {
        QString ip = QString("192.168.1.%1").arg(100 + i);
        QString hostname = QString("device%1").arg(i);
        Device device = createTestDevice(ip, hostname);
        mockCoordinator->simulateDeviceDiscovery(device);
    }

    // Wait for processing
    QTest::qWait(200);

    // Verify all devices were discovered
    QCOMPARE(deviceDiscoveredSpy.count(), 5);

    // Verify all devices are in cache
    QList<Device> cachedDevices = cache->getAll();
    QCOMPARE(cachedDevices.size(), 5);
}

// ============================================================================
// Helper Methods
// ============================================================================

Device ScanControllerTest::createTestDevice(const QString& ip, const QString& hostname) {
    Device device;
    device.setIp(ip);
    device.setHostname(hostname);
    device.setMacAddress("00:11:22:33:44:55");
    device.setOnline(true);
    device.setLastSeen(QDateTime::currentDateTime());
    return device;
}

QTEST_MAIN(ScanControllerTest)
#include "ScanControllerTest.moc"
