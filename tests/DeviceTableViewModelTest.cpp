#include <QtTest>
#include <QSignalSpy>
#include "viewmodels/DeviceTableViewModel.h"
#include "database/DeviceRepository.h"
#include "database/DatabaseManager.h"
#include "models/Device.h"
#include "models/PortInfo.h"
#include "models/NetworkMetrics.h"

/**
 * @brief Unit tests for DeviceTableViewModel
 *
 * Tests cover:
 * - QAbstractTableModel interface (rowCount, columnCount, data, headerData)
 * - Device management (add, update, remove, clear)
 * - Device loading from repository
 * - Signal emissions
 * - Data formatting
 * - Model index handling
 */
class DeviceTableViewModelTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Construction tests
    void testViewModelInitialization();

    // QAbstractTableModel interface tests
    void testRowCount();
    void testRowCount_Empty();
    void testColumnCount();
    void testData_DisplayRole();
    void testData_TextAlignmentRole();
    void testData_ForegroundRole();
    void testData_UserRole();
    void testData_InvalidIndex();
    void testHeaderData();

    // Device management tests
    void testAddDevice();
    void testAddDevice_Duplicate();
    void testUpdateDevice();
    void testUpdateDevice_NotFound();
    void testRemoveDevice();
    void testRemoveDevice_NotFound();
    void testClear();
    void testMarkAllDevicesOffline();

    // Load devices tests
    void testLoadDevices();
    void testLoadDevices_NullRepository();

    // Device lookup tests
    void testGetDeviceAt();
    void testGetDeviceAt_InvalidRow();
    void testFindDeviceRow();
    void testFindDeviceRow_NotFound();

    // Signal emission tests
    void testSignal_DeviceCountChanged();

private:
    DeviceTableViewModel* viewModel;
    DeviceRepository* repository;
    DatabaseManager* dbManager;

    Device createTestDevice(const QString& ip, const QString& hostname, bool online = true);
};

void DeviceTableViewModelTest::initTestCase() {
    // Initialize database manager with in-memory database
    dbManager = DatabaseManager::instance();
    dbManager->open(":memory:");
}

void DeviceTableViewModelTest::cleanupTestCase() {
    if (dbManager) {
        dbManager->close();
    }
}

void DeviceTableViewModelTest::init() {
    // Create fresh instances for each test
    repository = new DeviceRepository(dbManager);
    viewModel = new DeviceTableViewModel(repository);
}

void DeviceTableViewModelTest::cleanup() {
    // Clean up after each test
    delete viewModel;
    delete repository;

    viewModel = nullptr;
    repository = nullptr;
}

// ============================================================================
// Construction Tests
// ============================================================================

void DeviceTableViewModelTest::testViewModelInitialization() {
    QVERIFY(viewModel != nullptr);
    QCOMPARE(viewModel->rowCount(), 0);
    QCOMPARE(viewModel->columnCount(), DeviceTableViewModel::ColumnCount);
}

// ============================================================================
// QAbstractTableModel Interface Tests
// ============================================================================

void DeviceTableViewModelTest::testRowCount() {
    // Initially empty
    QCOMPARE(viewModel->rowCount(), 0);

    // Add devices
    viewModel->addDevice(createTestDevice("192.168.1.100", "device1"));
    QCOMPARE(viewModel->rowCount(), 1);

    viewModel->addDevice(createTestDevice("192.168.1.101", "device2"));
    QCOMPARE(viewModel->rowCount(), 2);
}

void DeviceTableViewModelTest::testRowCount_Empty() {
    viewModel->addDevice(createTestDevice("192.168.1.100", "device1"));
    QCOMPARE(viewModel->rowCount(), 1);

    viewModel->clear();
    QCOMPARE(viewModel->rowCount(), 0);
}

void DeviceTableViewModelTest::testColumnCount() {
    QCOMPARE(viewModel->columnCount(), DeviceTableViewModel::ColumnCount);
    QCOMPARE(viewModel->columnCount(), 8); // Verify enum count
}

