#ifndef QUALITYSCORECALCULATOR_H
#define QUALITYSCORECALCULATOR_H

#include "../../models/NetworkMetrics.h"

/**
 * @brief Calculator for connection quality scoring
 *
 * Combines latency, packet loss, jitter, and availability metrics
 * to produce a comprehensive quality score (0-100) and rating.
 */
class QualityScoreCalculator {
public:
    /**
     * @brief Quality rating categories
     */
    enum QualityRating {
        EXCELLENT,  ///< 90-100 points
        GOOD,       ///< 70-89 points
        FAIR,       ///< 50-69 points
        POOR,       ///< 30-49 points
        CRITICAL    ///< 0-29 points
    };

    /**
     * @brief Comprehensive quality score result
     */
    struct QualityScore {
        double score;          ///< Overall score (0-100)
        QualityRating rating;  ///< Quality rating category
        QString description;   ///< Human-readable description

        QualityScore()
            : score(0.0), rating(CRITICAL), description("No data") {}
    };

    QualityScoreCalculator() = default;
    ~QualityScoreCalculator() = default;

    /**
     * @brief Calculate quality score from network metrics
     * @param metrics NetworkMetrics containing latency, jitter, packet loss
     * @return QualityScore with overall assessment
     */
    QualityScore calculate(const NetworkMetrics& metrics);

    /**
     * @brief Calculate quality score from individual metrics
     * @param latency Average latency in milliseconds
     * @param packetLoss Packet loss percentage (0-100)
     * @param jitter Jitter in milliseconds
     * @param availability Availability percentage (0-100)
     * @return QualityScore with overall assessment
     */
    QualityScore calculate(double latency, double packetLoss, double jitter, double availability = 100.0);

private:
    /**
     * @brief Calculate latency component score
     * @param latency Latency in milliseconds
     * @return Score contribution (0-100)
     */
    double calculateLatencyScore(double latency);

    /**
     * @brief Calculate packet loss component score
     * @param loss Packet loss percentage (0-100)
     * @return Score contribution (0-100)
     */
    double calculatePacketLossScore(double loss);

    /**
     * @brief Calculate jitter component score
     * @param jitter Jitter in milliseconds
     * @return Score contribution (0-100)
     */
    double calculateJitterScore(double jitter);

    /**
     * @brief Calculate availability component score
     * @param availability Availability percentage (0-100)
     * @return Score contribution (0-100)
     */
    double calculateAvailabilityScore(double availability);

    /**
     * @brief Determine quality rating from score
     * @param score Overall quality score (0-100)
     * @return QualityRating category
     */
    QualityRating determineRating(double score);

    /**
     * @brief Generate description from rating
     * @param rating Quality rating category
     * @return Human-readable description
     */
    QString generateDescription(QualityRating rating);

    // Scoring weights (must sum to 1.0)
    static constexpr double LATENCY_WEIGHT = 0.30;
    static constexpr double PACKET_LOSS_WEIGHT = 0.40;
    static constexpr double JITTER_WEIGHT = 0.20;
    static constexpr double AVAILABILITY_WEIGHT = 0.10;
};

#endif // QUALITYSCORECALCULATOR_H
