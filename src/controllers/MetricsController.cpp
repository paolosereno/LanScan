#include "controllers/MetricsController.h"
#include "../network/diagnostics/MetricsAggregator.h"
#include "database/DeviceRepository.h"
#include "../models/NetworkMetrics.h"
#include "../models/Device.h"
#include "../utils/Logger.h"

#include <QTimer>

MetricsController::MetricsController(
    MetricsAggregator* aggregator,
    DeviceRepository* repository,
    QObject* parent
)
    : QObject(parent)
    , aggregator(aggregator)
    , repository(repository)
    , defaultInterval(1000)
{
    // Connect aggregator signals
    connect(aggregator, &MetricsAggregator::metricsUpdated,
            this, &MetricsController::onMetricsUpdated);

    Logger::info("MetricsController initialized");
}

MetricsController::~MetricsController() {
    stopMonitoringAll();
}

void MetricsController::startContinuousMonitoring(const QString& deviceId, int intervalMs) {
    if (deviceId.isEmpty()) {
        Logger::warn("Cannot start monitoring: empty device ID");
        return;
    }

    if (monitoringTimers.contains(deviceId)) {
        Logger::warn("Device " + deviceId + " is already being monitored");
        return;
    }

    Logger::info("Starting continuous monitoring for " + deviceId +
                 " (interval: " + QString::number(intervalMs) + "ms)");

    setupTimer(deviceId, intervalMs);
    emit monitoringStarted(deviceId);

    // Collect initial metrics immediately
    collectMetricsForDevice(deviceId);
}

void MetricsController::stopContinuousMonitoring(const QString& deviceId) {
    if (!monitoringTimers.contains(deviceId)) {
        Logger::warn("Device " + deviceId + " is not being monitored");
        return;
    }

    Logger::info("Stopping continuous monitoring for " + deviceId);

    cleanupTimer(deviceId);
    emit monitoringStopped(deviceId);
}

void MetricsController::collectMetricsOnce(const QString& deviceId) {
    if (deviceId.isEmpty()) {
        Logger::warn("Cannot collect metrics: empty device ID");
        emit metricsError(deviceId, "Empty device ID");
        return;
    }

    Logger::debug("Collecting metrics once for " + deviceId);
    collectMetricsForDevice(deviceId);
}

void MetricsController::startMonitoringAll(int intervalMs) {
    Logger::info("Starting monitoring for all devices");

    QList<Device> devices = repository->findAll();

    for (const Device& device : devices) {
        if (!device.getIp().isEmpty() && device.isOnline()) {
            startContinuousMonitoring(device.getIp(), intervalMs);
        }
    }

    Logger::info("Started monitoring " + QString::number(monitoringTimers.size()) + " devices");
}

void MetricsController::stopMonitoringAll() {
    Logger::info("Stopping monitoring for all devices");

    QStringList deviceIds = monitoringTimers.keys();
    for (const QString& deviceId : deviceIds) {
        stopContinuousMonitoring(deviceId);
    }

    Logger::info("All monitoring stopped");
}

bool MetricsController::isMonitoring(const QString& deviceId) const {
    return monitoringTimers.contains(deviceId);
}

int MetricsController::getMonitoredDeviceCount() const {
    return monitoringTimers.size();
}

void MetricsController::onMetricsUpdated(const NetworkMetrics& metrics) {
    // This slot is called when aggregator updates metrics
    // Emit with the current monitoring device ID
    if (!currentMonitoringDevice.isEmpty()) {
        Logger::debug(QString("Metrics updated for device %1").arg(currentMonitoringDevice));
        emit metricsCollected(currentMonitoringDevice, metrics);

        // Optionally save to repository
        saveMetrics(currentMonitoringDevice, metrics);
    } else {
        Logger::warn("Metrics updated but no current monitoring device set");
    }
}

void MetricsController::collectMetricsForDevice(const QString& deviceId) {
    try {
        // Set current monitoring device
        currentMonitoringDevice = deviceId;

        // Start collecting metrics using aggregator
        aggregator->startContinuousCollection(deviceId);

        Logger::debug(QString("Started metrics collection for device %1").arg(deviceId));

        // Note: Metrics will be emitted via metricsUpdated signal
        // We'll handle saving in the onMetricsUpdated slot if needed

    } catch (const std::exception& e) {
        QString error = "Failed to collect metrics: " + QString(e.what());
        Logger::error(error);
        emit metricsError(deviceId, error);
    }
}

void MetricsController::setupTimer(const QString& deviceId, int intervalMs) {
    QTimer* timer = new QTimer(this);
    timer->setInterval(intervalMs);

    // Connect timer to collection function
    connect(timer, &QTimer::timeout, this, [this, deviceId]() {
        collectMetricsForDevice(deviceId);
    });

    monitoringTimers[deviceId] = timer;
    timer->start();
}

void MetricsController::cleanupTimer(const QString& deviceId) {
    if (monitoringTimers.contains(deviceId)) {
        QTimer* timer = monitoringTimers.take(deviceId);
        timer->stop();
        timer->deleteLater();
    }

    // Stop aggregator collection if this is the current device
    if (aggregator && aggregator->isCollecting() && currentMonitoringDevice == deviceId) {
        aggregator->stopContinuousCollection();
        currentMonitoringDevice.clear();
        Logger::debug(QString("Stopped metrics collection for device %1").arg(deviceId));
    }
}

void MetricsController::saveMetrics(const QString& deviceId, const NetworkMetrics& metrics) {
    if (!repository) {
        return;
    }

    try {
        // Find the device in repository
        Device device = repository->findByIp(deviceId);

        if (device.getIp().isEmpty()) {
            Logger::warn("Device " + deviceId + " not found in repository");
            return;
        }

        // Update device metrics
        device.setMetrics(metrics);

        // Save updated device
        repository->update(device);

        Logger::debug("Metrics saved for device " + deviceId);

    } catch (const std::exception& e) {
        Logger::error("Failed to save metrics: " + QString(e.what()));
    }
}
