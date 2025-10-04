#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMutex>

class DatabaseManager {
public:
    static DatabaseManager* instance();

    bool open(const QString& dbPath);
    void close();
    bool isOpen() const;

    bool executeQuery(const QString& query);
    QSqlQuery prepareQuery(const QString& query);
    bool createSchema();

    QString getLastError() const;

    // Transaction support
    bool beginTransaction();
    bool commit();
    bool rollback();

private:
    DatabaseManager();
    ~DatabaseManager();
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    bool createDevicesTable();
    bool createPortsTable();
    bool createMetricsTable();
    bool createIndices();
    bool createSchemaVersionTable();

    QSqlDatabase db;
    static DatabaseManager* _instance;
    static QMutex mutex;
    QString lastError;
};

#endif // DATABASEMANAGER_H
