#include <QtTest>
#include <QSignalSpy>
#include "diagnostics/TraceRouteService.h"
#include "models/TraceRouteHop.h"

class TraceRouteServiceTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // TraceRouteHop tests
    void testHopConstruction();
    void testHopRttCalculations();
    void testHopTimeout();
    void testHopToString();
    void testHopEquality();

    // TraceRouteService tests
    void testServiceConstruction();
    void testTraceRouteToLocalhost();
    void testTraceRouteToGateway();
    void testInvalidTarget();
    void testCancelTrace();
    void testMultipleTraces();

private:
    TraceRouteService* m_service;
};

void TraceRouteServiceTest::initTestCase()
{
    qDebug() << "Starting TraceRouteService tests...";
}

void TraceRouteServiceTest::cleanupTestCase()
{
    qDebug() << "TraceRouteService tests completed.";
}

void TraceRouteServiceTest::init()
{
    m_service = new TraceRouteService();
}

void TraceRouteServiceTest::cleanup()
{
    delete m_service;
    m_service = nullptr;
}

void TraceRouteServiceTest::testHopConstruction()
{
    TraceRouteHop hop1;
    QCOMPARE(hop1.hopNumber(), 0);
    QVERIFY(hop1.ipAddress().isEmpty());
    QVERIFY(hop1.hostname().isEmpty());
    QVERIFY(!hop1.isTimeout());

    TraceRouteHop hop2(1, "192.168.1.1", "gateway");
    QCOMPARE(hop2.hopNumber(), 1);
    QCOMPARE(hop2.ipAddress(), QString("192.168.1.1"));
    QCOMPARE(hop2.hostname(), QString("gateway"));
    QVERIFY(!hop2.isTimeout());
}

void TraceRouteServiceTest::testHopRttCalculations()
{
    TraceRouteHop hop(1, "192.168.1.1");

    // Empty hop
    QCOMPARE(hop.minRtt(), 0.0);
    QCOMPARE(hop.maxRtt(), 0.0);
    QCOMPARE(hop.averageRtt(), 0.0);
    QVERIFY(!hop.hasValidRtt());

    // Add RTT values
    hop.addRtt(1.5);
    hop.addRtt(2.1);
    hop.addRtt(1.8);

    QCOMPARE(hop.rttList().size(), 3);
    QCOMPARE(hop.minRtt(), 1.5);
    QCOMPARE(hop.maxRtt(), 2.1);
    QVERIFY(qAbs(hop.averageRtt() - 1.8) < 0.01); // ~1.8
    QVERIFY(hop.hasValidRtt());

    // Clear RTT
    hop.clearRtt();
    QCOMPARE(hop.rttList().size(), 0);
    QVERIFY(!hop.hasValidRtt());
}

void TraceRouteServiceTest::testHopTimeout()
{
    TraceRouteHop hop(2, "");
    hop.setTimeout(true);

    QVERIFY(hop.isTimeout());
    QVERIFY(!hop.hasValidRtt());

    QString str = hop.toString();
    QVERIFY(str.contains("*"));
}

void TraceRouteServiceTest::testHopToString()
{
    TraceRouteHop hop1(1, "192.168.1.1");
    hop1.addRtt(1.5);
    hop1.addRtt(2.0);

    QString str1 = hop1.toString();
    QVERIFY(str1.contains("1"));
    QVERIFY(str1.contains("192.168.1.1"));
    QVERIFY(str1.contains("1.5 ms"));
    QVERIFY(str1.contains("2.0 ms"));

    TraceRouteHop hop2(2, "10.0.0.1", "gateway.local");
    hop2.addRtt(5.3);

    QString str2 = hop2.toString();
    QVERIFY(str2.contains("2"));
    QVERIFY(str2.contains("gateway.local"));
    QVERIFY(str2.contains("10.0.0.1"));
    QVERIFY(str2.contains("5.3 ms"));

    // Timeout hop
    TraceRouteHop hop3(3, "");
    hop3.setTimeout(true);

    QString str3 = hop3.toString();
    QVERIFY(str3.contains("3"));
    QVERIFY(str3.contains("*"));
}

void TraceRouteServiceTest::testHopEquality()
{
    TraceRouteHop hop1(1, "192.168.1.1", "gateway");
    hop1.addRtt(1.5);

    TraceRouteHop hop2(1, "192.168.1.1", "gateway");
    hop2.addRtt(1.5);

    QVERIFY(hop1 == hop2);

    TraceRouteHop hop3(2, "192.168.1.1", "gateway");
    hop3.addRtt(1.5);

    QVERIFY(!(hop1 == hop3));
}

void TraceRouteServiceTest::testServiceConstruction()
{
    QVERIFY(m_service != nullptr);
    QVERIFY(!m_service->isRunning());
    QVERIFY(m_service->target().isEmpty());
    QVERIFY(m_service->hops().isEmpty());
}

