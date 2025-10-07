#ifndef LATENCYCHART_H
#define LATENCYCHART_H

#include <QChartView>
#include <QChart>
#include <QLineSeries>
#include <QDateTimeAxis>
#include <QValueAxis>
#include <QDateTime>
#include <QList>
#include <QPointF>
#include "models/NetworkMetrics.h"

/**
 * @class LatencyChart
 * @brief Real-time latency chart with min/avg/max series
 *
 * Displays network latency over time using three line series:
 * - Min latency (green)
 * - Avg latency (blue, thicker)
 * - Max latency (red)
 */
class LatencyChart : public QChartView {
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param parent Optional parent widget
     */
    explicit LatencyChart(QWidget* parent = nullptr);

    /**
     * @brief Destructor
     */
    ~LatencyChart() override = default;

    /**
     * @brief Add a new latency data point
     * @param metrics Network metrics containing latency data
     * @param timestamp Time of the measurement
     */
    void addDataPoint(const NetworkMetrics& metrics, const QDateTime& timestamp);

    /**
     * @brief Clear all chart data
     */
    void clearData();

    /**
     * @brief Set maximum number of data points to display
     * @param max Maximum data points (default: 60)
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
    QChart* chart;                      ///< Chart container
    QLineSeries* minSeries;             ///< Min latency series
    QLineSeries* avgSeries;             ///< Avg latency series
    QLineSeries* maxSeries;             ///< Max latency series
    QDateTimeAxis* axisX;               ///< Time axis (X)
    QValueAxis* axisY;                  ///< Latency axis (Y)

    QList<QPointF> minDataPoints;       ///< Min latency data points
    QList<QPointF> avgDataPoints;       ///< Avg latency data points
    QList<QPointF> maxDataPoints;       ///< Max latency data points
    int maxDataPointsLimit;             ///< Maximum data points to retain

    /**
     * @brief Setup chart appearance and properties
     */
    void setupChart();

    /**
     * @brief Setup chart axes
     */
    void setupAxes();

    /**
     * @brief Setup line series
     */
    void setupSeries();

    /**
     * @brief Update axes ranges based on current data
     */
    void updateAxes();

    /**
     * @brief Remove old data points exceeding max limit
     */
    void pruneOldData();

    /**
     * @brief Update series with current data points
     */
    void updateSeries();
};

#endif // LATENCYCHART_H
