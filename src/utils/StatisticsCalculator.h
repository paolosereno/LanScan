#ifndef STATISTICSCALCULATOR_H
#define STATISTICSCALCULATOR_H

#include <QVector>

class StatisticsCalculator
{
public:
    static double calculateMean(const QVector<double>& values);
    static double calculateMedian(QVector<double> values);
    static double calculateStdDev(const QVector<double>& values);
    static double calculateMin(const QVector<double>& values);
    static double calculateMax(const QVector<double>& values);
    static double calculateVariance(const QVector<double>& values);
    static double calculateJitter(const QVector<double>& latencies);
    static double calculatePercentile(QVector<double> values, double percentile);

private:
    StatisticsCalculator() = delete;
};

#endif // STATISTICSCALCULATOR_H
