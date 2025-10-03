#ifndef JITTERCALCULATOR_H
#define JITTERCALCULATOR_H

#include "../../interfaces/IMetricsCalculator.h"
#include <QVector>

/**
 * @brief Calculator for jitter (latency variation)
 *
 * Jitter represents the variability in packet delay and is calculated
 * as the standard deviation of RTT values or the average of consecutive
 * RTT differences.
 */
class JitterCalculator : public IMetricsCalculator {
public:
    JitterCalculator() = default;
    ~JitterCalculator() override = default;

    /**
     * @brief Calculate jitter as standard deviation (IMetricsCalculator interface)
     * @param rttValues Vector of round-trip time values in milliseconds
     * @return Jitter value (standard deviation of latencies)
     */
    double calculate(const QVector<double>& rttValues) override;

    /**
     * @brief Get calculator name (IMetricsCalculator interface)
     * @return Calculator name
     */
    QString getCalculatorName() const override;

    /**
     * @brief Calculate consecutive jitter (difference between adjacent RTTs)
     *
     * This method calculates jitter as the average absolute difference
     * between consecutive measurements, which is useful for detecting
     * network instability.
     *
     * @param rttValues Vector of round-trip time values in milliseconds
     * @return Consecutive jitter value
     */
    double calculateConsecutiveJitter(const QVector<double>& rttValues);

private:
    /**
     * @brief Calculate standard deviation
     * @param values Input values
     * @param mean Mean of the values
     * @return Standard deviation
     */
    double calculateStdDev(const QVector<double>& values, double mean);

    /**
     * @brief Calculate average value
     * @param values Input values
     * @return Average value
     */
    double calculateAverage(const QVector<double>& values);
};

#endif // JITTERCALCULATOR_H
