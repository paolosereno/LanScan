#include <QtTest>
#include "services/WakeOnLanService.h"
#include "models/Device.h"

class WakeOnLanServiceTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    // WakeOnLanPacket tests
    void testIsValidMacAddress_data();
    void testIsValidMacAddress();
    void testCreateMagicPacket_validMac();
    void testCreateMagicPacket_invalidMac();
    void testMagicPacketStructure();
    void testMagicPacketSize();

    // WakeOnLanService tests
    void testServiceInitialization();
    void testSendWakeOnLan_validMac();
    void testSendWakeOnLan_invalidMac();
    void testSendWakeOnLanToDevice_validDevice();
    void testSendWakeOnLanToDevice_noMac();
    void testSignalEmission();

private:
    WakeOnLanService* service;
};

void WakeOnLanServiceTest::initTestCase() {
    service = nullptr;
}

void WakeOnLanServiceTest::cleanupTestCase() {
    if (service) {
        delete service;
        service = nullptr;
    }
}

// ============================================================================
// WakeOnLanPacket Tests
// ============================================================================

void WakeOnLanServiceTest::testIsValidMacAddress_data() {
    QTest::addColumn<QString>("macAddress");
    QTest::addColumn<bool>("expected");

    // Valid formats
    QTest::newRow("valid colon uppercase") << "AA:BB:CC:DD:EE:FF" << true;
    QTest::newRow("valid colon lowercase") << "aa:bb:cc:dd:ee:ff" << true;
    QTest::newRow("valid colon mixed") << "Aa:Bb:Cc:Dd:Ee:Ff" << true;
    QTest::newRow("valid dash uppercase") << "AA-BB-CC-DD-EE-FF" << true;
    QTest::newRow("valid dash lowercase") << "aa-bb-cc-dd-ee-ff" << true;
    QTest::newRow("valid with numbers") << "00:11:22:33:44:55" << true;

    // Invalid formats
    QTest::newRow("invalid no separators") << "AABBCCDDEEFF" << false;
    QTest::newRow("invalid too short") << "AA:BB:CC:DD:EE" << false;
    QTest::newRow("invalid too long") << "AA:BB:CC:DD:EE:FF:00" << false;
    QTest::newRow("invalid characters") << "GG:HH:II:JJ:KK:LL" << false;
    QTest::newRow("invalid mixed separators") << "AA:BB-CC:DD-EE:FF" << false;
    QTest::newRow("invalid empty") << "" << false;
    QTest::newRow("invalid single byte") << "AA" << false;
    QTest::newRow("invalid spaces") << "AA BB CC DD EE FF" << false;
}

void WakeOnLanServiceTest::testIsValidMacAddress() {
    QFETCH(QString, macAddress);
    QFETCH(bool, expected);

    bool result = WakeOnLanPacket::isValidMacAddress(macAddress);
    QCOMPARE(result, expected);
}

void WakeOnLanServiceTest::testCreateMagicPacket_validMac() {
    QString mac = "AA:BB:CC:DD:EE:FF";
    QByteArray packet = WakeOnLanPacket::createMagicPacket(mac);

    QVERIFY(!packet.isEmpty());
    QCOMPARE(packet.size(), 102); // 6 + (16 * 6) = 102 bytes
}

void WakeOnLanServiceTest::testCreateMagicPacket_invalidMac() {
    QString invalidMac = "INVALID_MAC";
    QByteArray packet = WakeOnLanPacket::createMagicPacket(invalidMac);

    QVERIFY(packet.isEmpty());
}

void WakeOnLanServiceTest::testMagicPacketStructure() {
    QString mac = "11:22:33:44:55:66";
    QByteArray packet = WakeOnLanPacket::createMagicPacket(mac);

    QVERIFY(!packet.isEmpty());

    // Check header (6 bytes of 0xFF)
    for (int i = 0; i < 6; i++) {
        QCOMPARE(static_cast<unsigned char>(packet.at(i)), 0xFF);
    }

    // Check MAC repetitions (16 times)
    QByteArray expectedMac;
    expectedMac.append(static_cast<char>(0x11));
    expectedMac.append(static_cast<char>(0x22));
    expectedMac.append(static_cast<char>(0x33));
    expectedMac.append(static_cast<char>(0x44));
    expectedMac.append(static_cast<char>(0x55));
    expectedMac.append(static_cast<char>(0x66));

    for (int i = 0; i < 16; i++) {
        int offset = 6 + (i * 6);
        QByteArray macSection = packet.mid(offset, 6);
        QCOMPARE(macSection, expectedMac);
    }
}

