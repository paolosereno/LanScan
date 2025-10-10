#ifndef METRICSDAO_H
#define METRICSDAO_H

#include <QString>
#include <QDateTime>
#include <QList>
#include "models/NetworkMetrics.h"

class DatabaseManager;
class QSqlQuery;

/**
 * @brief Data Access Object for historical network metrics
 *
 * Provides persistence layer for network metrics with query capabilities
 * including device filtering, date range queries, and statistical aggregations.
 */
class MetricsDao {
public:
    /**
     * @brief Constructor
     * @param dbManager Database manager instance
     */
    explicit MetricsDao(DatabaseManager* dbManager);

    /**
     * @brief Destructor
     */
    ~MetricsDao();

    /**
     * @brief Insert metrics for a device
     * @param deviceId Device identifier
     * @param metrics Network metrics to insert
     * @return True if successful
     */
    bool insert(const QString& deviceId, const NetworkMetrics& metrics);

    /**
     * @brief Insert multiple metrics in a single transaction
     * @param deviceId Device identifier
     * @param metricsList List of metrics to insert
     * @return Number of metrics successfully inserted
     */
    int insertBatch(const QString& deviceId, const QList<NetworkMetrics>& metricsList);

    /**
     * @brief Find metrics by device ID
     * @param deviceId Device identifier
     * @param limit Maximum number of entries to return (0 = no limit)
     * @return List of metrics for the device (most recent first)
     */
    QList<NetworkMetrics> findByDevice(const QString& deviceId, int limit = 100);

    /**
     * @brief Find metrics by device and date range
     * @param deviceId Device identifier
     * @param start Start date/time (inclusive)
     * @param end End date/time (inclusive)
     * @return List of metrics in the date range
     */
    QList<NetworkMetrics> findByDateRange(const QString& deviceId,
                                          const QDateTime& start,
                                          const QDateTime& end);

    /**
     * @brief Get average metrics for a device in a date range
     * @param deviceId Device identifier
     * @param start Start date/time (inclusive)
     * @param end End date/time (inclusive)
     * @return Aggregated average metrics
     */
    NetworkMetrics getAverageMetrics(const QString& deviceId,
                                     const QDateTime& start,
                                     const QDateTime& end);

    /**
     * @brief Get maximum latency for a device in a date range
     * @param deviceId Device identifier
     * @param start Start date/time (inclusive)
     * @param end End date/time (inclusive)
     * @return Maximum latency value (ms)
     */
    double getMaxLatency(const QString& deviceId,
                        const QDateTime& start,
                        const QDateTime& end);

    /**
     * @brief Get minimum latency for a device in a date range
     * @param deviceId Device identifier
     * @param start Start date/time (inclusive)
     * @param end End date/time (inclusive)
     * @return Minimum latency value (ms)
     */
    double getMinLatency(const QString& deviceId,
                        const QDateTime& start,
                        const QDateTime& end);

    /**
     * @brief Get average packet loss for a device in a date range
     * @param deviceId Device identifier
     * @param start Start date/time (inclusive)
     * @param end End date/time (inclusive)
     * @return Average packet loss percentage
     */
    double getAveragePacketLoss(const QString& deviceId,
                               const QDateTime& start,
                               const QDateTime& end);

    /**
     * @brief Get average jitter for a device in a date range
     * @param deviceId Device identifier
     * @param start Start date/time (inclusive)
     * @param end End date/time (inclusive)
     * @return Average jitter value (ms)
     */
    double getAverageJitter(const QString& deviceId,
                           const QDateTime& start,
                           const QDateTime& end);

    /**
     * @brief Delete metrics older than specified date
     * @param cutoffDate Cutoff date (metrics before this will be deleted)
     * @return Number of records deleted
     */
    int deleteOlderThan(const QDateTime& cutoffDate);

    /**
     * @brief Delete all metrics for a specific device
     * @param deviceId Device identifier
     * @return Number of records deleted
     */
    int deleteByDevice(const QString& deviceId);

    /**
     * @brief Get the total number of metrics records
     * @return Total metrics count
     */
    int getMetricsCount();

    /**
     * @brief Get metrics count for a specific device
     * @param deviceId Device identifier
     * @return Number of metrics records for that device
     */
    int getMetricsCountByDevice(const QString& deviceId);

private:
    DatabaseManager* dbManager;

    void createTable();
    NetworkMetrics metricsFromQuery(QSqlQuery& query);
};

#endif // METRICSDAO_H
