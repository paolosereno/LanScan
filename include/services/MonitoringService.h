#ifndef MONITORINGSERVICE_H
#define MONITORINGSERVICE_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QTimer>
#include "models/Alert.h"

class MetricsController;
class AlertService;
class HistoryService;
class NetworkMetrics;

/**
 * @brief Configuration for device monitoring
 */
struct MonitoringConfig {
    QString deviceId;                          // Device identifier
    int intervalMs = 1000;                     // Monitoring interval (default: 1 second)
    bool enableAlerts = true;                  // Enable alert generation
    double alertLatencyThreshold = 100.0;      // Latency threshold in ms
    double alertPacketLossThreshold = 5.0;     // Packet loss threshold in %
    double alertJitterThreshold = 30.0;        // Jitter threshold in ms
    bool enableHistory = true;                 // Enable history storage

    MonitoringConfig() = default;

    MonitoringConfig(const QString& devId, int interval = 1000)
        : deviceId(devId)
        , intervalMs(interval)
    {}
};

Q_DECLARE_METATYPE(MonitoringConfig)

/**
 * @brief Service for continuous network monitoring
 *
 * The MonitoringService orchestrates continuous monitoring of network devices.
 * It coordinates metrics collection, alert generation, and history storage.
 */
class MonitoringService : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param metricsController Metrics controller instance
     * @param alertService Alert service instance
     * @param historyService History service instance
     * @param parent Parent QObject
     */
    explicit MonitoringService(
        MetricsController* metricsController,
        AlertService* alertService,
        HistoryService* historyService,
        QObject* parent = nullptr
    );

    /**
     * @brief Destructor
     */
    ~MonitoringService();

    /**
     * @brief Start monitoring a device with configuration
     * @param config Monitoring configuration
     */
    void startMonitoring(const MonitoringConfig& config);

    /**
     * @brief Start monitoring a device with default configuration
     * @param deviceId Device identifier
     * @param intervalMs Monitoring interval in milliseconds
     */
    void startMonitoring(const QString& deviceId, int intervalMs = 1000);

    /**
     * @brief Stop monitoring a specific device
     * @param deviceId Device identifier
     */
    void stopMonitoring(const QString& deviceId);

    /**
     * @brief Stop all active monitoring sessions
     */
    void stopAllMonitoring();

    /**
     * @brief Check if a device is being monitored
     * @param deviceId Device identifier
     * @return True if device is being monitored
     */
    bool isMonitoring(const QString& deviceId) const;

    /**
     * @brief Get list of all monitored device IDs
     * @return List of device identifiers
     */
    QList<QString> getMonitoredDevices() const;

    /**
     * @brief Get monitoring configuration for a device
     * @param deviceId Device identifier
     * @return Monitoring configuration (default config if not found)
     */
    MonitoringConfig getConfig(const QString& deviceId) const;

    /**
     * @brief Update monitoring configuration for a device
     * @param config New configuration
     */
    void updateConfig(const MonitoringConfig& config);

    /**
     * @brief Get count of monitored devices
     * @return Number of devices being monitored
     */
    int getMonitoredDeviceCount() const;

    /**
     * @brief Enable/disable alerts for a device
     * @param deviceId Device identifier
     * @param enable True to enable alerts
     */
    void setAlertsEnabled(const QString& deviceId, bool enable);

    /**
     * @brief Enable/disable history for a device
     * @param deviceId Device identifier
     * @param enable True to enable history
     */
    void setHistoryEnabled(const QString& deviceId, bool enable);

signals:
    /**
     * @brief Emitted when monitoring starts for a device
     * @param deviceId Device identifier
     */
    void monitoringStarted(const QString& deviceId);

    /**
     * @brief Emitted when monitoring stops for a device
     * @param deviceId Device identifier
     */
    void monitoringStopped(const QString& deviceId);

    /**
     * @brief Emitted when metrics are collected
     * @param deviceId Device identifier
     * @param metrics Collected metrics
     */
    void metricsCollected(const QString& deviceId, const NetworkMetrics& metrics);

    /**
     * @brief Emitted when an alert is triggered
     * @param deviceId Device identifier
     * @param alert Generated alert
     */
    void alertTriggered(const QString& deviceId, const Alert& alert);

    /**
     * @brief Emitted when device status changes
     * @param deviceId Device identifier
     * @param online True if device is online
     */
    void deviceStatusChanged(const QString& deviceId, bool online);

private slots:
    /**
     * @brief Handle metrics collected from MetricsController
     * @param deviceId Device identifier
     * @param metrics Collected metrics
     */
    void onMetricsCollected(const QString& deviceId, const NetworkMetrics& metrics);

    /**
     * @brief Handle metrics collection error
     * @param deviceId Device identifier
     * @param error Error message
     */
    void onMetricsError(const QString& deviceId, const QString& error);

private:
    MetricsController* m_metricsController;
    AlertService* m_alertService;
    HistoryService* m_historyService;

    QMap<QString, MonitoringConfig> m_monitoringConfigs;
    QMap<QString, bool> m_lastDeviceStatus;  // Track online/offline status

    /**
     * @brief Check thresholds and generate alerts if needed
     * @param deviceId Device identifier
     * @param metrics Current metrics
     */
    void checkThresholds(const QString& deviceId, const NetworkMetrics& metrics);

    /**
     * @brief Check if device status changed (online/offline)
     * @param deviceId Device identifier
     * @param currentStatus Current online status
     * @return True if status changed
     */
    bool checkStatusChange(const QString& deviceId, bool currentStatus);

    /**
     * @brief Generate device online/offline alert
     * @param deviceId Device identifier
     * @param online True if device is online
     */
    void generateStatusAlert(const QString& deviceId, bool online);

    /**
     * @brief Store metrics in history
     * @param deviceId Device identifier
     * @param metrics Metrics to store
     */
    void storeMetrics(const QString& deviceId, const NetworkMetrics& metrics);

    /**
     * @brief Store event in history
     * @param deviceId Device identifier
     * @param eventType Event type
     * @param description Event description
     */
    void storeEvent(const QString& deviceId, const QString& eventType,
                   const QString& description);
};

#endif // MONITORINGSERVICE_H
