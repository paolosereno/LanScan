#include <QtTest>
#include "network/diagnostics/LatencyCalculator.h"

class LatencyCalculatorTest : public QObject
{
    Q_OBJECT

private slots:
    void testCalculateAverage();
    void testCalculateStats();
    void testCalculateMin();
    void testCalculateMax();
    void testCalculateMedian_Odd();
    void testCalculateMedian_Even();
    void testCalculateStdDev();
    void testEmptyVector();
    void testSingleValue();
};

void LatencyCalculatorTest::testCalculateAverage()
{
    LatencyCalculator calc;
    QVector<double> values = {10.0, 20.0, 30.0, 40.0, 50.0};

    double avg = calc.calculate(values);
    QCOMPARE(avg, 30.0);
}

void LatencyCalculatorTest::testCalculateStats()
{
    LatencyCalculator calc;
    QVector<double> values = {10.0, 20.0, 30.0, 40.0, 50.0};

    LatencyCalculator::LatencyStats stats = calc.calculateStats(values);

    QCOMPARE(stats.min, 10.0);
    QCOMPARE(stats.max, 50.0);
    QCOMPARE(stats.avg, 30.0);
    QCOMPARE(stats.median, 30.0);
    QVERIFY(stats.stdDev > 0.0);
}

void LatencyCalculatorTest::testCalculateMin()
{
    LatencyCalculator calc;
    QVector<double> values = {30.0, 10.0, 50.0, 20.0, 40.0};

    LatencyCalculator::LatencyStats stats = calc.calculateStats(values);
    QCOMPARE(stats.min, 10.0);
}

void LatencyCalculatorTest::testCalculateMax()
{
    LatencyCalculator calc;
    QVector<double> values = {30.0, 10.0, 50.0, 20.0, 40.0};

    LatencyCalculator::LatencyStats stats = calc.calculateStats(values);
    QCOMPARE(stats.max, 50.0);
}

void LatencyCalculatorTest::testCalculateMedian_Odd()
{
    LatencyCalculator calc;
    QVector<double> values = {10.0, 30.0, 20.0};

    LatencyCalculator::LatencyStats stats = calc.calculateStats(values);
    QCOMPARE(stats.median, 20.0);
}

void LatencyCalculatorTest::testCalculateMedian_Even()
{
    LatencyCalculator calc;
    QVector<double> values = {10.0, 20.0, 30.0, 40.0};

    LatencyCalculator::LatencyStats stats = calc.calculateStats(values);
    QCOMPARE(stats.median, 25.0);
}

void LatencyCalculatorTest::testCalculateStdDev()
{
    LatencyCalculator calc;
    QVector<double> values = {2.0, 4.0, 4.0, 4.0, 5.0, 5.0, 7.0, 9.0};

    LatencyCalculator::LatencyStats stats = calc.calculateStats(values);

    // Expected std dev ≈ 2.0
    QVERIFY(qAbs(stats.stdDev - 2.0) < 0.1);
}

void LatencyCalculatorTest::testEmptyVector()
{
    LatencyCalculator calc;
    QVector<double> values;

    double avg = calc.calculate(values);
    QCOMPARE(avg, 0.0);

    LatencyCalculator::LatencyStats stats = calc.calculateStats(values);
    QCOMPARE(stats.min, 0.0);
    QCOMPARE(stats.max, 0.0);
    QCOMPARE(stats.avg, 0.0);
}

void LatencyCalculatorTest::testSingleValue()
{
    LatencyCalculator calc;
    QVector<double> values = {42.0};

    LatencyCalculator::LatencyStats stats = calc.calculateStats(values);

    QCOMPARE(stats.min, 42.0);
    QCOMPARE(stats.max, 42.0);
    QCOMPARE(stats.avg, 42.0);
    QCOMPARE(stats.median, 42.0);
    QCOMPARE(stats.stdDev, 0.0);
}

QTEST_MAIN(LatencyCalculatorTest)
#include "LatencyCalculatorTest.moc"
