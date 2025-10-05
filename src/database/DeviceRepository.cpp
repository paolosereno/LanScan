#include "database/DeviceRepository.h"
#include "utils/Logger.h"
#include <QSqlError>
#include <QDateTime>
#include <QVariant>

DeviceRepository::DeviceRepository(DatabaseManager* dbManager)
    : db(dbManager), cacheEnabled(true) {
}

void DeviceRepository::save(const Device& device) {
    // Check if device exists by IP (not ID, since new devices don't have ID yet)
    Device existing = findByIp(device.getIp());

    if (!existing.getIp().isEmpty()) {
        // Device exists, update it
        Device updatedDevice = device;
        updatedDevice.setId(existing.getId());  // Keep the same ID
        update(updatedDevice);

        // Update cache
        if (cacheEnabled) {
            cache.put(updatedDevice.getId(), updatedDevice);
        }
    } else {
        // New device, insert it
        saveToDatabase(device);

        // Update cache
        if (cacheEnabled && !device.getId().isEmpty()) {
            cache.put(device.getId(), device);
        }
    }
}

Device DeviceRepository::findById(const QString& id) {
    // Check cache first
    if (cacheEnabled && cache.contains(id)) {
        Logger::debug("DeviceRepository: Cache hit for ID: " + id);
        return cache.get(id);
    }

    // Query database
    QSqlQuery query = db->prepareQuery("SELECT * FROM devices WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        Logger::error("DeviceRepository: Failed to find device by ID: " + query.lastError().text());
        return Device();
    }

    if (query.next()) {
        Device device = mapFromQuery(query);

        // Update cache
        if (cacheEnabled) {
            cache.put(id, device);
        }

        return device;
    }

    return Device(); // Not found
}

Device DeviceRepository::findByIp(const QString& ip) {
    QSqlQuery query = db->prepareQuery("SELECT * FROM devices WHERE ip = :ip");
    query.bindValue(":ip", ip);

    if (!query.exec()) {
        Logger::error("DeviceRepository: Failed to find device by IP: " + query.lastError().text());
        return Device();
    }

    if (query.next()) {
        Device device = mapFromQuery(query);

        // Update cache
        if (cacheEnabled) {
            cache.put(device.getId(), device);
        }

        return device;
    }

    return Device();
}

QList<Device> DeviceRepository::findAll() {
    QList<Device> devices;

    QSqlQuery query = db->prepareQuery("SELECT * FROM devices ORDER BY ip");

    if (!query.exec()) {
        Logger::error("DeviceRepository: Failed to find all devices: " + query.lastError().text());
        return devices;
    }

    while (query.next()) {
        Device device = mapFromQuery(query);
        devices.append(device);

        // Update cache
        if (cacheEnabled) {
            cache.put(device.getId(), device);
        }
    }

    return devices;
}

QList<Device> DeviceRepository::findBySubnet(const QString& cidr) {
    // For simplicity, we'll do pattern matching on IP prefix
    // More sophisticated subnet matching would require IP range calculation
    QList<Device> devices;

    QString pattern = cidr.split('/').first() + "%";
    QSqlQuery query = db->prepareQuery("SELECT * FROM devices WHERE ip LIKE :pattern");
    query.bindValue(":pattern", pattern);

    if (!query.exec()) {
        Logger::error("DeviceRepository: Failed to find devices by subnet: " + query.lastError().text());
        return devices;
    }

    while (query.next()) {
        devices.append(mapFromQuery(query));
    }

    return devices;
}

void DeviceRepository::remove(const QString& id) {
    QSqlQuery query = db->prepareQuery("DELETE FROM devices WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        Logger::error("DeviceRepository: Failed to remove device: " + query.lastError().text());
        return;
    }

    // Remove from cache
    if (cacheEnabled) {
        cache.remove(id);
    }

    Logger::info("DeviceRepository: Device removed: " + id);
}

void DeviceRepository::update(const Device& device) {
    updateInDatabase(device);

    // Update cache
    if (cacheEnabled) {
        cache.put(device.getId(), device);
    }
}

bool DeviceRepository::exists(const QString& id) {
    QSqlQuery query = db->prepareQuery("SELECT COUNT(*) FROM devices WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec() || !query.next()) {
        return false;
    }

    return query.value(0).toInt() > 0;
}

void DeviceRepository::clearCache() {
    cache.clear();
    Logger::info("DeviceRepository: Cache cleared");
}

void DeviceRepository::enableCache(bool enable) {
    cacheEnabled = enable;
    if (!enable) {
        clearCache();
    }
}

void DeviceRepository::clear() {
    db->executeQuery("DELETE FROM devices");
    clearCache();
    Logger::info("DeviceRepository: All devices cleared");
}

int DeviceRepository::count() const {
    QSqlQuery query = db->prepareQuery("SELECT COUNT(*) FROM devices");
    if (!query.exec() || !query.next()) {
        return 0;
    }
    return query.value(0).toInt();
}

