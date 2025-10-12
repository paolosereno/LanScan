#include <QtTest>
#include <QSignalSpy>
#include <QTemporaryFile>
#include <QFile>
#include "controllers/ExportController.h"
#include "database/DeviceRepository.h"
#include "models/Device.h"
#include "database/DatabaseManager.h"

/**
 * @brief Unit tests for ExportController
 *
 * Tests cover:
 * - Export all devices from repository
 * - Export single device
 * - Export filtered devices
 * - All export formats (CSV, JSON, XML, HTML)
 * - Format conversion (string to enum and vice versa)
 * - Error handling (empty data, invalid paths)
 * - Signal emissions
 */
class ExportControllerTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Construction tests
    void testControllerInitialization();

    // Format conversion tests
    void testFormatToString();
    void testStringToFormat();
    void testGetSupportedFormats();

    // Export all devices tests
    void testExportDevices_CSV();
    void testExportDevices_JSON();
    void testExportDevices_XML();
    void testExportDevices_HTML();

    // Export single device test
    void testExportDevice();

    // Export filtered devices test
    void testExportFiltered();

    // Error handling tests
    void testExportDevices_EmptyData();
    void testExportFiltered_EmptyData();

    // Signal emission tests
    void testSignal_ExportStarted();
    void testSignal_ExportProgress();
    void testSignal_ExportCompleted();
    void testSignal_ExportError();

private:
    ExportController* controller;
    DeviceRepository* repository;
    DatabaseManager* dbManager;

    Device createTestDevice(const QString& ip, const QString& hostname, const QString& mac);
    QList<Device> createTestDevices(int count);
    QString readFileContent(const QString& filepath);
};

void ExportControllerTest::initTestCase() {
    // Initialize database manager with in-memory database
    dbManager = DatabaseManager::instance();
    dbManager->open(":memory:");
}

void ExportControllerTest::cleanupTestCase() {
    if (dbManager) {
        dbManager->close();
    }
}

void ExportControllerTest::init() {
    // Create fresh instances for each test
    repository = new DeviceRepository(dbManager);
    controller = new ExportController(repository);
}

void ExportControllerTest::cleanup() {
    // Clean up after each test
    delete controller;
    delete repository;

    controller = nullptr;
    repository = nullptr;
}

// ============================================================================
// Construction Tests
// ============================================================================

void ExportControllerTest::testControllerInitialization() {
    QVERIFY(controller != nullptr);
}

// ============================================================================
// Format Conversion Tests
// ============================================================================

void ExportControllerTest::testFormatToString() {
    QCOMPARE(ExportController::formatToString(ExportController::CSV), QString("CSV"));
    QCOMPARE(ExportController::formatToString(ExportController::JSON), QString("JSON"));
    QCOMPARE(ExportController::formatToString(ExportController::XML), QString("XML"));
    QCOMPARE(ExportController::formatToString(ExportController::HTML), QString("HTML"));
}

void ExportControllerTest::testStringToFormat() {
    QCOMPARE(ExportController::stringToFormat("CSV"), ExportController::CSV);
    QCOMPARE(ExportController::stringToFormat("csv"), ExportController::CSV);
    QCOMPARE(ExportController::stringToFormat("JSON"), ExportController::JSON);
    QCOMPARE(ExportController::stringToFormat("json"), ExportController::JSON);
    QCOMPARE(ExportController::stringToFormat("XML"), ExportController::XML);
    QCOMPARE(ExportController::stringToFormat("xml"), ExportController::XML);
    QCOMPARE(ExportController::stringToFormat("HTML"), ExportController::HTML);
    QCOMPARE(ExportController::stringToFormat("html"), ExportController::HTML);

    // Unknown format should default to CSV
    QCOMPARE(ExportController::stringToFormat("UNKNOWN"), ExportController::CSV);
}

void ExportControllerTest::testGetSupportedFormats() {
    QStringList formats = ExportController::getSupportedFormats();

    QCOMPARE(formats.size(), 4);
    QVERIFY(formats.contains("CSV"));
    QVERIFY(formats.contains("JSON"));
    QVERIFY(formats.contains("XML"));
    QVERIFY(formats.contains("HTML"));
}

// ============================================================================
// Export All Devices Tests
// ============================================================================

