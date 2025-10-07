#include "charts/LatencyChart.h"
#include "utils/Logger.h"
#include <QPen>
#include <QColor>

LatencyChart::LatencyChart(QWidget* parent)
    : QChartView(parent)
    , chart(nullptr)
    , minSeries(nullptr)
    , avgSeries(nullptr)
    , maxSeries(nullptr)
    , axisX(nullptr)
    , axisY(nullptr)
    , maxDataPointsLimit(60)
{
    setupChart();
    setupSeries();
    setupAxes();

    Logger::debug("LatencyChart: Initialized with 3 line series (min/avg/max)");
}

void LatencyChart::setupChart() {
    chart = new QChart();
    chart->setTitle("Network Latency (ms)");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    setChart(chart);
    setRenderHint(QPainter::Antialiasing);
}

void LatencyChart::setupSeries() {
    // Min latency series (green)
    minSeries = new QLineSeries();
    minSeries->setName("Min");
    minSeries->setPen(QPen(QColor("#4CAF50"), 2));

    // Avg latency series (blue, thicker)
    avgSeries = new QLineSeries();
    avgSeries->setName("Avg");
    avgSeries->setPen(QPen(QColor("#2196F3"), 3));

    // Max latency series (red)
    maxSeries = new QLineSeries();
    maxSeries->setName("Max");
    maxSeries->setPen(QPen(QColor("#F44336"), 2));

    chart->addSeries(minSeries);
    chart->addSeries(avgSeries);
    chart->addSeries(maxSeries);
}

void LatencyChart::setupAxes() {
    // X axis (time)
    axisX = new QDateTimeAxis();
    axisX->setFormat("hh:mm:ss");
    axisX->setTitleText("Time");
    axisX->setLabelsAngle(-45);
    chart->addAxis(axisX, Qt::AlignBottom);

    // Y axis (latency in ms)
    axisY = new QValueAxis();
    axisY->setTitleText("Latency (ms)");
    axisY->setRange(0, 100);
    axisY->setLabelFormat("%.1f");
    chart->addAxis(axisY, Qt::AlignLeft);

    // Attach series to axes
    minSeries->attachAxis(axisX);
    minSeries->attachAxis(axisY);
    avgSeries->attachAxis(axisX);
    avgSeries->attachAxis(axisY);
    maxSeries->attachAxis(axisX);
    maxSeries->attachAxis(axisY);
}

void LatencyChart::addDataPoint(const NetworkMetrics& metrics, const QDateTime& timestamp) {
    qint64 msecs = timestamp.toMSecsSinceEpoch();

    // Add data points
    minDataPoints.append(QPointF(msecs, metrics.latencyMin()));
    avgDataPoints.append(QPointF(msecs, metrics.latencyAvg()));
    maxDataPoints.append(QPointF(msecs, metrics.latencyMax()));

    // Prune old data
    pruneOldData();

    // Update series
    updateSeries();

    // Update axes
    updateAxes();

    emit chartUpdated();
}

void LatencyChart::onMetricsUpdated(const NetworkMetrics& metrics) {
    addDataPoint(metrics, QDateTime::currentDateTime());
}

void LatencyChart::clearData() {
    minDataPoints.clear();
    avgDataPoints.clear();
    maxDataPoints.clear();

    minSeries->clear();
    avgSeries->clear();
    maxSeries->clear();

    Logger::debug("LatencyChart: Data cleared");
    emit chartUpdated();
}

void LatencyChart::setMaxDataPoints(int max) {
    if (max < 1) {
        Logger::warn(QString("LatencyChart: Invalid maxDataPoints: %1, using default 60").arg(max));
        maxDataPointsLimit = 60;
        return;
    }

    if (max > 10000) {
        Logger::warn(QString("LatencyChart: MaxDataPoints too large: %1, capping at 10000").arg(max));
        maxDataPointsLimit = 10000;
    } else {
        maxDataPointsLimit = max;
    }

    Logger::debug(QString("LatencyChart: MaxDataPoints set to %1").arg(maxDataPointsLimit));
}

int LatencyChart::getMaxDataPoints() const {
    return maxDataPointsLimit;
}

void LatencyChart::pruneOldData() {
    while (minDataPoints.size() > maxDataPointsLimit) {
        minDataPoints.removeFirst();
    }
    while (avgDataPoints.size() > maxDataPointsLimit) {
        avgDataPoints.removeFirst();
    }
    while (maxDataPoints.size() > maxDataPointsLimit) {
        maxDataPoints.removeFirst();
    }
}

void LatencyChart::updateSeries() {
    // Update min series
    minSeries->clear();
    for (const QPointF& point : minDataPoints) {
        minSeries->append(point);
    }

    // Update avg series
    avgSeries->clear();
    for (const QPointF& point : avgDataPoints) {
        avgSeries->append(point);
    }

    // Update max series
    maxSeries->clear();
    for (const QPointF& point : maxDataPoints) {
        maxSeries->append(point);
    }
}

void LatencyChart::updateAxes() {
    if (avgDataPoints.isEmpty()) {
        return;
    }

    // Update X axis range (time)
    qint64 minTime = avgDataPoints.first().x();
    qint64 maxTime = avgDataPoints.last().x();
    axisX->setRange(QDateTime::fromMSecsSinceEpoch(minTime),
                    QDateTime::fromMSecsSinceEpoch(maxTime));

    // Update Y axis range (auto-scale with 20% margin)
    double maxLatency = 0;
    for (const QPointF& point : maxDataPoints) {
        if (point.y() > maxLatency) {
            maxLatency = point.y();
        }
    }

    // Set range with margin
    if (maxLatency > 0) {
        axisY->setRange(0, maxLatency * 1.2);
    } else {
        axisY->setRange(0, 100); // Default range
    }
}
