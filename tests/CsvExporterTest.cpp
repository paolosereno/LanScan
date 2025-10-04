#include <QtTest>
#include "export/CsvExporter.h"
#include "models/Device.h"
#include <QFile>
#include <QTextStream>
#include <QTemporaryFile>

class CsvExporterTest : public QObject {
    Q_OBJECT

private slots:
    void testExportData();
    void testExportEmptyList();
    void testCsvFormat();
    void testFieldEscaping();

private:
    QList<Device> createTestDevices();
};

QList<Device> CsvExporterTest::createTestDevices() {
    QList<Device> devices;

    Device device1;
    device1.setIp("192.168.1.1");
    device1.setHostname("router.local");
    device1.setMacAddress("AA:BB:CC:DD:EE:FF");
    device1.setVendor("Cisco");
    device1.setOnline(true);
    device1.setLastSeen(QDateTime::fromString("2024-01-15 10:30:00", "yyyy-MM-dd HH:mm:ss"));

    PortInfo port1;
    port1.setPortNumber(80);
    port1.setProtocol(PortInfo::TCP);
    port1.setService("HTTP");
    port1.setState(PortInfo::Open);
    device1.setOpenPorts({port1});

    NetworkMetrics metrics1;
    metrics1.setLatencyAvg(5.2);
    metrics1.setPacketLoss(0.0);
    metrics1.setJitter(1.2);
    metrics1.setQualityScore(NetworkMetrics::Excellent);
    device1.setMetrics(metrics1);

    devices.append(device1);

    Device device2;
    device2.setIp("192.168.1.10");
    device2.setHostname("nas.local");
    device2.setMacAddress("11:22:33:44:55:66");
    device2.setVendor("Synology");
    device2.setOnline(true);

    NetworkMetrics metrics2;
    metrics2.setLatencyAvg(12.5);
    metrics2.setPacketLoss(0.5);
    metrics2.setJitter(2.3);
    metrics2.setQualityScore(NetworkMetrics::Good);
    device2.setMetrics(metrics2);

    devices.append(device2);

    return devices;
}

void CsvExporterTest::testExportData() {
    CsvExporter exporter;
    QList<Device> devices = createTestDevices();

    QTemporaryFile tempFile;
    QVERIFY(tempFile.open());
    QString filepath = tempFile.fileName();
    tempFile.close();

    QVERIFY(exporter.exportData(devices, filepath));

    // Verify file exists and has content
    QFile file(filepath);
    QVERIFY(file.exists());
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));

    QTextStream in(&file);
    QString content = in.readAll();
    file.close();

    QVERIFY(!content.isEmpty());
    QVERIFY(content.contains("IP,Hostname,MAC Address"));
    QVERIFY(content.contains("192.168.1.1"));
    QVERIFY(content.contains("router.local"));
}

void CsvExporterTest::testExportEmptyList() {
    CsvExporter exporter;
    QList<Device> devices;

    QTemporaryFile tempFile;
    QVERIFY(tempFile.open());
    QString filepath = tempFile.fileName();
    tempFile.close();

    QVERIFY(exporter.exportData(devices, filepath));

    QFile file(filepath);
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));

    QTextStream in(&file);
    QString content = in.readAll();
    file.close();

    // Should only have header
    QStringList lines = content.split('\n', Qt::SkipEmptyParts);
    QCOMPARE(lines.size(), 1);
    QVERIFY(lines[0].contains("IP,Hostname"));
}

void CsvExporterTest::testCsvFormat() {
    CsvExporter exporter;
    QList<Device> devices = createTestDevices();

    QTemporaryFile tempFile;
    QVERIFY(tempFile.open());
    QString filepath = tempFile.fileName();
    tempFile.close();

    QVERIFY(exporter.exportData(devices, filepath));

    QFile file(filepath);
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));

    QTextStream in(&file);
    QStringList lines;
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (!line.isEmpty()) {
            lines.append(line);
        }
    }
    file.close();

    // Header + 2 data rows
    QVERIFY(lines.size() >= 2);

    // Check header format
    QVERIFY(lines[0].contains("IP"));
    QVERIFY(lines[0].contains("Hostname"));
    QVERIFY(lines[0].contains("MAC Address"));
}

void CsvExporterTest::testFieldEscaping() {
    CsvExporter exporter;

    Device device;
    device.setIp("192.168.1.1");
    device.setHostname("Device, with comma");
    device.setMacAddress("AA:BB:CC:DD:EE:FF");

    QList<Device> devices = {device};

    QTemporaryFile tempFile;
    QVERIFY(tempFile.open());
    QString filepath = tempFile.fileName();
    tempFile.close();

    QVERIFY(exporter.exportData(devices, filepath));

    QFile file(filepath);
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));

    QTextStream in(&file);
    QString content = in.readAll();
    file.close();

    // Hostname with comma should be quoted
    QVERIFY(content.contains("\"Device, with comma\""));
}

QTEST_MAIN(CsvExporterTest)
#include "CsvExporterTest.moc"
