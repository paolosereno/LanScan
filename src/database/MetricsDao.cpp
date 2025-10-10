#include "database/MetricsDao.h"
#include "database/DatabaseManager.h"
#include "utils/Logger.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QUuid>

MetricsDao::MetricsDao(DatabaseManager* dbManager)
    : dbManager(dbManager)
{
    createTable();
    Logger::info("MetricsDao initialized");
}

MetricsDao::~MetricsDao() {
}

void MetricsDao::createTable() {
    QSqlQuery query(dbManager->database());

    QString sql = R"(
        CREATE TABLE IF NOT EXISTS metrics_history (
            id TEXT PRIMARY KEY,
            device_id TEXT NOT NULL,
            latency_min REAL,
            latency_avg REAL,
            latency_max REAL,
            latency_median REAL,
            latency_stddev REAL,
            jitter REAL,
            packet_loss REAL,
            packets_sent INTEGER,
            packets_received INTEGER,
            quality_score INTEGER,
            timestamp DATETIME NOT NULL,
            FOREIGN KEY (device_id) REFERENCES devices(id)
        )
    )";

    if (!query.exec(sql)) {
        Logger::error("Failed to create metrics_history table: " + query.lastError().text());
        return;
    }

    // Create indices for faster queries
    query.exec("CREATE INDEX IF NOT EXISTS idx_metrics_device ON metrics_history(device_id)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_metrics_timestamp ON metrics_history(timestamp)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_metrics_device_timestamp ON metrics_history(device_id, timestamp)");

    Logger::debug("Metrics history table created/verified");
}

bool MetricsDao::insert(const QString& deviceId, const NetworkMetrics& metrics) {
    if (deviceId.isEmpty()) {
        Logger::error("Cannot insert metrics: device ID is empty");
        return false;
    }

    QSqlQuery query(dbManager->database());
    query.prepare(R"(
        INSERT INTO metrics_history (
            id, device_id, latency_min, latency_avg, latency_max, latency_median, latency_stddev,
            jitter, packet_loss, packets_sent, packets_received, quality_score, timestamp
        ) VALUES (
            :id, :device_id, :latency_min, :latency_avg, :latency_max, :latency_median, :latency_stddev,
            :jitter, :packet_loss, :packets_sent, :packets_received, :quality_score, :timestamp
        )
    )");

    QString recordId = QUuid::createUuid().toString(QUuid::WithoutBraces);

    query.bindValue(":id", recordId);
    query.bindValue(":device_id", deviceId);
    query.bindValue(":latency_min", metrics.getLatencyMin());
    query.bindValue(":latency_avg", metrics.getLatencyAvg());
    query.bindValue(":latency_max", metrics.getLatencyMax());
    query.bindValue(":latency_median", metrics.getLatencyMedian());
    query.bindValue(":latency_stddev", 0.0); // Not available in NetworkMetrics
    query.bindValue(":jitter", metrics.getJitter());
    query.bindValue(":packet_loss", metrics.getPacketLoss());
    query.bindValue(":packets_sent", 0); // Not available in NetworkMetrics
    query.bindValue(":packets_received", 0); // Not available in NetworkMetrics
    query.bindValue(":quality_score", static_cast<int>(metrics.getQualityScore()));
    query.bindValue(":timestamp", metrics.timestamp().toString(Qt::ISODate));

    if (!query.exec()) {
        Logger::error("Failed to insert metrics: " + query.lastError().text());
        return false;
    }

    Logger::debug("Metrics inserted for device " + deviceId);
    return true;
}

int MetricsDao::insertBatch(const QString& deviceId, const QList<NetworkMetrics>& metricsList) {
    if (metricsList.isEmpty()) {
        return 0;
    }

    QSqlDatabase db = dbManager->database();
    db.transaction();

    int insertedCount = 0;
    for (const NetworkMetrics& metrics : metricsList) {
        if (insert(deviceId, metrics)) {
            insertedCount++;
        }
    }

    if (db.commit()) {
        Logger::info("Inserted " + QString::number(insertedCount) + " metrics records in batch");
        return insertedCount;
    } else {
        db.rollback();
        Logger::error("Failed to commit batch insert of metrics");
        return 0;
    }
}

QList<NetworkMetrics> MetricsDao::findByDevice(const QString& deviceId, int limit) {
    QList<NetworkMetrics> metricsList;

    QSqlQuery query(dbManager->database());
    QString sql = "SELECT * FROM metrics_history WHERE device_id = :device_id ORDER BY timestamp DESC";
    if (limit > 0) {
        sql += " LIMIT :limit";
    }

    query.prepare(sql);
    query.bindValue(":device_id", deviceId);
    if (limit > 0) {
        query.bindValue(":limit", limit);
    }

    if (!query.exec()) {
        Logger::error("Failed to query metrics by device: " + query.lastError().text());
        return metricsList;
    }

    while (query.next()) {
        metricsList.append(metricsFromQuery(query));
    }

    Logger::debug("Found " + QString::number(metricsList.size()) + " metrics for device " + deviceId);
    return metricsList;
}