void ExportControllerTest::testExportDevices_CSV() {
    // Add test devices to repository
    QList<Device> testDevices = createTestDevices(3);
    for (const Device& device : testDevices) {
        repository->save(device);
    }

    QSignalSpy exportCompletedSpy(controller, &ExportController::exportCompleted);

    // Create temporary file
    QTemporaryFile tempFile;
    tempFile.setAutoRemove(false);
    QVERIFY(tempFile.open());
    QString filepath = tempFile.fileName();
    tempFile.close();

    // Export to CSV
    controller->exportDevices(ExportController::CSV, filepath);

    // Wait for export completion
    QVERIFY(exportCompletedSpy.wait(2000));

    // Verify file exists
    QVERIFY(QFile::exists(filepath));

    // Verify file has content
    QString content = readFileContent(filepath);
    QVERIFY(!content.isEmpty());
    QVERIFY(content.contains("IP Address")); // CSV header

    // Cleanup
    QFile::remove(filepath);
}

void ExportControllerTest::testExportDevices_JSON() {
    // Add test devices to repository
    QList<Device> testDevices = createTestDevices(2);
    for (const Device& device : testDevices) {
        repository->save(device);
    }

    QSignalSpy exportCompletedSpy(controller, &ExportController::exportCompleted);

    // Create temporary file
    QTemporaryFile tempFile;
    tempFile.setAutoRemove(false);
    QVERIFY(tempFile.open());
    QString filepath = tempFile.fileName();
    tempFile.close();

    // Export to JSON
    controller->exportDevices(ExportController::JSON, filepath);

    // Wait for export completion
    QVERIFY(exportCompletedSpy.wait(2000));

    // Verify file exists
    QVERIFY(QFile::exists(filepath));

    // Verify file has JSON content
    QString content = readFileContent(filepath);
    QVERIFY(!content.isEmpty());
    QVERIFY(content.contains("{")); // JSON structure
    QVERIFY(content.contains("devices"));

    // Cleanup
    QFile::remove(filepath);
}

void ExportControllerTest::testExportDevices_XML() {
    // Add test devices to repository
    QList<Device> testDevices = createTestDevices(2);
    for (const Device& device : testDevices) {
        repository->save(device);
    }

    QSignalSpy exportCompletedSpy(controller, &ExportController::exportCompleted);

    // Create temporary file
    QTemporaryFile tempFile;
    tempFile.setAutoRemove(false);
    QVERIFY(tempFile.open());
    QString filepath = tempFile.fileName();
    tempFile.close();

    // Export to XML
    controller->exportDevices(ExportController::XML, filepath);

    // Wait for export completion
    QVERIFY(exportCompletedSpy.wait(2000));

    // Verify file exists
    QVERIFY(QFile::exists(filepath));

    // Verify file has XML content
    QString content = readFileContent(filepath);
    QVERIFY(!content.isEmpty());
    QVERIFY(content.contains("<?xml")); // XML declaration
    QVERIFY(content.contains("<devices>"));

    // Cleanup
    QFile::remove(filepath);
}

void ExportControllerTest::testExportDevices_HTML() {
    // Add test devices to repository
    QList<Device> testDevices = createTestDevices(2);
    for (const Device& device : testDevices) {
        repository->save(device);
    }

    QSignalSpy exportCompletedSpy(controller, &ExportController::exportCompleted);

    // Create temporary file
    QTemporaryFile tempFile;
    tempFile.setAutoRemove(false);
    QVERIFY(tempFile.open());
    QString filepath = tempFile.fileName();
    tempFile.close();

    // Export to HTML
    controller->exportDevices(ExportController::HTML, filepath);

    // Wait for export completion
    QVERIFY(exportCompletedSpy.wait(2000));

    // Verify file exists
    QVERIFY(QFile::exists(filepath));

    // Verify file has HTML content
    QString content = readFileContent(filepath);
    QVERIFY(!content.isEmpty());
    QVERIFY(content.contains("<!DOCTYPE html>")); // HTML declaration
    QVERIFY(content.contains("<html>"));

    // Cleanup
    QFile::remove(filepath);
}

// ============================================================================
// Export Single Device Test
// ============================================================================

