#include "database/DeviceCache.h"
#include <QMutexLocker>

DeviceCache::DeviceCache() : maxCacheSize(1000) {
}

void DeviceCache::put(const QString& key, const Device& device) {
    QMutexLocker locker(&mutex);

    // If key exists, remove from access order
    if (cache.contains(key)) {
        accessOrder.removeAll(key);
    } else if (cache.size() >= maxCacheSize) {
        // Cache is full, evict LRU entry
        evictLRU();
    }

    cache[key] = device;
    accessOrder.enqueue(key);
}

Device DeviceCache::get(const QString& key) {
    QMutexLocker locker(&mutex);

    if (!cache.contains(key)) {
        return Device(); // Return empty device if not found
    }

    // Update access order (move to end for LRU)
    updateAccessOrder(key);

    return cache[key];
}

bool DeviceCache::contains(const QString& key) const {
    QMutexLocker locker(&mutex);
    return cache.contains(key);
}

void DeviceCache::remove(const QString& key) {
    QMutexLocker locker(&mutex);

    if (cache.contains(key)) {
        cache.remove(key);
        accessOrder.removeAll(key);
    }
}

void DeviceCache::clear() {
    QMutexLocker locker(&mutex);
    cache.clear();
    accessOrder.clear();
}

QList<Device> DeviceCache::getAll() const {
    QMutexLocker locker(&mutex);
    return cache.values();
}

void DeviceCache::setMaxSize(int size) {
    QMutexLocker locker(&mutex);
    maxCacheSize = size;

    // Evict entries if current size exceeds new max size
    while (cache.size() > maxCacheSize) {
        evictLRU();
    }
}

int DeviceCache::getMaxSize() const {
    QMutexLocker locker(&mutex);
    return maxCacheSize;
}

int DeviceCache::getCurrentSize() const {
    QMutexLocker locker(&mutex);
    return cache.size();
}

void DeviceCache::evictLRU() {
    // Assumes mutex is already locked by caller
    if (accessOrder.isEmpty()) {
        return;
    }

    QString lruKey = accessOrder.dequeue();
    cache.remove(lruKey);
}

void DeviceCache::updateAccessOrder(const QString& key) {
    // Assumes mutex is already locked by caller
    accessOrder.removeAll(key);
    accessOrder.enqueue(key);
}
