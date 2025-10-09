#include "services/HistoryService.h"
#include "utils/Logger.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

HistoryService::HistoryService(DatabaseManager* dbManager, QObject* parent)
    : QObject(parent)
    , m_dbManager(dbManager)
{
    if (!m_dbManager) {
        Logger::error("HistoryService: DatabaseManager is null");
    }
}

HistoryService::~HistoryService()
{
    Logger::info("HistoryService destroyed");
}

bool HistoryService::initialize()
{
    if (!m_dbManager || !m_dbManager->isOpen()) {
        Logger::error("HistoryService: Database is not open");
        return false;
    }

    if (!createTables()) {
        Logger::error("HistoryService: Failed to create history tables");
        return false;
    }

    Logger::info("HistoryService initialized successfully");
    return true;
}

bool HistoryService::saveMetrics(const QString& deviceId, const NetworkMetrics& metrics)
{
    if (!m_dbManager || !m_dbManager->isOpen()) {
        Logger::error("HistoryService: Cannot save metrics, database not open");
        return false;
    }

    QString query = "INSERT INTO metrics_history "
                   "(device_id, timestamp, latency_avg, latency_min, latency_max, "
                   "latency_median, jitter, packet_loss, quality_score) "
                   "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)";

    QSqlQuery sqlQuery = m_dbManager->prepareQuery(query);
    sqlQuery.addBindValue(deviceId);
    sqlQuery.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
    sqlQuery.addBindValue(metrics.getLatencyAvg());
    sqlQuery.addBindValue(metrics.getLatencyMin());
    sqlQuery.addBindValue(metrics.getLatencyMax());
    sqlQuery.addBindValue(metrics.getLatencyMedian());
    sqlQuery.addBindValue(metrics.getJitter());
    sqlQuery.addBindValue(metrics.getPacketLoss());
    sqlQuery.addBindValue(static_cast<int>(metrics.getQualityScore()));

    if (!sqlQuery.exec()) {
        Logger::error("Failed to save metrics: " + sqlQuery.lastError().text());
        return false;
    }

    emit metricsStored(deviceId);
    return true;
}

bool HistoryService::saveEvent(const QString& deviceId, const QString& eventType,
                               const QString& description)
{
    if (!m_dbManager || !m_dbManager->isOpen()) {
        Logger::error("HistoryService: Cannot save event, database not open");
        return false;
    }

    QString query = "INSERT INTO events_history "
                   "(device_id, event_type, description, timestamp) "
                   "VALUES (?, ?, ?, ?)";

    QSqlQuery sqlQuery = m_dbManager->prepareQuery(query);
    sqlQuery.addBindValue(deviceId);
    sqlQuery.addBindValue(eventType);
    sqlQuery.addBindValue(description);
    sqlQuery.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));

    if (!sqlQuery.exec()) {
        Logger::error("Failed to save event: " + sqlQuery.lastError().text());
        return false;
    }

    Logger::debug(QString("Event saved: %1 - %2 - %3")
                  .arg(deviceId).arg(eventType).arg(description));

    emit eventStored(deviceId);
    return true;
}

QList<NetworkMetrics> HistoryService::getMetricsHistory(const QString& deviceId,
                                                         const QDateTime& start,
                                                         const QDateTime& end)
{
    QList<NetworkMetrics> metricsList;

    if (!m_dbManager || !m_dbManager->isOpen()) {
        Logger::error("HistoryService: Cannot get metrics, database not open");
        return metricsList;
    }

    QString query = "SELECT * FROM metrics_history "
                   "WHERE device_id = ? AND timestamp >= ? AND timestamp <= ? "
                   "ORDER BY timestamp DESC";

    QSqlQuery sqlQuery = m_dbManager->prepareQuery(query);
    sqlQuery.addBindValue(deviceId);
    sqlQuery.addBindValue(start.toString(Qt::ISODate));
    sqlQuery.addBindValue(end.toString(Qt::ISODate));

    if (!sqlQuery.exec()) {
        Logger::error("Failed to get metrics history: " + sqlQuery.lastError().text());
        return metricsList;
    }

    while (sqlQuery.next()) {
        metricsList.append(parseMetricsFromQuery(sqlQuery));
    }

    return metricsList;
}

QList<NetworkMetrics> HistoryService::getAllMetricsForDevice(const QString& deviceId, int limit)
{
    QList<NetworkMetrics> metricsList;

    if (!m_dbManager || !m_dbManager->isOpen()) {
        Logger::error("HistoryService: Cannot get metrics, database not open");
        return metricsList;
    }

    QString query = "SELECT * FROM metrics_history "
                   "WHERE device_id = ? "
                   "ORDER BY timestamp DESC";

    if (limit > 0) {
        query += QString(" LIMIT %1").arg(limit);
    }

    QSqlQuery sqlQuery = m_dbManager->prepareQuery(query);
    sqlQuery.addBindValue(deviceId);

    if (!sqlQuery.exec()) {
        Logger::error("Failed to get all metrics: " + sqlQuery.lastError().text());
        return metricsList;
    }

    while (sqlQuery.next()) {
        metricsList.append(parseMetricsFromQuery(sqlQuery));
    }

    return metricsList;
}

