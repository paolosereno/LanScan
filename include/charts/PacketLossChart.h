#ifndef PACKETLOSSCHART_H
#define PACKETLOSSCHART_H

#include <QChartView>
#include <QChart>
#include <QBarSeries>
#include <QBarSet>
#include <QBarCategoryAxis>
#include <QValueAxis>
#include <QDateTime>
#include <QList>
#include <QPair>
#include "models/NetworkMetrics.h"

/**
 * @class PacketLossChart
 * @brief Real-time packet loss chart with bar series
 *
 * Displays packet loss percentage over time using colored bars:
 * - Green: < 1% (Excellent)
 * - Orange: 1-5% (Fair)
 * - Red: > 5% (Poor)
 */
class PacketLossChart : public QChartView {
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param parent Optional parent widget
     */
    explicit PacketLossChart(QWidget* parent = nullptr);

    /**
     * @brief Destructor
     */
    ~PacketLossChart() override = default;

    /**
     * @brief Add a new packet loss data point
     * @param packetLoss Packet loss percentage
     * @param timestamp Time of the measurement
     */
    void addDataPoint(double packetLoss, const QDateTime& timestamp);

    /**
     * @brief Clear all chart data
     */
    void clearData();

    /**
     * @brief Set maximum number of data points to display
     * @param max Maximum data points (default: 20)
     */
    void setMaxDataPoints(int max);

    /**
     * @brief Get current maximum data points
     * @return Maximum data points
     */
    int getMaxDataPoints() const;

public slots:
    /**
     * @brief Slot called when metrics are updated
     * @param metrics Updated network metrics
     */
    void onMetricsUpdated(const NetworkMetrics& metrics);

signals:
    /**
     * @brief Emitted when chart data has been updated
     */
    void chartUpdated();

private:
    QChart* chart;                          ///< Chart container
    QBarSeries* barSeries;                  ///< Bar series
    QBarSet* packetLossSet;                 ///< Packet loss bar set
    QBarCategoryAxis* axisX;                ///< Time axis (X)
    QValueAxis* axisY;                      ///< Packet loss axis (Y)

    QList<QPair<QString, double>> dataPoints;  ///< Data points (time label, packet loss)
    int maxDataPointsLimit;                 ///< Maximum data points to retain

    /**
     * @brief Setup chart appearance and properties
     */
    void setupChart();

    /**
     * @brief Setup chart axes
     */
    void setupAxes();

    /**
     * @brief Update chart with current data
     */
    void updateChart();

    /**
     * @brief Get color based on packet loss severity
     * @param packetLoss Packet loss percentage
     * @return Color for the bar
     */
    QColor getColorForPacketLoss(double packetLoss) const;
};

#endif // PACKETLOSSCHART_H
