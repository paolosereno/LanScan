#include "MetricsAggregator.h"
#include "QualityScoreCalculator.h"
#include "LatencyCalculator.h"
#include "../../utils/Logger.h"
#include <QDateTime>

MetricsAggregator::MetricsAggregator(
    IMetricsCalculator* latencyCalc,
    IMetricsCalculator* jitterCalc,
    IMetricsCalculator* packetLossCalc,
    QualityScoreCalculator* qualityCalc,
    QObject* parent)
    : QObject(parent)
    , latencyCalculator(latencyCalc)
    , jitterCalculator(jitterCalc)
    , packetLossCalculator(packetLossCalc)
    , qualityCalculator(qualityCalc)
    , pingService(new PingService(this))
    , m_isCollecting(false)
    , maxHistorySize(1000)
{
    connect(pingService, &PingService::pingResult,
            this, &MetricsAggregator::onPingResult);
    connect(pingService, &PingService::pingCompleted,
            this, &MetricsAggregator::onPingCompleted);
    connect(pingService, &PingService::errorOccurred,
            this, &MetricsAggregator::onPingError);
}

MetricsAggregator::~MetricsAggregator() {
    stopContinuousCollection();
}

NetworkMetrics MetricsAggregator::aggregate(const QVector<PingService::PingResult>& results) {
    NetworkMetrics metrics;
    metrics.setTimestamp(QDateTime::currentDateTime());

    if (results.isEmpty()) {
        Logger::warn("MetricsAggregator: No ping results to aggregate");
        return metrics;
    }

    // Extract RTT values from successful pings
    QVector<double> rttValues = extractRttValues(results);

    if (rttValues.isEmpty()) {
        Logger::warn("MetricsAggregator: No successful pings in results");
        return metrics;
    }

    // Calculate latency metrics
    calculateLatencyMetrics(results, metrics);

    // Calculate jitter
    double jitter = jitterCalculator->calculate(rttValues);
    metrics.setJitter(jitter);

    // Calculate packet loss
    QVector<bool> successVector = extractSuccessVector(results);
    double packetLoss = packetLossCalculator->calculate(
        QVector<double>(successVector.size(), 1.0).toList().toVector()
    );

    // Convert success vector to packet loss percentage
    int totalPackets = results.size();
    int successfulPackets = rttValues.size();
    double lossPercentage = ((totalPackets - successfulPackets) / static_cast<double>(totalPackets)) * 100.0;
    metrics.setPacketLoss(lossPercentage);

    // Calculate quality score
    metrics.calculateQualityScore();

    return metrics;
}

void MetricsAggregator::startContinuousCollection(const QString& host, int interval) {
    if (m_isCollecting) {
        Logger::warn("MetricsAggregator: Already collecting metrics");
        return;
    }

    m_currentHost = host;
    m_isCollecting = true;
    resultHistory.clear();
    metricsHistory.clear();

    pingService->continuousPing(host, interval);

    Logger::info(QString("MetricsAggregator: Started continuous collection for %1").arg(host));
}

void MetricsAggregator::stopContinuousCollection() {
    if (!m_isCollecting) {
        return;
    }

    pingService->stopContinuousPing();
    m_isCollecting = false;

    Logger::info("MetricsAggregator: Stopped continuous collection");
}

bool MetricsAggregator::isCollecting() const {
    return m_isCollecting;
}

QString MetricsAggregator::currentHost() const {
    return m_currentHost;
}

void MetricsAggregator::onPingResult(const PingService::PingResult& result) {
    // Add to rolling history
    resultHistory.append(result);

    // Limit history size
    if (resultHistory.size() > maxHistorySize) {
        resultHistory.removeFirst();
    }

    // Calculate metrics from recent history (e.g., last 10 pings)
    int recentCount = std::min(10, static_cast<int>(resultHistory.size()));
    QVector<PingService::PingResult> recentResults =
        resultHistory.mid(resultHistory.size() - recentCount);

    NetworkMetrics metrics = aggregate(recentResults);
    addToHistory(metrics);

    emit metricsUpdated(metrics);
}

void MetricsAggregator::onPingCompleted(const QVector<PingService::PingResult>& results) {
    NetworkMetrics metrics = aggregate(results);
    emit metricsUpdated(metrics);
}

void MetricsAggregator::onPingError(const QString& error) {
    Logger::error(QString("MetricsAggregator: Ping error: %1").arg(error));
    emit errorOccurred(error);
}

QVector<double> MetricsAggregator::extractRttValues(const QVector<PingService::PingResult>& results) {
    QVector<double> rttValues;
    rttValues.reserve(results.size());

    for (const PingService::PingResult& result : results) {
        if (result.success && result.latency > 0.0) {
            rttValues.append(result.latency);
        }
    }

    return rttValues;
}

QVector<bool> MetricsAggregator::extractSuccessVector(const QVector<PingService::PingResult>& results) {
    QVector<bool> successVector;
    successVector.reserve(results.size());

    for (const PingService::PingResult& result : results) {
        successVector.append(result.success);
    }

    return successVector;
}

void MetricsAggregator::calculateLatencyMetrics(
    const QVector<PingService::PingResult>& results, NetworkMetrics& metrics)
{
    QVector<double> rttValues = extractRttValues(results);

    if (rttValues.isEmpty()) {
        return;
    }

    // Use LatencyCalculator to get detailed stats
    LatencyCalculator* latCalc = dynamic_cast<LatencyCalculator*>(latencyCalculator);
    if (latCalc) {
        LatencyCalculator::LatencyStats stats = latCalc->calculateStats(rttValues);
        metrics.setLatencyMin(stats.min);
        metrics.setLatencyAvg(stats.avg);
        metrics.setLatencyMax(stats.max);
        metrics.setLatencyMedian(stats.median);
    } else {
        // Fallback: just set average
        double avg = latencyCalculator->calculate(rttValues);
        metrics.setLatencyAvg(avg);
    }
}

void MetricsAggregator::addToHistory(const NetworkMetrics& metrics) {
    metricsHistory.append(metrics);

    // Limit history size
    if (metricsHistory.size() > maxHistorySize) {
        metricsHistory.removeFirst();
    }

    emit metricsHistoryUpdated(metricsHistory);
}