QList<NetworkMetrics> MetricsDao::findByDateRange(const QString& deviceId,
                                                   const QDateTime& start,
                                                   const QDateTime& end) {
    QList<NetworkMetrics> metricsList;

    QSqlQuery query(dbManager->database());
    query.prepare(R"(
        SELECT * FROM metrics_history
        WHERE device_id = :device_id
        AND timestamp BETWEEN :start AND :end
        ORDER BY timestamp ASC
    )");

    query.bindValue(":device_id", deviceId);
    query.bindValue(":start", start.toString(Qt::ISODate));
    query.bindValue(":end", end.toString(Qt::ISODate));

    if (!query.exec()) {
        Logger::error("Failed to query metrics by date range: " + query.lastError().text());
        return metricsList;
    }

    while (query.next()) {
        metricsList.append(metricsFromQuery(query));
    }

    Logger::debug("Found " + QString::number(metricsList.size()) +
                 " metrics for device " + deviceId + " in date range");
    return metricsList;
}

NetworkMetrics MetricsDao::getAverageMetrics(const QString& deviceId,
                                             const QDateTime& start,
                                             const QDateTime& end) {
    NetworkMetrics avgMetrics;

    QSqlQuery query(dbManager->database());
    query.prepare(R"(
        SELECT
            AVG(latency_min) as avg_latency_min,
            AVG(latency_avg) as avg_latency_avg,
            AVG(latency_max) as avg_latency_max,
            AVG(latency_median) as avg_latency_median,
            AVG(latency_stddev) as avg_latency_stddev,
            AVG(jitter) as avg_jitter,
            AVG(packet_loss) as avg_packet_loss,
            SUM(packets_sent) as total_packets_sent,
            SUM(packets_received) as total_packets_received,
            AVG(quality_score) as avg_quality_score
        FROM metrics_history
        WHERE device_id = :device_id
        AND timestamp BETWEEN :start AND :end
    )");

    query.bindValue(":device_id", deviceId);
    query.bindValue(":start", start.toString(Qt::ISODate));
    query.bindValue(":end", end.toString(Qt::ISODate));

    if (!query.exec() || !query.next()) {
        Logger::error("Failed to calculate average metrics: " + query.lastError().text());
        return avgMetrics;
    }

    avgMetrics.setLatencyMin(query.value("avg_latency_min").toDouble());
    avgMetrics.setLatencyAvg(query.value("avg_latency_avg").toDouble());
    avgMetrics.setLatencyMax(query.value("avg_latency_max").toDouble());
    avgMetrics.setLatencyMedian(query.value("avg_latency_median").toDouble());
    // latencyStdDev not available in NetworkMetrics
    avgMetrics.setJitter(query.value("avg_jitter").toDouble());
    avgMetrics.setPacketLoss(query.value("avg_packet_loss").toDouble());
    // packetsSent and packetsReceived not available in NetworkMetrics
    avgMetrics.setQualityScore(static_cast<NetworkMetrics::QualityScore>(query.value("avg_quality_score").toInt()));

    Logger::debug("Calculated average metrics for device " + deviceId);
    return avgMetrics;
}

double MetricsDao::getMaxLatency(const QString& deviceId,
                                 const QDateTime& start,
                                 const QDateTime& end) {
    QSqlQuery query(dbManager->database());
    query.prepare(R"(
        SELECT MAX(latency_max) as max_latency
        FROM metrics_history
        WHERE device_id = :device_id
        AND timestamp BETWEEN :start AND :end
    )");

    query.bindValue(":device_id", deviceId);
    query.bindValue(":start", start.toString(Qt::ISODate));
    query.bindValue(":end", end.toString(Qt::ISODate));

    if (!query.exec() || !query.next()) {
        Logger::error("Failed to get max latency: " + query.lastError().text());
        return 0.0;
    }

    return query.value("max_latency").toDouble();
}

double MetricsDao::getMinLatency(const QString& deviceId,
                                 const QDateTime& start,
                                 const QDateTime& end) {
    QSqlQuery query(dbManager->database());
    query.prepare(R"(
        SELECT MIN(latency_min) as min_latency
        FROM metrics_history
        WHERE device_id = :device_id
        AND timestamp BETWEEN :start AND :end
    )");

    query.bindValue(":device_id", deviceId);
    query.bindValue(":start", start.toString(Qt::ISODate));
    query.bindValue(":end", end.toString(Qt::ISODate));

    if (!query.exec() || !query.next()) {
        Logger::error("Failed to get min latency: " + query.lastError().text());
        return 0.0;
    }

    return query.value("min_latency").toDouble();
}

