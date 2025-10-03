#include <QtTest>
#include "models/Device.h"

class DeviceTest : public QObject
{
    Q_OBJECT

private slots:
    void testDefaultConstructor();
    void testConstructorWithParams();
    void testSettersAndGetters();
    void testAddPort();
    void testRemovePort();
    void testHasPort();
};

void DeviceTest::testDefaultConstructor()
{
    Device device;
    QVERIFY(device.ip().isEmpty());
    QVERIFY(device.hostname().isEmpty());
    QVERIFY(!device.isOnline());
}

void DeviceTest::testConstructorWithParams()
{
    Device device("192.168.1.1", "router");
    QCOMPARE(device.ip(), QString("192.168.1.1"));
    QCOMPARE(device.hostname(), QString("router"));
    QVERIFY(!device.isOnline());
}

void DeviceTest::testSettersAndGetters()
{
    Device device;

    device.setIp("192.168.1.100");
    QCOMPARE(device.ip(), QString("192.168.1.100"));

    device.setHostname("workstation");
    QCOMPARE(device.hostname(), QString("workstation"));

    device.setMacAddress("AA:BB:CC:DD:EE:FF");
    QCOMPARE(device.macAddress(), QString("AA:BB:CC:DD:EE:FF"));

    device.setVendor("Cisco");
    QCOMPARE(device.vendor(), QString("Cisco"));

    device.setOnline(true);
    QVERIFY(device.isOnline());

    QDateTime now = QDateTime::currentDateTime();
    device.setLastSeen(now);
    QCOMPARE(device.lastSeen(), now);
}

void DeviceTest::testAddPort()
{
    Device device;
    PortInfo port(80, PortInfo::TCP);

    device.addPort(port);
    QCOMPARE(device.openPorts().size(), 1);
    QVERIFY(device.hasPort(80));

    // Adding duplicate should not increase count
    device.addPort(port);
    QCOMPARE(device.openPorts().size(), 1);
}

void DeviceTest::testRemovePort()
{
    Device device;
    PortInfo port1(80, PortInfo::TCP);
    PortInfo port2(443, PortInfo::TCP);

    device.addPort(port1);
    device.addPort(port2);
    QCOMPARE(device.openPorts().size(), 2);

    device.removePort(80);
    QCOMPARE(device.openPorts().size(), 1);
    QVERIFY(!device.hasPort(80));
    QVERIFY(device.hasPort(443));
}

void DeviceTest::testHasPort()
{
    Device device;
    PortInfo port(22, PortInfo::TCP);

    QVERIFY(!device.hasPort(22));

    device.addPort(port);
    QVERIFY(device.hasPort(22));
}

QTEST_MAIN(DeviceTest)
#include "DeviceTest.moc"
