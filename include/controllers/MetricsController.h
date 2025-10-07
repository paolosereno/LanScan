#ifndef METRICSCONTROLLER_H
#define METRICSCONTROLLER_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QTimer>

class MetricsAggregator;
class DeviceRepository;
class NetworkMetrics;

/**
 * @brief Controls metrics collection and continuous monitoring
 */
class MetricsController : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param aggregator Metrics aggregator instance
     * @param repository Device repository instance
     * @param parent Parent QObject
     */
    explicit MetricsController(
        MetricsAggregator* aggregator,
        DeviceRepository* repository,
        QObject* parent = nullptr
    );

    /**
     * @brief Destructor
     */
    ~MetricsController();

    /**
     * @brief Start continuous monitoring for a specific device
     * @param deviceId Device identifier (typically IP address)
     * @param intervalMs Monitoring interval in milliseconds (default: 1000ms)
     */
    void startContinuousMonitoring(const QString& deviceId, int intervalMs = 1000);

    /**
     * @brief Stop continuous monitoring for a specific device
     * @param deviceId Device identifier
     */
    void stopContinuousMonitoring(const QString& deviceId);

    /**
     * @brief Collect metrics once for a specific device
     * @param deviceId Device identifier
     */
    void collectMetricsOnce(const QString& deviceId);

    /**
     * @brief Start monitoring all devices in the repository
     * @param intervalMs Monitoring interval in milliseconds (default: 1000ms)
     */
    void startMonitoringAll(int intervalMs = 1000);

    /**
     * @brief Stop monitoring all devices
     */
    void stopMonitoringAll();

    /**
     * @brief Check if a device is being monitored
     * @param deviceId Device identifier
     * @return True if device is being monitored
     */
    bool isMonitoring(const QString& deviceId) const;

    /**
     * @brief Get the number of devices currently being monitored
     * @return Number of monitored devices
     */
    int getMonitoredDeviceCount() const;

signals:
    /**
     * @brief Emitted when metrics are collected for a device
     * @param deviceId Device identifier
     * @param metrics Collected metrics
     */
    void metricsCollected(const QString& deviceId, const NetworkMetrics& metrics);

    /**
     * @brief Emitted when metrics collection fails
     * @param deviceId Device identifier
     * @param error Error message
     */
    void metricsError(const QString& deviceId, const QString& error);

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

private slots:
    void onMetricsUpdated(const NetworkMetrics& metrics);
    void collectMetricsForDevice(const QString& deviceId);

private:
    MetricsAggregator* aggregator;
    DeviceRepository* repository;

    QMap<QString, QTimer*> monitoringTimers;
    QString currentMonitoringDevice;  // Track currently monitored device
    int defaultInterval;

    void setupTimer(const QString& deviceId, int intervalMs);
    void cleanupTimer(const QString& deviceId);
    void saveMetrics(const QString& deviceId, const NetworkMetrics& metrics);
};

#endif // METRICSCONTROLLER_H