Device DeviceRepository::mapFromQuery(const QSqlQuery& query) {
    Device device;
    device.setId(query.value("id").toString());
    device.setIp(query.value("ip").toString());
    device.setHostname(query.value("hostname").toString());
    device.setMacAddress(query.value("mac_address").toString());
    device.setVendor(query.value("vendor").toString());
    device.setOnline(query.value("is_online").toBool());
    device.setLastSeen(query.value("last_seen").toDateTime());

    // Load ports for this device
    QSqlQuery portsQuery = db->prepareQuery(
        "SELECT * FROM ports WHERE device_id = :device_id"
    );
    portsQuery.bindValue(":device_id", device.getId());

    if (portsQuery.exec()) {
        QList<PortInfo> ports;
        while (portsQuery.next()) {
            PortInfo port;
            port.setPortNumber(portsQuery.value("port_number").toInt());

            // Convert protocol string to enum
            QString protocolStr = portsQuery.value("protocol").toString();
            port.setProtocol(protocolStr == "UDP" ? PortInfo::UDP : PortInfo::TCP);

            port.setService(portsQuery.value("service").toString());

            // Convert state string to enum
            QString stateStr = portsQuery.value("state").toString();
            if (stateStr == "Closed") {
                port.setState(PortInfo::Closed);
            } else if (stateStr == "Filtered") {
                port.setState(PortInfo::Filtered);
            } else {
                port.setState(PortInfo::Open);
            }

            ports.append(port);
        }
        device.setOpenPorts(ports);
    }

    return device;
}

void DeviceRepository::saveToDatabase(const Device& device) {
    QString query = R"(
        INSERT INTO devices (id, ip, hostname, mac_address, vendor, is_online, last_seen)
        VALUES (:id, :ip, :hostname, :mac, :vendor, :online, :last_seen)
    )";

    QSqlQuery sqlQuery = db->prepareQuery(query);
    sqlQuery.bindValue(":id", device.getId());
    sqlQuery.bindValue(":ip", device.getIp());
    sqlQuery.bindValue(":hostname", device.getHostname());
    sqlQuery.bindValue(":mac", device.getMacAddress());
    sqlQuery.bindValue(":vendor", device.getVendor());
    sqlQuery.bindValue(":online", device.isOnline() ? 1 : 0);
    sqlQuery.bindValue(":last_seen", device.getLastSeen());

    if (!sqlQuery.exec()) {
        Logger::error("DeviceRepository: Failed to save device: " + sqlQuery.lastError().text());
        return;
    }

    // Save ports
    savePorts(device.getId(), device.getOpenPorts());

    Logger::info("DeviceRepository: Device saved: " + device.getIp());
}

void DeviceRepository::updateInDatabase(const Device& device) {
    QString query = R"(
        UPDATE devices
        SET ip = :ip, hostname = :hostname, mac_address = :mac,
            vendor = :vendor, is_online = :online, last_seen = :last_seen,
            updated_at = CURRENT_TIMESTAMP
        WHERE id = :id
    )";

    QSqlQuery sqlQuery = db->prepareQuery(query);
    sqlQuery.bindValue(":id", device.getId());
    sqlQuery.bindValue(":ip", device.getIp());
    sqlQuery.bindValue(":hostname", device.getHostname());
    sqlQuery.bindValue(":mac", device.getMacAddress());
    sqlQuery.bindValue(":vendor", device.getVendor());
    sqlQuery.bindValue(":online", device.isOnline() ? 1 : 0);
    sqlQuery.bindValue(":last_seen", device.getLastSeen());

    if (!sqlQuery.exec()) {
        Logger::error("DeviceRepository: Failed to update device: " + sqlQuery.lastError().text());
        return;
    }

    // Delete old ports and save new ones
    QSqlQuery deleteQuery = db->prepareQuery("DELETE FROM ports WHERE device_id = :id");
    deleteQuery.bindValue(":id", device.getId());
    deleteQuery.exec();

    savePorts(device.getId(), device.getOpenPorts());

    Logger::info("DeviceRepository: Device updated: " + device.getIp());
}

void DeviceRepository::savePorts(const QString& deviceId, const QList<PortInfo>& ports) {
    for (const PortInfo& port : ports) {
        QString query = R"(
            INSERT INTO ports (device_id, port_number, protocol, service, state)
            VALUES (:device_id, :port, :protocol, :service, :state)
        )";

        QSqlQuery sqlQuery = db->prepareQuery(query);
        sqlQuery.bindValue(":device_id", deviceId);
        sqlQuery.bindValue(":port", port.getPort());
        sqlQuery.bindValue(":protocol", port.protocolString());
        sqlQuery.bindValue(":service", port.getService());
        sqlQuery.bindValue(":state", port.stateString());

        if (!sqlQuery.exec()) {
            Logger::warn("DeviceRepository: Failed to save port: " + sqlQuery.lastError().text());
        }
    }
}

void DeviceRepository::saveMetrics(const QString& deviceId, const NetworkMetrics& metrics) {
    QString query = R"(
        INSERT INTO metrics (device_id, latency_min, latency_avg, latency_max,
                           latency_median, jitter, packet_loss, quality_score)
        VALUES (:device_id, :min, :avg, :max, :median, :jitter, :loss, :quality)
    )";

    QSqlQuery sqlQuery = db->prepareQuery(query);
    sqlQuery.bindValue(":device_id", deviceId);
    sqlQuery.bindValue(":min", metrics.getLatencyMin());
    sqlQuery.bindValue(":avg", metrics.getLatencyAvg());
    sqlQuery.bindValue(":max", metrics.getLatencyMax());
    sqlQuery.bindValue(":median", metrics.getLatencyMedian());
    sqlQuery.bindValue(":jitter", metrics.getJitter());
    sqlQuery.bindValue(":loss", metrics.getPacketLoss());
    sqlQuery.bindValue(":quality", metrics.getQualityScore());

    if (!sqlQuery.exec()) {
        Logger::warn("DeviceRepository: Failed to save metrics: " + sqlQuery.lastError().text());
    }
}
