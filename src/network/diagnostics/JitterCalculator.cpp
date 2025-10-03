#include "JitterCalculator.h"
#include <cmath>

double JitterCalculator::calculate(const QVector<double>& rttValues) {
    if (rttValues.size() <= 1) {
        return 0.0;
    }

    double mean = calculateAverage(rttValues);
    return calculateStdDev(rttValues, mean);
}

QString JitterCalculator::getCalculatorName() const {
    return "JitterCalculator";
}

double JitterCalculator::calculateConsecutiveJitter(const QVector<double>& rttValues) {
    if (rttValues.size() <= 1) {
        return 0.0;
    }

    QVector<double> differences;
    differences.reserve(rttValues.size() - 1);

    // Calculate absolute differences between consecutive RTT values
    for (int i = 1; i < rttValues.size(); ++i) {
        double diff = std::abs(rttValues[i] - rttValues[i - 1]);
        differences.append(diff);
    }

    return calculateAverage(differences);
}

double JitterCalculator::calculateStdDev(const QVector<double>& values, double mean) {
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

double JitterCalculator::calculateAverage(const QVector<double>& values) {
    if (values.isEmpty()) {
        return 0.0;
    }

    double sum = 0.0;
    for (double value : values) {
        sum += value;
    }
    return sum / values.size();
}