double MetricsDao::getAveragePacketLoss(const QString& deviceId,
                                        const QDateTime& start,
                                        const QDateTime& end) {
    QSqlQuery query(dbManager->database());
    query.prepare(R"(
        SELECT AVG(packet_loss) as avg_packet_loss
        FROM metrics_history
        WHERE device_id = :device_id
        AND timestamp BETWEEN :start AND :end
    )");

    query.bindValue(":device_id", deviceId);
    query.bindValue(":start", start.toString(Qt::ISODate));
    query.bindValue(":end", end.toString(Qt::ISODate));

    if (!query.exec() || !query.next()) {
        Logger::error("Failed to get average packet loss: " + query.lastError().text());
        return 0.0;
    }

    return query.value("avg_packet_loss").toDouble();
}

double MetricsDao::getAverageJitter(const QString& deviceId,
                                    const QDateTime& start,
                                    const QDateTime& end) {
    QSqlQuery query(dbManager->database());
    query.prepare(R"(
        SELECT AVG(jitter) as avg_jitter
        FROM metrics_history
        WHERE device_id = :device_id
        AND timestamp BETWEEN :start AND :end
    )");

    query.bindValue(":device_id", deviceId);
    query.bindValue(":start", start.toString(Qt::ISODate));
    query.bindValue(":end", end.toString(Qt::ISODate));

    if (!query.exec() || !query.next()) {
        Logger::error("Failed to get average jitter: " + query.lastError().text());
        return 0.0;
    }

    return query.value("avg_jitter").toDouble();
}

int MetricsDao::deleteOlderThan(const QDateTime& cutoffDate) {
    QSqlQuery query(dbManager->database());
    query.prepare("DELETE FROM metrics_history WHERE timestamp < :cutoff");
    query.bindValue(":cutoff", cutoffDate.toString(Qt::ISODate));

    if (!query.exec()) {
        Logger::error("Failed to delete old metrics: " + query.lastError().text());
        return 0;
    }

    int deletedCount = query.numRowsAffected();
    Logger::info("Deleted " + QString::number(deletedCount) + " old metrics records");
    return deletedCount;
}

int MetricsDao::deleteByDevice(const QString& deviceId) {
    QSqlQuery query(dbManager->database());
    query.prepare("DELETE FROM metrics_history WHERE device_id = :device_id");
    query.bindValue(":device_id", deviceId);

    if (!query.exec()) {
        Logger::error("Failed to delete metrics for device: " + query.lastError().text());
        return 0;
    }

    int deletedCount = query.numRowsAffected();
    Logger::info("Deleted " + QString::number(deletedCount) + " metrics for device " + deviceId);
    return deletedCount;
}

int MetricsDao::getMetricsCount() {
    QSqlQuery query(dbManager->database());
    query.prepare("SELECT COUNT(*) FROM metrics_history");

    if (!query.exec() || !query.next()) {
        Logger::error("Failed to get metrics count: " + query.lastError().text());
        return 0;
    }

    return query.value(0).toInt();
}

int MetricsDao::getMetricsCountByDevice(const QString& deviceId) {
    QSqlQuery query(dbManager->database());
    query.prepare("SELECT COUNT(*) FROM metrics_history WHERE device_id = :device_id");
    query.bindValue(":device_id", deviceId);

    if (!query.exec() || !query.next()) {
        Logger::error("Failed to get metrics count by device: " + query.lastError().text());
        return 0;
    }

    return query.value(0).toInt();
}

NetworkMetrics MetricsDao::metricsFromQuery(QSqlQuery& query) {
    NetworkMetrics metrics;

    metrics.setLatencyMin(query.value("latency_min").toDouble());
    metrics.setLatencyAvg(query.value("latency_avg").toDouble());
    metrics.setLatencyMax(query.value("latency_max").toDouble());
    metrics.setLatencyMedian(query.value("latency_median").toDouble());
    // latencyStdDev not available in NetworkMetrics
    metrics.setJitter(query.value("jitter").toDouble());
    metrics.setPacketLoss(query.value("packet_loss").toDouble());
    // packetsSent and packetsReceived not available in NetworkMetrics
    metrics.setQualityScore(static_cast<NetworkMetrics::QualityScore>(query.value("quality_score").toInt()));

    // Set timestamp from database
    QString timestampStr = query.value("timestamp").toString();
    if (!timestampStr.isEmpty()) {
        metrics.setTimestamp(QDateTime::fromString(timestampStr, Qt::ISODate));
    }

    return metrics;
}
