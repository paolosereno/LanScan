#ifndef LATENCYCALCULATOR_H
#define LATENCYCALCULATOR_H

#include "../../interfaces/IMetricsCalculator.h"
#include <QVector>

/**
 * @brief Calculator for latency statistics
 *
 * Computes comprehensive latency metrics including min, max,
 * average, median, and standard deviation from RTT values.
 */
class LatencyCalculator : public IMetricsCalculator {
public:
    /**
     * @brief Detailed latency statistics
     */
    struct LatencyStats {
        double min;      ///< Minimum latency (ms)
        double max;      ///< Maximum latency (ms)
        double avg;      ///< Average latency (ms)
        double median;   ///< Median latency (ms)
        double stdDev;   ///< Standard deviation (ms)

        LatencyStats()
            : min(0.0), max(0.0), avg(0.0), median(0.0), stdDev(0.0) {}
    };

    LatencyCalculator() = default;
    ~LatencyCalculator() override = default;

    /**
     * @brief Calculate average latency (IMetricsCalculator interface)
     * @param rttValues Vector of round-trip time values in milliseconds
     * @return Average latency
     */
    double calculate(const QVector<double>& rttValues) override;

    /**
     * @brief Get calculator name (IMetricsCalculator interface)
     * @return Calculator name
     */
    QString getCalculatorName() const override;

    /**
     * @brief Calculate comprehensive latency statistics
     * @param rttValues Vector of round-trip time values in milliseconds
     * @return LatencyStats containing all metrics
     */
    LatencyStats calculateStats(const QVector<double>& rttValues);

private:
    /**
     * @brief Calculate minimum value
     * @param values Input values
     * @return Minimum value
     */
    double calculateMin(const QVector<double>& values);

    /**
     * @brief Calculate maximum value
     * @param values Input values
     * @return Maximum value
     */
    double calculateMax(const QVector<double>& values);

    /**
     * @brief Calculate average value
     * @param values Input values
     * @return Average value
     */
    double calculateAverage(const QVector<double>& values);

    /**
     * @brief Calculate median value
     * @param sorted Sorted input values
     * @return Median value
     */
    double calculateMedian(const QVector<double>& sorted);

    /**
     * @brief Calculate standard deviation
     * @param values Input values
     * @param mean Mean of the values
     * @return Standard deviation
     */
    double calculateStdDev(const QVector<double>& values, double mean);
};

#endif // LATENCYCALCULATOR_H
