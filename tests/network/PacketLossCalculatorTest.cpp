#include <QtTest>
#include "network/diagnostics/PacketLossCalculator.h"

class PacketLossCalculatorTest : public QObject
{
    Q_OBJECT

private slots:
    void testCalculateNoLoss();
    void testCalculatePartialLoss();
    void testCalculateFullLoss();
    void testCalculateStatsFromCounts();
    void testCalculateStatsFromVector();
    void testDetectBurstPattern();
    void testDetectRandomPattern();
    void testEmptyVector();
};

void PacketLossCalculatorTest::testCalculateNoLoss()
{
    PacketLossCalculator calc;
    QVector<double> data = {1.0, 1.0, 1.0, 1.0, 1.0}; // All successful

    double loss = calc.calculate(data);
    QCOMPARE(loss, 0.0);
}

void PacketLossCalculatorTest::testCalculatePartialLoss()
{
    PacketLossCalculator calc;
    QVector<double> data = {1.0, 0.0, 1.0, 1.0, 0.0}; // 2 lost out of 5

    double loss = calc.calculate(data);
    QCOMPARE(loss, 40.0); // 40% loss
}

void PacketLossCalculatorTest::testCalculateFullLoss()
{
    PacketLossCalculator calc;
    QVector<double> data = {0.0, 0.0, 0.0, 0.0, 0.0}; // All lost

    double loss = calc.calculate(data);
    QCOMPARE(loss, 100.0);
}

void PacketLossCalculatorTest::testCalculateStatsFromCounts()
{
    PacketLossCalculator calc;

    PacketLossCalculator::PacketLossStats stats = calc.calculateStats(10, 8);

    QCOMPARE(stats.sent, 10);
    QCOMPARE(stats.received, 8);
    QCOMPARE(stats.lost, 2);
    QCOMPARE(stats.lossPercentage, 20.0);
}

void PacketLossCalculatorTest::testCalculateStatsFromVector()
{
    PacketLossCalculator calc;
    QVector<bool> successVector = {true, false, true, true, false};

    PacketLossCalculator::PacketLossStats stats = calc.calculateStats(successVector);

    QCOMPARE(stats.sent, 5);
    QCOMPARE(stats.received, 3);
    QCOMPARE(stats.lost, 2);
    QCOMPARE(stats.lossPercentage, 40.0);
}

void PacketLossCalculatorTest::testDetectBurstPattern()
{
    PacketLossCalculator calc;

    // 3 consecutive losses = burst
    QVector<bool> burstPattern = {true, false, false, false, true};
    QVERIFY(calc.detectBurstPattern(burstPattern));

    PacketLossCalculator::PacketLossStats stats = calc.calculateStats(burstPattern);
    QVERIFY(stats.isBurstLoss);
}

void PacketLossCalculatorTest::testDetectRandomPattern()
{
    PacketLossCalculator calc;

    // Scattered losses = random
    QVector<bool> randomPattern = {true, false, true, false, true};
    QVERIFY(!calc.detectBurstPattern(randomPattern));

    PacketLossCalculator::PacketLossStats stats = calc.calculateStats(randomPattern);
    QVERIFY(!stats.isBurstLoss);
    QVERIFY(stats.isRandomLoss);
}

void PacketLossCalculatorTest::testEmptyVector()
{
    PacketLossCalculator calc;
    QVector<double> data;

    double loss = calc.calculate(data);
    QCOMPARE(loss, 0.0);
}

QTEST_MAIN(PacketLossCalculatorTest)
#include "PacketLossCalculatorTest.moc"