void ExportControllerTest::testExportDevice() {
    QSignalSpy exportCompletedSpy(controller, &ExportController::exportCompleted);

    // Create a single test device
    Device device = createTestDevice("192.168.1.100", "test-device", "AA:BB:CC:DD:EE:FF");

    // Create temporary file
    QTemporaryFile tempFile;
    tempFile.setAutoRemove(false);
    QVERIFY(tempFile.open());
    QString filepath = tempFile.fileName();
    tempFile.close();

    // Export single device to CSV
    controller->exportDevice(device, ExportController::CSV, filepath);

    // Wait for export completion
    QVERIFY(exportCompletedSpy.wait(2000));

    // Verify file exists
    QVERIFY(QFile::exists(filepath));

    // Verify file has content with device data
    QString content = readFileContent(filepath);
    QVERIFY(!content.isEmpty());
    QVERIFY(content.contains("192.168.1.100"));
    QVERIFY(content.contains("test-device"));

    // Cleanup
    QFile::remove(filepath);
}

// ============================================================================
// Export Filtered Devices Test
// ============================================================================

void ExportControllerTest::testExportFiltered() {
    QSignalSpy exportCompletedSpy(controller, &ExportController::exportCompleted);

    // Create filtered list of devices (not in repository)
    QList<Device> filteredDevices;
    filteredDevices.append(createTestDevice("192.168.1.100", "device1", "AA:BB:CC:DD:EE:FF"));
    filteredDevices.append(createTestDevice("192.168.1.101", "device2", "11:22:33:44:55:66"));

    // Create temporary file
    QTemporaryFile tempFile;
    tempFile.setAutoRemove(false);
    QVERIFY(tempFile.open());
    QString filepath = tempFile.fileName();
    tempFile.close();

    // Export filtered devices to JSON
    controller->exportFiltered(filteredDevices, ExportController::JSON, filepath);

    // Wait for export completion
    QVERIFY(exportCompletedSpy.wait(2000));

    // Verify file exists
    QVERIFY(QFile::exists(filepath));

    // Verify file has both devices
    QString content = readFileContent(filepath);
    QVERIFY(content.contains("192.168.1.100"));
    QVERIFY(content.contains("192.168.1.101"));
    QVERIFY(content.contains("device1"));
    QVERIFY(content.contains("device2"));

    // Cleanup
    QFile::remove(filepath);
}

// ============================================================================
// Error Handling Tests
// ============================================================================

void ExportControllerTest::testExportDevices_EmptyData() {
    QSignalSpy exportErrorSpy(controller, &ExportController::exportError);

    // Repository is empty, no devices to export

    // Create temporary file
    QTemporaryFile tempFile;
    tempFile.setAutoRemove(false);
    QVERIFY(tempFile.open());
    QString filepath = tempFile.fileName();
    tempFile.close();

    // Try to export (should fail with error)
    controller->exportDevices(ExportController::CSV, filepath);

    // Wait for error signal
    QVERIFY(exportErrorSpy.wait(2000));

    // Verify error signal was emitted
    QCOMPARE(exportErrorSpy.count(), 1);
    QString errorMessage = exportErrorSpy.takeFirst().at(0).toString();
    QVERIFY(errorMessage.contains("No devices to export"));

    // Cleanup
    QFile::remove(filepath);
}

void ExportControllerTest::testExportFiltered_EmptyData() {
    QSignalSpy exportErrorSpy(controller, &ExportController::exportError);

    // Empty device list
    QList<Device> emptyDevices;

    // Create temporary file
    QTemporaryFile tempFile;
    tempFile.setAutoRemove(false);
    QVERIFY(tempFile.open());
    QString filepath = tempFile.fileName();
    tempFile.close();

    // Try to export (should fail with error)
    controller->exportFiltered(emptyDevices, ExportController::CSV, filepath);

    // Wait for error signal
    QVERIFY(exportErrorSpy.wait(2000));

    // Verify error signal was emitted
    QCOMPARE(exportErrorSpy.count(), 1);
    QString errorMessage = exportErrorSpy.takeFirst().at(0).toString();
    QVERIFY(errorMessage.contains("No devices to export"));

    // Cleanup
    QFile::remove(filepath);
}

// ============================================================================
// Signal Emission Tests
// ============================================================================

void ExportControllerTest::testSignal_ExportStarted() {
    // Add test devices to repository
    QList<Device> testDevices = createTestDevices(2);
    for (const Device& device : testDevices) {
        repository->save(device);
    }

    QSignalSpy exportStartedSpy(controller, &ExportController::exportStarted);

    // Create temporary file
    QTemporaryFile tempFile;
    tempFile.setAutoRemove(false);
    QVERIFY(tempFile.open());
    QString filepath = tempFile.fileName();
    tempFile.close();

    // Export devices
    controller->exportDevices(ExportController::CSV, filepath);

    // Verify exportStarted signal was emitted
    QVERIFY(exportStartedSpy.count() > 0);

    // Cleanup
    QFile::remove(filepath);
}