void DeviceTableViewModelTest::testData_DisplayRole() {
    Device device = createTestDevice("192.168.1.100", "test-device");
    device.setMacAddress("AA:BB:CC:DD:EE:FF");
    device.setVendor("Test Vendor");

    NetworkMetrics metrics;
    metrics.setLatencyAvg(12.5);
    metrics.setQualityScore(NetworkMetrics::QualityScore::Excellent);
    device.setMetrics(metrics);

    PortInfo port(80, PortInfo::TCP);
    port.setService("HTTP");
    port.setState(PortInfo::Open);
    device.addPort(port);

    viewModel->addDevice(device);

    QModelIndex indexIp = viewModel->index(0, DeviceTableViewModel::IpAddress);
    QCOMPARE(viewModel->data(indexIp, Qt::DisplayRole).toString(), QString("192.168.1.100"));

    QModelIndex indexHostname = viewModel->index(0, DeviceTableViewModel::Hostname);
    QCOMPARE(viewModel->data(indexHostname, Qt::DisplayRole).toString(), QString("test-device"));

    QModelIndex indexMac = viewModel->index(0, DeviceTableViewModel::MacAddress);
    QCOMPARE(viewModel->data(indexMac, Qt::DisplayRole).toString(), QString("AA:BB:CC:DD:EE:FF"));

    QModelIndex indexVendor = viewModel->index(0, DeviceTableViewModel::Vendor);
    QCOMPARE(viewModel->data(indexVendor, Qt::DisplayRole).toString(), QString("Test Vendor"));
}

void DeviceTableViewModelTest::testData_TextAlignmentRole() {
    viewModel->addDevice(createTestDevice("192.168.1.100", "device1"));

    // Status column should be center-aligned
    QModelIndex statusIndex = viewModel->index(0, DeviceTableViewModel::Status);
    int alignment = viewModel->data(statusIndex, Qt::TextAlignmentRole).toInt();
    QVERIFY(alignment & Qt::AlignCenter);

    // IP Address column should be left-aligned
    QModelIndex ipIndex = viewModel->index(0, DeviceTableViewModel::IpAddress);
    alignment = viewModel->data(ipIndex, Qt::TextAlignmentRole).toInt();
    QVERIFY(alignment & Qt::AlignLeft);
}

void DeviceTableViewModelTest::testData_ForegroundRole() {
    Device device = createTestDevice("192.168.1.100", "device1");
    NetworkMetrics metrics;
    metrics.setQualityScore(NetworkMetrics::QualityScore::Excellent); // Excellent quality
    device.setMetrics(metrics);

    viewModel->addDevice(device);

    // Quality column should have color based on score
    QModelIndex qualityIndex = viewModel->index(0, DeviceTableViewModel::QualityScore);
    QVariant colorVariant = viewModel->data(qualityIndex, Qt::ForegroundRole);
    QVERIFY(colorVariant.isValid());
    QVERIFY(colorVariant.canConvert<QColor>());
}

void DeviceTableViewModelTest::testData_UserRole() {
    Device device = createTestDevice("192.168.1.100", "device1");
    viewModel->addDevice(device);

    // UserRole should return the full Device object
    QModelIndex index = viewModel->index(0, DeviceTableViewModel::IpAddress);
    QVariant deviceVariant = viewModel->data(index, Qt::UserRole);
    QVERIFY(deviceVariant.isValid());
    QVERIFY(deviceVariant.canConvert<Device>());

    Device retrievedDevice = deviceVariant.value<Device>();
    QCOMPARE(retrievedDevice.getIp(), device.getIp());
}

void DeviceTableViewModelTest::testData_InvalidIndex() {
    viewModel->addDevice(createTestDevice("192.168.1.100", "device1"));

    // Invalid row
    QModelIndex invalidIndex = viewModel->index(999, DeviceTableViewModel::IpAddress);
    QVERIFY(!viewModel->data(invalidIndex, Qt::DisplayRole).isValid());

    // Invalid column
    invalidIndex = viewModel->index(0, 999);
    QVERIFY(!viewModel->data(invalidIndex, Qt::DisplayRole).isValid());
}

