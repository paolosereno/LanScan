#ifndef METRICSWIDGET_H
#define METRICSWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include "models/Device.h"
#include "models/NetworkMetrics.h"

// Forward declarations
class MetricsViewModel;
class LatencyChart;
class PacketLossChart;
class JitterChart;

namespace Ui {
class MetricsWidget;
}

/**
 * @class MetricsWidget
 * @brief Widget for displaying real-time network metrics with charts
 *
 * This widget provides:
 * - Device selection
 * - Start/Stop monitoring controls
 * - Current metrics summary panel
 * - Three chart tabs: Latency, Packet Loss, Jitter
 */
class MetricsWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Construct a MetricsWidget
     * @param viewModel Pointer to MetricsViewModel
     * @param parent Parent widget
     */
    explicit MetricsWidget(MetricsViewModel* viewModel, QWidget* parent = nullptr);

    /**
     * @brief Destructor
     */
    ~MetricsWidget();

    /**
     * @brief Set the device to monitor
     * @param device Device to monitor
     */
    void setDevice(const Device& device);

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
     * @brief Populate device combo box with available devices
     * @param devices List of devices
     */
    void setDevices(const QList<Device>& devices);

    /**
     * @brief Clear all chart data
     */
    void clearCharts();

private slots:
    /**
     * @brief Handle device selection from combo box
     * @param index Selected index
     */
    void onDeviceSelected(int index);

    /**
     * @brief Handle metrics update
     * @param metrics Updated metrics
     */
    void onMetricsUpdated(const NetworkMetrics& metrics);

    /**
     * @brief Handle start button click
     */
    void onStartMonitoringClicked();

    /**
     * @brief Handle stop button click
     */
    void onStopMonitoringClicked();

    /**
     * @brief Handle monitoring started signal
     */
    void onMonitoringStarted();

    /**
     * @brief Handle monitoring stopped signal
     */
    void onMonitoringStopped();

private:
    /**
     * @brief Setup chart widgets
     */
    void setupCharts();

    /**
     * @brief Setup signal/slot connections
     */
    void setupConnections();

    /**
     * @brief Update summary panel labels with metrics
     * @param metrics Network metrics to display
     */
    void updateSummaryLabels(const NetworkMetrics& metrics);

    /**
     * @brief Get color for quality score
     * @param qualityScore Quality score string
     * @return QColor for the score
     */
    QColor getQualityColor(const QString& qualityScore) const;

    // UI
    Ui::MetricsWidget* ui;

    // ViewModel
    MetricsViewModel* viewModel;

    // Charts
    LatencyChart* latencyChart;
    PacketLossChart* packetLossChart;
    JitterChart* jitterChart;

    // Device list
    QList<Device> devices;

    // State
    int currentMonitoringInterval;
};

#endif // METRICSWIDGET_H
