#include <QtTest>
#include <QSignalSpy>
#include "viewmodels/ScanConfigViewModel.h"
#include "coordinators/ScanCoordinator.h"

/**
 * @brief Unit tests for ScanConfigViewModel
 *
 * Tests cover:
 * - Initialization with default values
 * - Getters and setters for all properties
 * - Subnet validation
 * - Preset loading (Quick, Deep, Custom)
 * - Signal emissions
 * - Conversion to ScanConfig
 * - Network detection
 */
class ScanConfigViewModelTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Construction tests
    void testViewModelInitialization();

    // Getters and setters tests
    void testSetGetSubnet();
    void testSetGetScanType();
    void testSetGetThreadCount();
    void testSetGetTimeout();
    void testSetGetResolveDns();
    void testSetGetResolveArp();
    void testSetGetScanPorts();
    void testSetGetPortsToScan();

    // Validation tests
    void testSubnetValidation_Valid();
    void testSubnetValidation_Invalid();
    void testSubnetValidation_Empty();
    void testIsValid();
    void testGetSubnetError();

    // Preset tests
    void testLoadQuickScanPreset();
    void testLoadDeepScanPreset();
    void testLoadCustomScanPreset();
    void testLoadPreset();

    // Signal emission tests
    void testSignal_SubnetChanged();
    void testSignal_ScanTypeChanged();
    void testSignal_ValidationChanged();

    // Conversion tests
    void testToScanConfig();

    // Network detection tests
    void testDetectLocalNetworks();

private:
    ScanConfigViewModel* viewModel;
};

void ScanConfigViewModelTest::initTestCase() {
    // Nothing to initialize
}

void ScanConfigViewModelTest::cleanupTestCase() {
    // Nothing to cleanup
}

void ScanConfigViewModelTest::init() {
    // Create fresh instance for each test
    viewModel = new ScanConfigViewModel();
}

void ScanConfigViewModelTest::cleanup() {
    // Clean up after each test
    delete viewModel;
    viewModel = nullptr;
}

// ============================================================================
// Construction Tests
// ============================================================================

void ScanConfigViewModelTest::testViewModelInitialization() {
    QVERIFY(viewModel != nullptr);

    // Verify default values (Quick scan preset)
    QCOMPARE(viewModel->getScanType(), ScanConfigViewModel::Quick);
    QVERIFY(viewModel->isResolveDns());
    QVERIFY(!viewModel->isResolveArp());
    QVERIFY(!viewModel->isScanPorts());
    QCOMPARE(viewModel->getTimeout(), 1000);
    QVERIFY(viewModel->getThreadCount() > 0); // Should use ideal thread count
}

// ============================================================================
// Getters and Setters Tests
// ============================================================================

void ScanConfigViewModelTest::testSetGetSubnet() {
    QString subnet = "192.168.1.0/24";
    viewModel->setSubnet(subnet);

    QCOMPARE(viewModel->getSubnet(), subnet);
}

void ScanConfigViewModelTest::testSetGetScanType() {
    viewModel->setScanType(ScanConfigViewModel::Deep);
    QCOMPARE(viewModel->getScanType(), ScanConfigViewModel::Deep);

    viewModel->setScanType(ScanConfigViewModel::Custom);
    QCOMPARE(viewModel->getScanType(), ScanConfigViewModel::Custom);

    viewModel->setScanType(ScanConfigViewModel::Quick);
    QCOMPARE(viewModel->getScanType(), ScanConfigViewModel::Quick);
}

void ScanConfigViewModelTest::testSetGetThreadCount() {
    viewModel->setThreadCount(8);
    QCOMPARE(viewModel->getThreadCount(), 8);

    viewModel->setThreadCount(16);
    QCOMPARE(viewModel->getThreadCount(), 16);
}

void ScanConfigViewModelTest::testSetGetTimeout() {
    viewModel->setTimeout(2000);
    QCOMPARE(viewModel->getTimeout(), 2000);

    viewModel->setTimeout(5000);
    QCOMPARE(viewModel->getTimeout(), 5000);
}

void ScanConfigViewModelTest::testSetGetResolveDns() {
    viewModel->setResolveDns(true);
    QVERIFY(viewModel->isResolveDns());

    viewModel->setResolveDns(false);
    QVERIFY(!viewModel->isResolveDns());
}

void ScanConfigViewModelTest::testSetGetResolveArp() {
    viewModel->setResolveArp(true);
    QVERIFY(viewModel->isResolveArp());

    viewModel->setResolveArp(false);
    QVERIFY(!viewModel->isResolveArp());
}

void ScanConfigViewModelTest::testSetGetScanPorts() {
    viewModel->setScanPorts(true);
    QVERIFY(viewModel->isScanPorts());

    viewModel->setScanPorts(false);
    QVERIFY(!viewModel->isScanPorts());
}

