#include <QtTest>
#include "utils/StatisticsCalculator.h"

class StatisticsCalculatorTest : public QObject
{
    Q_OBJECT

private slots:
    void testCalculateMean();
    void testCalculateMedian_OddCount();
    void testCalculateMedian_EvenCount();
    void testCalculateMin();
    void testCalculateMax();
    void testCalculateStdDev();
    void testCalculateVariance();
    void testCalculateJitter();
    void testCalculatePercentile();
    void testEmptyVector();
};

void StatisticsCalculatorTest::testCalculateMean()
{
    QVector<double> values = {10.0, 20.0, 30.0, 40.0, 50.0};
    double mean = StatisticsCalculator::calculateMean(values);
    QCOMPARE(mean, 30.0);
}

void StatisticsCalculatorTest::testCalculateMedian_OddCount()
{
    QVector<double> values = {10.0, 30.0, 20.0, 50.0, 40.0};
    double median = StatisticsCalculator::calculateMedian(values);
    QCOMPARE(median, 30.0);
}

void StatisticsCalculatorTest::testCalculateMedian_EvenCount()
{
    QVector<double> values = {10.0, 20.0, 30.0, 40.0};
    double median = StatisticsCalculator::calculateMedian(values);
    QCOMPARE(median, 25.0);
}

void StatisticsCalculatorTest::testCalculateMin()
{
    QVector<double> values = {30.0, 10.0, 50.0, 20.0, 40.0};
    double min = StatisticsCalculator::calculateMin(values);
    QCOMPARE(min, 10.0);
}

void StatisticsCalculatorTest::testCalculateMax()
{
    QVector<double> values = {30.0, 10.0, 50.0, 20.0, 40.0};
    double max = StatisticsCalculator::calculateMax(values);
    QCOMPARE(max, 50.0);
}

void StatisticsCalculatorTest::testCalculateStdDev()
{
    QVector<double> values = {2.0, 4.0, 4.0, 4.0, 5.0, 5.0, 7.0, 9.0};
    double stdDev = StatisticsCalculator::calculateStdDev(values);
    QVERIFY(qAbs(stdDev - 2.138) < 0.01); // Sample standard deviation
}

void StatisticsCalculatorTest::testCalculateVariance()
{
    QVector<double> values = {2.0, 4.0, 4.0, 4.0, 5.0, 5.0, 7.0, 9.0};
    double variance = StatisticsCalculator::calculateVariance(values);
    QVERIFY(qAbs(variance - 4.571) < 0.01); // Sample variance
}

void StatisticsCalculatorTest::testCalculateJitter()
{
    QVector<double> latencies = {10.0, 12.0, 11.0, 13.0, 10.0};
    double jitter = StatisticsCalculator::calculateJitter(latencies);
    QVERIFY(jitter > 0.0);
    QVERIFY(jitter < 5.0);
}

void StatisticsCalculatorTest::testCalculatePercentile()
{
    QVector<double> values = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};

    double p50 = StatisticsCalculator::calculatePercentile(values, 50.0);
    QCOMPARE(p50, 5.5);

    double p95 = StatisticsCalculator::calculatePercentile(values, 95.0);
    QCOMPARE(p95, 9.55);

    double p0 = StatisticsCalculator::calculatePercentile(values, 0.0);
    QCOMPARE(p0, 1.0);

    double p100 = StatisticsCalculator::calculatePercentile(values, 100.0);
    QCOMPARE(p100, 10.0);
}

void StatisticsCalculatorTest::testEmptyVector()
{
    QVector<double> empty;

    QCOMPARE(StatisticsCalculator::calculateMean(empty), 0.0);
    QCOMPARE(StatisticsCalculator::calculateMedian(empty), 0.0);
    QCOMPARE(StatisticsCalculator::calculateMin(empty), 0.0);
    QCOMPARE(StatisticsCalculator::calculateMax(empty), 0.0);
    QCOMPARE(StatisticsCalculator::calculateStdDev(empty), 0.0);
    QCOMPARE(StatisticsCalculator::calculateVariance(empty), 0.0);
    QCOMPARE(StatisticsCalculator::calculateJitter(empty), 0.0);
}

QTEST_MAIN(StatisticsCalculatorTest)
#include "StatisticsCalculatorTest.moc"
