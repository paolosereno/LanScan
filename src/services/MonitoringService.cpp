#include "services/MonitoringService.h"
#include "controllers/MetricsController.h"
#include "services/AlertService.h"
#include "services/HistoryService.h"
#include "models/NetworkMetrics.h"
#include "utils/Logger.h"

MonitoringService::MonitoringService(
    MetricsController* metricsController,
    AlertService* alertService,
    HistoryService* historyService,
    QObject* parent)
    : QObject(parent)
    , m_metricsController(metricsController)
    , m_alertService(alertService)
    , m_historyService(historyService)
{
    if (!m_metricsController) {
        Logger::error("MonitoringService: MetricsController is null");
    }
    if (!m_alertService) {
        Logger::error("MonitoringService: AlertService is null");
    }
    if (!m_historyService) {
        Logger::error("MonitoringService: HistoryService is null");
    }

    // Connect signals from MetricsController
    if (m_metricsController) {
        connect(m_metricsController, &MetricsController::metricsCollected,
                this, &MonitoringService::onMetricsCollected);
        connect(m_metricsController, &MetricsController::metricsError,
                this, &MonitoringService::onMetricsError);
    }

    Logger::info("MonitoringService initialized");
}

MonitoringService::~MonitoringService()
{
    stopAllMonitoring();
    Logger::info("MonitoringService destroyed");
}

void MonitoringService::startMonitoring(const MonitoringConfig& config)
{
    if (config.deviceId.isEmpty()) {
        Logger::warn("MonitoringService: Cannot start monitoring with empty device ID");
        return;
    }

    if (!m_metricsController) {
        Logger::error("MonitoringService: Cannot start monitoring, MetricsController is null");
        return;
    }

    // Stop existing monitoring if any
    if (isMonitoring(config.deviceId)) {
        stopMonitoring(config.deviceId);
    }

    // Store configuration
    m_monitoringConfigs[config.deviceId] = config;

    // Initialize device status
    m_lastDeviceStatus[config.deviceId] = false;

    // Start metrics collection via MetricsController
    if (m_metricsController) {
        m_metricsController->startContinuousMonitoring(config.deviceId, config.intervalMs);
    }

    // Store event in history
    if (config.enableHistory && m_historyService) {
        storeEvent(config.deviceId, "monitoring_started",
                  QString("Monitoring started with interval: %1ms").arg(config.intervalMs));
    }

    Logger::info(QString("MonitoringService: Started monitoring device %1 with interval %2ms")
                 .arg(config.deviceId).arg(config.intervalMs));

    emit monitoringStarted(config.deviceId);
}

void MonitoringService::startMonitoring(const QString& deviceId, int intervalMs)
{
    MonitoringConfig config(deviceId, intervalMs);
    startMonitoring(config);
}

void MonitoringService::stopMonitoring(const QString& deviceId)
{
    if (!isMonitoring(deviceId)) {
        Logger::debug(QString("MonitoringService: Device %1 is not being monitored").arg(deviceId));
        return;
    }

    // Stop metrics collection
    if (m_metricsController) {
        m_metricsController->stopContinuousMonitoring(deviceId);
    }

    // Get config before removing
    MonitoringConfig config = m_monitoringConfigs.value(deviceId);

    // Remove from tracking maps
    m_monitoringConfigs.remove(deviceId);
    m_lastDeviceStatus.remove(deviceId);

    // Store event in history
    if (config.enableHistory && m_historyService) {
        storeEvent(deviceId, "monitoring_stopped", "Monitoring stopped");
    }

    Logger::info(QString("MonitoringService: Stopped monitoring device %1").arg(deviceId));

    emit monitoringStopped(deviceId);
}

void MonitoringService::stopAllMonitoring()
{
    QList<QString> devices = getMonitoredDevices();

    for (const QString& deviceId : devices) {
        stopMonitoring(deviceId);
    }

    Logger::info(QString("MonitoringService: Stopped all monitoring (%1 devices)")
                 .arg(devices.size()));
}

bool MonitoringService::isMonitoring(const QString& deviceId) const
{
    return m_monitoringConfigs.contains(deviceId);
}

QList<QString> MonitoringService::getMonitoredDevices() const
{
    return m_monitoringConfigs.keys();
}

MonitoringConfig MonitoringService::getConfig(const QString& deviceId) const
{
    return m_monitoringConfigs.value(deviceId, MonitoringConfig());
}

void MonitoringService::updateConfig(const MonitoringConfig& config)
{
    if (config.deviceId.isEmpty()) {
        Logger::warn("MonitoringService: Cannot update config with empty device ID");
        return;
    }

    bool wasMonitoring = isMonitoring(config.deviceId);

    if (wasMonitoring) {
        // Restart monitoring with new config
        startMonitoring(config);
        Logger::info(QString("MonitoringService: Updated config for device %1").arg(config.deviceId));
    } else {
        // Just store the config
        m_monitoringConfigs[config.deviceId] = config;
        Logger::debug(QString("MonitoringService: Stored config for device %1 (not monitoring)")
                     .arg(config.deviceId));
    }
}

int MonitoringService::getMonitoredDeviceCount() const
{
    return m_monitoringConfigs.size();
}

void MonitoringService::setAlertsEnabled(const QString& deviceId, bool enable)
{
    if (!isMonitoring(deviceId)) {
        Logger::warn(QString("MonitoringService: Device %1 is not being monitored").arg(deviceId));
        return;
    }

    m_monitoringConfigs[deviceId].enableAlerts = enable;

    Logger::info(QString("MonitoringService: Alerts %1 for device %2")
                 .arg(enable ? "enabled" : "disabled")
                 .arg(deviceId));
}

