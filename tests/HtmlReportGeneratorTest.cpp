#include <QtTest>
#include "export/HtmlReportGenerator.h"
#include "models/Device.h"
#include <QFile>
#include <QTextStream>
#include <QTemporaryFile>

class HtmlReportGeneratorTest : public QObject {
    Q_OBJECT

private slots:
    void testExportData();
    void testExportEmptyList();
    void testHtmlStructure();
    void testSummarySection();
    void testDeviceTable();
    void testQualityColors();
    void testOnlineOfflineStatus();

private:
    QList<Device> createTestDevices();
    QString readFileContent(const QString& filepath);
};

QList<Device> HtmlReportGeneratorTest::createTestDevices() {
    QList<Device> devices;

    // Online device with excellent quality
    Device device1;
    device1.setIp("192.168.1.1");
    device1.setHostname("router.local");
    device1.setMacAddress("AA:BB:CC:DD:EE:FF");
    device1.setVendor("Cisco");
    device1.setOnline(true);

    PortInfo port1;
    port1.setPortNumber(80);
    port1.setProtocol(PortInfo::TCP);
    port1.setService("HTTP");
    port1.setState(PortInfo::Open);

    PortInfo port2;
    port2.setPortNumber(443);
    port2.setProtocol(PortInfo::TCP);
    port2.setService("HTTPS");
    port2.setState(PortInfo::Open);

    device1.setOpenPorts({port1, port2});

    NetworkMetrics metrics1;
    metrics1.setLatencyAvg(5.2);
    metrics1.setPacketLoss(0.0);
    metrics1.setJitter(1.2);
    metrics1.setQualityScore(NetworkMetrics::Excellent);
    device1.setMetrics(metrics1);

    devices.append(device1);

    // Offline device
    Device device2;
    device2.setIp("192.168.1.10");
    device2.setHostname("nas.local");
    device2.setMacAddress("11:22:33:44:55:66");
    device2.setVendor("Synology");
    device2.setOnline(false);

    NetworkMetrics metrics2;
    metrics2.setLatencyAvg(0.0);
    metrics2.setPacketLoss(100.0);
    metrics2.setJitter(0.0);
    metrics2.setQualityScore(NetworkMetrics::Bad);
    device2.setMetrics(metrics2);

    devices.append(device2);

    // Online device with poor quality
    Device device3;
    device3.setIp("192.168.1.20");
    device3.setHostname("iot.local");
    device3.setOnline(true);

    NetworkMetrics metrics3;
    metrics3.setLatencyAvg(150.0);
    metrics3.setPacketLoss(10.0);
    metrics3.setJitter(50.0);
    metrics3.setQualityScore(NetworkMetrics::Poor);
    device3.setMetrics(metrics3);

    devices.append(device3);

    return devices;
}

QString HtmlReportGeneratorTest::readFileContent(const QString& filepath) {
    QFile file(filepath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QString();
    }

    QTextStream in(&file);
    QString content = in.readAll();
    file.close();

    return content;
}

void HtmlReportGeneratorTest::testExportData() {
    HtmlReportGenerator generator;
    QList<Device> devices = createTestDevices();

    QTemporaryFile tempFile;
    QVERIFY(tempFile.open());
    QString filepath = tempFile.fileName();
    tempFile.close();

    QVERIFY(generator.exportData(devices, filepath));

    // Verify file exists and has content
    QFile file(filepath);
    QVERIFY(file.exists());
    QVERIFY(file.size() > 0);

    QString content = readFileContent(filepath);
    QVERIFY(!content.isEmpty());
}

void HtmlReportGeneratorTest::testExportEmptyList() {
    HtmlReportGenerator generator;
    QList<Device> devices;

    QTemporaryFile tempFile;
    QVERIFY(tempFile.open());
    QString filepath = tempFile.fileName();
    tempFile.close();

    QVERIFY(generator.exportData(devices, filepath));

    QString content = readFileContent(filepath);
    QVERIFY(content.contains("Total Devices"));
    QVERIFY(content.contains("<div class=\"value\">0</div>"));
}

