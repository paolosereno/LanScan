#include "StatisticsCalculator.h"
#include <algorithm>
#include <cmath>
#include <limits>

double StatisticsCalculator::calculateMean(const QVector<double>& values)
{
    if (values.isEmpty()) {
        return 0.0;
    }

    double sum = 0.0;
    for (double value : values) {
        sum += value;
    }

    return sum / values.size();
}

double StatisticsCalculator::calculateMedian(QVector<double> values)
{
    if (values.isEmpty()) {
        return 0.0;
    }

    std::sort(values.begin(), values.end());

    int size = values.size();
    if (size % 2 == 0) {
        return (values[size / 2 - 1] + values[size / 2]) / 2.0;
    } else {
        return values[size / 2];
    }
}

double StatisticsCalculator::calculateStdDev(const QVector<double>& values)
{
    if (values.size() < 2) {
        return 0.0;
    }

    double variance = calculateVariance(values);
    return std::sqrt(variance);
}

double StatisticsCalculator::calculateMin(const QVector<double>& values)
{
    if (values.isEmpty()) {
        return 0.0;
    }

    return *std::min_element(values.begin(), values.end());
}

double StatisticsCalculator::calculateMax(const QVector<double>& values)
{
    if (values.isEmpty()) {
        return 0.0;
    }

    return *std::max_element(values.begin(), values.end());
}

double StatisticsCalculator::calculateVariance(const QVector<double>& values)
{
    if (values.size() < 2) {
        return 0.0;
    }

    double mean = calculateMean(values);
    double sumSquaredDiff = 0.0;

    for (double value : values) {
        double diff = value - mean;
        sumSquaredDiff += diff * diff;
    }

    return sumSquaredDiff / (values.size() - 1);
}

double StatisticsCalculator::calculateJitter(const QVector<double>& latencies)
{
    if (latencies.size() < 2) {
        return 0.0;
    }

    QVector<double> differences;
    for (int i = 1; i < latencies.size(); ++i) {
        differences.append(std::abs(latencies[i] - latencies[i - 1]));
    }

    return calculateMean(differences);
}

double StatisticsCalculator::calculatePercentile(QVector<double> values, double percentile)
{
    if (values.isEmpty() || percentile < 0.0 || percentile > 100.0) {
        return 0.0;
    }

    std::sort(values.begin(), values.end());

    double index = (percentile / 100.0) * (values.size() - 1);
    int lowerIndex = std::floor(index);
    int upperIndex = std::ceil(index);

    if (lowerIndex == upperIndex) {
        return values[lowerIndex];
    }

    double weight = index - lowerIndex;
    return values[lowerIndex] * (1.0 - weight) + values[upperIndex] * weight;
}
