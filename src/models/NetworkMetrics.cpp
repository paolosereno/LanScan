#include "NetworkMetrics.h"

NetworkMetrics::NetworkMetrics()
    : m_latencyMin(0.0)
    , m_latencyAvg(0.0)
    , m_latencyMax(0.0)
    , m_latencyMedian(0.0)
    , m_jitter(0.0)
    , m_packetLoss(0.0)
    , m_qualityScore(Critical)
{
}

// Getters
double NetworkMetrics::latencyMin() const
{
    return m_latencyMin;
}

double NetworkMetrics::latencyAvg() const
{
    return m_latencyAvg;
}

double NetworkMetrics::latencyMax() const
{
    return m_latencyMax;
}

double NetworkMetrics::latencyMedian() const
{
    return m_latencyMedian;
}

double NetworkMetrics::jitter() const
{
    return m_jitter;
}

double NetworkMetrics::packetLoss() const
{
    return m_packetLoss;
}

NetworkMetrics::QualityScore NetworkMetrics::qualityScore() const
{
    return m_qualityScore;
}

QString NetworkMetrics::qualityScoreString() const
{
    switch (m_qualityScore) {
        case Excellent: return "Excellent";
        case Good: return "Good";
        case Fair: return "Fair";
        case Poor: return "Poor";
        case Critical: return "Critical";
        default: return "Unknown";
    }
}

QDateTime NetworkMetrics::timestamp() const
{
    return m_timestamp;
}

// Setters
void NetworkMetrics::setLatencyMin(double latency)
{
    m_latencyMin = latency;
}

void NetworkMetrics::setLatencyAvg(double latency)
{
    m_latencyAvg = latency;
}

void NetworkMetrics::setLatencyMax(double latency)
{
    m_latencyMax = latency;
}

void NetworkMetrics::setLatencyMedian(double latency)
{
    m_latencyMedian = latency;
}

void NetworkMetrics::setJitter(double jitter)
{
    m_jitter = jitter;
}

void NetworkMetrics::setPacketLoss(double loss)
{
    m_packetLoss = loss;
}

void NetworkMetrics::setQualityScore(QualityScore score)
{
    m_qualityScore = score;
}

void NetworkMetrics::setTimestamp(const QDateTime& timestamp)
{
    m_timestamp = timestamp;
}

// Utility methods
void NetworkMetrics::calculateQualityScore()
{
    // Excellent: <20ms, <5% loss, <2ms jitter
    if (m_latencyAvg < 20.0 && m_packetLoss < 5.0 && m_jitter < 2.0) {
        m_qualityScore = Excellent;
    }
    // Good: <50ms, <10% loss, <5ms jitter
    else if (m_latencyAvg < 50.0 && m_packetLoss < 10.0 && m_jitter < 5.0) {
        m_qualityScore = Good;
    }
    // Fair: <100ms, <20% loss, <10ms jitter
    else if (m_latencyAvg < 100.0 && m_packetLoss < 20.0 && m_jitter < 10.0) {
        m_qualityScore = Fair;
    }
    // Poor: <200ms, <30% loss, <20ms jitter
    else if (m_latencyAvg < 200.0 && m_packetLoss < 30.0 && m_jitter < 20.0) {
        m_qualityScore = Poor;
    }
    // Critical: everything else
    else {
        m_qualityScore = Critical;
    }
}

bool NetworkMetrics::isValid() const
{
    return m_latencyAvg > 0.0 || m_packetLoss > 0.0;
}
