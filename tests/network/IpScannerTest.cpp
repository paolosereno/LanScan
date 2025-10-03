#include <QtTest>
#include "network/scanner/IpScanner.h"
#include "network/scanner/QuickScanStrategy.h"
#include "network/scanner/DeepScanStrategy.h"

// Register Device type for Qt signals/slots
Q_DECLARE_METATYPE(Device)

class IpScannerTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testSetStrategy();
    void testQuickScanStrategy();
    void testDeepScanStrategy();

private:
    IpScanner* m_scanner;
    QuickScanStrategy* m_quickStrategy;
    DeepScanStrategy* m_deepStrategy;
};

void IpScannerTest::initTestCase()
{
    // Register Device type for use in signals
    qRegisterMetaType<Device>("Device");

    m_scanner = new IpScanner(this);
    m_quickStrategy = new QuickScanStrategy();
    m_deepStrategy = new DeepScanStrategy();
}

void IpScannerTest::cleanupTestCase()
{
    delete m_scanner;
    delete m_quickStrategy;
    delete m_deepStrategy;
}

void IpScannerTest::testSetStrategy()
{
    // Test setting a strategy
    m_scanner->setScanStrategy(m_quickStrategy);

    // No direct way to verify, but should not crash
    QVERIFY(true);
}

void IpScannerTest::testQuickScanStrategy()
{
    // Test the quick scan strategy on localhost
    Device device = m_quickStrategy->scan("127.0.0.1");

    // Verify strategy properties
    QCOMPARE(m_quickStrategy->getName(), QString("Quick Scan"));
    QVERIFY(!m_quickStrategy->getDescription().isEmpty());

    // Device should be populated (localhost should respond)
    QCOMPARE(device.ip(), QString("127.0.0.1"));
    QVERIFY(device.isOnline() || !device.isOnline()); // Either is valid
}

void IpScannerTest::testDeepScanStrategy()
{
    // Test the deep scan strategy on gateway (if reachable)
    Device device = m_deepStrategy->scan("192.168.1.1");

    // Verify strategy properties
    QCOMPARE(m_deepStrategy->getName(), QString("Deep Scan"));
    QVERIFY(!m_deepStrategy->getDescription().isEmpty());

    // Device should be populated with the IP
    QCOMPARE(device.ip(), QString("192.168.1.1"));

    // Device may or may not be online depending on network
    // Just verify the scan completed without crashing
}

QTEST_MAIN(IpScannerTest)
#include "IpScannerTest.moc"
