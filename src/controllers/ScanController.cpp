#include "controllers/ScanController.h"
#include "../models/Device.h"
#include "database/DeviceRepository.h"
#include "database/DeviceCache.h"
#include "../utils/Logger.h"

ScanController::ScanController(
    ScanCoordinator* coordinator,
    DeviceRepository* repository,
    DeviceCache* cache,
    QObject* parent
)
    : QObject(parent)
    , coordinator(coordinator)
    , repository(repository)
    , cache(cache)
{
    connectSignals();
    Logger::info("ScanController initialized");
}

void ScanController::executeQuickScan(const QString& subnet) {
    Logger::info("Executing quick scan on " + subnet);
    ScanCoordinator::ScanConfig config = createQuickScanConfig(subnet);
    coordinator->startScan(config);
}

void ScanController::executeDeepScan(const QString& subnet) {
    Logger::info("Executing deep scan on " + subnet);
    ScanCoordinator::ScanConfig config = createDeepScanConfig(subnet);
    coordinator->startScan(config);
}

void ScanController::executeCustomScan(const ScanCoordinator::ScanConfig& config) {
    Logger::info("Executing custom scan on " + config.subnet);
    coordinator->startScan(config);
}

void ScanController::stopCurrentScan() {
    Logger::info("Stopping current scan");
    coordinator->stopScan();
}

void ScanController::pauseCurrentScan() {
    Logger::info("Pausing current scan");
    coordinator->pauseScan();
}

void ScanController::resumeCurrentScan() {
    Logger::info("Resuming current scan");
    coordinator->resumeScan();
}

QList<Device> ScanController::getAllDevices() {
    // First try to get from cache
    QList<Device> devices = cache->getAll();

    if (devices.isEmpty()) {
        // Fallback to repository
        devices = repository->findAll();

        // Populate cache
        for (const Device& device : devices) {
            cache->put(device.getIp(), device);
        }
    }

    return devices;
}

void ScanController::clearAllDevices() {
    Logger::info("Clearing all devices");
    cache->clear();
    // Note: We don't clear the repository to preserve historical data
    emit devicesUpdated();
}

void ScanController::onScanStarted(int totalHosts) {
    QString status = "Scan started: " + QString::number(totalHosts) + " hosts to scan";
    Logger::info(status);
    emit scanStatusChanged(status);
}

void ScanController::onDeviceDiscovered(const Device& device) {
    Logger::debug("Device discovered: " + device.getIp());

    // Save to cache and repository
    saveDevice(device);

    // Emit updated notification
    emit devicesUpdated();
}

void ScanController::onScanProgress(int current, int total, const QString& currentIp) {
    double percentage = (total > 0) ? (current * 100.0 / total) : 0.0;
    emit scanProgressUpdated(current, total, percentage);

    if (current % 10 == 0) {  // Log every 10 hosts
        Logger::debug("Scan progress: " + QString::number(current) + "/" +
                      QString::number(total) + " (" +
                      QString::number(percentage, 'f', 1) + "%)");
    }
}

void ScanController::onScanCompleted(int count, qint64 duration) {
    QString status = "Scan completed: " + QString::number(count) +
                     " devices found in " + QString::number(duration) + " ms";
    Logger::info(status);
    emit scanStatusChanged(status);

    // Emit final notification
    emit devicesUpdated();
}

void ScanController::onScanError(const QString& error) {
    Logger::error("Scan error: " + error);
    emit scanStatusChanged("Error: " + error);
}

void ScanController::onScanPaused() {
    Logger::info("Scan paused");
    emit scanStatusChanged("Scan paused");
}

void ScanController::onScanResumed() {
    Logger::info("Scan resumed");
    emit scanStatusChanged("Scan resumed");
}

ScanCoordinator::ScanConfig ScanController::createQuickScanConfig(const QString& subnet) {
    ScanCoordinator::ScanConfig config;
    config.subnet = subnet;
    config.resolveDns = true;
    config.resolveArp = false;
    config.scanPorts = false;
    config.timeout = 1000;  // 1 second timeout for quick scan
    config.maxThreads = 0;  // Auto-detect
    return config;
}

ScanCoordinator::ScanConfig ScanController::createDeepScanConfig(const QString& subnet) {
    ScanCoordinator::ScanConfig config;
    config.subnet = subnet;
    config.resolveDns = true;
    config.resolveArp = true;
    config.scanPorts = true;
    config.portsToScan = {21, 22, 23, 25, 53, 80, 110, 135, 139, 143, 443, 445, 3389, 8080};
    config.timeout = 3000;  // 3 second timeout for deep scan
    config.maxThreads = 0;  // Auto-detect
    return config;
}

void ScanController::saveDevice(const Device& device) {
    // Save to cache
    cache->put(device.getIp(), device);

    // Save to repository (async to avoid blocking)
    if (repository) {
        try {
            repository->save(device);
        } catch (const std::exception& e) {
            Logger::error("Failed to save device to repository: " + QString(e.what()));
        }
    }
}

void ScanController::connectSignals() {
    connect(coordinator, &ScanCoordinator::scanStarted,
            this, &ScanController::onScanStarted);

    connect(coordinator, &ScanCoordinator::deviceDiscovered,
            this, &ScanController::onDeviceDiscovered);

    connect(coordinator, &ScanCoordinator::scanProgress,
            this, &ScanController::onScanProgress);

    connect(coordinator, &ScanCoordinator::scanCompleted,
            this, &ScanController::onScanCompleted);

    connect(coordinator, &ScanCoordinator::scanError,
            this, &ScanController::onScanError);

    connect(coordinator, &ScanCoordinator::scanPaused,
            this, &ScanController::onScanPaused);

    connect(coordinator, &ScanCoordinator::scanResumed,
            this, &ScanController::onScanResumed);
}
