#include "charts/JitterChart.h"
#include "utils/Logger.h"
#include <QPen>
#include <QColor>

JitterChart::JitterChart(QWidget* parent)
    : QChartView(parent)
    , chart(nullptr)
    , jitterSeries(nullptr)
    , axisX(nullptr)
    , axisY(nullptr)
    , maxDataPointsLimit(60)
{
    setupChart();
    setupAxes();

    Logger::debug("JitterChart: Initialized with spline series");
}

void JitterChart::setupChart() {
    chart = new QChart();
    chart->setTitle("Network Jitter (ms)");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    // Spline series for smooth curves
    jitterSeries = new QSplineSeries();
    jitterSeries->setName("Jitter");
    jitterSeries->setPen(QPen(QColor("#9C27B0"), 2)); // Purple

    chart->addSeries(jitterSeries);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    setChart(chart);
    setRenderHint(QPainter::Antialiasing);
}

void JitterChart::setupAxes() {
    // X axis (time)
    axisX = new QDateTimeAxis();
    axisX->setFormat("hh:mm:ss");
    axisX->setTitleText("Time");
    axisX->setLabelsAngle(-45);
    chart->addAxis(axisX, Qt::AlignBottom);
    jitterSeries->attachAxis(axisX);

    // Y axis (jitter in ms)
    axisY = new QValueAxis();
    axisY->setTitleText("Jitter (ms)");
    axisY->setRange(0, 10);
    axisY->setLabelFormat("%.2f");
    chart->addAxis(axisY, Qt::AlignLeft);
    jitterSeries->attachAxis(axisY);
}

void JitterChart::addDataPoint(double jitter, const QDateTime& timestamp) {
    qint64 msecs = timestamp.toMSecsSinceEpoch();
    dataPoints.append(QPointF(msecs, jitter));

    // Prune old data
    while (dataPoints.size() > maxDataPointsLimit) {
        dataPoints.removeFirst();
    }

    updateChart();
    emit chartUpdated();
}

void JitterChart::onMetricsUpdated(const NetworkMetrics& metrics) {
    addDataPoint(metrics.jitter(), QDateTime::currentDateTime());
}

void JitterChart::clearData() {
    dataPoints.clear();
    jitterSeries->clear();

    Logger::debug("JitterChart: Data cleared");
    emit chartUpdated();
}

void JitterChart::setMaxDataPoints(int max) {
    if (max < 1) {
        Logger::warn(QString("JitterChart: Invalid maxDataPoints: %1, using default 60").arg(max));
        maxDataPointsLimit = 60;
        return;
    }

    if (max > 10000) {
        Logger::warn(QString("JitterChart: MaxDataPoints too large: %1, capping at 10000").arg(max));
        maxDataPointsLimit = 10000;
    } else {
        maxDataPointsLimit = max;
    }

    Logger::debug(QString("JitterChart: MaxDataPoints set to %1").arg(maxDataPointsLimit));
}

int JitterChart::getMaxDataPoints() const {
    return maxDataPointsLimit;
}

void JitterChart::updateChart() {
    // Update spline series
    jitterSeries->clear();
    for (const QPointF& point : dataPoints) {
        jitterSeries->append(point);
    }

    if (dataPoints.isEmpty()) {
        return;
    }

    // Update X axis (time)
    qint64 minTime = dataPoints.first().x();
    qint64 maxTime = dataPoints.last().x();
    axisX->setRange(QDateTime::fromMSecsSinceEpoch(minTime),
                    QDateTime::fromMSecsSinceEpoch(maxTime));

    // Update Y axis (auto-scale with margin)
    double maxJitter = 0;
    for (const QPointF& point : dataPoints) {
        if (point.y() > maxJitter) {
            maxJitter = point.y();
        }
    }

    if (maxJitter > 0) {
        axisY->setRange(0, maxJitter * 1.2);
    } else {
        axisY->setRange(0, 10); // Default range
    }
}
