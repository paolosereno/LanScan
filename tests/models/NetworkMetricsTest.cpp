#include <QtTest>
#include "models/NetworkMetrics.h"

class NetworkMetricsTest : public QObject
{
    Q_OBJECT

private slots:
    void testDefaultConstructor();
    void testSettersAndGetters();
    void testCalculateQualityScore_Excellent();
    void testCalculateQualityScore_Good();
    void testCalculateQualityScore_Fair();
    void testCalculateQualityScore_Poor();
    void testCalculateQualityScore_Critical();
    void testQualityScoreString();
    void testIsValid();
};

void NetworkMetricsTest::testDefaultConstructor()
{
    NetworkMetrics metrics;
    QCOMPARE(metrics.latencyMin(), 0.0);
    QCOMPARE(metrics.latencyAvg(), 0.0);
    QCOMPARE(metrics.latencyMax(), 0.0);
    QCOMPARE(metrics.jitter(), 0.0);
    QCOMPARE(metrics.packetLoss(), 0.0);
    QCOMPARE(metrics.qualityScore(), NetworkMetrics::Critical);
}

void NetworkMetricsTest::testSettersAndGetters()
{
    NetworkMetrics metrics;

    metrics.setLatencyMin(5.0);
    QCOMPARE(metrics.latencyMin(), 5.0);

    metrics.setLatencyAvg(10.0);
    QCOMPARE(metrics.latencyAvg(), 10.0);

    metrics.setLatencyMax(15.0);
    QCOMPARE(metrics.latencyMax(), 15.0);

    metrics.setLatencyMedian(9.5);
    QCOMPARE(metrics.latencyMedian(), 9.5);

    metrics.setJitter(1.0);
    QCOMPARE(metrics.jitter(), 1.0);

    metrics.setPacketLoss(2.5);
    QCOMPARE(metrics.packetLoss(), 2.5);

    QDateTime now = QDateTime::currentDateTime();
    metrics.setTimestamp(now);
    QCOMPARE(metrics.timestamp(), now);
}

void NetworkMetricsTest::testCalculateQualityScore_Excellent()
{
    NetworkMetrics metrics;
    metrics.setLatencyAvg(15.0);
    metrics.setJitter(1.5);
    metrics.setPacketLoss(3.0);
    metrics.calculateQualityScore();

    QCOMPARE(metrics.qualityScore(), NetworkMetrics::Excellent);
}

void NetworkMetricsTest::testCalculateQualityScore_Good()
{
    NetworkMetrics metrics;
    metrics.setLatencyAvg(45.0);
    metrics.setJitter(4.0);
    metrics.setPacketLoss(8.0);
    metrics.calculateQualityScore();

    QCOMPARE(metrics.qualityScore(), NetworkMetrics::Good);
}

void NetworkMetricsTest::testCalculateQualityScore_Fair()
{
    NetworkMetrics metrics;
    metrics.setLatencyAvg(80.0);
    metrics.setJitter(8.0);
    metrics.setPacketLoss(15.0);
    metrics.calculateQualityScore();

    QCOMPARE(metrics.qualityScore(), NetworkMetrics::Fair);
}

void NetworkMetricsTest::testCalculateQualityScore_Poor()
{
    NetworkMetrics metrics;
    metrics.setLatencyAvg(150.0);
    metrics.setJitter(15.0);
    metrics.setPacketLoss(25.0);
    metrics.calculateQualityScore();

    QCOMPARE(metrics.qualityScore(), NetworkMetrics::Poor);
}

void NetworkMetricsTest::testCalculateQualityScore_Critical()
{
    NetworkMetrics metrics;
    metrics.setLatencyAvg(250.0);
    metrics.setJitter(25.0);
    metrics.setPacketLoss(40.0);
    metrics.calculateQualityScore();

    QCOMPARE(metrics.qualityScore(), NetworkMetrics::Critical);
}

void NetworkMetricsTest::testQualityScoreString()
{
    NetworkMetrics metrics;

    metrics.setQualityScore(NetworkMetrics::Excellent);
    QCOMPARE(metrics.qualityScoreString(), QString("Excellent"));

    metrics.setQualityScore(NetworkMetrics::Good);
    QCOMPARE(metrics.qualityScoreString(), QString("Good"));

    metrics.setQualityScore(NetworkMetrics::Fair);
    QCOMPARE(metrics.qualityScoreString(), QString("Fair"));

    metrics.setQualityScore(NetworkMetrics::Poor);
    QCOMPARE(metrics.qualityScoreString(), QString("Poor"));

    metrics.setQualityScore(NetworkMetrics::Critical);
    QCOMPARE(metrics.qualityScoreString(), QString("Critical"));
}

void NetworkMetricsTest::testIsValid()
{
    NetworkMetrics metrics;
    QVERIFY(!metrics.isValid());

    metrics.setLatencyAvg(10.0);
    QVERIFY(metrics.isValid());

    NetworkMetrics metrics2;
    metrics2.setPacketLoss(5.0);
    QVERIFY(metrics2.isValid());
}

QTEST_MAIN(NetworkMetricsTest)
#include "NetworkMetricsTest.moc"
