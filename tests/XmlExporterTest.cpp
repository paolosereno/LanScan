#include <QtTest>
#include "export/XmlExporter.h"
#include "models/Device.h"
#include <QFile>
#include <QXmlStreamReader>
#include <QTemporaryFile>

class XmlExporterTest : public QObject {
    Q_OBJECT

private slots:
    void testExportData();
    void testExportEmptyList();
    void testXmlStructure();
    void testDeviceElements();
    void testMetricsElements();
    void testPortsElements();

private:
    QList<Device> createTestDevices();
    bool verifyXmlWellFormed(const QString& filepath);
};

QList<Device> XmlExporterTest::createTestDevices() {
    QList<Device> devices;

    Device device1;
    device1.setIp("192.168.1.1");
    device1.setHostname("router.local");
    device1.setMacAddress("AA:BB:CC:DD:EE:FF");
    device1.setVendor("Cisco");
    device1.setOnline(true);
    device1.setLastSeen(QDateTime::fromString("2024-01-15T10:30:00", Qt::ISODate));

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
    metrics1.setLatencyMin(4.5);
    metrics1.setLatencyMax(6.8);
    metrics1.setLatencyMedian(5.1);
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
    device2.setOnline(false);

    NetworkMetrics metrics2;
    metrics2.setLatencyAvg(12.5);
    metrics2.setPacketLoss(0.5);
    metrics2.setJitter(2.3);
    metrics2.setQualityScore(NetworkMetrics::Good);
    device2.setMetrics(metrics2);

    devices.append(device2);

    return devices;
}

bool XmlExporterTest::verifyXmlWellFormed(const QString& filepath) {
    QFile file(filepath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QXmlStreamReader reader(&file);
    while (!reader.atEnd()) {
        reader.readNext();
    }

    file.close();
    return !reader.hasError();
}

void XmlExporterTest::testExportData() {
    XmlExporter exporter;
    QList<Device> devices = createTestDevices();

    QTemporaryFile tempFile;
    QVERIFY(tempFile.open());
    QString filepath = tempFile.fileName();
    tempFile.close();

    QVERIFY(exporter.exportData(devices, filepath));

    // Verify file exists
    QFile file(filepath);
    QVERIFY(file.exists());
    QVERIFY(file.size() > 0);

    // Verify XML is well-formed
    QVERIFY(verifyXmlWellFormed(filepath));
}

void XmlExporterTest::testExportEmptyList() {
    XmlExporter exporter;
    QList<Device> devices;

    QTemporaryFile tempFile;
    QVERIFY(tempFile.open());
    QString filepath = tempFile.fileName();
    tempFile.close();

    QVERIFY(exporter.exportData(devices, filepath));

    // Verify XML is well-formed
    QVERIFY(verifyXmlWellFormed(filepath));

    QFile file(filepath);
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));

    QXmlStreamReader reader(&file);
    int deviceCount = 0;

    while (!reader.atEnd()) {
        reader.readNext();
        if (reader.name() == QString("Device") && reader.isStartElement()) {
            deviceCount++;
        }
    }

    file.close();
    QCOMPARE(deviceCount, 0);
}

void XmlExporterTest::testXmlStructure() {
    XmlExporter exporter;
    QList<Device> devices = createTestDevices();

    QTemporaryFile tempFile;
    QVERIFY(tempFile.open());
    QString filepath = tempFile.fileName();
    tempFile.close();

    QVERIFY(exporter.exportData(devices, filepath));

    QFile file(filepath);
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));

    QXmlStreamReader reader(&file);

    // Check root element
    while (!reader.atEnd()) {
        reader.readNext();
        if (reader.isStartElement()) {
            if (reader.name() == QString("LanScanExport")) {
                QVERIFY(reader.attributes().hasAttribute("version"));
                QVERIFY(reader.attributes().hasAttribute("exportDate"));
                QVERIFY(reader.attributes().hasAttribute("totalDevices"));
                QCOMPARE(reader.attributes().value("totalDevices").toString(), QString::number(devices.size()));
                break;
            }
        }
    }

    file.close();
}