QList<HistoryEvent> HistoryService::getEventHistory(const QString& deviceId,
                                                     const QDateTime& start,
                                                     const QDateTime& end)
{
    QList<HistoryEvent> eventsList;

    if (!m_dbManager || !m_dbManager->isOpen()) {
        Logger::error("HistoryService: Cannot get events, database not open");
        return eventsList;
    }

    QString query = "SELECT * FROM events_history "
                   "WHERE device_id = ? AND timestamp >= ? AND timestamp <= ? "
                   "ORDER BY timestamp DESC";

    QSqlQuery sqlQuery = m_dbManager->prepareQuery(query);
    sqlQuery.addBindValue(deviceId);
    sqlQuery.addBindValue(start.toString(Qt::ISODate));
    sqlQuery.addBindValue(end.toString(Qt::ISODate));

    if (!sqlQuery.exec()) {
        Logger::error("Failed to get event history: " + sqlQuery.lastError().text());
        return eventsList;
    }

    while (sqlQuery.next()) {
        eventsList.append(parseEventFromQuery(sqlQuery));
    }

    return eventsList;
}

QList<HistoryEvent> HistoryService::getAllEventsForDevice(const QString& deviceId, int limit)
{
    QList<HistoryEvent> eventsList;

    if (!m_dbManager || !m_dbManager->isOpen()) {
        Logger::error("HistoryService: Cannot get events, database not open");
        return eventsList;
    }

    QString query = "SELECT * FROM events_history "
                   "WHERE device_id = ? "
                   "ORDER BY timestamp DESC";

    if (limit > 0) {
        query += QString(" LIMIT %1").arg(limit);
    }

    QSqlQuery sqlQuery = m_dbManager->prepareQuery(query);
    sqlQuery.addBindValue(deviceId);

    if (!sqlQuery.exec()) {
        Logger::error("Failed to get all events: " + sqlQuery.lastError().text());
        return eventsList;
    }

    while (sqlQuery.next()) {
        eventsList.append(parseEventFromQuery(sqlQuery));
    }

    return eventsList;
}

NetworkMetrics HistoryService::getLatestMetrics(const QString& deviceId)
{
    if (!m_dbManager || !m_dbManager->isOpen()) {
        Logger::error("HistoryService: Cannot get latest metrics, database not open");
        return NetworkMetrics();
    }

    QString query = "SELECT * FROM metrics_history "
                   "WHERE device_id = ? "
                   "ORDER BY timestamp DESC "
                   "LIMIT 1";

    QSqlQuery sqlQuery = m_dbManager->prepareQuery(query);
    sqlQuery.addBindValue(deviceId);

    if (!sqlQuery.exec()) {
        Logger::error("Failed to get latest metrics: " + sqlQuery.lastError().text());
        return NetworkMetrics();
    }

    if (sqlQuery.next()) {
        return parseMetricsFromQuery(sqlQuery);
    }

    return NetworkMetrics();
}

int HistoryService::pruneOldData(int daysToKeep)
{
    if (!m_dbManager || !m_dbManager->isOpen()) {
        Logger::error("HistoryService: Cannot prune data, database not open");
        return 0;
    }

    QDateTime cutoffDate = QDateTime::currentDateTime().addDays(-daysToKeep);
    int totalDeleted = 0;

    // Delete old metrics
    QString metricsQuery = "DELETE FROM metrics_history WHERE timestamp < ?";
    QSqlQuery sqlQuery1 = m_dbManager->prepareQuery(metricsQuery);
    sqlQuery1.addBindValue(cutoffDate.toString(Qt::ISODate));

    if (sqlQuery1.exec()) {
        int metricsDeleted = sqlQuery1.numRowsAffected();
        totalDeleted += metricsDeleted;
        Logger::info(QString("Pruned %1 old metrics records").arg(metricsDeleted));
    } else {
        Logger::error("Failed to prune metrics: " + sqlQuery1.lastError().text());
    }

    // Delete old events
    QString eventsQuery = "DELETE FROM events_history WHERE timestamp < ?";
    QSqlQuery sqlQuery2 = m_dbManager->prepareQuery(eventsQuery);
    sqlQuery2.addBindValue(cutoffDate.toString(Qt::ISODate));

    if (sqlQuery2.exec()) {
        int eventsDeleted = sqlQuery2.numRowsAffected();
        totalDeleted += eventsDeleted;
        Logger::info(QString("Pruned %1 old event records").arg(eventsDeleted));
    } else {
        Logger::error("Failed to prune events: " + sqlQuery2.lastError().text());
    }

    if (totalDeleted > 0) {
        emit dataPruned(totalDeleted);
    }

    return totalDeleted;
}

