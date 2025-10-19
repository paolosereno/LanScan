#include "database/DatabaseManager.h"
#include "utils/Logger.h"
#include <QSqlError>
#include <QMutexLocker>

DatabaseManager* DatabaseManager::_instance = nullptr;
QMutex DatabaseManager::mutex;

DatabaseManager::DatabaseManager() {
}

DatabaseManager::~DatabaseManager() {
    close();
}

DatabaseManager* DatabaseManager::instance() {
    if (_instance == nullptr) {
        QMutexLocker locker(&mutex);
        if (_instance == nullptr) {
            _instance = new DatabaseManager();
        }
    }
    return _instance;
}

bool DatabaseManager::open(const QString& dbPath) {
    if (db.isOpen()) {
        Logger::warn("DatabaseManager: Database already open");
        return true;
    }

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbPath);

    if (!db.open()) {
        lastError = db.lastError().text();
        Logger::error("DatabaseManager: Failed to open database: " + lastError);
        return false;
    }

    Logger::info("DatabaseManager: Database opened successfully: " + dbPath);

    // Create schema if it doesn't exist
    if (!createSchema()) {
        Logger::error("DatabaseManager: Failed to create schema");
        return false;
    }

    return true;
}

void DatabaseManager::close() {
    if (db.isOpen()) {
        db.close();
        Logger::info("DatabaseManager: Database closed");
    }
}

bool DatabaseManager::isOpen() const {
    return db.isOpen();
}

bool DatabaseManager::executeQuery(const QString& query) {
    QSqlQuery sqlQuery(db);
    if (!sqlQuery.exec(query)) {
        lastError = sqlQuery.lastError().text();
        Logger::error("DatabaseManager: Query execution failed: " + lastError);
        return false;
    }
    return true;
}

QSqlQuery DatabaseManager::prepareQuery(const QString& query) {
    QSqlQuery sqlQuery(db);
    if (!sqlQuery.prepare(query)) {
        lastError = sqlQuery.lastError().text();
        Logger::error("DatabaseManager: Query preparation failed: " + lastError);
    }
    return sqlQuery;
}

bool DatabaseManager::createSchema() {
    if (!isOpen()) {
        lastError = "Database not open";
        Logger::error("DatabaseManager: " + lastError);
        return false;
    }

    if (!beginTransaction()) {
        return false;
    }

    bool success = createSchemaVersionTable() &&
                   createDevicesTable() &&
                   createPortsTable() &&
                   createMetricsTable() &&
                   createIndices();

    if (success) {
        if (!commit()) {
            rollback();
            return false;
        }
        Logger::info("DatabaseManager: Schema created successfully");
        return true;
    } else {
        rollback();
        Logger::error("DatabaseManager: Schema creation failed");
        return false;
    }
}

bool DatabaseManager::createSchemaVersionTable() {
    QString query = R"(
        CREATE TABLE IF NOT EXISTS schema_version (
            version TEXT PRIMARY KEY,
            applied_at DATETIME DEFAULT CURRENT_TIMESTAMP
        )
    )";

    if (!executeQuery(query)) {
        return false;
    }

    // Insert version 1.0
    QString insertQuery = R"(
        INSERT OR IGNORE INTO schema_version (version) VALUES ('1.0')
    )";

    return executeQuery(insertQuery);
}

bool DatabaseManager::createDevicesTable() {
    QString query = R"(
        CREATE TABLE IF NOT EXISTS devices (
            id TEXT PRIMARY KEY,
            ip TEXT NOT NULL UNIQUE,
            hostname TEXT,
            mac_address TEXT,
            vendor TEXT,
            is_online INTEGER,
            last_seen DATETIME,
            comments TEXT,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            updated_at DATETIME DEFAULT CURRENT_TIMESTAMP
        )
    )";

    return executeQuery(query);
}

bool DatabaseManager::createPortsTable() {
    QString query = R"(
        CREATE TABLE IF NOT EXISTS ports (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            device_id TEXT,
            port_number INTEGER,
            protocol TEXT,
            service TEXT,
            state TEXT,
            FOREIGN KEY (device_id) REFERENCES devices(id) ON DELETE CASCADE
        )
    )";

    return executeQuery(query);
}

bool DatabaseManager::createMetricsTable() {
    QString query = R"(
        CREATE TABLE IF NOT EXISTS metrics (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            device_id TEXT,
            latency_min REAL,
            latency_avg REAL,
            latency_max REAL,
            latency_median REAL,
            jitter REAL,
            packet_loss REAL,
            quality_score TEXT,
            timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (device_id) REFERENCES devices(id) ON DELETE CASCADE
        )
    )";

    return executeQuery(query);
}

bool DatabaseManager::createIndices() {
    QStringList indices = {
        "CREATE INDEX IF NOT EXISTS idx_devices_ip ON devices(ip)",
        "CREATE INDEX IF NOT EXISTS idx_devices_last_seen ON devices(last_seen)",
        "CREATE INDEX IF NOT EXISTS idx_metrics_device_timestamp ON metrics(device_id, timestamp)",
        "CREATE INDEX IF NOT EXISTS idx_ports_device ON ports(device_id)"
    };

    for (const QString& index : indices) {
        if (!executeQuery(index)) {
            return false;
        }
    }

    return true;
}

QString DatabaseManager::getLastError() const {
    return lastError;
}

QSqlDatabase DatabaseManager::database() {
    return db;
}

bool DatabaseManager::beginTransaction() {
    if (!db.transaction()) {
        lastError = db.lastError().text();
        Logger::error("DatabaseManager: Failed to begin transaction: " + lastError);
        return false;
    }
    return true;
}

bool DatabaseManager::commit() {
    if (!db.commit()) {
        lastError = db.lastError().text();
        Logger::error("DatabaseManager: Failed to commit transaction: " + lastError);
        return false;
    }
    return true;
}

bool DatabaseManager::rollback() {
    if (!db.rollback()) {
        lastError = db.lastError().text();
        Logger::error("DatabaseManager: Failed to rollback transaction: " + lastError);
        return false;
    }
    return true;
}