void DeviceTableViewModelTest::testHeaderData() {
    // Test horizontal headers
    QCOMPARE(viewModel->headerData(DeviceTableViewModel::Status, Qt::Horizontal, Qt::DisplayRole).toString(),
             QString("Status"));
    QCOMPARE(viewModel->headerData(DeviceTableViewModel::IpAddress, Qt::Horizontal, Qt::DisplayRole).toString(),
             QString("IP Address"));
    QCOMPARE(viewModel->headerData(DeviceTableViewModel::Hostname, Qt::Horizontal, Qt::DisplayRole).toString(),
             QString("Hostname"));
    QCOMPARE(viewModel->headerData(DeviceTableViewModel::MacAddress, Qt::Horizontal, Qt::DisplayRole).toString(),
             QString("MAC Address"));
    QCOMPARE(viewModel->headerData(DeviceTableViewModel::Vendor, Qt::Horizontal, Qt::DisplayRole).toString(),
             QString("Vendor"));
    QCOMPARE(viewModel->headerData(DeviceTableViewModel::OpenPorts, Qt::Horizontal, Qt::DisplayRole).toString(),
             QString("Open Ports"));
    QCOMPARE(viewModel->headerData(DeviceTableViewModel::Latency, Qt::Horizontal, Qt::DisplayRole).toString(),
             QString("Latency"));
    QCOMPARE(viewModel->headerData(DeviceTableViewModel::QualityScore, Qt::Horizontal, Qt::DisplayRole).toString(),
             QString("Quality"));
}

// ============================================================================
// Device Management Tests
// ============================================================================

void DeviceTableViewModelTest::testAddDevice() {
    QSignalSpy rowsInsertedSpy(viewModel, &QAbstractItemModel::rowsInserted);
    QSignalSpy deviceCountSpy(viewModel, &DeviceTableViewModel::deviceCountChanged);

    Device device = createTestDevice("192.168.1.100", "device1");
    viewModel->addDevice(device);

    // Verify row was inserted
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(viewModel->rowCount(), 1);

    // Verify deviceCountChanged signal
    QCOMPARE(deviceCountSpy.count(), 1);
    QCOMPARE(deviceCountSpy.takeFirst().at(0).toInt(), 1);

    // Verify data
    QModelIndex index = viewModel->index(0, DeviceTableViewModel::IpAddress);
    QCOMPARE(viewModel->data(index, Qt::DisplayRole).toString(), QString("192.168.1.100"));
}

void DeviceTableViewModelTest::testAddDevice_Duplicate() {
    Device device = createTestDevice("192.168.1.100", "device1");
    viewModel->addDevice(device);
    QCOMPARE(viewModel->rowCount(), 1);

    QSignalSpy dataChangedSpy(viewModel, &QAbstractItemModel::dataChanged);

    // Add same device again (should update, not insert)
    Device updatedDevice = createTestDevice("192.168.1.100", "device1-updated");
    viewModel->addDevice(updatedDevice);

    // Row count should still be 1
    QCOMPARE(viewModel->rowCount(), 1);

    // Should have emitted dataChanged (update happened)
    QVERIFY(dataChangedSpy.count() > 0);
}

void DeviceTableViewModelTest::testUpdateDevice() {
    Device device = createTestDevice("192.168.1.100", "device1");
    viewModel->addDevice(device);

    QSignalSpy dataChangedSpy(viewModel, &QAbstractItemModel::dataChanged);

    // Update device
    Device updatedDevice = createTestDevice("192.168.1.100", "device1-updated");
    viewModel->updateDevice(updatedDevice);

    // Verify dataChanged signal
    QCOMPARE(dataChangedSpy.count(), 1);

    // Verify updated data
    QModelIndex index = viewModel->index(0, DeviceTableViewModel::Hostname);
    QCOMPARE(viewModel->data(index, Qt::DisplayRole).toString(), QString("device1-updated"));
}

