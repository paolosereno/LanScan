#ifndef NETWORKMETRICS_H
#define NETWORKMETRICS_H

#include <QString>
#include <QDateTime>

class NetworkMetrics
{
public:
    enum QualityScore {
        Excellent,  // <20ms, <5% loss, <2ms jitter
        Good,       // <50ms, <10% loss, <5ms jitter
        Fair,       // <100ms, <20% loss, <10ms jitter
        Poor,       // <200ms, <30% loss, <20ms jitter
        Critical    // >=200ms or >=30% loss or >=20ms jitter
    };

    NetworkMetrics();

    // Getters
    double latencyMin() const;
    double latencyAvg() const;
    double latencyMax() const;
    double latencyMedian() const;
    double jitter() const;
    double packetLoss() const;
    QualityScore qualityScore() const;
    QString qualityScoreString() const;
    QDateTime timestamp() const;

    // Setters
    void setLatencyMin(double latency);
    void setLatencyAvg(double latency);
    void setLatencyMax(double latency);
    void setLatencyMedian(double latency);
    void setJitter(double jitter);
    void setPacketLoss(double loss);
    void setQualityScore(QualityScore score);
    void setTimestamp(const QDateTime& timestamp);

    // Utility methods
    void calculateQualityScore();
    bool isValid() const;

private:
    double m_latencyMin;
    double m_latencyAvg;
    double m_latencyMax;
    double m_latencyMedian;
    double m_jitter;
    double m_packetLoss;
    QualityScore m_qualityScore;
    QDateTime m_timestamp;
};

#endif // NETWORKMETRICS_H
