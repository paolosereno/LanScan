#ifndef DEVICEREPOSITORY_H
#define DEVICEREPOSITORY_H

#include "interfaces/IDeviceRepository.h"
#include "database/DatabaseManager.h"
#include "database/DeviceCache.h"
#include <QSqlQuery>

class DeviceRepository : public IDeviceRepository {
public:
    explicit DeviceRepository(DatabaseManager* dbManager);
    ~DeviceRepository() override = default;

    // IDeviceRepository interface
    void save(const Device& device) override;
    Device findById(const QString& id) override;
    QList<Device> findAll() override;
    void remove(const QString& id) override;
    void clear() override;
    int count() const override;

    // Additional methods
    Device findByIp(const QString& ip);
    QList<Device> findBySubnet(const QString& cidr);
    void update(const Device& device);
    bool exists(const QString& id);

    // Cache management
    void clearCache();
    void enableCache(bool enable);

private:
    Device mapFromQuery(const QSqlQuery& query);
    void saveToDatabase(const Device& device);
    void updateInDatabase(const Device& device);
    void savePorts(const QString& deviceId, const QList<PortInfo>& ports);
    void saveMetrics(const QString& deviceId, const NetworkMetrics& metrics);

    DatabaseManager* db;
    DeviceCache cache;
    bool cacheEnabled;
};

#endif // DEVICEREPOSITORY_H