void ScanConfigViewModelTest::testSetGetPortsToScan() {
    QList<int> ports = {22, 80, 443, 8080};
    viewModel->setPortsToScan(ports);

    QCOMPARE(viewModel->getPortsToScan(), ports);
}

// ============================================================================
// Validation Tests
// ============================================================================

void ScanConfigViewModelTest::testSubnetValidation_Valid() {
    // Test valid CIDR notations
    viewModel->setSubnet("192.168.1.0/24");
    QVERIFY(viewModel->isSubnetValid());

    viewModel->setSubnet("10.0.0.0/8");
    QVERIFY(viewModel->isSubnetValid());

    viewModel->setSubnet("172.16.0.0/12");
    QVERIFY(viewModel->isSubnetValid());

    viewModel->setSubnet("192.168.100.0/28");
    QVERIFY(viewModel->isSubnetValid());
}

void ScanConfigViewModelTest::testSubnetValidation_Invalid() {
    // Test invalid CIDR notations
    viewModel->setSubnet("192.168.1.0");
    QVERIFY(!viewModel->isSubnetValid());

    viewModel->setSubnet("192.168.1.0/");
    QVERIFY(!viewModel->isSubnetValid());

    viewModel->setSubnet("192.168.1.0/33");
    QVERIFY(!viewModel->isSubnetValid());

    viewModel->setSubnet("invalid");
    QVERIFY(!viewModel->isSubnetValid());
}

void ScanConfigViewModelTest::testSubnetValidation_Empty() {
    viewModel->setSubnet("");
    QVERIFY(!viewModel->isSubnetValid());
}

void ScanConfigViewModelTest::testIsValid() {
    // Invalid: empty subnet
    viewModel->setSubnet("");
    QVERIFY(!viewModel->isValid());

    // Invalid: invalid CIDR
    viewModel->setSubnet("192.168.1.0");
    QVERIFY(!viewModel->isValid());

    // Valid: proper CIDR with valid settings
    viewModel->setSubnet("192.168.1.0/24");
    viewModel->setThreadCount(4);
    viewModel->setTimeout(1000);
    QVERIFY(viewModel->isValid());

    // Invalid: zero thread count
    viewModel->setThreadCount(0);
    QVERIFY(!viewModel->isValid());

    // Invalid: zero timeout
    viewModel->setThreadCount(4);
    viewModel->setTimeout(0);
    QVERIFY(!viewModel->isValid());
}

void ScanConfigViewModelTest::testGetSubnetError() {
    // Empty subnet
    viewModel->setSubnet("");
    QString error = viewModel->getSubnetError();
    QVERIFY(!error.isEmpty());
    QVERIFY(error.contains("required") || error.contains("Required"));

    // Invalid CIDR
    viewModel->setSubnet("192.168.1.0");
    error = viewModel->getSubnetError();
    QVERIFY(!error.isEmpty());
    QVERIFY(error.contains("CIDR") || error.contains("notation"));

    // Valid subnet
    viewModel->setSubnet("192.168.1.0/24");
    error = viewModel->getSubnetError();
    QVERIFY(error.isEmpty());
}

// ============================================================================
// Preset Tests
// ============================================================================

void ScanConfigViewModelTest::testLoadQuickScanPreset() {
    viewModel->loadQuickScanPreset();

    QCOMPARE(viewModel->getScanType(), ScanConfigViewModel::Quick);
    QVERIFY(viewModel->isResolveDns());
    QVERIFY(!viewModel->isResolveArp());
    QVERIFY(!viewModel->isScanPorts());
    QCOMPARE(viewModel->getTimeout(), 1000);
    QVERIFY(viewModel->getPortsToScan().isEmpty());
}

void ScanConfigViewModelTest::testLoadDeepScanPreset() {
    viewModel->loadDeepScanPreset();

    QCOMPARE(viewModel->getScanType(), ScanConfigViewModel::Deep);
    QVERIFY(viewModel->isResolveDns());
    QVERIFY(viewModel->isResolveArp());
    QVERIFY(viewModel->isScanPorts());
    QCOMPARE(viewModel->getTimeout(), 3000);
    QVERIFY(!viewModel->getPortsToScan().isEmpty());

    // Verify common ports are included
    QList<int> ports = viewModel->getPortsToScan();
    QVERIFY(ports.contains(22));   // SSH
    QVERIFY(ports.contains(80));   // HTTP
    QVERIFY(ports.contains(443));  // HTTPS
}