void TraceRouteServiceTest::testTraceRouteToLocalhost()
{
    QSignalSpy spyHop(m_service, &TraceRouteService::hopDiscovered);
    QSignalSpy spyComplete(m_service, &TraceRouteService::traceCompleted);
    QSignalSpy spyError(m_service, &TraceRouteService::traceError);

    bool started = m_service->traceRoute("127.0.0.1", 5, 2000);
    QVERIFY(started);
    QVERIFY(m_service->isRunning());
    QCOMPARE(m_service->target(), QString("127.0.0.1"));

    // Wait for completion (localhost should be quick)
    QVERIFY(spyComplete.wait(10000) || spyError.wait(10000));

    // Should have at least 1 hop (localhost)
    if (spyComplete.count() > 0) {
        QList<TraceRouteHop> hops = qvariant_cast<QList<TraceRouteHop>>(spyComplete.at(0).at(0));
        QVERIFY(hops.size() >= 1);
        qDebug() << "Discovered" << hops.size() << "hops to localhost";

        for (const TraceRouteHop& hop : hops) {
            qDebug() << hop.toString();
        }
    } else {
        qDebug() << "Traceroute error (may require admin privileges):"
                 << spyError.at(0).at(0).toString();
    }

    QVERIFY(!m_service->isRunning());
}

void TraceRouteServiceTest::testTraceRouteToGateway()
{
    QSignalSpy spyHop(m_service, &TraceRouteService::hopDiscovered);
    QSignalSpy spyComplete(m_service, &TraceRouteService::traceCompleted);
    QSignalSpy spyError(m_service, &TraceRouteService::traceError);
    QSignalSpy spyProgress(m_service, &TraceRouteService::progressUpdated);

    // Try to trace to common gateway (adjust if needed)
    bool started = m_service->traceRoute("192.168.1.1", 5, 2000);
    QVERIFY(started);

    // Wait for completion
    QVERIFY(spyComplete.wait(15000) || spyError.wait(15000));

    if (spyComplete.count() > 0) {
        QVERIFY(spyHop.count() >= 1);
        QVERIFY(spyProgress.count() >= 1);

        QList<TraceRouteHop> hops = qvariant_cast<QList<TraceRouteHop>>(spyComplete.at(0).at(0));
        qDebug() << "Discovered" << hops.size() << "hops to gateway";

        for (const TraceRouteHop& hop : hops) {
            qDebug() << hop.toString();
            QVERIFY(hop.hopNumber() > 0);
        }
    } else {
        qDebug() << "Traceroute to gateway failed (network dependent):"
                 << spyError.at(0).at(0).toString();
    }
}

void TraceRouteServiceTest::testInvalidTarget()
{
    // Empty target
    bool started = m_service->traceRoute("", 5, 2000);
    QVERIFY(!started);

    // Cannot start while running
    QSignalSpy spyComplete(m_service, &TraceRouteService::traceCompleted);
    QSignalSpy spyError(m_service, &TraceRouteService::traceError);

    m_service->traceRoute("127.0.0.1", 5, 2000);
    bool startedAgain = m_service->traceRoute("8.8.8.8", 5, 2000);
    QVERIFY(!startedAgain);

    // Wait for first trace to complete
    QVERIFY(spyComplete.wait(10000) || spyError.wait(10000));
}

void TraceRouteServiceTest::testCancelTrace()
{
    QSignalSpy spyComplete(m_service, &TraceRouteService::traceCompleted);

    // Start a trace with many hops
    bool started = m_service->traceRoute("8.8.8.8", 30, 5000);
    QVERIFY(started);
    QVERIFY(m_service->isRunning());

    // Cancel immediately
    QTest::qWait(100);
    m_service->cancel();

    // Wait a bit for process to stop
    QTest::qWait(500);
    QVERIFY(!m_service->isRunning());
}

void TraceRouteServiceTest::testMultipleTraces()
{
    QSignalSpy spyComplete(m_service, &TraceRouteService::traceCompleted);
    QSignalSpy spyError(m_service, &TraceRouteService::traceError);

    // First trace
    m_service->traceRoute("127.0.0.1", 3, 2000);
    QVERIFY(spyComplete.wait(10000) || spyError.wait(10000));

    spyComplete.clear();
    spyError.clear();

    // Second trace
    m_service->traceRoute("192.168.1.1", 3, 2000);
    QVERIFY(spyComplete.wait(15000) || spyError.wait(15000));

    // Both traces should complete without crashes
    QVERIFY(!m_service->isRunning());
}

QTEST_MAIN(TraceRouteServiceTest)
#include "TraceRouteServiceTest.moc"
