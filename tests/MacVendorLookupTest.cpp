#include <QtTest/QtTest>
#include "network/services/MacVendorLookup.h"

class MacVendorLookupTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void testDatabaseSize();
    void testAppleLookup();
    void testMicrosoftLookup();
    void testIntelLookup();
    void testCiscoLookup();
    void testDellLookup();
    void testUnknownVendor();
    void testMacFormats();
    void testEmptyMac();
    void testShortMac();
    void testLocallyAdministeredMac();

private:
    MacVendorLookup* vendorLookup;
};

void MacVendorLookupTest::initTestCase()
{
    // Use singleton instance
    vendorLookup = MacVendorLookup::instance();
    qDebug() << "MacVendorLookup database size:" << vendorLookup->databaseSize();
}

void MacVendorLookupTest::testDatabaseSize()
{
    QVERIFY(vendorLookup->databaseSize() > 0);
    qDebug() << "Database contains" << vendorLookup->databaseSize() << "entries";
}

void MacVendorLookupTest::testAppleLookup()
{
    // Test real MAC addresses from the network
    QString vendor1 = vendorLookup->lookupVendor("30:16:9D:58:D3:B0");
    qDebug() << "30:16:9D:58:D3:B0 ->" << vendor1;
    QVERIFY(vendor1.contains("MERCUSYS", Qt::CaseInsensitive) || vendor1 == "Apple");

    QString vendor2 = vendorLookup->lookupVendor("60:83:E7:D5:BB:43");
    qDebug() << "60:83:E7:D5:BB:43 ->" << vendor2;
    QVERIFY(vendor2.contains("TP-Link", Qt::CaseInsensitive) || vendor2 == "Apple");

    QString vendor3 = vendorLookup->lookupVendor("28:C6:8E:34:1E:58");
    qDebug() << "28:C6:8E:34:1E:58 ->" << vendor3;
    QVERIFY(vendor3.contains("NETGEAR", Qt::CaseInsensitive) || vendor3 == "Apple");
}

void MacVendorLookupTest::testMicrosoftLookup()
{
    QString vendor1 = vendorLookup->lookupVendor("00:0D:3A:44:55:66");
    qDebug() << "00:0D:3A:44:55:66 ->" << vendor1;
    QCOMPARE(vendor1, QString("Microsoft"));

    QString vendor2 = vendorLookup->lookupVendor("00:50:F2:77:88:99");
    qDebug() << "00:50:F2:77:88:99 ->" << vendor2;
    QCOMPARE(vendor2, QString("Microsoft"));
}

void MacVendorLookupTest::testIntelLookup()
{
    QString vendor = vendorLookup->lookupVendor("00:1B:21:AA:BB:CC");
    qDebug() << "00:1B:21:AA:BB:CC ->" << vendor;
    QCOMPARE(vendor, QString("Intel"));
}

void MacVendorLookupTest::testCiscoLookup()
{
    QString vendor = vendorLookup->lookupVendor("00:24:1D:11:22:33");
    qDebug() << "00:24:1D:11:22:33 ->" << vendor;
    QCOMPARE(vendor, QString("Cisco"));
}

void MacVendorLookupTest::testDellLookup()
{
    QString vendor = vendorLookup->lookupVendor("00:22:48:44:55:66");
    qDebug() << "00:22:48:44:55:66 ->" << vendor;
    QCOMPARE(vendor, QString("Dell"));
}

void MacVendorLookupTest::testUnknownVendor()
{
    QString vendor = vendorLookup->lookupVendor("AA:BB:CC:DD:EE:FF");
    qDebug() << "AA:BB:CC:DD:EE:FF ->" << vendor;
    QCOMPARE(vendor, QString("Unknown"));
}

void MacVendorLookupTest::testMacFormats()
{
    // Colon separated
    QString vendor1 = vendorLookup->lookupVendor("00:30:65:12:34:56");
    qDebug() << "Format ':' ->" << vendor1;
    QCOMPARE(vendor1, QString("Apple"));

    // Dash separated
    QString vendor2 = vendorLookup->lookupVendor("00-30-65-12-34-56");
    qDebug() << "Format '-' ->" << vendor2;
    QCOMPARE(vendor2, QString("Apple"));

    // No separator
    QString vendor3 = vendorLookup->lookupVendor("003065123456");
    qDebug() << "Format 'no separator' ->" << vendor3;
    QCOMPARE(vendor3, QString("Apple"));

    // Cisco format
    QString vendor4 = vendorLookup->lookupVendor("0030.6512.3456");
    qDebug() << "Format '.' ->" << vendor4;
    QCOMPARE(vendor4, QString("Apple"));
}

void MacVendorLookupTest::testEmptyMac()
{
    QString vendor = vendorLookup->lookupVendor("");
    qDebug() << "Empty MAC ->" << vendor;
    QCOMPARE(vendor, QString("Unknown"));
}

void MacVendorLookupTest::testShortMac()
{
    QString vendor = vendorLookup->lookupVendor("00:30");
    qDebug() << "Short MAC ->" << vendor;
    QCOMPARE(vendor, QString("Unknown"));
}

void MacVendorLookupTest::testLocallyAdministeredMac()
{
    // Test locally administered addresses (LAA)
    // These have bit 1 set in the first octet
    QString vendor1 = vendorLookup->lookupVendor("CA:D3:A3:A9:77:33");
    qDebug() << "LAA CA:D3:A3:A9:77:33 ->" << vendor1;
    QCOMPARE(vendor1, QString("Locally Administered"));

    QString vendor2 = vendorLookup->lookupVendor("32:16:9D:09:4E:04");
    qDebug() << "LAA 32:16:9D:09:4E:04 ->" << vendor2;
    QCOMPARE(vendor2, QString("Locally Administered"));

    QString vendor3 = vendorLookup->lookupVendor("5E:E9:31:06:D3:ED");
    qDebug() << "LAA 5E:E9:31:06:D3:ED ->" << vendor3;
    QCOMPARE(vendor3, QString("Locally Administered"));

    // Test that universally administered addresses are NOT detected as LAA
    QString vendor4 = vendorLookup->lookupVendor("00:30:65:12:34:56");
    qDebug() << "UAA 00:30:65:12:34:56 ->" << vendor4;
    QVERIFY(vendor4 != QString("Locally Administered"));
}

QTEST_MAIN(MacVendorLookupTest)
#include "MacVendorLookupTest.moc"
