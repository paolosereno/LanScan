#include "database/HistoryDao.h"
#include "database/DatabaseManager.h"
#include "utils/Logger.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QJsonDocument>
#include <QVariant>
#include <QUuid>

HistoryDao::HistoryDao(DatabaseManager* dbManager)
    : dbManager(dbManager)
{
    createTable();
    Logger::info("HistoryDao initialized");
}

HistoryDao::~HistoryDao() {
}

void HistoryDao::createTable() {
    QSqlQuery query(dbManager->database());

    QString sql = R"(
        CREATE TABLE IF NOT EXISTS history_events (
            id TEXT PRIMARY KEY,
            device_id TEXT NOT NULL,
            event_type TEXT NOT NULL,
            description TEXT,
            metadata TEXT,
            timestamp DATETIME NOT NULL,
            FOREIGN KEY (device_id) REFERENCES devices(id)
        )
    )";

    if (!query.exec(sql)) {
        Logger::error("Failed to create history_events table: " + query.lastError().text());
        return;
    }

    // Create indices for faster queries
    query.exec("CREATE INDEX IF NOT EXISTS idx_history_device ON history_events(device_id)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_history_type ON history_events(event_type)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_history_timestamp ON history_events(timestamp)");

    Logger::debug("History events table created/verified");
}

bool HistoryDao::insert(const HistoryEvent& event) {
    if (!event.isValid()) {
        Logger::error("Cannot insert invalid history event");
        return false;
    }

    QSqlQuery query(dbManager->database());
    query.prepare(R"(
        INSERT INTO history_events (id, device_id, event_type, description, metadata, timestamp)
        VALUES (:id, :device_id, :event_type, :description, :metadata, :timestamp)
    )");

    QString eventId = event.id.isEmpty() ? QUuid::createUuid().toString(QUuid::WithoutBraces) : event.id;

    query.bindValue(":id", eventId);
    query.bindValue(":device_id", event.deviceId);
    query.bindValue(":event_type", event.eventType);
    query.bindValue(":description", event.description);
    query.bindValue(":metadata", QJsonDocument(event.metadata).toJson(QJsonDocument::Compact));
    query.bindValue(":timestamp", event.timestamp.toString(Qt::ISODate));

    if (!query.exec()) {
        Logger::error("Failed to insert history event: " + query.lastError().text());
        return false;
    }

    Logger::debug("History event inserted: " + event.eventType + " for device " + event.deviceId);
    return true;
}

int HistoryDao::insertBatch(const QList<HistoryEvent>& events) {
    if (events.isEmpty()) {
        return 0;
    }

    QSqlDatabase db = dbManager->database();
    db.transaction();

    int insertedCount = 0;
    for (const HistoryEvent& event : events) {
        if (insert(event)) {
            insertedCount++;
        }
    }

    if (db.commit()) {
        Logger::info("Inserted " + QString::number(insertedCount) + " history events in batch");
        return insertedCount;
    } else {
        db.rollback();
        Logger::error("Failed to commit batch insert of history events");
        return 0;
    }
}

QList<HistoryEvent> HistoryDao::findByDevice(const QString& deviceId, int limit) {
    QList<HistoryEvent> events;

    QSqlQuery query(dbManager->database());
    QString sql = "SELECT * FROM history_events WHERE device_id = :device_id ORDER BY timestamp DESC";
    if (limit > 0) {
        sql += " LIMIT :limit";
    }

    query.prepare(sql);
    query.bindValue(":device_id", deviceId);
    if (limit > 0) {
        query.bindValue(":limit", limit);
    }

    if (!query.exec()) {
        Logger::error("Failed to query history events by device: " + query.lastError().text());
        return events;
    }

    while (query.next()) {
        events.append(eventFromQuery(query));
    }

    Logger::debug("Found " + QString::number(events.size()) + " events for device " + deviceId);
    return events;
}

QList<HistoryEvent> HistoryDao::findByType(const QString& eventType, int limit) {
    QList<HistoryEvent> events;

    QSqlQuery query(dbManager->database());
    QString sql = "SELECT * FROM history_events WHERE event_type = :event_type ORDER BY timestamp DESC";
    if (limit > 0) {
        sql += " LIMIT :limit";
    }

    query.prepare(sql);
    query.bindValue(":event_type", eventType);
    if (limit > 0) {
        query.bindValue(":limit", limit);
    }

    if (!query.exec()) {
        Logger::error("Failed to query history events by type: " + query.lastError().text());
        return events;
    }

    while (query.next()) {
        events.append(eventFromQuery(query));
    }

    Logger::debug("Found " + QString::number(events.size()) + " events of type " + eventType);
    return events;
}

QList<HistoryEvent> HistoryDao::findByDateRange(const QDateTime& start, const QDateTime& end, int limit) {
    QList<HistoryEvent> events;

    QSqlQuery query(dbManager->database());
    QString sql = "SELECT * FROM history_events WHERE timestamp BETWEEN :start AND :end ORDER BY timestamp DESC";
    if (limit > 0) {
        sql += " LIMIT :limit";
    }

    query.prepare(sql);
    query.bindValue(":start", start.toString(Qt::ISODate));
    query.bindValue(":end", end.toString(Qt::ISODate));
    if (limit > 0) {
        query.bindValue(":limit", limit);
    }

    if (!query.exec()) {
        Logger::error("Failed to query history events by date range: " + query.lastError().text());
        return events;
    }

    while (query.next()) {
        events.append(eventFromQuery(query));
    }

    Logger::debug("Found " + QString::number(events.size()) + " events in date range");
    return events;
}

