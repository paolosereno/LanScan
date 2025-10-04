#ifndef DEVICECACHE_H
#define DEVICECACHE_H

#include "models/Device.h"
#include <QMap>
#include <QQueue>
#include <QMutex>
#include <QList>

class DeviceCache {
public:
    DeviceCache();

    void put(const QString& key, const Device& device);
    Device get(const QString& key);
    bool contains(const QString& key) const;
    void remove(const QString& key);
    void clear();
    QList<Device> getAll() const;

    // Cache eviction policy
    void setMaxSize(int size);
    int getMaxSize() const;
    int getCurrentSize() const;

private:
    void evictLRU();
    void updateAccessOrder(const QString& key);

    QMap<QString, Device> cache;
    QQueue<QString> accessOrder; // For LRU tracking
    int maxCacheSize;
    mutable QMutex mutex;
};

#endif // DEVICECACHE_H
