#ifndef JITTERCHART_H
#define JITTERCHART_H

#include <QChartView>
#include <QChart>
#include <QSplineSeries>
#include <QDateTimeAxis>
#include <QValueAxis>
#include <QDateTime>
#include <QList>
#include <QPointF>
#include "models/NetworkMetrics.h"

/**
 * @class JitterChart
 * @brief Real-time jitter chart with spline series
 *
 * Displays network jitter over time using a smooth spline curve (purple).
 * Jitter represents the variance in latency and indicates network stability.
 */
class JitterChart : public QChartView {
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param parent Optional parent widget
     */
    explicit JitterChart(QWidget* parent = nullptr);

    /**
     * @brief Destructor
     */
    ~JitterChart() override = default;

    /**
     * @brief Add a new jitter data point
     * @param jitter Jitter value in milliseconds
     * @param timestamp Time of the measurement
     */
    void addDataPoint(double jitter, const QDateTime& timestamp);

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
    QSplineSeries* jitterSeries;        ///< Jitter spline series
    QDateTimeAxis* axisX;               ///< Time axis (X)
    QValueAxis* axisY;                  ///< Jitter axis (Y)

    QList<QPointF> dataPoints;          ///< Jitter data points
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
     * @brief Update chart with current data
     */
    void updateChart();
};

#endif // JITTERCHART_H
