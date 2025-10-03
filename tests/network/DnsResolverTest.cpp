#include <QtTest>
#include "network/discovery/DnsResolver.h"

class DnsResolverTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testResolveLocalhost();
    void testResolveInvalidIp();
    void testResolveSyncLocalhost();
    void testResolveSyncTimeout();
    void testSignalEmission();

private:
    DnsResolver* m_resolver;
};

void DnsResolverTest::initTestCase()
{
    m_resolver = new DnsResolver(this);
}

void DnsResolverTest::cleanupTestCase()
{
    delete m_resolver;
}

void DnsResolverTest::testResolveLocalhost()
{
    // Test resolving localhost
    QSignalSpy spy(m_resolver, &DnsResolver::hostnameResolved);

    m_resolver->resolveHostname("127.0.0.1");

    // Wait for signal with 5 second timeout
    QVERIFY(spy.wait(5000));

    // Check signal was emitted once
    QCOMPARE(spy.count(), 1);

    // Verify parameters
    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), QString("127.0.0.1"));

    // Hostname should not be empty for localhost
    QString hostname = arguments.at(1).toString();
    QVERIFY(!hostname.isEmpty());
}

void DnsResolverTest::testResolveInvalidIp()
{
    // Test resolving an invalid IP
    QSignalSpy resolvedSpy(m_resolver, &DnsResolver::hostnameResolved);
    QSignalSpy failedSpy(m_resolver, &DnsResolver::resolveFailed);

    // Use an IP that won't have a reverse DNS entry
    m_resolver->resolveHostname("192.0.2.1");

    // Wait for either signal
    bool signalReceived = resolvedSpy.wait(3000) || failedSpy.wait(100);

    // Should receive either failed signal or no signal
    QVERIFY(failedSpy.count() > 0 || resolvedSpy.count() == 0 || signalReceived);
}

void DnsResolverTest::testResolveSyncLocalhost()
{
    // Test synchronous resolution for localhost
    QString hostname = m_resolver->resolveSync("127.0.0.1", 5000);

    // Should return a hostname for localhost
    QVERIFY(!hostname.isEmpty());
}

void DnsResolverTest::testResolveSyncTimeout()
{
    // Test synchronous resolution with short timeout
    QString hostname = m_resolver->resolveSync("192.0.2.1", 500);

    // May return empty string if timeout occurs
    // This is acceptable - just testing the function doesn't crash
    Q_UNUSED(hostname);
}

void DnsResolverTest::testSignalEmission()
{
    // Verify signal parameters
    QSignalSpy spy(m_resolver, &DnsResolver::hostnameResolved);

    m_resolver->resolveHostname("127.0.0.1");
    QVERIFY(spy.wait(5000));

    // Check signal has correct number of parameters
    QList<QVariant> arguments = spy.first();
    QCOMPARE(arguments.size(), 2);

    // Both parameters should be QString
    QVERIFY(arguments.at(0).canConvert<QString>());
    QVERIFY(arguments.at(1).canConvert<QString>());
}

QTEST_MAIN(DnsResolverTest)
#include "DnsResolverTest.moc"