QList<HistoryEvent> HistoryDao::findByDeviceAndDateRange(const QString& deviceId,
                                                          const QDateTime& start,
                                                          const QDateTime& end,
                                                          int limit) {
    QList<HistoryEvent> events;

    QSqlQuery query(dbManager->database());
    QString sql = R"(
        SELECT * FROM history_events
        WHERE device_id = :device_id
        AND timestamp BETWEEN :start AND :end
        ORDER BY timestamp DESC
    )";
    if (limit > 0) {
        sql += " LIMIT :limit";
    }

    query.prepare(sql);
    query.bindValue(":device_id", deviceId);
    query.bindValue(":start", start.toString(Qt::ISODate));
    query.bindValue(":end", end.toString(Qt::ISODate));
    if (limit > 0) {
        query.bindValue(":limit", limit);
    }

    if (!query.exec()) {
        Logger::error("Failed to query history events by device and date range: " + query.lastError().text());
        return events;
    }

    while (query.next()) {
        events.append(eventFromQuery(query));
    }

    Logger::debug("Found " + QString::number(events.size()) +
                 " events for device " + deviceId + " in date range");
    return events;
}

QList<HistoryEvent> HistoryDao::findAll(int limit) {
    QList<HistoryEvent> events;

    QSqlQuery query(dbManager->database());
    QString sql = "SELECT * FROM history_events ORDER BY timestamp DESC";
    if (limit > 0) {
        sql += " LIMIT :limit";
    }

    query.prepare(sql);
    if (limit > 0) {
        query.bindValue(":limit", limit);
    }

    if (!query.exec()) {
        Logger::error("Failed to query all history events: " + query.lastError().text());
        return events;
    }

    while (query.next()) {
        events.append(eventFromQuery(query));
    }

    Logger::debug("Found " + QString::number(events.size()) + " total events");
    return events;
}

int HistoryDao::deleteOlderThan(const QDateTime& cutoffDate) {
    QSqlQuery query(dbManager->database());
    query.prepare("DELETE FROM history_events WHERE timestamp < :cutoff");
    query.bindValue(":cutoff", cutoffDate.toString(Qt::ISODate));

    if (!query.exec()) {
        Logger::error("Failed to delete old history events: " + query.lastError().text());
        return 0;
    }

    int deletedCount = query.numRowsAffected();
    Logger::info("Deleted " + QString::number(deletedCount) + " old history events");
    return deletedCount;
}

int HistoryDao::deleteByDevice(const QString& deviceId) {
    QSqlQuery query(dbManager->database());
    query.prepare("DELETE FROM history_events WHERE device_id = :device_id");
    query.bindValue(":device_id", deviceId);

    if (!query.exec()) {
        Logger::error("Failed to delete history events for device: " + query.lastError().text());
        return 0;
    }

    int deletedCount = query.numRowsAffected();
    Logger::info("Deleted " + QString::number(deletedCount) + " events for device " + deviceId);
    return deletedCount;
}

bool HistoryDao::deleteEvent(const QString& eventId) {
    QSqlQuery query(dbManager->database());
    query.prepare("DELETE FROM history_events WHERE id = :id");
    query.bindValue(":id", eventId);

    if (!query.exec()) {
        Logger::error("Failed to delete history event: " + query.lastError().text());
        return false;
    }

    Logger::debug("Deleted history event: " + eventId);
    return true;
}

int HistoryDao::getEventCount() {
    QSqlQuery query(dbManager->database());
    query.prepare("SELECT COUNT(*) FROM history_events");

    if (!query.exec() || !query.next()) {
        Logger::error("Failed to get event count: " + query.lastError().text());
        return 0;
    }

    return query.value(0).toInt();
}

int HistoryDao::getEventCountByType(const QString& eventType) {
    QSqlQuery query(dbManager->database());
    query.prepare("SELECT COUNT(*) FROM history_events WHERE event_type = :event_type");
    query.bindValue(":event_type", eventType);

    if (!query.exec() || !query.next()) {
        Logger::error("Failed to get event count by type: " + query.lastError().text());
        return 0;
    }

    return query.value(0).toInt();
}

HistoryEvent HistoryDao::eventFromQuery(QSqlQuery& query) {
    HistoryEvent event;

    event.id = query.value("id").toString();
    event.deviceId = query.value("device_id").toString();
    event.eventType = query.value("event_type").toString();
    event.description = query.value("description").toString();
    event.timestamp = QDateTime::fromString(query.value("timestamp").toString(), Qt::ISODate);

    // Parse JSON metadata
    QString metadataJson = query.value("metadata").toString();
    if (!metadataJson.isEmpty()) {
        QJsonDocument doc = QJsonDocument::fromJson(metadataJson.toUtf8());
        if (doc.isObject()) {
            event.metadata = doc.object();
        }
    }

    return event;
}
