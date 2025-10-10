#ifndef HISTORYDAO_H
#define HISTORYDAO_H

#include <QString>
#include <QDateTime>
#include <QList>
#include <QJsonObject>

class DatabaseManager;
class QSqlQuery;

/**
 * @brief History event data structure
 */
class HistoryEvent {
public:
    QString id;              ///< Unique event identifier
    QString deviceId;        ///< Device identifier
    QString eventType;       ///< Event type: "scan", "status_change", "alert", "user_action"
    QString description;     ///< Human-readable description
    QJsonObject metadata;    ///< Additional event data (JSON)
    QDateTime timestamp;     ///< Event timestamp

    /**
     * @brief Default constructor
     */
    HistoryEvent()
        : timestamp(QDateTime::currentDateTime())
    {}

    /**
     * @brief Check if event is valid
     * @return True if event has required fields
     */
    bool isValid() const {
        return !id.isEmpty() && !deviceId.isEmpty() && !eventType.isEmpty() && timestamp.isValid();
    }
};

/**
 * @brief Data Access Object for history events
 *
 * Provides persistence layer for historical events with query capabilities
 * including device filtering, type filtering, and date range queries.
 */
class HistoryDao {
public:
    /**
     * @brief Constructor
     * @param dbManager Database manager instance
     */
    explicit HistoryDao(DatabaseManager* dbManager);

    /**
     * @brief Destructor
     */
    ~HistoryDao();

    /**
     * @brief Insert a single history event
     * @param event Event to insert
     * @return True if successful
     */
    bool insert(const HistoryEvent& event);

    /**
     * @brief Insert multiple events in a single transaction
     * @param events Events to insert
     * @return Number of events successfully inserted
     */
    int insertBatch(const QList<HistoryEvent>& events);

    /**
     * @brief Find events by device ID
     * @param deviceId Device identifier
     * @param limit Maximum number of events to return (0 = no limit)
     * @return List of events for the device
     */
    QList<HistoryEvent> findByDevice(const QString& deviceId, int limit = 0);

    /**
     * @brief Find events by event type
     * @param eventType Event type to search for
     * @param limit Maximum number of events to return (0 = no limit)
     * @return List of events of the specified type
     */
    QList<HistoryEvent> findByType(const QString& eventType, int limit = 0);

    /**
     * @brief Find events within a date range
     * @param start Start date/time (inclusive)
     * @param end End date/time (inclusive)
     * @param limit Maximum number of events to return (0 = no limit)
     * @return List of events in the date range
     */
    QList<HistoryEvent> findByDateRange(const QDateTime& start, const QDateTime& end, int limit = 0);

    /**
     * @brief Find events by device and date range
     * @param deviceId Device identifier
     * @param start Start date/time (inclusive)
     * @param end End date/time (inclusive)
     * @param limit Maximum number of events to return (0 = no limit)
     * @return List of events matching criteria
     */
    QList<HistoryEvent> findByDeviceAndDateRange(const QString& deviceId,
                                                  const QDateTime& start,
                                                  const QDateTime& end,
                                                  int limit = 0);

    /**
     * @brief Get all events
     * @param limit Maximum number of events to return (0 = no limit)
     * @return List of all events
     */
    QList<HistoryEvent> findAll(int limit = 1000);

    /**
     * @brief Delete events older than specified date
     * @param cutoffDate Cutoff date (events before this will be deleted)
     * @return Number of events deleted
     */
    int deleteOlderThan(const QDateTime& cutoffDate);

    /**
     * @brief Delete all events for a specific device
     * @param deviceId Device identifier
     * @return Number of events deleted
     */
    int deleteByDevice(const QString& deviceId);

    /**
     * @brief Delete a specific event
     * @param eventId Event identifier
     * @return True if successful
     */
    bool deleteEvent(const QString& eventId);

    /**
     * @brief Get the total number of events
     * @return Total event count
     */
    int getEventCount();

    /**
     * @brief Get event count by type
     * @param eventType Event type
     * @return Number of events of that type
     */
    int getEventCountByType(const QString& eventType);

private:
    DatabaseManager* dbManager;

    void createTable();
    HistoryEvent eventFromQuery(QSqlQuery& query);
};

#endif // HISTORYDAO_H
