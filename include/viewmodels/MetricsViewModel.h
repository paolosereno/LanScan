#ifndef METRICSVIEWMODEL_H
#define METRICSVIEWMODEL_H

#include <QObject>
#include <QList>
#include <QTimer>
#include "models/Device.h"
#include "models/NetworkMetrics.h"

// Forward declarations
class MetricsController;
class IDeviceRepository;

/**
 * @class MetricsViewModel
 * @brief ViewModel for managing metrics monitoring and data
 *
 * This ViewModel manages real-time metrics monitoring for a selected device.
 * It coordinates between the MetricsController and the view (MetricsWidget),
 * providing metrics history and monitoring control.
 */
class MetricsViewModel : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Construct a MetricsViewModel
     * @param metricsController Pointer to MetricsController for metrics collection
     * @param deviceRepository Pointer to device repository for device data
     * @param parent Parent QObject
     */
    explicit MetricsViewModel(
        MetricsController* metricsController,
        IDeviceRepository* deviceRepository,
        QObject* parent = nullptr
    );

    /**
     * @brief Destructor
     */
    ~MetricsViewModel();

    /**
     * @brief Set the current device to monitor
     * @param device Device to monitor
     */
    void setDevice(const Device& device);

    /**
     * @brief Get the current device being monitored
     * @return Current device
     */
    Device getCurrentDevice() const;

    /**
     * @brief Start monitoring metrics
     * @param intervalMs Monitoring interval in milliseconds (default: 1000ms)
     */
    void startMonitoring(int intervalMs = 1000);

    /**
     * @brief Stop monitoring metrics
     */
    void stopMonitoring();

    /**
     * @brief Check if monitoring is active
     * @return True if monitoring, false otherwise
     */
    bool isMonitoring() const;

    /**
     * @brief Get the latest collected metrics
     * @return Latest NetworkMetrics
     */
    NetworkMetrics getLatestMetrics() const;

    /**
     * @brief Get the full metrics history
     * @return List of NetworkMetrics
     */
    QList<NetworkMetrics> getMetricsHistory() const;

    /**
     * @brief Clear the metrics history
     */
    void clearHistory();

    /**
     * @brief Set maximum history size
     * @param size Maximum number of metrics to keep in history
     */
    void setMaxHistorySize(int size);

    /**
     * @brief Get maximum history size
     * @return Maximum history size
     */
    int getMaxHistorySize() const;

signals:
    /**
     * @brief Emitted when new metrics are collected
     * @param metrics The collected NetworkMetrics
     */
    void metricsUpdated(const NetworkMetrics& metrics);

    /**
     * @brief Emitted when monitoring starts
     */
    void monitoringStarted();

    /**
     * @brief Emitted when monitoring stops
     */
    void monitoringStopped();

    /**
     * @brief Emitted when the device changes
     * @param device The new device
     */
    void deviceChanged(const Device& device);

    /**
     * @brief Emitted when history is cleared
     */
    void historyCleared();

private slots:
    /**
     * @brief Handle metrics collected from MetricsController
     * @param deviceId Device ID
     * @param metrics Collected metrics
     */
    void onMetricsCollected(const QString& deviceId, const NetworkMetrics& metrics);

    /**
     * @brief Timer callback for periodic metrics collection
     */
    void onMonitoringTimerTimeout();

private:
    /**
     * @brief Prune metrics history when it exceeds max size
     */
    void pruneHistory();

    // Dependencies
    MetricsController* metricsController;
    IDeviceRepository* deviceRepository;

    // State
    Device currentDevice;
    NetworkMetrics latestMetrics;
    QList<NetworkMetrics> metricsHistory;
    bool monitoring;
    int maxHistorySize;
    int monitoringInterval;

    // Timer for periodic monitoring
    QTimer* monitoringTimer;
};

#endif // METRICSVIEWMODEL_H
