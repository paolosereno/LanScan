#include <QtTest>
#include "network/discovery/ArpDiscovery.h"

class ArpDiscoveryTest : public QObject
{
    Q_OBJECT

private slots:
    void testGetArpTable();
    void testGetMacAddress();
    void testArpTableFormat();
    void testInvalidIp();
};

void ArpDiscoveryTest::testGetArpTable()
{
    // Get ARP table
    QMap<QString, QString> arpTable = ArpDiscovery::getArpTable();

    // ARP table might be empty or populated depending on network state
    // Just verify it returns without crashing
    QVERIFY(arpTable.size() >= 0);

    // If there are entries, verify they have proper format
    if (!arpTable.isEmpty()) {
        for (auto it = arpTable.begin(); it != arpTable.end(); ++it) {
            QString ip = it.key();
            QString mac = it.value();

            // IP should contain dots
            QVERIFY(ip.contains('.'));

            // MAC should contain colons and be uppercase
            QVERIFY(mac.contains(':'));
            QCOMPARE(mac, mac.toUpper());

            // MAC should be in format XX:XX:XX:XX:XX:XX
            QStringList macParts = mac.split(':');
            QCOMPARE(macParts.size(), 6);
        }
    }
}

void ArpDiscoveryTest::testGetMacAddress()
{
    // First get the ARP table to see if we have any entries
    QMap<QString, QString> arpTable = ArpDiscovery::getArpTable();

    if (!arpTable.isEmpty()) {
        // Test with a known IP from the table
        QString testIp = arpTable.firstKey();
        QString mac = ArpDiscovery::getMacAddress(testIp);

        // Should return the MAC address
        QVERIFY(!mac.isEmpty());
        QCOMPARE(mac, arpTable.value(testIp));
    }

    // Test with an IP that definitely doesn't exist
    QString invalidMac = ArpDiscovery::getMacAddress("192.0.2.255");
    QVERIFY(invalidMac.isEmpty());
}

void ArpDiscoveryTest::testArpTableFormat()
{
    // Verify that MAC addresses are in the correct format
    QMap<QString, QString> arpTable = ArpDiscovery::getArpTable();

    for (const QString& mac : arpTable.values()) {
        // Should be in format XX:XX:XX:XX:XX:XX
        QRegularExpression macRegex("^[0-9A-F]{2}:[0-9A-F]{2}:[0-9A-F]{2}:[0-9A-F]{2}:[0-9A-F]{2}:[0-9A-F]{2}$");
        QVERIFY(macRegex.match(mac).hasMatch());
    }
}

void ArpDiscoveryTest::testInvalidIp()
{
    // Test with various invalid IPs
    QVERIFY(ArpDiscovery::getMacAddress("").isEmpty());
    QVERIFY(ArpDiscovery::getMacAddress("invalid").isEmpty());
    QVERIFY(ArpDiscovery::getMacAddress("999.999.999.999").isEmpty());
}

QTEST_MAIN(ArpDiscoveryTest)
#include "ArpDiscoveryTest.moc"
