#include "views/TrendsWidget.h"
#include "utils/Logger.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QFileDialog>

TrendsWidget::TrendsWidget(MetricsDao* metricsDao, QWidget* parent)
    : QWidget(parent)
    , metricsDao(metricsDao)
{
    setupUI();
    setupConnections();
}

TrendsWidget::~TrendsWidget() {
}

void TrendsWidget::setDevice(const QString& deviceId) {
    currentDeviceId = deviceId;
    deviceLabel->setText("<b>Device:</b> " + deviceId);

    // Set default time range to last 24 hours
    timeRangeCombo->setCurrentIndex(2); // Last 24 Hours
    onLoadTrends();
}

void TrendsWidget::setDateRange(const QDateTime& start, const QDateTime& end) {
    startDate = start;
    endDate = end;

    startDateEdit->setDateTime(start);
    endDateEdit->setDateTime(end);

    // Set to custom range
    timeRangeCombo->setCurrentIndex(6); // Custom
}

void TrendsWidget::refresh() {
    onLoadTrends();
}

void TrendsWidget::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(5, 5, 5, 5);

    // Title and device info
    QLabel* titleLabel = new QLabel("<h3>Historical Metrics Trends</h3>", this);
    deviceLabel = new QLabel("<b>Device:</b> Not selected", this);
    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(deviceLabel);

    // Controls group
    QGroupBox* controlsGroup = new QGroupBox("Time Range", this);
    QHBoxLayout* controlsLayout = new QHBoxLayout(controlsGroup);

    // Time range combo
    timeRangeCombo = new QComboBox(this);
    timeRangeCombo->addItem("Last Hour", 1);
    timeRangeCombo->addItem("Last 6 Hours", 6);
    timeRangeCombo->addItem("Last 24 Hours", 24);
    timeRangeCombo->addItem("Last 7 Days", 168);
    timeRangeCombo->addItem("Last 30 Days", 720);
    timeRangeCombo->addItem("Last 90 Days", 2160);
    timeRangeCombo->addItem("Custom Range", -1);
    timeRangeCombo->setCurrentIndex(2); // Default: Last 24 Hours
    controlsLayout->addWidget(new QLabel("Range:", this));
    controlsLayout->addWidget(timeRangeCombo);

    // Custom date range editors
    controlsLayout->addWidget(new QLabel("From:", this));
    startDateEdit = new QDateTimeEdit(this);
    startDateEdit->setCalendarPopup(true);
    startDateEdit->setDisplayFormat("yyyy-MM-dd HH:mm");
    startDateEdit->setEnabled(false);
    controlsLayout->addWidget(startDateEdit);

    controlsLayout->addWidget(new QLabel("To:", this));
    endDateEdit = new QDateTimeEdit(this);
    endDateEdit->setCalendarPopup(true);
    endDateEdit->setDisplayFormat("yyyy-MM-dd HH:mm");
    endDateEdit->setEnabled(false);
    controlsLayout->addWidget(endDateEdit);

    // Refresh button
    refreshButton = new QPushButton("Refresh", this);
    controlsLayout->addWidget(refreshButton);

    // Export button
    exportButton = new QPushButton("Export", this);
    controlsLayout->addWidget(exportButton);

    controlsLayout->addStretch();
    mainLayout->addWidget(controlsGroup);

    // Statistics label
    statsLabel = new QLabel(this);
    statsLabel->setStyleSheet("QLabel { padding: 5px; background-color: #f0f0f0; border-radius: 3px; }");
    mainLayout->addWidget(statsLabel);

    // Chart
    trendsChart = new LatencyChart(this);
    trendsChart->setMinimumHeight(300);
    mainLayout->addWidget(trendsChart);

    setLayout(mainLayout);

    // Set initial date range
    calculateAndDisplayDateRange(timeRangeCombo->currentIndex());
}

void TrendsWidget::setupConnections() {
    connect(timeRangeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &TrendsWidget::onTimeRangeChanged);
    connect(startDateEdit, &QDateTimeEdit::dateTimeChanged,
            this, &TrendsWidget::onStartDateChanged);
    connect(endDateEdit, &QDateTimeEdit::dateTimeChanged,
            this, &TrendsWidget::onEndDateChanged);
    connect(refreshButton, &QPushButton::clicked,
            this, &TrendsWidget::onRefreshClicked);
    connect(exportButton, &QPushButton::clicked,
            this, &TrendsWidget::onExportClicked);
}

void TrendsWidget::onLoadTrends() {
    if (currentDeviceId.isEmpty()) {
        statsLabel->setText("⚠ No device selected");
        return;
    }

    loadHistoricalData();
}

void TrendsWidget::onTimeRangeChanged(int index) {
    bool isCustom = (timeRangeCombo->itemData(index).toInt() == -1);
    startDateEdit->setEnabled(isCustom);
    endDateEdit->setEnabled(isCustom);

    if (!isCustom) {
        calculateAndDisplayDateRange(index);
        onLoadTrends();
    }
}