bool HistoryService::deleteDeviceHistory(const QString& deviceId)
{
    if (!m_dbManager || !m_dbManager->isOpen()) {
        Logger::error("HistoryService: Cannot delete history, database not open");
        return false;
    }

    bool success = true;

    // Delete metrics
    QString metricsQuery = "DELETE FROM metrics_history WHERE device_id = ?";
    QSqlQuery sqlQuery1 = m_dbManager->prepareQuery(metricsQuery);
    sqlQuery1.addBindValue(deviceId);

    if (!sqlQuery1.exec()) {
        Logger::error("Failed to delete metrics history: " + sqlQuery1.lastError().text());
        success = false;
    }

    // Delete events
    QString eventsQuery = "DELETE FROM events_history WHERE device_id = ?";
    QSqlQuery sqlQuery2 = m_dbManager->prepareQuery(eventsQuery);
    sqlQuery2.addBindValue(deviceId);

    if (!sqlQuery2.exec()) {
        Logger::error("Failed to delete events history: " + sqlQuery2.lastError().text());
        success = false;
    }

    if (success) {
        Logger::info(QString("Deleted all history for device: %1").arg(deviceId));
    }

    return success;
}

int HistoryService::getMetricsCount(const QString& deviceId)
{
    if (!m_dbManager || !m_dbManager->isOpen()) {
        return 0;
    }

    QString query = "SELECT COUNT(*) FROM metrics_history WHERE device_id = ?";
    QSqlQuery sqlQuery = m_dbManager->prepareQuery(query);
    sqlQuery.addBindValue(deviceId);

    if (sqlQuery.exec() && sqlQuery.next()) {
        return sqlQuery.value(0).toInt();
    }

    return 0;
}

int HistoryService::getEventCount(const QString& deviceId)
{
    if (!m_dbManager || !m_dbManager->isOpen()) {
        return 0;
    }

    QString query = "SELECT COUNT(*) FROM events_history WHERE device_id = ?";
    QSqlQuery sqlQuery = m_dbManager->prepareQuery(query);
    sqlQuery.addBindValue(deviceId);

    if (sqlQuery.exec() && sqlQuery.next()) {
        return sqlQuery.value(0).toInt();
    }

    return 0;
}

bool HistoryService::createTables()
{
    return createMetricsHistoryTable() &&
           createEventsHistoryTable() &&
           createHistoryIndices();
}

bool HistoryService::createMetricsHistoryTable()
{
    QString query = R"(
        CREATE TABLE IF NOT EXISTS metrics_history (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            device_id TEXT NOT NULL,
            timestamp TEXT NOT NULL,
            latency_avg REAL,
            latency_min REAL,
            latency_max REAL,
            latency_median REAL,
            jitter REAL,
            packet_loss REAL,
            quality_score INTEGER
        )
    )";

    if (!m_dbManager->executeQuery(query)) {
        Logger::error("Failed to create metrics_history table: " + m_dbManager->getLastError());
        return false;
    }

    Logger::debug("metrics_history table created or already exists");
    return true;
}

bool HistoryService::createEventsHistoryTable()
{
    QString query = R"(
        CREATE TABLE IF NOT EXISTS events_history (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            device_id TEXT NOT NULL,
            event_type TEXT NOT NULL,
            description TEXT,
            timestamp TEXT NOT NULL
        )
    )";

    if (!m_dbManager->executeQuery(query)) {
        Logger::error("Failed to create events_history table: " + m_dbManager->getLastError());
        return false;
    }

    Logger::debug("events_history table created or already exists");
    return true;
}

bool HistoryService::createHistoryIndices()
{
    QStringList indices = {
        "CREATE INDEX IF NOT EXISTS idx_metrics_device_timestamp ON metrics_history(device_id, timestamp)",
        "CREATE INDEX IF NOT EXISTS idx_events_device_timestamp ON events_history(device_id, timestamp)",
        "CREATE INDEX IF NOT EXISTS idx_metrics_timestamp ON metrics_history(timestamp)",
        "CREATE INDEX IF NOT EXISTS idx_events_timestamp ON events_history(timestamp)"
    };

    for (const QString& index : indices) {
        if (!m_dbManager->executeQuery(index)) {
            Logger::error("Failed to create history index: " + m_dbManager->getLastError());
            return false;
        }
    }

    Logger::debug("History indices created or already exist");
    return true;
}

NetworkMetrics HistoryService::parseMetricsFromQuery(const QSqlQuery& query)
{
    NetworkMetrics metrics;

    metrics.setLatencyAvg(query.value("latency_avg").toDouble());
    metrics.setLatencyMin(query.value("latency_min").toDouble());
    metrics.setLatencyMax(query.value("latency_max").toDouble());
    metrics.setLatencyMedian(query.value("latency_median").toDouble());
    metrics.setJitter(query.value("jitter").toDouble());
    metrics.setPacketLoss(query.value("packet_loss").toDouble());
    metrics.setQualityScore(static_cast<NetworkMetrics::QualityScore>(query.value("quality_score").toInt()));

    return metrics;
}

HistoryEvent HistoryService::parseEventFromQuery(const QSqlQuery& query)
{
    HistoryEvent event;

    event.id = query.value("id").toInt();
    event.deviceId = query.value("device_id").toString();
    event.eventType = query.value("event_type").toString();
    event.description = query.value("description").toString();
    event.timestamp = QDateTime::fromString(query.value("timestamp").toString(), Qt::ISODate);

    return event;
}
