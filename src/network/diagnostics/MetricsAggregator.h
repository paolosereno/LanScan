#ifndef METRICSAGGREGATOR_H
#define METRICSAGGREGATOR_H

#include <QObject>
#include <QVector>
#include <QTimer>
#include "PingService.h"
#include "../../models/NetworkMetrics.h"
#include "../../interfaces/IMetricsCalculator.h"

// Forward declarations
class QualityScoreCalculator;

/**
 * @brief Aggregates metrics from multiple sources
 *
 * Collects ping results and calculates comprehensive network metrics
 * including latency, jitter, packet loss, and quality score.
 * Supports both one-time and continuous metric collection.
 */
class MetricsAggregator : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Construct MetricsAggregator with injected calculators
     * @param latencyCalc Latency calculator
     * @param jitterCalc Jitter calculator
     * @param packetLossCalc Packet loss calculator
     * @param qualityCalc Quality score calculator
     * @param parent Parent QObject
     */
    MetricsAggregator(
        IMetricsCalculator* latencyCalc,
        IMetricsCalculator* jitterCalc,
        IMetricsCalculator* packetLossCalc,
        QualityScoreCalculator* qualityCalc,
        QObject* parent = nullptr
    );

    ~MetricsAggregator();

    /**
     * @brief Aggregate metrics from ping results
     * @param results Vector of ping results
     * @return Aggregated NetworkMetrics
     */
    NetworkMetrics aggregate(const QVector<PingService::PingResult>& results);

    /**
     * @brief Start continuous metric collection
     * @param host Target host to monitor
     * @param interval Ping interval in milliseconds (default: 1000)
     */
    void startContinuousCollection(const QString& host, int interval = 1000);

    /**
     * @brief Stop continuous metric collection
     */
    void stopContinuousCollection();

    /**
     * @brief Check if continuous collection is active
     * @return True if collecting metrics continuously
     */
    bool isCollecting() const;

    /**
     * @brief Get current host being monitored
     * @return Host IP or hostname
     */
    QString currentHost() const;

signals:
    /**
     * @brief Emitted when new metrics are calculated
     * @param metrics Updated network metrics
     */
    void metricsUpdated(const NetworkMetrics& metrics);

    /**
     * @brief Emitted when metrics history is updated (continuous mode)
     * @param history Vector of historical metrics
     */
    void metricsHistoryUpdated(const QVector<NetworkMetrics>& history);

    /**
     * @brief Emitted when an error occurs
     * @param error Error message
     */
    void errorOccurred(const QString& error);

private slots:
    void onPingResult(const PingService::PingResult& result);
    void onPingCompleted(const QVector<PingService::PingResult>& results);
    void onPingError(const QString& error);

private:
    // Calculator dependencies
    IMetricsCalculator* latencyCalculator;
    IMetricsCalculator* jitterCalculator;
    IMetricsCalculator* packetLossCalculator;
    QualityScoreCalculator* qualityCalculator;

    // Services
    PingService* pingService;

    // State
    QString m_currentHost;
    QVector<PingService::PingResult> resultHistory;
    QVector<NetworkMetrics> metricsHistory;
    bool m_isCollecting;
    int maxHistorySize;

    /**
     * @brief Extract RTT values from ping results
     * @param results Ping results
     * @return Vector of RTT values in milliseconds
     */
    QVector<double> extractRttValues(const QVector<PingService::PingResult>& results);

    /**
     * @brief Extract success vector from ping results
     * @param results Ping results
     * @return Vector where true = success, false = failure
     */
    QVector<bool> extractSuccessVector(const QVector<PingService::PingResult>& results);

    /**
     * @brief Calculate latency statistics from results
     * @param results Ping results
     * @param metrics NetworkMetrics to update
     */
    void calculateLatencyMetrics(const QVector<PingService::PingResult>& results, NetworkMetrics& metrics);

    /**
     * @brief Add metrics to history and manage size
     * @param metrics NetworkMetrics to add
     */
    void addToHistory(const NetworkMetrics& metrics);
};

#endif // METRICSAGGREGATOR_H