void ExportControllerTest::testSignal_ExportProgress() {
    // Add test devices to repository
    QList<Device> testDevices = createTestDevices(2);
    for (const Device& device : testDevices) {
        repository->save(device);
    }

    QSignalSpy exportProgressSpy(controller, &ExportController::exportProgress);

    // Create temporary file
    QTemporaryFile tempFile;
    tempFile.setAutoRemove(false);
    QVERIFY(tempFile.open());
    QString filepath = tempFile.fileName();
    tempFile.close();

    // Export devices
    controller->exportDevices(ExportController::CSV, filepath);

    // Wait for export to complete
    QTest::qWait(1000);

    // Verify exportProgress signal was emitted
    QVERIFY(exportProgressSpy.count() > 0);

    // Verify progress values are reasonable (0-100)
    for (const QList<QVariant>& args : exportProgressSpy) {
        int progress = args.at(0).toInt();
        QVERIFY(progress >= 0 && progress <= 100);
    }

    // Cleanup
    QFile::remove(filepath);
}

void ExportControllerTest::testSignal_ExportCompleted() {
    // Add test devices to repository
    QList<Device> testDevices = createTestDevices(2);
    for (const Device& device : testDevices) {
        repository->save(device);
    }

    QSignalSpy exportCompletedSpy(controller, &ExportController::exportCompleted);

    // Create temporary file
    QTemporaryFile tempFile;
    tempFile.setAutoRemove(false);
    QVERIFY(tempFile.open());
    QString filepath = tempFile.fileName();
    tempFile.close();

    // Export devices
    controller->exportDevices(ExportController::CSV, filepath);

    // Wait for export completion
    QVERIFY(exportCompletedSpy.wait(2000));

    // Verify exportCompleted signal was emitted
    QCOMPARE(exportCompletedSpy.count(), 1);

    // Verify filepath argument
    QString completedPath = exportCompletedSpy.takeFirst().at(0).toString();
    QCOMPARE(completedPath, filepath);

    // Cleanup
    QFile::remove(filepath);
}

void ExportControllerTest::testSignal_ExportError() {
    QSignalSpy exportErrorSpy(controller, &ExportController::exportError);

    // Repository is empty, export should fail

    // Create temporary file
    QTemporaryFile tempFile;
    tempFile.setAutoRemove(false);
    QVERIFY(tempFile.open());
    QString filepath = tempFile.fileName();
    tempFile.close();

    // Try to export (should fail)
    controller->exportDevices(ExportController::CSV, filepath);

    // Wait for error signal
    QVERIFY(exportErrorSpy.wait(2000));

    // Verify exportError signal was emitted
    QCOMPARE(exportErrorSpy.count(), 1);

    // Verify error message is not empty
    QString errorMessage = exportErrorSpy.takeFirst().at(0).toString();
    QVERIFY(!errorMessage.isEmpty());

    // Cleanup
    QFile::remove(filepath);
}

// ============================================================================
// Helper Methods
// ============================================================================

Device ExportControllerTest::createTestDevice(const QString& ip, const QString& hostname, const QString& mac) {
    Device device;
    device.setIp(ip);
    device.setHostname(hostname);
    device.setMacAddress(mac);
    device.setOnline(true);
    device.setLastSeen(QDateTime::currentDateTime());
    return device;
}

QList<Device> ExportControllerTest::createTestDevices(int count) {
    QList<Device> devices;
    for (int i = 0; i < count; i++) {
        QString ip = QString("192.168.1.%1").arg(100 + i);
        QString hostname = QString("device%1").arg(i + 1);
        QString mac = QString("AA:BB:CC:DD:EE:%1").arg(i, 2, 16, QChar('0')).toUpper();
        devices.append(createTestDevice(ip, hostname, mac));
    }
    return devices;
}

QString ExportControllerTest::readFileContent(const QString& filepath) {
    QFile file(filepath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QString();
    }

    QTextStream in(&file);
    QString content = in.readAll();
    file.close();

    return content;
}

QTEST_MAIN(ExportControllerTest)
#include "ExportControllerTest.moc"
