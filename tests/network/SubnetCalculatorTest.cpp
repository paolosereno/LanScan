#include <QtTest>
#include "network/services/SubnetCalculator.h"

class SubnetCalculatorTest : public QObject
{
    Q_OBJECT

private slots:
    void testGetNetworkAddress();
    void testGetBroadcastAddress();
    void testGetIpRange();
    void testGetHostCount();
    void testIsIpInSubnet();
    void testCidrToSubnetMask();
    void testSubnetMaskToCidr();
};

void SubnetCalculatorTest::testGetNetworkAddress()
{
    QString network = SubnetCalculator::getNetworkAddress("192.168.1.100", "255.255.255.0");
    QCOMPARE(network, QString("192.168.1.0"));

    network = SubnetCalculator::getNetworkAddress("10.0.15.45", "255.255.0.0");
    QCOMPARE(network, QString("10.0.0.0"));
}

void SubnetCalculatorTest::testGetBroadcastAddress()
{
    QString broadcast = SubnetCalculator::getBroadcastAddress("192.168.1.100", "255.255.255.0");
    QCOMPARE(broadcast, QString("192.168.1.255"));

    broadcast = SubnetCalculator::getBroadcastAddress("10.0.15.45", "255.255.0.0");
    QCOMPARE(broadcast, QString("10.0.255.255"));
}

void SubnetCalculatorTest::testGetIpRange()
{
    QStringList range = SubnetCalculator::getIpRange("192.168.1.0/30");
    QCOMPARE(range.size(), 2); // /30 has 2 usable IPs
    QVERIFY(range.contains("192.168.1.1"));
    QVERIFY(range.contains("192.168.1.2"));

    range = SubnetCalculator::getIpRange("192.168.1.0/29");
    QCOMPARE(range.size(), 6); // /29 has 6 usable IPs
}

void SubnetCalculatorTest::testGetHostCount()
{
    QCOMPARE(SubnetCalculator::getHostCount("192.168.1.0/24"), 254);
    QCOMPARE(SubnetCalculator::getHostCount("192.168.1.0/30"), 2);
    QCOMPARE(SubnetCalculator::getHostCount("192.168.1.0/32"), 0);
}

void SubnetCalculatorTest::testIsIpInSubnet()
{
    QVERIFY(SubnetCalculator::isIpInSubnet("192.168.1.100", "192.168.1.0/24"));
    QVERIFY(SubnetCalculator::isIpInSubnet("192.168.1.1", "192.168.1.0/24"));
    QVERIFY(!SubnetCalculator::isIpInSubnet("192.168.2.1", "192.168.1.0/24"));
    QVERIFY(!SubnetCalculator::isIpInSubnet("10.0.0.1", "192.168.1.0/24"));
}

void SubnetCalculatorTest::testCidrToSubnetMask()
{
    QCOMPARE(SubnetCalculator::cidrToSubnetMask(24), QString("255.255.255.0"));
    QCOMPARE(SubnetCalculator::cidrToSubnetMask(16), QString("255.255.0.0"));
    QCOMPARE(SubnetCalculator::cidrToSubnetMask(8), QString("255.0.0.0"));
    QCOMPARE(SubnetCalculator::cidrToSubnetMask(30), QString("255.255.255.252"));
}

void SubnetCalculatorTest::testSubnetMaskToCidr()
{
    QCOMPARE(SubnetCalculator::subnetMaskToCidr("255.255.255.0"), 24);
    QCOMPARE(SubnetCalculator::subnetMaskToCidr("255.255.0.0"), 16);
    QCOMPARE(SubnetCalculator::subnetMaskToCidr("255.0.0.0"), 8);
    QCOMPARE(SubnetCalculator::subnetMaskToCidr("255.255.255.252"), 30);
}

QTEST_MAIN(SubnetCalculatorTest)
#include "SubnetCalculatorTest.moc"