void WakeOnLanServiceTest::testMagicPacketSize() {
    QStringList testMacs = {
        "00:00:00:00:00:00",
        "FF:FF:FF:FF:FF:FF",
        "12:34:56:78:9A:BC",
        "AA-BB-CC-DD-EE-FF"
    };

    for (const QString& mac : testMacs) {
        QByteArray packet = WakeOnLanPacket::createMagicPacket(mac);
        QCOMPARE(packet.size(), 102);
    }
}

// ============================================================================
// WakeOnLanService Tests
// ============================================================================

void WakeOnLanServiceTest::testServiceInitialization() {
    service = new WakeOnLanService();
    QVERIFY(service != nullptr);
}

void WakeOnLanServiceTest::testSendWakeOnLan_validMac() {
    if (!service) {
        service = new WakeOnLanService();
    }

    QSignalSpy spyPacketSent(service, &WakeOnLanService::packetSent);
    QSignalSpy spyError(service, &WakeOnLanService::sendError);

    QString mac = "AA:BB:CC:DD:EE:FF";
    service->sendWakeOnLan(mac, "255.255.255.255");

    // Should emit packetSent signal
    QCOMPARE(spyPacketSent.count(), 1);
    QCOMPARE(spyError.count(), 0);

    QList<QVariant> arguments = spyPacketSent.takeFirst();
    QCOMPARE(arguments.at(0).toString(), mac);
}

void WakeOnLanServiceTest::testSendWakeOnLan_invalidMac() {
    if (!service) {
        service = new WakeOnLanService();
    }

    QSignalSpy spyPacketSent(service, &WakeOnLanService::packetSent);
    QSignalSpy spyError(service, &WakeOnLanService::sendError);

    QString invalidMac = "INVALID";
    service->sendWakeOnLan(invalidMac, "255.255.255.255");

    // Should emit sendError signal
    QCOMPARE(spyPacketSent.count(), 0);
    QCOMPARE(spyError.count(), 1);
}

void WakeOnLanServiceTest::testSendWakeOnLanToDevice_validDevice() {
    if (!service) {
        service = new WakeOnLanService();
    }

    QSignalSpy spyPacketSent(service, &WakeOnLanService::packetSent);
    QSignalSpy spyError(service, &WakeOnLanService::sendError);

    Device device;
    device.setIp("192.168.1.100");
    device.setHostname("test-device");
    device.setMacAddress("11:22:33:44:55:66");

    service->sendWakeOnLanToDevice(device);

    // Should emit packetSent signal
    QCOMPARE(spyPacketSent.count(), 1);
    QCOMPARE(spyError.count(), 0);
}

void WakeOnLanServiceTest::testSendWakeOnLanToDevice_noMac() {
    if (!service) {
        service = new WakeOnLanService();
    }

    QSignalSpy spyPacketSent(service, &WakeOnLanService::packetSent);
    QSignalSpy spyError(service, &WakeOnLanService::sendError);

    Device device;
    device.setIp("192.168.1.100");
    device.setHostname("test-device");
    device.setMacAddress(""); // Empty MAC

    service->sendWakeOnLanToDevice(device);

    // Should emit sendError signal
    QCOMPARE(spyPacketSent.count(), 0);
    QCOMPARE(spyError.count(), 1);
}

void WakeOnLanServiceTest::testSignalEmission() {
    if (!service) {
        service = new WakeOnLanService();
    }

    QSignalSpy spyPacketSent(service, &WakeOnLanService::packetSent);
    QSignalSpy spyError(service, &WakeOnLanService::sendError);

    // Test valid MAC - should emit packetSent
    service->sendWakeOnLan("AA:BB:CC:DD:EE:FF");
    QCOMPARE(spyPacketSent.count(), 1);
    QCOMPARE(spyError.count(), 0);

    spyPacketSent.clear();
    spyError.clear();

    // Test invalid MAC - should emit sendError
    service->sendWakeOnLan("INVALID");
    QCOMPARE(spyPacketSent.count(), 0);
    QCOMPARE(spyError.count(), 1);
}

QTEST_MAIN(WakeOnLanServiceTest)
#include "WakeOnLanServiceTest.moc"
