#include "QualityScoreCalculator.h"
#include <algorithm>

QualityScoreCalculator::QualityScore QualityScoreCalculator::calculate(const NetworkMetrics& metrics) {
    return calculate(
        metrics.latencyAvg(),
        metrics.packetLoss(),
        metrics.jitter(),
        100.0  // Assume 100% availability if not tracked
    );
}

QualityScoreCalculator::QualityScore QualityScoreCalculator::calculate(
    double latency, double packetLoss, double jitter, double availability)
{
    QualityScore result;

    // Calculate component scores
    double latencyScore = calculateLatencyScore(latency);
    double packetLossScore = calculatePacketLossScore(packetLoss);
    double jitterScore = calculateJitterScore(jitter);
    double availabilityScore = calculateAvailabilityScore(availability);

    // Calculate weighted total score
    result.score = (latencyScore * LATENCY_WEIGHT) +
                   (packetLossScore * PACKET_LOSS_WEIGHT) +
                   (jitterScore * JITTER_WEIGHT) +
                   (availabilityScore * AVAILABILITY_WEIGHT);

    // Ensure score is within bounds
    result.score = std::max(0.0, std::min(100.0, result.score));

    // Determine rating and description
    result.rating = determineRating(result.score);
    result.description = generateDescription(result.rating);

    return result;
}

double QualityScoreCalculator::calculateLatencyScore(double latency) {
    // Latency scoring:
    // < 20ms:  100 points
    // < 50ms:  80 points
    // < 100ms: 60 points
    // < 200ms: 40 points
    // >= 200ms: 20 points

    if (latency < 20.0) {
        return 100.0;
    } else if (latency < 50.0) {
        // Linear interpolation between 20ms (100) and 50ms (80)
        return 100.0 - ((latency - 20.0) / 30.0) * 20.0;
    } else if (latency < 100.0) {
        // Linear interpolation between 50ms (80) and 100ms (60)
        return 80.0 - ((latency - 50.0) / 50.0) * 20.0;
    } else if (latency < 200.0) {
        // Linear interpolation between 100ms (60) and 200ms (40)
        return 60.0 - ((latency - 100.0) / 100.0) * 20.0;
    } else {
        // >= 200ms: 20 points with degradation
        return std::max(0.0, 20.0 - ((latency - 200.0) / 100.0) * 10.0);
    }
}

double QualityScoreCalculator::calculatePacketLossScore(double loss) {
    // Packet Loss scoring:
    // 0%:      100 points
    // < 1%:    80 points
    // < 5%:    50 points
    // < 10%:   20 points
    // >= 10%:  0 points

    if (loss <= 0.0) {
        return 100.0;
    } else if (loss < 1.0) {
        // Linear interpolation between 0% (100) and 1% (80)
        return 100.0 - (loss / 1.0) * 20.0;
    } else if (loss < 5.0) {
        // Linear interpolation between 1% (80) and 5% (50)
        return 80.0 - ((loss - 1.0) / 4.0) * 30.0;
    } else if (loss < 10.0) {
        // Linear interpolation between 5% (50) and 10% (20)
        return 50.0 - ((loss - 5.0) / 5.0) * 30.0;
    } else {
        // >= 10%: 0 points
        return 0.0;
    }
}

double QualityScoreCalculator::calculateJitterScore(double jitter) {
    // Jitter scoring:
    // < 5ms:   100 points
    // < 20ms:  70 points
    // < 50ms:  40 points
    // >= 50ms: 10 points

    if (jitter < 5.0) {
        return 100.0;
    } else if (jitter < 20.0) {
        // Linear interpolation between 5ms (100) and 20ms (70)
        return 100.0 - ((jitter - 5.0) / 15.0) * 30.0;
    } else if (jitter < 50.0) {
        // Linear interpolation between 20ms (70) and 50ms (40)
        return 70.0 - ((jitter - 20.0) / 30.0) * 30.0;
    } else {
        // >= 50ms: 10 points with degradation
        return std::max(0.0, 10.0 - ((jitter - 50.0) / 50.0) * 10.0);
    }
}

double QualityScoreCalculator::calculateAvailabilityScore(double availability) {
    // Availability scoring:
    // 100% uptime: 100 points
    // < 99%:       50 points
    // < 95%:       0 points

    if (availability >= 99.0) {
        // Linear interpolation between 99% (50) and 100% (100)
        return 50.0 + ((availability - 99.0) / 1.0) * 50.0;
    } else if (availability >= 95.0) {
        // Linear interpolation between 95% (0) and 99% (50)
        return ((availability - 95.0) / 4.0) * 50.0;
    } else {
        return 0.0;
    }
}

QualityScoreCalculator::QualityRating QualityScoreCalculator::determineRating(double score) {
    if (score >= 90.0) {
        return EXCELLENT;
    } else if (score >= 70.0) {
        return GOOD;
    } else if (score >= 50.0) {
        return FAIR;
    } else if (score >= 30.0) {
        return POOR;
    } else {
        return CRITICAL;
    }
}

QString QualityScoreCalculator::generateDescription(QualityRating rating) {
    switch (rating) {
        case EXCELLENT:
            return "Excellent connection quality";
        case GOOD:
            return "Good connection quality";
        case FAIR:
            return "Fair connection quality";
        case POOR:
            return "Poor connection quality";
        case CRITICAL:
            return "Critical connection issues";
        default:
            return "Unknown quality";
    }
}