void DeviceTableViewModelTest::testUpdateDevice_NotFound() {
    QSignalSpy dataChangedSpy(viewModel, &QAbstractItemModel::dataChanged);

    // Try to update non-existent device
    Device device = createTestDevice("192.168.1.100", "device1");
    viewModel->updateDevice(device);

    // No signal should be emitted
    QCOMPARE(dataChangedSpy.count(), 0);
}

void DeviceTableViewModelTest::testRemoveDevice() {
    Device device = createTestDevice("192.168.1.100", "device1");
    viewModel->addDevice(device);
    QCOMPARE(viewModel->rowCount(), 1);

    QSignalSpy rowsRemovedSpy(viewModel, &QAbstractItemModel::rowsRemoved);
    QSignalSpy deviceCountSpy(viewModel, &DeviceTableViewModel::deviceCountChanged);

    // Remove device
    viewModel->removeDevice("192.168.1.100");

    // Verify row was removed
    QCOMPARE(rowsRemovedSpy.count(), 1);
    QCOMPARE(viewModel->rowCount(), 0);

    // Verify deviceCountChanged signal
    QCOMPARE(deviceCountSpy.count(), 1);
    QCOMPARE(deviceCountSpy.takeFirst().at(0).toInt(), 0);
}

void DeviceTableViewModelTest::testRemoveDevice_NotFound() {
    QSignalSpy rowsRemovedSpy(viewModel, &QAbstractItemModel::rowsRemoved);

    // Try to remove non-existent device
    viewModel->removeDevice("192.168.1.100");

    // No signal should be emitted
    QCOMPARE(rowsRemovedSpy.count(), 0);
}

void DeviceTableViewModelTest::testClear() {
    viewModel->addDevice(createTestDevice("192.168.1.100", "device1"));
    viewModel->addDevice(createTestDevice("192.168.1.101", "device2"));
    QCOMPARE(viewModel->rowCount(), 2);

    QSignalSpy modelResetSpy(viewModel, &QAbstractItemModel::modelReset);
    QSignalSpy deviceCountSpy(viewModel, &DeviceTableViewModel::deviceCountChanged);

    // Clear all devices
    viewModel->clear();

    // Verify model was reset
    QCOMPARE(modelResetSpy.count(), 1);
    QCOMPARE(viewModel->rowCount(), 0);

    // Verify deviceCountChanged signal
    QCOMPARE(deviceCountSpy.count(), 1);
    QCOMPARE(deviceCountSpy.takeFirst().at(0).toInt(), 0);
}

void DeviceTableViewModelTest::testMarkAllDevicesOffline() {
    Device device1 = createTestDevice("192.168.1.100", "device1", true);
    Device device2 = createTestDevice("192.168.1.101", "device2", true);

    viewModel->addDevice(device1);
    viewModel->addDevice(device2);

    QSignalSpy dataChangedSpy(viewModel, &QAbstractItemModel::dataChanged);

    // Mark all devices offline
    viewModel->markAllDevicesOffline();

    // Verify dataChanged signal was emitted
    QVERIFY(dataChangedSpy.count() > 0);

    // Verify devices are offline
    Device retrievedDevice = viewModel->getDeviceAt(0);
    QVERIFY(!retrievedDevice.isOnline());
}

// ============================================================================
// Load Devices Tests
// ============================================================================

void DeviceTableViewModelTest::testLoadDevices() {
    // Add devices to repository
    repository->save(createTestDevice("192.168.1.100", "device1"));
    repository->save(createTestDevice("192.168.1.101", "device2"));

    QSignalSpy modelResetSpy(viewModel, &QAbstractItemModel::modelReset);
    QSignalSpy deviceCountSpy(viewModel, &DeviceTableViewModel::deviceCountChanged);

    // Load devices from repository
    viewModel->loadDevices();

    // Verify model was reset
    QCOMPARE(modelResetSpy.count(), 1);

    // Verify devices were loaded
    QCOMPARE(viewModel->rowCount(), 2);

    // Verify deviceCountChanged signal
    QCOMPARE(deviceCountSpy.count(), 1);
    QCOMPARE(deviceCountSpy.takeFirst().at(0).toInt(), 2);
}

