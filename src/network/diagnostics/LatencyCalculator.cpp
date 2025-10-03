#include "LatencyCalculator.h"
#include <algorithm>
#include <cmath>
#include <limits>

double LatencyCalculator::calculate(const QVector<double>& rttValues) {
    return calculateAverage(rttValues);
}

QString LatencyCalculator::getCalculatorName() const {
    return "LatencyCalculator";
}

LatencyCalculator::LatencyStats LatencyCalculator::calculateStats(const QVector<double>& rttValues) {
    LatencyStats stats;

    if (rttValues.isEmpty()) {
        return stats;
    }

    stats.min = calculateMin(rttValues);
    stats.max = calculateMax(rttValues);
    stats.avg = calculateAverage(rttValues);

    // Create sorted copy for median calculation
    QVector<double> sorted = rttValues;
    std::sort(sorted.begin(), sorted.end());
    stats.median = calculateMedian(sorted);

    stats.stdDev = calculateStdDev(rttValues, stats.avg);

    return stats;
}

double LatencyCalculator::calculateMin(const QVector<double>& values) {
    if (values.isEmpty()) {
        return 0.0;
    }

    double minVal = std::numeric_limits<double>::max();
    for (double value : values) {
        if (value < minVal) {
            minVal = value;
        }
    }
    return minVal;
}

double LatencyCalculator::calculateMax(const QVector<double>& values) {
    if (values.isEmpty()) {
        return 0.0;
    }

    double maxVal = std::numeric_limits<double>::lowest();
    for (double value : values) {
        if (value > maxVal) {
            maxVal = value;
        }
    }
    return maxVal;
}

double LatencyCalculator::calculateAverage(const QVector<double>& values) {
    if (values.isEmpty()) {
        return 0.0;
    }

    double sum = 0.0;
    for (double value : values) {
        sum += value;
    }
    return sum / values.size();
}

double LatencyCalculator::calculateMedian(const QVector<double>& sorted) {
    if (sorted.isEmpty()) {
        return 0.0;
    }

    int size = sorted.size();
    if (size % 2 == 0) {
        // Even number of elements: average of two middle values
        return (sorted[size / 2 - 1] + sorted[size / 2]) / 2.0;
    } else {
        // Odd number of elements: middle value
        return sorted[size / 2];
    }
}

double LatencyCalculator::calculateStdDev(const QVector<double>& values, double mean) {
    if (values.size() <= 1) {
        return 0.0;
    }

    double sumSquaredDiff = 0.0;
    for (double value : values) {
        double diff = value - mean;
        sumSquaredDiff += diff * diff;
    }

    // Using sample standard deviation (n-1)
    return std::sqrt(sumSquaredDiff / (values.size() - 1));
}