void MonitoringService::setHistoryEnabled(const QString& deviceId, bool enable)
{
    if (!isMonitoring(deviceId)) {
        Logger::warn(QString("MonitoringService: Device %1 is not being monitored").arg(deviceId));
        return;
    }

    m_monitoringConfigs[deviceId].enableHistory = enable;

    Logger::info(QString("MonitoringService: History %1 for device %2")
                 .arg(enable ? "enabled" : "disabled")
                 .arg(deviceId));
}

void MonitoringService::onMetricsCollected(const QString& deviceId, const NetworkMetrics& metrics)
{
    if (!isMonitoring(deviceId)) {
        // Metrics collected but device is no longer monitored
        return;
    }

    MonitoringConfig config = m_monitoringConfigs.value(deviceId);

    // Check device status change (online/offline)
    bool isOnline = (metrics.getPacketLoss() < 100.0);
    if (checkStatusChange(deviceId, isOnline)) {
        if (config.enableAlerts && m_alertService) {
            generateStatusAlert(deviceId, isOnline);
        }
        emit deviceStatusChanged(deviceId, isOnline);
    }

    // Check thresholds for alerts
    if (config.enableAlerts && m_alertService && isOnline) {
        checkThresholds(deviceId, metrics);
    }

    // Store metrics in history
    if (config.enableHistory && m_historyService) {
        storeMetrics(deviceId, metrics);
    }

    // Forward metrics to listeners
    emit metricsCollected(deviceId, metrics);
}

void MonitoringService::onMetricsError(const QString& deviceId, const QString& error)
{
    Logger::error(QString("MonitoringService: Metrics error for device %1: %2")
                  .arg(deviceId).arg(error));

    if (!isMonitoring(deviceId)) {
        return;
    }

    MonitoringConfig config = m_monitoringConfigs.value(deviceId);

    // Store error event
    if (config.enableHistory && m_historyService) {
        storeEvent(deviceId, "metrics_error", error);
    }
}

void MonitoringService::checkThresholds(const QString& deviceId, const NetworkMetrics& metrics)
{
    if (!m_alertService) {
        return;
    }

    MonitoringConfig config = m_monitoringConfigs.value(deviceId);

    // Check high latency
    if (metrics.getLatencyAvg() > config.alertLatencyThreshold) {
        Alert alert = m_alertService->createAlert(
            AlertType::HighLatency,
            AlertSeverity::Warning,
            deviceId,
            QString("High latency detected: %1 ms (threshold: %2 ms)")
                .arg(metrics.getLatencyAvg(), 0, 'f', 2)
                .arg(config.alertLatencyThreshold, 0, 'f', 2)
        );

        Logger::warn(QString("Alert: %1").arg(alert.message()));
        emit alertTriggered(deviceId, alert);
    }

    // Check packet loss
    if (metrics.getPacketLoss() > config.alertPacketLossThreshold) {
        Alert alert = m_alertService->createAlert(
            AlertType::PacketLoss,
            AlertSeverity::Critical,
            deviceId,
            QString("Packet loss detected: %1% (threshold: %2%)")
                .arg(metrics.getPacketLoss(), 0, 'f', 2)
                .arg(config.alertPacketLossThreshold, 0, 'f', 2)
        );

        Logger::warn(QString("Alert: %1").arg(alert.message()));
        emit alertTriggered(deviceId, alert);
    }

    // Check high jitter
    if (metrics.getJitter() > config.alertJitterThreshold) {
        Alert alert = m_alertService->createAlert(
            AlertType::HighJitter,
            AlertSeverity::Warning,
            deviceId,
            QString("High jitter detected: %1 ms (threshold: %2 ms)")
                .arg(metrics.getJitter(), 0, 'f', 2)
                .arg(config.alertJitterThreshold, 0, 'f', 2)
        );

        Logger::warn(QString("Alert: %1").arg(alert.message()));
        emit alertTriggered(deviceId, alert);
    }
}

bool MonitoringService::checkStatusChange(const QString& deviceId, bool currentStatus)
{
    bool lastStatus = m_lastDeviceStatus.value(deviceId, false);

    if (lastStatus != currentStatus) {
        m_lastDeviceStatus[deviceId] = currentStatus;
        return true;
    }

    return false;
}

void MonitoringService::generateStatusAlert(const QString& deviceId, bool online)
{
    if (!m_alertService) {
        return;
    }

    AlertType type = online ? AlertType::DeviceOnline : AlertType::DeviceOffline;
    AlertSeverity severity = online ? AlertSeverity::Info : AlertSeverity::Critical;
    QString message = online ? "Device came online" : "Device went offline";

    Alert alert = m_alertService->createAlert(type, severity, deviceId, message);

    Logger::info(QString("Status Alert: Device %1 - %2").arg(deviceId).arg(message));
    emit alertTriggered(deviceId, alert);

    // Store status change event
    if (m_historyService) {
        storeEvent(deviceId, online ? "device_online" : "device_offline", message);
    }
}

void MonitoringService::storeMetrics(const QString& deviceId, const NetworkMetrics& metrics)
{
    if (!m_historyService) {
        return;
    }

    if (!m_historyService->saveMetrics(deviceId, metrics)) {
        Logger::error(QString("MonitoringService: Failed to save metrics for device %1")
                     .arg(deviceId));
    }
}

void MonitoringService::storeEvent(const QString& deviceId, const QString& eventType,
                                   const QString& description)
{
    if (!m_historyService) {
        return;
    }

    if (!m_historyService->saveEvent(deviceId, eventType, description)) {
        Logger::error(QString("MonitoringService: Failed to save event for device %1")
                     .arg(deviceId));
    }
}