void XmlExporterTest::testDeviceElements() {
    XmlExporter exporter;
    QList<Device> devices = createTestDevices();

    QTemporaryFile tempFile;
    QVERIFY(tempFile.open());
    QString filepath = tempFile.fileName();
    tempFile.close();

    QVERIFY(exporter.exportData(devices, filepath));

    QFile file(filepath);
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));

    QXmlStreamReader reader(&file);
    bool foundDevice = false;
    bool foundIP = false;
    bool foundHostname = false;
    bool foundMAC = false;

    while (!reader.atEnd()) {
        reader.readNext();
        if (reader.isStartElement()) {
            if (reader.name() == QString("Device")) {
                foundDevice = true;
            } else if (reader.name() == QString("IP")) {
                QString ip = reader.readElementText();
                if (ip == "192.168.1.1") {
                    foundIP = true;
                }
            } else if (reader.name() == QString("Hostname")) {
                QString hostname = reader.readElementText();
                if (hostname == "router.local") {
                    foundHostname = true;
                }
            } else if (reader.name() == QString("MacAddress")) {
                QString mac = reader.readElementText();
                if (mac == "AA:BB:CC:DD:EE:FF") {
                    foundMAC = true;
                }
            }
        }
    }

    file.close();

    QVERIFY(foundDevice);
    QVERIFY(foundIP);
    QVERIFY(foundHostname);
    QVERIFY(foundMAC);
}

void XmlExporterTest::testMetricsElements() {
    XmlExporter exporter;
    QList<Device> devices = createTestDevices();

    QTemporaryFile tempFile;
    QVERIFY(tempFile.open());
    QString filepath = tempFile.fileName();
    tempFile.close();

    QVERIFY(exporter.exportData(devices, filepath));

    QFile file(filepath);
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));

    QXmlStreamReader reader(&file);
    bool foundMetrics = false;
    bool foundLatency = false;
    bool foundJitter = false;
    bool foundPacketLoss = false;
    bool foundQuality = false;

    while (!reader.atEnd()) {
        reader.readNext();
        if (reader.isStartElement()) {
            QString name = reader.name().toString();
            if (name == "Metrics") {
                foundMetrics = true;
            } else if (name == "Latency") {
                foundLatency = true;
                QVERIFY(reader.attributes().hasAttribute("unit"));
            } else if (name == "Jitter") {
                foundJitter = true;
            } else if (name == "PacketLoss") {
                foundPacketLoss = true;
            } else if (name == "Quality") {
                foundQuality = true;
            }
        }
    }

    file.close();

    QVERIFY(foundMetrics);
    QVERIFY(foundLatency);
    QVERIFY(foundJitter);
    QVERIFY(foundPacketLoss);
    QVERIFY(foundQuality);
}

void XmlExporterTest::testPortsElements() {
    XmlExporter exporter;
    QList<Device> devices = createTestDevices();

    QTemporaryFile tempFile;
    QVERIFY(tempFile.open());
    QString filepath = tempFile.fileName();
    tempFile.close();

    QVERIFY(exporter.exportData(devices, filepath));

    QFile file(filepath);
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));

    QXmlStreamReader reader(&file);
    bool foundOpenPorts = false;
    bool foundPort = false;
    int portCount = 0;

    while (!reader.atEnd()) {
        reader.readNext();
        if (reader.isStartElement()) {
            QString name = reader.name().toString();
            if (name == "OpenPorts") {
                foundOpenPorts = true;
                QVERIFY(reader.attributes().hasAttribute("count"));
            } else if (name == "Port") {
                foundPort = true;
                portCount++;
                QVERIFY(reader.attributes().hasAttribute("number"));
            }
        }
    }

    file.close();

    QVERIFY(foundOpenPorts);
    QVERIFY(foundPort);
    QVERIFY(portCount >= 2); // Device1 has 2 ports
}

QTEST_MAIN(XmlExporterTest)
#include "XmlExporterTest.moc"
