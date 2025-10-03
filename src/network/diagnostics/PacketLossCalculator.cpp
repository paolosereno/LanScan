#include "PacketLossCalculator.h"
#include <algorithm>

double PacketLossCalculator::calculate(const QVector<double>& data) {
    if (data.isEmpty()) {
        return 0.0;
    }

    int received = 0;
    for (double value : data) {
        if (value > 0.0) {
            received++;
        }
    }

    return calculateLossPercentage(data.size(), received);
}

QString PacketLossCalculator::getCalculatorName() const {
    return "PacketLossCalculator";
}

PacketLossCalculator::PacketLossStats PacketLossCalculator::calculateStats(int sent, int received) {
    PacketLossStats stats;

    if (sent <= 0) {
        return stats;
    }

    stats.sent = sent;
    stats.received = std::max(0, std::min(received, sent));
    stats.lost = sent - stats.received;
    stats.lossPercentage = calculateLossPercentage(sent, stats.received);

    // Without detailed packet sequence, we can't determine burst vs random
    stats.isBurstLoss = false;
    stats.isRandomLoss = stats.lost > 0;

    return stats;
}

PacketLossCalculator::PacketLossStats PacketLossCalculator::calculateStats(const QVector<bool>& successVector) {
    PacketLossStats stats;

    if (successVector.isEmpty()) {
        return stats;
    }

    stats.sent = successVector.size();
    stats.received = 0;

    for (bool success : successVector) {
        if (success) {
            stats.received++;
        }
    }

    stats.lost = stats.sent - stats.received;
    stats.lossPercentage = calculateLossPercentage(stats.sent, stats.received);

    // Determine loss pattern
    if (stats.lost > 0) {
        stats.isBurstLoss = detectBurstPattern(successVector);
        stats.isRandomLoss = !stats.isBurstLoss;
    }

    return stats;
}

bool PacketLossCalculator::detectBurstPattern(const QVector<bool>& successVector) {
    int maxConsecutive = countConsecutiveLosses(successVector);
    // Burst pattern: 3 or more consecutive losses
    return maxConsecutive >= 3;
}

int PacketLossCalculator::countConsecutiveLosses(const QVector<bool>& successVector) {
    int maxConsecutive = 0;
    int currentConsecutive = 0;

    for (bool success : successVector) {
        if (!success) {
            currentConsecutive++;
            maxConsecutive = std::max(maxConsecutive, currentConsecutive);
        } else {
            currentConsecutive = 0;
        }
    }

    return maxConsecutive;
}

double PacketLossCalculator::calculateLossPercentage(int sent, int received) {
    if (sent <= 0) {
        return 0.0;
    }

    int lost = sent - received;
    return (static_cast<double>(lost) / sent) * 100.0;
}
