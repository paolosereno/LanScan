#ifndef HISTORYSERVICE_H
#define HISTORYSERVICE_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QList>
#include "models/NetworkMetrics.h"
#include "database/DatabaseManager.h"

/**
 * @brief Historical event record
 */
struct HistoryEvent {
    int id;                      // Event ID
    QString deviceId;            // Device identifier
    QString eventType;           // Event type (e.g., "online", "offline", "alert")
    QString description;         // Event description
    QDateTime timestamp;         // When event occurred

    QString toString() const {
        return QString("[%1] %2: %3 - %4")
            .arg(timestamp.toString("yyyy-MM-dd hh:mm:ss"))
            .arg(deviceId)
            .arg(eventType)
            .arg(description);
    }
};

Q_DECLARE_METATYPE(HistoryEvent)

/**
 * @brief Service for managing historical monitoring data
 *
 * The HistoryService handles persistent storage and retrieval of
 * network metrics and events in the database. It provides time-based
 * querying and automatic data pruning.
 */
class HistoryService : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param dbManager Database manager instance
     * @param parent Parent QObject
     */
    explicit HistoryService(DatabaseManager* dbManager, QObject* parent = nullptr);

    /**
     * @brief Destructor
     */
    ~HistoryService();

    /**
     * @brief Initialize history service (create tables if needed)
     * @return True if initialization successful
     */
    bool initialize();

    /**
     * @brief Save network metrics to database
     * @param deviceId Device identifier
     * @param metrics Network metrics to save
     * @return True if save successful
     */
    bool saveMetrics(const QString& deviceId, const NetworkMetrics& metrics);

    /**
     * @brief Save an event to database
     * @param deviceId Device identifier
     * @param eventType Event type
     * @param description Event description
     * @return True if save successful
     */
    bool saveEvent(const QString& deviceId, const QString& eventType,
                   const QString& description);

    /**
     * @brief Get metrics history for a device within time range
     * @param deviceId Device identifier
     * @param start Start datetime
     * @param end End datetime
     * @return List of network metrics
     */
    QList<NetworkMetrics> getMetricsHistory(const QString& deviceId,
                                            const QDateTime& start,
                                            const QDateTime& end);

    /**
     * @brief Get all metrics history for a device
     * @param deviceId Device identifier
     * @param limit Maximum number of records (0 = no limit)
     * @return List of network metrics
     */
    QList<NetworkMetrics> getAllMetricsForDevice(const QString& deviceId, int limit = 100);

    /**
     * @brief Get event history for a device within time range
     * @param deviceId Device identifier
     * @param start Start datetime
     * @param end End datetime
     * @return List of history events
     */
    QList<HistoryEvent> getEventHistory(const QString& deviceId,
                                        const QDateTime& start,
                                        const QDateTime& end);

    /**
     * @brief Get all event history for a device
     * @param deviceId Device identifier
     * @param limit Maximum number of records (0 = no limit)
     * @return List of history events
     */
    QList<HistoryEvent> getAllEventsForDevice(const QString& deviceId, int limit = 100);

    /**
     * @brief Get latest metrics for a device
     * @param deviceId Device identifier
     * @return NetworkMetrics object (default if not found)
     */
    NetworkMetrics getLatestMetrics(const QString& deviceId);

    /**
     * @brief Prune old historical data
     * @param daysToKeep Number of days to keep (older data will be deleted)
     * @return Number of records deleted
     */
    int pruneOldData(int daysToKeep = 30);

    /**
     * @brief Delete all history for a device
     * @param deviceId Device identifier
     * @return True if deletion successful
     */
    bool deleteDeviceHistory(const QString& deviceId);

    /**
     * @brief Get metrics count for a device
     * @param deviceId Device identifier
     * @return Number of metrics records
     */
    int getMetricsCount(const QString& deviceId);

    /**
     * @brief Get event count for a device
     * @param deviceId Device identifier
     * @return Number of event records
     */
    int getEventCount(const QString& deviceId);

signals:
    /**
     * @brief Emitted when metrics are stored
     * @param deviceId Device identifier
     */
    void metricsStored(const QString& deviceId);

    /**
     * @brief Emitted when an event is stored
     * @param deviceId Device identifier
     */
    void eventStored(const QString& deviceId);

    /**
     * @brief Emitted when data is pruned
     * @param recordsDeleted Number of records deleted
     */
    void dataPruned(int recordsDeleted);

private:
    DatabaseManager* m_dbManager;

    /**
     * @brief Create history tables if they don't exist
     * @return True if successful
     */
    bool createTables();

    /**
     * @brief Create metrics history table
     * @return True if successful
     */
    bool createMetricsHistoryTable();

    /**
     * @brief Create events history table
     * @return True if successful
     */
    bool createEventsHistoryTable();

    /**
     * @brief Create indices for performance
     * @return True if successful
     */
    bool createHistoryIndices();

    /**
     * @brief Parse metrics from SQL query result
     * @param query SQL query with results
     * @return NetworkMetrics object
     */
    NetworkMetrics parseMetricsFromQuery(const QSqlQuery& query);

    /**
     * @brief Parse event from SQL query result
     * @param query SQL query with results
     * @return HistoryEvent object
     */
    HistoryEvent parseEventFromQuery(const QSqlQuery& query);
};

#endif // HISTORYSERVICE_H
