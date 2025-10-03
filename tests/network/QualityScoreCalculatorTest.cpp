#include <QtTest>
#include "network/diagnostics/QualityScoreCalculator.h"
#include "models/NetworkMetrics.h"

class QualityScoreCalculatorTest : public QObject
{
    Q_OBJECT

private slots:
    void testExcellentQuality();
    void testGoodQuality();
    void testFairQuality();
    void testPoorQuality();
    void testCriticalQuality();
    void testLatencyScoring();
    void testPacketLossScoring();
    void testJitterScoring();
    void testWeightedScore();
};

void QualityScoreCalculatorTest::testExcellentQuality()
{
    QualityScoreCalculator calc;

    // Excellent: low latency, no packet loss, low jitter
    QualityScoreCalculator::QualityScore score = calc.calculate(15.0, 0.0, 3.0, 100.0);

    QVERIFY(score.score >= 90.0);
    QCOMPARE(score.rating, QualityScoreCalculator::EXCELLENT);
}

void QualityScoreCalculatorTest::testGoodQuality()
{
    QualityScoreCalculator calc;

    // Good: moderate latency, minimal loss
    QualityScoreCalculator::QualityScore score = calc.calculate(40.0, 0.5, 10.0, 100.0);

    QVERIFY(score.score >= 70.0 && score.score < 90.0);
    QCOMPARE(score.rating, QualityScoreCalculator::GOOD);
}

void QualityScoreCalculatorTest::testFairQuality()
{
    QualityScoreCalculator calc;

    // Fair: higher latency, some loss
    QualityScoreCalculator::QualityScore score = calc.calculate(80.0, 3.0, 15.0, 100.0);

    QVERIFY(score.score >= 50.0 && score.score < 70.0);
    QCOMPARE(score.rating, QualityScoreCalculator::FAIR);
}

void QualityScoreCalculatorTest::testPoorQuality()
{
    QualityScoreCalculator calc;

    // Poor: high latency, significant loss
    QualityScoreCalculator::QualityScore score = calc.calculate(150.0, 7.0, 30.0, 98.0);

    QVERIFY(score.score >= 30.0 && score.score < 50.0);
    QCOMPARE(score.rating, QualityScoreCalculator::POOR);
}

void QualityScoreCalculatorTest::testCriticalQuality()
{
    QualityScoreCalculator calc;

    // Critical: very high latency, high packet loss
    QualityScoreCalculator::QualityScore score = calc.calculate(250.0, 15.0, 60.0, 90.0);

    QVERIFY(score.score < 30.0);
    QCOMPARE(score.rating, QualityScoreCalculator::CRITICAL);
}

void QualityScoreCalculatorTest::testLatencyScoring()
{
    QualityScoreCalculator calc;

    // Test latency component only (no loss, no jitter)
    QualityScoreCalculator::QualityScore score1 = calc.calculate(10.0, 0.0, 0.0, 100.0);
    QualityScoreCalculator::QualityScore score2 = calc.calculate(250.0, 0.0, 0.0, 100.0);

    // Lower latency should give higher score
    QVERIFY(score1.score > score2.score);
}

void QualityScoreCalculatorTest::testPacketLossScoring()
{
    QualityScoreCalculator calc;

    // Test packet loss component only
    QualityScoreCalculator::QualityScore score1 = calc.calculate(20.0, 0.0, 5.0, 100.0);
    QualityScoreCalculator::QualityScore score2 = calc.calculate(20.0, 12.0, 5.0, 100.0);

    // Lower packet loss should give higher score
    QVERIFY(score1.score > score2.score);
}

void QualityScoreCalculatorTest::testJitterScoring()
{
    QualityScoreCalculator calc;

    // Test jitter component only
    QualityScoreCalculator::QualityScore score1 = calc.calculate(20.0, 0.0, 3.0, 100.0);
    QualityScoreCalculator::QualityScore score2 = calc.calculate(20.0, 0.0, 60.0, 100.0);

    // Lower jitter should give higher score
    QVERIFY(score1.score > score2.score);
}

void QualityScoreCalculatorTest::testWeightedScore()
{
    QualityScoreCalculator calc;

    // Verify score is between 0 and 100
    QualityScoreCalculator::QualityScore score = calc.calculate(100.0, 5.0, 20.0, 95.0);

    QVERIFY(score.score >= 0.0);
    QVERIFY(score.score <= 100.0);
}

QTEST_MAIN(QualityScoreCalculatorTest)
#include "QualityScoreCalculatorTest.moc"
