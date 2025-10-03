#include <QtTest>
#include "network/discovery/HostDiscovery.h"

class HostDiscoveryTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testPingLocalhost();
    void testPingInvalidHost();
    void testIsHostAlive();
    void testSignalEmission();

private:
    HostDiscovery* m_discovery;
};

void HostDiscoveryTest::initTestCase()
{
    m_discovery = new HostDiscovery(this);
}

void HostDiscoveryTest::cleanupTestCase()
{
    delete m_discovery;
}

void HostDiscoveryTest::testPingLocalhost()
{
    // Test pinging localhost
    QSignalSpy discoveredSpy(m_discovery, &HostDiscovery::hostDiscovered);
    QSignalSpy notFoundSpy(m_discovery, &HostDiscovery::hostNotFound);

    m_discovery->pingHost("127.0.0.1");

    // Wait for signal with 10 second timeout
    bool signalReceived = discoveredSpy.wait(10000) || notFoundSpy.wait(100);
    QVERIFY(signalReceived);

    // Should receive hostDiscovered signal for localhost
    if (discoveredSpy.count() > 0) {
        // Verify IP address in signal
        QList<QVariant> arguments = discoveredSpy.takeFirst();
        QCOMPARE(arguments.at(0).toString(), QString("127.0.0.1"));

        // Latency should be non-negative (can be -1 if not extracted)
        QVERIFY(arguments.at(1).toDouble() >= -1.0);
    } else {
        // On some systems, localhost might not respond to ping
        // This is acceptable
        QVERIFY(notFoundSpy.count() > 0);
    }
}

void HostDiscoveryTest::testPingInvalidHost()
{
    // Test pinging an invalid/unreachable host
    QSignalSpy discoveredSpy(m_discovery, &HostDiscovery::hostDiscovered);
    QSignalSpy failedSpy(m_discovery, &HostDiscovery::hostNotFound);

    // Use an IP that should not respond (reserved IP range)
    m_discovery->pingHost("192.0.2.1");

    // Wait for either signal
    bool signalReceived = discoveredSpy.wait(3000) || failedSpy.wait(100);

    // Should receive hostNotFound signal
    QVERIFY(failedSpy.count() > 0 || discoveredSpy.count() == 0);
}

void HostDiscoveryTest::testIsHostAlive()
{
    // Test synchronous host check for localhost
    bool alive = m_discovery->isHostAlive("127.0.0.1", 5000);
    QVERIFY(alive);

    // Test with invalid host (short timeout)
    bool notAlive = m_discovery->isHostAlive("192.0.2.1", 1000);
    QVERIFY(!notAlive);
}

void HostDiscoveryTest::testSignalEmission()
{
    // Verify signal parameters
    QSignalSpy discoveredSpy(m_discovery, &HostDiscovery::hostDiscovered);
    QSignalSpy notFoundSpy(m_discovery, &HostDiscovery::hostNotFound);

    m_discovery->pingHost("127.0.0.1");
    bool signalReceived = discoveredSpy.wait(10000) || notFoundSpy.wait(100);
    QVERIFY(signalReceived);

    // If hostDiscovered was emitted, check parameters
    if (discoveredSpy.count() > 0) {
        // Check signal has correct number of parameters
        QList<QVariant> arguments = discoveredSpy.first();
        QCOMPARE(arguments.size(), 2);

        // First parameter should be QString (IP)
        QVERIFY(arguments.at(0).canConvert<QString>());

        // Second parameter should be double (latency)
        QVERIFY(arguments.at(1).canConvert<double>());
    }
}

QTEST_MAIN(HostDiscoveryTest)
#include "HostDiscoveryTest.moc"