void TrendsWidget::onStartDateChanged(const QDateTime& dateTime) {
    startDate = dateTime;
    if (timeRangeCombo->currentData().toInt() == -1) { // Custom range
        onLoadTrends();
    }
}

void TrendsWidget::onEndDateChanged(const QDateTime& dateTime) {
    endDate = dateTime;
    if (timeRangeCombo->currentData().toInt() == -1) { // Custom range
        onLoadTrends();
    }
}

void TrendsWidget::onRefreshClicked() {
    onLoadTrends();
}

void TrendsWidget::onExportClicked() {
    if (currentDeviceId.isEmpty()) {
        QMessageBox::warning(this, tr("Export Trends"),
                           tr("No device selected"));
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this,
                                                   tr("Export Trends"),
                                                   "trends_" + currentDeviceId + ".csv",
                                                   tr("CSV Files (*.csv)"));

    if (fileName.isEmpty()) {
        return;
    }

    QList<NetworkMetrics> metrics = metricsDao->findByDateRange(currentDeviceId, startDate, endDate);

    if (metrics.isEmpty()) {
        QMessageBox::information(this, tr("Export Trends"),
                               tr("No data to export"));
        return;
    }

    // Export to CSV
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Export Error"),
                           tr("Failed to create export file"));
        return;
    }

    QTextStream out(&file);
    out << "Timestamp,Latency Min,Latency Avg,Latency Max,Jitter,Packet Loss,Quality Score\n";

    for (const NetworkMetrics& m : metrics) {
        out << m.timestamp().toString(Qt::ISODate) << ","
            << m.getLatencyMin() << ","
            << m.getLatencyAvg() << ","
            << m.getLatencyMax() << ","
            << m.getJitter() << ","
            << m.getPacketLoss() << ","
            << static_cast<int>(m.getQualityScore()) << "\n";
    }

    file.close();

    QMessageBox::information(this, tr("Export Successful"),
                           tr("Exported %1 records to %2")
                           .arg(metrics.size())
                           .arg(fileName));
}

void TrendsWidget::loadHistoricalData() {
    if (!startDate.isValid() || !endDate.isValid()) {
        Logger::warn("Invalid date range for trends");
        return;
    }

    QList<NetworkMetrics> metrics = metricsDao->findByDateRange(currentDeviceId, startDate, endDate);

    if (metrics.isEmpty()) {
        statsLabel->setText("⚠ No data available for the selected time range");
        trendsChart->clearData();
        emit trendsLoaded(0);
        Logger::info("No metrics data found for device " + currentDeviceId);
        return;
    }

    displayTrends(metrics);
    updateStatistics(metrics);
    emit trendsLoaded(metrics.size());

    Logger::info("Loaded " + QString::number(metrics.size()) +
                " metrics for device " + currentDeviceId);
}

void TrendsWidget::displayTrends(const QList<NetworkMetrics>& metrics) {
    trendsChart->clearData();

    for (const NetworkMetrics& m : metrics) {
        trendsChart->addDataPoint(m, m.timestamp());
    }
}

void TrendsWidget::updateStatistics(const QList<NetworkMetrics>& metrics) {
    if (metrics.isEmpty()) {
        return;
    }

    // Calculate aggregated statistics
    NetworkMetrics avgMetrics = metricsDao->getAverageMetrics(currentDeviceId, startDate, endDate);
    double maxLatency = metricsDao->getMaxLatency(currentDeviceId, startDate, endDate);
    double minLatency = metricsDao->getMinLatency(currentDeviceId, startDate, endDate);

    QString statsText = QString(
        "📊 <b>Statistics:</b> %1 data points | "
        "<b>Latency:</b> Min: %2ms, Avg: %3ms, Max: %4ms | "
        "<b>Packet Loss:</b> %5% | "
        "<b>Jitter:</b> %6ms | "
        "<b>Quality:</b> %7/100"
    ).arg(metrics.size())
     .arg(QString::number(minLatency, 'f', 2))
     .arg(QString::number(avgMetrics.getLatencyAvg(), 'f', 2))
     .arg(QString::number(maxLatency, 'f', 2))
     .arg(QString::number(avgMetrics.getPacketLoss(), 'f', 2))
     .arg(QString::number(avgMetrics.getJitter(), 'f', 2))
     .arg(static_cast<int>(avgMetrics.getQualityScore()));

    statsLabel->setText(statsText);
}

void TrendsWidget::calculateAndDisplayDateRange(int rangeIndex) {
    int hours = timeRangeCombo->itemData(rangeIndex).toInt();

    if (hours == -1) {
        // Custom range - don't calculate
        return;
    }

    endDate = QDateTime::currentDateTime();
    startDate = endDate.addSecs(-hours * 3600);

    startDateEdit->setDateTime(startDate);
    endDateEdit->setDateTime(endDate);
}
