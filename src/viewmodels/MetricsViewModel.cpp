#include "viewmodels/MetricsViewModel.h"
#include "controllers/MetricsController.h"
#include "interfaces/IDeviceRepository.h"
#include "utils/Logger.h"
#include <QTimer>

MetricsViewModel::MetricsViewModel(
    MetricsController* metricsController,
    IDeviceRepository* deviceRepository,
    QObject* parent
)
    : QObject(parent)
    , metricsController(metricsController)
    , deviceRepository(deviceRepository)
    , monitoring(false)
    , maxHistorySize(1000)
    , monitoringInterval(1000)
    , monitoringTimer(nullptr)
{
    if (!metricsController) {
        Logger::warn("MetricsViewModel: metricsController is null");
    }

    if (!deviceRepository) {
        Logger::warn("MetricsViewModel: deviceRepository is null");
    }

    // Create monitoring timer
    monitoringTimer = new QTimer(this);
    monitoringTimer->setSingleShot(false);
    connect(monitoringTimer, &QTimer::timeout,
            this, &MetricsViewModel::onMonitoringTimerTimeout);

    // Connect to MetricsController if available
    if (metricsController) {
        connect(metricsController, &MetricsController::metricsCollected,
                this, &MetricsViewModel::onMetricsCollected);
    }

    Logger::debug("MetricsViewModel: Initialized");
}

MetricsViewModel::~MetricsViewModel() {
    stopMonitoring();
    Logger::debug("MetricsViewModel: Destroyed");
}

void MetricsViewModel::setDevice(const Device& device) {
    // Compare by ID if available, otherwise by IP
    QString currentIdentifier = currentDevice.getId().isEmpty() ? currentDevice.getIp() : currentDevice.getId();
    QString newIdentifier = device.getId().isEmpty() ? device.getIp() : device.getId();

    if (currentIdentifier == newIdentifier && !currentIdentifier.isEmpty()) {
        return;
    }

    // Stop monitoring if active
    bool wasMonitoring = monitoring;
    if (wasMonitoring) {
        stopMonitoring();
    }

    // Clear history when device changes
    clearHistory();

    currentDevice = device;
    Logger::debug(QString("MetricsViewModel: Device set to %1 (%2)")
                      .arg(device.hostname().isEmpty() ? "Unknown" : device.hostname())
                      .arg(device.ip()));

    emit deviceChanged(device);

    // Restart monitoring if it was active
    if (wasMonitoring) {
        startMonitoring(monitoringInterval);
    }
}

Device MetricsViewModel::getCurrentDevice() const {
    return currentDevice;
}

void MetricsViewModel::startMonitoring(int intervalMs) {
    if (monitoring) {
        Logger::warn("MetricsViewModel: Monitoring already started");
        return;
    }

    if (intervalMs < 100) {
        Logger::warn(QString("MetricsViewModel: Invalid interval %1ms, using 100ms minimum").arg(intervalMs));
        intervalMs = 100;
    }

    if (intervalMs > 60000) {
        Logger::warn(QString("MetricsViewModel: Interval %1ms too large, capping at 60000ms").arg(intervalMs));
        intervalMs = 60000;
    }

    // Check if device has IP (required for ping operations)
    QString deviceIp = currentDevice.getIp();
    if (deviceIp.isEmpty()) {
        Logger::warn("MetricsViewModel: Cannot start monitoring without a device IP address");
        return;
    }

    if (!metricsController) {
        Logger::error("MetricsViewModel: Cannot start monitoring, metricsController is null");
        return;
    }

    monitoringInterval = intervalMs;
    monitoring = true;

    // Start continuous monitoring in MetricsController
    // MetricsController manages its own timer and calls to PingService
    // Always use IP address for network operations, not UUID
    metricsController->startContinuousMonitoring(deviceIp, monitoringInterval);

    Logger::info(QString("MetricsViewModel: Monitoring started for device %1 (%2) with interval %3ms")
                     .arg(currentDevice.hostname())
                     .arg(deviceIp)
                     .arg(monitoringInterval));

    emit monitoringStarted();
}

void MetricsViewModel::stopMonitoring() {
    if (!monitoring) {
        return;
    }

    monitoring = false;

    // Stop continuous monitoring in MetricsController
    if (metricsController) {
        QString deviceIp = currentDevice.getIp();
        if (!deviceIp.isEmpty()) {
            metricsController->stopContinuousMonitoring(deviceIp);
        }
    }

    Logger::info("MetricsViewModel: Monitoring stopped");
    emit monitoringStopped();
}

bool MetricsViewModel::isMonitoring() const {
    return monitoring;
}

NetworkMetrics MetricsViewModel::getLatestMetrics() const {
    return latestMetrics;
}

QList<NetworkMetrics> MetricsViewModel::getMetricsHistory() const {
    return metricsHistory;
}

void MetricsViewModel::clearHistory() {
    metricsHistory.clear();
    latestMetrics = NetworkMetrics();

    Logger::debug("MetricsViewModel: History cleared");
    emit historyCleared();
}

void MetricsViewModel::setMaxHistorySize(int size) {
    if (size < 1) {
        Logger::warn(QString("MetricsViewModel: Invalid maxHistorySize %1, using default 1000").arg(size));
        maxHistorySize = 1000;
        return;
    }

    if (size > 100000) {
        Logger::warn(QString("MetricsViewModel: maxHistorySize %1 too large, capping at 100000").arg(size));
        maxHistorySize = 100000;
    } else {
        maxHistorySize = size;
    }

    Logger::debug(QString("MetricsViewModel: MaxHistorySize set to %1").arg(maxHistorySize));

    // Prune existing history if needed
    pruneHistory();
}

int MetricsViewModel::getMaxHistorySize() const {
    return maxHistorySize;
}

void MetricsViewModel::onMetricsCollected(const QString& deviceId, const NetworkMetrics& metrics) {
    // Always use IP address for network operations comparison
    QString currentDeviceIp = currentDevice.getIp();

    // Only process metrics for the current device (compare by IP)
    if (deviceId != currentDeviceIp) {
        return;
    }

    // Update in main thread
    QMetaObject::invokeMethod(this, [this, metrics]() {
        latestMetrics = metrics;
        metricsHistory.append(metrics);
        pruneHistory();

        Logger::debug(QString("MetricsViewModel: Metrics updated - Latency: %1ms, PacketLoss: %2%, Jitter: %3ms")
                          .arg(metrics.latencyAvg(), 0, 'f', 2)
                          .arg(metrics.packetLoss(), 0, 'f', 2)
                          .arg(metrics.jitter(), 0, 'f', 2));

        emit metricsUpdated(metrics);
    }, Qt::QueuedConnection);
}

void MetricsViewModel::onMonitoringTimerTimeout() {
    if (!monitoring || !metricsController) {
        return;
    }

    // Always use IP address for network operations
    QString deviceIp = currentDevice.getIp();

    // Request metrics collection for current device
    Logger::debug(QString("MetricsViewModel: Collecting metrics for device %1 (%2)")
                      .arg(currentDevice.hostname())
                      .arg(deviceIp));
    metricsController->collectMetricsOnce(deviceIp);
}

void MetricsViewModel::pruneHistory() {
    while (metricsHistory.size() > maxHistorySize) {
        metricsHistory.removeFirst();
    }
}