void DeviceTableViewModelTest::testLoadDevices_NullRepository() {
    // Create view model with null repository
    DeviceTableViewModel* nullRepoViewModel = new DeviceTableViewModel(nullptr);

    QSignalSpy modelResetSpy(nullRepoViewModel, &QAbstractItemModel::modelReset);

    // Load devices (should handle null repository gracefully)
    nullRepoViewModel->loadDevices();

    // Model should reset but remain empty
    QCOMPARE(modelResetSpy.count(), 1);
    QCOMPARE(nullRepoViewModel->rowCount(), 0);

    delete nullRepoViewModel;
}

// ============================================================================
// Device Lookup Tests
// ============================================================================

void DeviceTableViewModelTest::testGetDeviceAt() {
    Device device = createTestDevice("192.168.1.100", "device1");
    viewModel->addDevice(device);

    Device retrievedDevice = viewModel->getDeviceAt(0);

    QCOMPARE(retrievedDevice.getIp(), device.getIp());
    QCOMPARE(retrievedDevice.getHostname(), device.getHostname());
}

void DeviceTableViewModelTest::testGetDeviceAt_InvalidRow() {
    viewModel->addDevice(createTestDevice("192.168.1.100", "device1"));

    // Negative row
    Device device = viewModel->getDeviceAt(-1);
    QVERIFY(device.getIp().isEmpty());

    // Row out of bounds
    device = viewModel->getDeviceAt(999);
    QVERIFY(device.getIp().isEmpty());
}

void DeviceTableViewModelTest::testFindDeviceRow() {
    viewModel->addDevice(createTestDevice("192.168.1.100", "device1"));
    viewModel->addDevice(createTestDevice("192.168.1.101", "device2"));
    viewModel->addDevice(createTestDevice("192.168.1.102", "device3"));

    QCOMPARE(viewModel->findDeviceRow("192.168.1.100"), 0);
    QCOMPARE(viewModel->findDeviceRow("192.168.1.101"), 1);
    QCOMPARE(viewModel->findDeviceRow("192.168.1.102"), 2);
}

void DeviceTableViewModelTest::testFindDeviceRow_NotFound() {
    viewModel->addDevice(createTestDevice("192.168.1.100", "device1"));

    int row = viewModel->findDeviceRow("192.168.1.999");
    QCOMPARE(row, -1);
}

// ============================================================================
// Signal Emission Tests
// ============================================================================

void DeviceTableViewModelTest::testSignal_DeviceCountChanged() {
    QSignalSpy deviceCountSpy(viewModel, &DeviceTableViewModel::deviceCountChanged);

    // Add device
    viewModel->addDevice(createTestDevice("192.168.1.100", "device1"));
    QCOMPARE(deviceCountSpy.count(), 1);
    QCOMPARE(deviceCountSpy.takeFirst().at(0).toInt(), 1);

    // Add another device
    viewModel->addDevice(createTestDevice("192.168.1.101", "device2"));
    QCOMPARE(deviceCountSpy.count(), 1);
    QCOMPARE(deviceCountSpy.takeFirst().at(0).toInt(), 2);

    // Remove device
    viewModel->removeDevice("192.168.1.100");
    QCOMPARE(deviceCountSpy.count(), 1);
    QCOMPARE(deviceCountSpy.takeFirst().at(0).toInt(), 1);

    // Clear all
    viewModel->clear();
    QCOMPARE(deviceCountSpy.count(), 1);
    QCOMPARE(deviceCountSpy.takeFirst().at(0).toInt(), 0);
}

// ============================================================================
// Helper Methods
// ============================================================================

Device DeviceTableViewModelTest::createTestDevice(const QString& ip, const QString& hostname, bool online) {
    Device device;
    device.setIp(ip);
    device.setHostname(hostname);
    device.setMacAddress("00:11:22:33:44:55");
    device.setVendor("Test Vendor");
    device.setOnline(online);
    device.setLastSeen(QDateTime::currentDateTime());
    return device;
}

QTEST_MAIN(DeviceTableViewModelTest)
#include "DeviceTableViewModelTest.moc"
