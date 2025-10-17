#include "charts/PacketLossChart.h"
#include "utils/Logger.h"
#include <QColor>

PacketLossChart::PacketLossChart(QWidget* parent)
    : QChartView(parent)
    , chart(nullptr)
    , barSeries(nullptr)
    , packetLossSet(nullptr)
    , axisX(nullptr)
    , axisY(nullptr)
    , maxDataPointsLimit(20)
{
    setupChart();
    setupAxes();

    Logger::debug("PacketLossChart: Initialized with bar series");
}

void PacketLossChart::setupChart() {
    chart = new QChart();
    chart->setTitle("Packet Loss (%)");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    // Bar set
    packetLossSet = new QBarSet("Packet Loss");
    packetLossSet->setColor(QColor("#FF9800")); // Default: Orange

    // Bar series
    barSeries = new QBarSeries();
    barSeries->append(packetLossSet);

    chart->addSeries(barSeries);

    setChart(chart);
    setRenderHint(QPainter::Antialiasing);
}

void PacketLossChart::setupAxes() {
    // X axis (time categories)
    axisX = new QBarCategoryAxis();
    chart->addAxis(axisX, Qt::AlignBottom);
    barSeries->attachAxis(axisX);

    // Y axis (packet loss percentage)
    axisY = new QValueAxis();
    axisY->setTitleText("Packet Loss (%)");
    axisY->setRange(0, 10);
    axisY->setLabelFormat("%.1f");
    chart->addAxis(axisY, Qt::AlignLeft);
    barSeries->attachAxis(axisY);
}

void PacketLossChart::addDataPoint(double packetLoss, const QDateTime& timestamp) {
    QString timeLabel = timestamp.toString("hh:mm:ss");
    dataPoints.append(qMakePair(timeLabel, packetLoss));

    // Prune old data
    while (dataPoints.size() > maxDataPointsLimit) {
        dataPoints.removeFirst();
    }

    updateChart();
    emit chartUpdated();
}

void PacketLossChart::onMetricsUpdated(const NetworkMetrics& metrics) {
    addDataPoint(metrics.packetLoss(), QDateTime::currentDateTime());
}

void PacketLossChart::clearData() {
    dataPoints.clear();
    packetLossSet->remove(0, packetLossSet->count());
    axisX->clear();

    Logger::debug("PacketLossChart: Data cleared");
    emit chartUpdated();
}

void PacketLossChart::setMaxDataPoints(int max) {
    if (max < 1) {
        Logger::warn(QString("PacketLossChart: Invalid maxDataPoints: %1, using default 20").arg(max));
        maxDataPointsLimit = 20;
        return;
    }

    if (max > 100) {
        Logger::warn(QString("PacketLossChart: MaxDataPoints too large: %1, capping at 100").arg(max));
        maxDataPointsLimit = 100;
    } else {
        maxDataPointsLimit = max;
    }

    Logger::debug(QString("PacketLossChart: MaxDataPoints set to %1").arg(maxDataPointsLimit));
}

int PacketLossChart::getMaxDataPoints() const {
    return maxDataPointsLimit;
}

void PacketLossChart::updateChart() {
    // Clear existing data
    packetLossSet->remove(0, packetLossSet->count());

    // Categories for X axis
    QStringList categories;

    // Track max packet loss for Y axis scaling
    double maxPacketLoss = 0;
    bool allZero = true;

    // Add new data
    for (const auto& pair : dataPoints) {
        categories << pair.first;
        *packetLossSet << pair.second;

        if (pair.second > maxPacketLoss) {
            maxPacketLoss = pair.second;
        }
        if (pair.second > 0.01) {
            allZero = false;
        }
    }

    // Update X axis
    axisX->clear();
    axisX->append(categories);

    // Update Y axis range (auto-scale with margin)
    if (maxPacketLoss > 0) {
        axisY->setRange(0, maxPacketLoss * 1.2);
        chart->setTitle("Packet Loss (%)");
    } else {
        // When all values are zero, set a small range so the 0% bars are visible
        axisY->setRange(0, 5); // Show 0-5% range to make 0% bars visible
        chart->setTitle("Packet Loss (%) - No packet loss detected");
    }

    // Color bars based on packet loss severity
    // Note: QBarSet doesn't support per-bar colors easily,
    // so we use the overall color based on the last value
    if (!dataPoints.isEmpty()) {
        double lastValue = dataPoints.last().second;
        packetLossSet->setColor(getColorForPacketLoss(lastValue));
    }
}

QColor PacketLossChart::getColorForPacketLoss(double packetLoss) const {
    if (packetLoss < 1.0) {
        return QColor("#4CAF50"); // Green: Excellent
    } else if (packetLoss < 5.0) {
        return QColor("#FF9800"); // Orange: Fair
    } else {
        return QColor("#F44336"); // Red: Poor
    }
}
