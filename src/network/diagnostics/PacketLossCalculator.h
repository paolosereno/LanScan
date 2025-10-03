#ifndef PACKETLOSSCALCULATOR_H
#define PACKETLOSSCALCULATOR_H

#include "../../interfaces/IMetricsCalculator.h"
#include <QVector>

/**
 * @brief Calculator for packet loss statistics
 *
 * Analyzes packet loss patterns to determine percentage, type
 * (burst vs random), and severity of connection issues.
 */
class PacketLossCalculator : public IMetricsCalculator {
public:
    /**
     * @brief Detailed packet loss statistics
     */
    struct PacketLossStats {
        int sent;               ///< Total packets sent
        int received;           ///< Total packets received
        int lost;               ///< Total packets lost
        double lossPercentage;  ///< Loss percentage (0-100)
        bool isBurstLoss;       ///< True if consecutive losses detected
        bool isRandomLoss;      ///< True if scattered losses detected

        PacketLossStats()
            : sent(0), received(0), lost(0), lossPercentage(0.0),
              isBurstLoss(false), isRandomLoss(false) {}
    };

    PacketLossCalculator() = default;
    ~PacketLossCalculator() override = default;

    /**
     * @brief Calculate packet loss percentage (IMetricsCalculator interface)
     *
     * Expects data vector where positive values indicate successful packets
     * and non-positive values indicate lost packets.
     *
     * @param data Vector of packet results (>0 = success, <=0 = loss)
     * @return Packet loss percentage (0-100)
     */
    double calculate(const QVector<double>& data) override;

    /**
     * @brief Get calculator name (IMetricsCalculator interface)
     * @return Calculator name
     */
    QString getCalculatorName() const override;

    /**
     * @brief Calculate comprehensive packet loss statistics
     * @param sent Total number of packets sent
     * @param received Total number of packets received
     * @return PacketLossStats containing detailed loss information
     */
    PacketLossStats calculateStats(int sent, int received);

    /**
     * @brief Calculate packet loss statistics from success vector
     * @param successVector Vector where true = received, false = lost
     * @return PacketLossStats containing detailed loss information
     */
    PacketLossStats calculateStats(const QVector<bool>& successVector);

    /**
     * @brief Detect if packet loss follows a burst pattern
     *
     * Burst pattern is defined as 3 or more consecutive packet losses.
     *
     * @param successVector Vector where true = received, false = lost
     * @return True if burst pattern detected
     */
    bool detectBurstPattern(const QVector<bool>& successVector);

private:
    /**
     * @brief Count consecutive losses in the vector
     * @param successVector Vector where true = received, false = lost
     * @return Maximum number of consecutive losses
     */
    int countConsecutiveLosses(const QVector<bool>& successVector);

    /**
     * @brief Calculate loss percentage
     * @param sent Total packets sent
     * @param received Total packets received
     * @return Loss percentage (0-100)
     */
    double calculateLossPercentage(int sent, int received);
};

#endif // PACKETLOSSCALCULATOR_H
