#include <QtTest>
#include "network/diagnostics/JitterCalculator.h"

class JitterCalculatorTest : public QObject
{
    Q_OBJECT

private slots:
    void testCalculateJitter();
    void testCalculateConsecutiveJitter();
    void testEmptyVector();
    void testSingleValue();
    void testConstantValues();
    void testHighVariability();
};

void JitterCalculatorTest::testCalculateJitter()
{
    JitterCalculator calc;
    QVector<double> values = {10.0, 15.0, 12.0, 18.0, 14.0};

    double jitter = calc.calculate(values);

    // Jitter should be > 0 for varying values
    QVERIFY(jitter > 0.0);
}

void JitterCalculatorTest::testCalculateConsecutiveJitter()
{
    JitterCalculator calc;
    QVector<double> values = {10.0, 15.0, 12.0, 18.0, 14.0};

    // Consecutive differences: |15-10|=5, |12-15|=3, |18-12|=6, |14-18|=4
    // Average: (5+3+6+4)/4 = 4.5
    double jitter = calc.calculateConsecutiveJitter(values);

    QCOMPARE(jitter, 4.5);
}

void JitterCalculatorTest::testEmptyVector()
{
    JitterCalculator calc;
    QVector<double> values;

    double jitter = calc.calculate(values);
    QCOMPARE(jitter, 0.0);

    double consJitter = calc.calculateConsecutiveJitter(values);
    QCOMPARE(consJitter, 0.0);
}

void JitterCalculatorTest::testSingleValue()
{
    JitterCalculator calc;
    QVector<double> values = {42.0};

    double jitter = calc.calculate(values);
    QCOMPARE(jitter, 0.0);

    double consJitter = calc.calculateConsecutiveJitter(values);
    QCOMPARE(consJitter, 0.0);
}

void JitterCalculatorTest::testConstantValues()
{
    JitterCalculator calc;
    QVector<double> values = {20.0, 20.0, 20.0, 20.0, 20.0};

    double jitter = calc.calculate(values);
    QCOMPARE(jitter, 0.0);

    double consJitter = calc.calculateConsecutiveJitter(values);
    QCOMPARE(consJitter, 0.0);
}

void JitterCalculatorTest::testHighVariability()
{
    JitterCalculator calc;
    QVector<double> values = {10.0, 50.0, 10.0, 50.0, 10.0};

    double jitter = calc.calculate(values);

    // High variability should result in high jitter
    QVERIFY(jitter > 15.0);
}

QTEST_MAIN(JitterCalculatorTest)
#include "JitterCalculatorTest.moc"