void HtmlReportGeneratorTest::testHtmlStructure() {
    HtmlReportGenerator generator;
    QList<Device> devices = createTestDevices();

    QTemporaryFile tempFile;
    QVERIFY(tempFile.open());
    QString filepath = tempFile.fileName();
    tempFile.close();

    QVERIFY(generator.exportData(devices, filepath));

    QString content = readFileContent(filepath);

    // Check HTML structure
    QVERIFY(content.contains("<!DOCTYPE html>"));
    QVERIFY(content.contains("<html"));
    QVERIFY(content.contains("<head>"));
    QVERIFY(content.contains("<body>"));
    QVERIFY(content.contains("</html>"));

    // Check title
    QVERIFY(content.contains("<title>LanScan Network Report</title>"));

    // Check CSS presence
    QVERIFY(content.contains("<style>"));
    QVERIFY(content.contains("font-family"));

    // Check main content
    QVERIFY(content.contains("LanScan Network Report"));
}

void HtmlReportGeneratorTest::testSummarySection() {
    HtmlReportGenerator generator;
    QList<Device> devices = createTestDevices();

    QTemporaryFile tempFile;
    QVERIFY(tempFile.open());
    QString filepath = tempFile.fileName();
    tempFile.close();

    QVERIFY(generator.exportData(devices, filepath));

    QString content = readFileContent(filepath);

    // Check summary cards
    QVERIFY(content.contains("Total Devices"));
    QVERIFY(content.contains("Online Devices"));
    QVERIFY(content.contains("Offline Devices"));

    // Check values (3 total, 2 online, 1 offline)
    QVERIFY(content.contains("<div class=\"value\">3</div>"));
    QVERIFY(content.contains("<div class=\"value\">2</div>"));
    QVERIFY(content.contains("<div class=\"value\">1</div>"));
}

void HtmlReportGeneratorTest::testDeviceTable() {
    HtmlReportGenerator generator;
    QList<Device> devices = createTestDevices();

    QTemporaryFile tempFile;
    QVERIFY(tempFile.open());
    QString filepath = tempFile.fileName();
    tempFile.close();

    QVERIFY(generator.exportData(devices, filepath));

    QString content = readFileContent(filepath);

    // Check table structure
    QVERIFY(content.contains("<table>"));
    QVERIFY(content.contains("<thead>"));
    QVERIFY(content.contains("<tbody>"));

    // Check headers
    QVERIFY(content.contains("<th>IP Address</th>"));
    QVERIFY(content.contains("<th>Hostname</th>"));
    QVERIFY(content.contains("<th>MAC Address</th>"));
    QVERIFY(content.contains("<th>Vendor</th>"));
    QVERIFY(content.contains("<th>Status</th>"));
    QVERIFY(content.contains("<th>Latency (ms)</th>"));
    QVERIFY(content.contains("<th>Quality</th>"));

    // Check device data
    QVERIFY(content.contains("192.168.1.1"));
    QVERIFY(content.contains("router.local"));
    QVERIFY(content.contains("AA:BB:CC:DD:EE:FF"));
    QVERIFY(content.contains("Cisco"));

    QVERIFY(content.contains("192.168.1.10"));
    QVERIFY(content.contains("nas.local"));

    QVERIFY(content.contains("192.168.1.20"));
    QVERIFY(content.contains("iot.local"));
}

void HtmlReportGeneratorTest::testQualityColors() {
    HtmlReportGenerator generator;
    QList<Device> devices = createTestDevices();

    QTemporaryFile tempFile;
    QVERIFY(tempFile.open());
    QString filepath = tempFile.fileName();
    tempFile.close();

    QVERIFY(generator.exportData(devices, filepath));

    QString content = readFileContent(filepath);

    // Check quality color classes
    QVERIFY(content.contains("quality-excellent"));
    QVERIFY(content.contains("quality-poor"));
    QVERIFY(content.contains("quality-bad"));
}

void HtmlReportGeneratorTest::testOnlineOfflineStatus() {
    HtmlReportGenerator generator;
    QList<Device> devices = createTestDevices();

    QTemporaryFile tempFile;
    QVERIFY(tempFile.open());
    QString filepath = tempFile.fileName();
    tempFile.close();

    QVERIFY(generator.exportData(devices, filepath));

    QString content = readFileContent(filepath);

    // Check status badges
    QVERIFY(content.contains("status-badge status-online"));
    QVERIFY(content.contains("status-badge status-offline"));
    QVERIFY(content.contains(">Online</span>"));
    QVERIFY(content.contains(">Offline</span>"));
}

QTEST_MAIN(HtmlReportGeneratorTest)
#include "HtmlReportGeneratorTest.moc"
