#include <QtTest>
#include "export/JsonExporter.h"
#include "models/Device.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTemporaryFile>

class JsonExporterTest : public QObject {
    Q_OBJECT

private slots:
    void testExportData();
    void testExportEmptyList();
    void testJsonStructure();
    void testDeviceFields();

private:
    QList<Device> createTestDevices();
};

QList<Device> JsonExporterTest::createTestDevices() {
    QList<Device> devices;

    Device device1;
    device1.setId("device-1");
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
    device1.setOpenPorts({port1});

    NetworkMetrics metrics1;
    metrics1.setLatencyMin(3.0);
    metrics1.setLatencyAvg(5.2);
    metrics1.setLatencyMax(8.0);
    metrics1.setLatencyMedian(5.0);
    metrics1.setJitter(1.2);
    metrics1.setPacketLoss(0.0);
    metrics1.setQualityScore(NetworkMetrics::Excellent);
    device1.setMetrics(metrics1);

    devices.append(device1);

    return devices;
}

void JsonExporterTest::testExportData() {
    JsonExporter exporter;
    QList<Device> devices = createTestDevices();

    QTemporaryFile tempFile;
    QVERIFY(tempFile.open());
    QString filepath = tempFile.fileName();
    tempFile.close();

    QVERIFY(exporter.exportData(devices, filepath));

    QFile file(filepath);
    QVERIFY(file.exists());
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));

    QByteArray data = file.readAll();
    file.close();

    QVERIFY(!data.isEmpty());

    QJsonDocument doc = QJsonDocument::fromJson(data);
    QVERIFY(!doc.isNull());
    QVERIFY(doc.isObject());
}

void JsonExporterTest::testExportEmptyList() {
    JsonExporter exporter;
    QList<Device> devices;

    QTemporaryFile tempFile;
    QVERIFY(tempFile.open());
    QString filepath = tempFile.fileName();
    tempFile.close();

    QVERIFY(exporter.exportData(devices, filepath));

    QFile file(filepath);
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    QVERIFY(doc.isObject());
    QJsonObject root = doc.object();
    QCOMPARE(root["total_devices"].toInt(), 0);
    QVERIFY(root["devices"].isArray());
    QCOMPARE(root["devices"].toArray().size(), 0);
}

void JsonExporterTest::testJsonStructure() {
    JsonExporter exporter;
    QList<Device> devices = createTestDevices();

    QTemporaryFile tempFile;
    QVERIFY(tempFile.open());
    QString filepath = tempFile.fileName();
    tempFile.close();

    QVERIFY(exporter.exportData(devices, filepath));

    QFile file(filepath);
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    QJsonObject root = doc.object();

    // Check root structure
    QVERIFY(root.contains("export_date"));
    QVERIFY(root.contains("total_devices"));
    QVERIFY(root.contains("devices"));

    QCOMPARE(root["total_devices"].toInt(), 1);

    QJsonArray devicesArray = root["devices"].toArray();
    QCOMPARE(devicesArray.size(), 1);
}

void JsonExporterTest::testDeviceFields() {
    JsonExporter exporter;
    QList<Device> devices = createTestDevices();

    QTemporaryFile tempFile;
    QVERIFY(tempFile.open());
    QString filepath = tempFile.fileName();
    tempFile.close();

    QVERIFY(exporter.exportData(devices, filepath));

    QFile file(filepath);
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    QJsonObject root = doc.object();
    QJsonArray devicesArray = root["devices"].toArray();
    QJsonObject device = devicesArray[0].toObject();

    // Check device fields
    QCOMPARE(device["id"].toString(), QString("device-1"));
    QCOMPARE(device["ip"].toString(), QString("192.168.1.1"));
    QCOMPARE(device["hostname"].toString(), QString("router.local"));
    QCOMPARE(device["mac_address"].toString(), QString("AA:BB:CC:DD:EE:FF"));
    QCOMPARE(device["vendor"].toString(), QString("Cisco"));
    QVERIFY(device["is_online"].toBool());

    // Check metrics
    QVERIFY(device.contains("metrics"));
    QJsonObject metrics = device["metrics"].toObject();
    QCOMPARE(metrics["latency_avg"].toDouble(), 5.2);
    QCOMPARE(metrics["packet_loss"].toDouble(), 0.0);
    QCOMPARE(metrics["quality_score"].toString(), QString("Excellent"));

    // Check ports
    QVERIFY(device.contains("open_ports"));
    QJsonArray ports = device["open_ports"].toArray();
    QCOMPARE(ports.size(), 1);
    QJsonObject port = ports[0].toObject();
    QCOMPARE(port["port"].toInt(), 80);
    QCOMPARE(port["service"].toString(), QString("HTTP"));
}

QTEST_MAIN(JsonExporterTest)
#include "JsonExporterTest.moc"
