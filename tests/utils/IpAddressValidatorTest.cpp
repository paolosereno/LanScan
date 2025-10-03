#include <QtTest>
#include "utils/IpAddressValidator.h"

class IpAddressValidatorTest : public QObject
{
    Q_OBJECT

private slots:
    void testIsValidIpv4_Valid();
    void testIsValidIpv4_Invalid();
    void testIsValidCidr_Valid();
    void testIsValidCidr_Invalid();
    void testIsValidIpRange_Valid();
    void testIsValidIpRange_Invalid();
    void testIsValidPort_Valid();
    void testIsValidPort_Invalid();
    void testIsValidMacAddress_Valid();
    void testIsValidMacAddress_Invalid();
    void testParseCidr();
    void testIpToUInt32();
    void testUInt32ToIp();
    void testCalculateHostCount();
};

void IpAddressValidatorTest::testIsValidIpv4_Valid()
{
    QVERIFY(IpAddressValidator::isValidIpv4("192.168.1.1"));
    QVERIFY(IpAddressValidator::isValidIpv4("10.0.0.1"));
    QVERIFY(IpAddressValidator::isValidIpv4("8.8.8.8"));
    QVERIFY(IpAddressValidator::isValidIpv4("255.255.255.255"));
    QVERIFY(IpAddressValidator::isValidIpv4("0.0.0.0"));
}

void IpAddressValidatorTest::testIsValidIpv4_Invalid()
{
    QVERIFY(!IpAddressValidator::isValidIpv4("256.1.1.1"));
    QVERIFY(!IpAddressValidator::isValidIpv4("192.168.1"));
    QVERIFY(!IpAddressValidator::isValidIpv4("192.168.1.1.1"));
    QVERIFY(!IpAddressValidator::isValidIpv4("abc.def.ghi.jkl"));
    QVERIFY(!IpAddressValidator::isValidIpv4(""));
}

void IpAddressValidatorTest::testIsValidCidr_Valid()
{
    QVERIFY(IpAddressValidator::isValidCidr("192.168.1.0/24"));
    QVERIFY(IpAddressValidator::isValidCidr("10.0.0.0/8"));
    QVERIFY(IpAddressValidator::isValidCidr("172.16.0.0/12"));
    QVERIFY(IpAddressValidator::isValidCidr("192.168.1.1/32"));
    QVERIFY(IpAddressValidator::isValidCidr("0.0.0.0/0"));
}

void IpAddressValidatorTest::testIsValidCidr_Invalid()
{
    QVERIFY(!IpAddressValidator::isValidCidr("192.168.1.0/33"));
    QVERIFY(!IpAddressValidator::isValidCidr("192.168.1.0/-1"));
    QVERIFY(!IpAddressValidator::isValidCidr("192.168.1.0"));
    QVERIFY(!IpAddressValidator::isValidCidr("256.1.1.1/24"));
    QVERIFY(!IpAddressValidator::isValidCidr(""));
}

void IpAddressValidatorTest::testIsValidIpRange_Valid()
{
    QVERIFY(IpAddressValidator::isValidIpRange("192.168.1.1", "192.168.1.254"));
    QVERIFY(IpAddressValidator::isValidIpRange("10.0.0.1", "10.0.0.1"));
    QVERIFY(IpAddressValidator::isValidIpRange("192.168.0.1", "192.168.255.254"));
}

void IpAddressValidatorTest::testIsValidIpRange_Invalid()
{
    QVERIFY(!IpAddressValidator::isValidIpRange("192.168.1.254", "192.168.1.1"));
    QVERIFY(!IpAddressValidator::isValidIpRange("256.1.1.1", "192.168.1.1"));
    QVERIFY(!IpAddressValidator::isValidIpRange("192.168.1.1", "invalid"));
}

void IpAddressValidatorTest::testIsValidPort_Valid()
{
    QVERIFY(IpAddressValidator::isValidPort(1));
    QVERIFY(IpAddressValidator::isValidPort(80));
    QVERIFY(IpAddressValidator::isValidPort(443));
    QVERIFY(IpAddressValidator::isValidPort(65535));
}

void IpAddressValidatorTest::testIsValidPort_Invalid()
{
    QVERIFY(!IpAddressValidator::isValidPort(0));
    QVERIFY(!IpAddressValidator::isValidPort(-1));
    QVERIFY(!IpAddressValidator::isValidPort(65536));
    QVERIFY(!IpAddressValidator::isValidPort(100000));
}

void IpAddressValidatorTest::testIsValidMacAddress_Valid()
{
    QVERIFY(IpAddressValidator::isValidMacAddress("AA:BB:CC:DD:EE:FF"));
    QVERIFY(IpAddressValidator::isValidMacAddress("00:11:22:33:44:55"));
    QVERIFY(IpAddressValidator::isValidMacAddress("AA-BB-CC-DD-EE-FF"));
    QVERIFY(IpAddressValidator::isValidMacAddress("aa:bb:cc:dd:ee:ff"));
}

void IpAddressValidatorTest::testIsValidMacAddress_Invalid()
{
    QVERIFY(!IpAddressValidator::isValidMacAddress("AA:BB:CC:DD:EE"));
    QVERIFY(!IpAddressValidator::isValidMacAddress("AA:BB:CC:DD:EE:GG"));
    QVERIFY(!IpAddressValidator::isValidMacAddress("AABBCCDDEEFF"));
    QVERIFY(!IpAddressValidator::isValidMacAddress(""));
}

void IpAddressValidatorTest::testParseCidr()
{
    auto result = IpAddressValidator::parseCidr("192.168.1.0/24");
    QCOMPARE(result.first, QString("192.168.1.0"));
    QCOMPARE(result.second, 24);

    auto invalid = IpAddressValidator::parseCidr("invalid");
    QVERIFY(invalid.first.isEmpty());
    QCOMPARE(invalid.second, 0);
}

void IpAddressValidatorTest::testIpToUInt32()
{
    quint32 result = IpAddressValidator::ipToUInt32("192.168.1.1");
    QVERIFY(result > 0);

    quint32 zero = IpAddressValidator::ipToUInt32("0.0.0.0");
    QCOMPARE(zero, (quint32)0);
}

void IpAddressValidatorTest::testUInt32ToIp()
{
    quint32 value = IpAddressValidator::ipToUInt32("192.168.1.1");
    QString ip = IpAddressValidator::uInt32ToIp(value);
    QCOMPARE(ip, QString("192.168.1.1"));
}

void IpAddressValidatorTest::testCalculateHostCount()
{
    QCOMPARE(IpAddressValidator::calculateHostCount(24), 254);
    QCOMPARE(IpAddressValidator::calculateHostCount(16), 65534);
    QCOMPARE(IpAddressValidator::calculateHostCount(8), 16777214);
    QCOMPARE(IpAddressValidator::calculateHostCount(32), 0);
    QCOMPARE(IpAddressValidator::calculateHostCount(0), 4294967294);
}

QTEST_MAIN(IpAddressValidatorTest)
#include "IpAddressValidatorTest.moc"