void ScanConfigViewModelTest::testLoadCustomScanPreset() {
    // Set some custom values
    viewModel->setResolveDns(false);
    viewModel->setTimeout(2500);
    viewModel->setPortsToScan({8080, 9000});

    // Load custom preset (should keep current settings)
    viewModel->loadCustomScanPreset();

    QCOMPARE(viewModel->getScanType(), ScanConfigViewModel::Custom);
    QVERIFY(!viewModel->isResolveDns()); // Should keep custom value
    QCOMPARE(viewModel->getTimeout(), 2500); // Should keep custom value
}

void ScanConfigViewModelTest::testLoadPreset() {
    // Test loading presets via loadPreset method
    viewModel->loadPreset(ScanConfigViewModel::Quick);
    QCOMPARE(viewModel->getScanType(), ScanConfigViewModel::Quick);
    QCOMPARE(viewModel->getTimeout(), 1000);

    viewModel->loadPreset(ScanConfigViewModel::Deep);
    QCOMPARE(viewModel->getScanType(), ScanConfigViewModel::Deep);
    QCOMPARE(viewModel->getTimeout(), 3000);

    viewModel->loadPreset(ScanConfigViewModel::Custom);
    QCOMPARE(viewModel->getScanType(), ScanConfigViewModel::Custom);
}

// ============================================================================
// Signal Emission Tests
// ============================================================================

void ScanConfigViewModelTest::testSignal_SubnetChanged() {
    QSignalSpy subnetChangedSpy(viewModel, &ScanConfigViewModel::subnetChanged);

    QString subnet = "192.168.1.0/24";
    viewModel->setSubnet(subnet);

    QCOMPARE(subnetChangedSpy.count(), 1);
    QCOMPARE(subnetChangedSpy.takeFirst().at(0).toString(), subnet);

    // Setting same subnet should not emit signal
    viewModel->setSubnet(subnet);
    QCOMPARE(subnetChangedSpy.count(), 0);
}

void ScanConfigViewModelTest::testSignal_ScanTypeChanged() {
    QSignalSpy scanTypeChangedSpy(viewModel, &ScanConfigViewModel::scanTypeChanged);

    viewModel->setScanType(ScanConfigViewModel::Deep);

    QCOMPARE(scanTypeChangedSpy.count(), 1);
    QList<QVariant> arguments = scanTypeChangedSpy.takeFirst();
    QCOMPARE(arguments.at(0).value<ScanConfigViewModel::ScanType>(), ScanConfigViewModel::Deep);

    // Setting same type should not emit signal
    viewModel->setScanType(ScanConfigViewModel::Deep);
    QCOMPARE(scanTypeChangedSpy.count(), 0);
}

void ScanConfigViewModelTest::testSignal_ValidationChanged() {
    QSignalSpy validationChangedSpy(viewModel, &ScanConfigViewModel::validationChanged);

    // Set invalid subnet
    viewModel->setSubnet("invalid");
    QVERIFY(validationChangedSpy.count() > 0);

    validationChangedSpy.clear();

    // Set valid subnet
    viewModel->setSubnet("192.168.1.0/24");
    QVERIFY(validationChangedSpy.count() > 0);
}

// ============================================================================
// Conversion Tests
// ============================================================================

void ScanConfigViewModelTest::testToScanConfig() {
    // Configure view model
    viewModel->setSubnet("192.168.1.0/24");
    viewModel->setResolveDns(true);
    viewModel->setResolveArp(false);
    viewModel->setScanPorts(true);
    viewModel->setPortsToScan({22, 80, 443});
    viewModel->setTimeout(2000);
    viewModel->setThreadCount(8);

    // Convert to ScanConfig
    ScanCoordinator::ScanConfig config = viewModel->toScanConfig();

    // Verify conversion
    QCOMPARE(config.subnet, QString("192.168.1.0/24"));
    QVERIFY(config.resolveDns);
    QVERIFY(!config.resolveArp);
    QVERIFY(config.scanPorts);
    QCOMPARE(config.portsToScan.size(), 3);
    QVERIFY(config.portsToScan.contains(22));
    QVERIFY(config.portsToScan.contains(80));
    QVERIFY(config.portsToScan.contains(443));
    QCOMPARE(config.timeout, 2000);
    QCOMPARE(config.maxThreads, 8);
}

// ============================================================================
// Network Detection Tests
// ============================================================================

void ScanConfigViewModelTest::testDetectLocalNetworks() {
    QStringList networks = viewModel->detectLocalNetworks();

    // Should return at least some networks (placeholder implementation)
    QVERIFY(!networks.isEmpty());

    // Verify format (should be CIDR notation)
    for (const QString& network : networks) {
        QVERIFY(network.contains("/"));
    }
}

QTEST_MAIN(ScanConfigViewModelTest)
#include "ScanConfigViewModelTest.moc"
