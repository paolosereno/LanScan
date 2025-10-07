#include "views/MetricsWidget.h"
#include "ui_metricswidget.h"
#include "viewmodels/MetricsViewModel.h"
#include "charts/LatencyChart.h"
#include "charts/PacketLossChart.h"
#include "charts/JitterChart.h"
#include "utils/Logger.h"
#include <QVBoxLayout>
#include <QColor>

MetricsWidget::MetricsWidget(MetricsViewModel* viewModel, QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::MetricsWidget)
    , viewModel(viewModel)
    , latencyChart(nullptr)
    , packetLossChart(nullptr)
    , jitterChart(nullptr)
    , currentMonitoringInterval(1000)
{
    ui->setupUi(this);

    if (!viewModel) {
        Logger::error("MetricsWidget: viewModel is null");
        return;
    }

    setupCharts();
    setupConnections();

    Logger::debug("MetricsWidget: Initialized");
}

MetricsWidget::~MetricsWidget() {
    delete ui;
    Logger::debug("MetricsWidget: Destroyed");
}

void MetricsWidget::setupCharts() {
    // Create chart instances
    latencyChart = new LatencyChart(this);
    packetLossChart = new PacketLossChart(this);
    jitterChart = new JitterChart(this);

    // Add charts to their respective tab containers
    QVBoxLayout* latencyLayout = new QVBoxLayout(ui->latencyChartContainer);
    latencyLayout->setContentsMargins(0, 0, 0, 0);
    latencyLayout->addWidget(latencyChart);

    QVBoxLayout* packetLossLayout = new QVBoxLayout(ui->packetLossChartContainer);
    packetLossLayout->setContentsMargins(0, 0, 0, 0);
    packetLossLayout->addWidget(packetLossChart);

    QVBoxLayout* jitterLayout = new QVBoxLayout(ui->jitterChartContainer);
    jitterLayout->setContentsMargins(0, 0, 0, 0);
    jitterLayout->addWidget(jitterChart);

    Logger::debug("MetricsWidget: Charts setup completed");
}

void MetricsWidget::setupConnections() {
    if (!viewModel) {
        return;
    }

    // Connect ViewModel signals
    connect(viewModel, &MetricsViewModel::metricsUpdated,
            this, &MetricsWidget::onMetricsUpdated);
    connect(viewModel, &MetricsViewModel::monitoringStarted,
            this, &MetricsWidget::onMonitoringStarted);
    connect(viewModel, &MetricsViewModel::monitoringStopped,
            this, &MetricsWidget::onMonitoringStopped);

    // Connect charts to ViewModel
    connect(viewModel, &MetricsViewModel::metricsUpdated,
            latencyChart, &LatencyChart::onMetricsUpdated);
    connect(viewModel, &MetricsViewModel::metricsUpdated,
            packetLossChart, &PacketLossChart::onMetricsUpdated);
    connect(viewModel, &MetricsViewModel::metricsUpdated,
            jitterChart, &JitterChart::onMetricsUpdated);

    // Connect UI controls
    connect(ui->startButton, &QPushButton::clicked,
            this, &MetricsWidget::onStartMonitoringClicked);
    connect(ui->stopButton, &QPushButton::clicked,
            this, &MetricsWidget::onStopMonitoringClicked);
    connect(ui->deviceComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MetricsWidget::onDeviceSelected);

    Logger::debug("MetricsWidget: Connections setup completed");
}

void MetricsWidget::setDevice(const Device& device) {
    if (!viewModel) {
        Logger::error("MetricsWidget::setDevice: viewModel is null");
        return;
    }

    Logger::debug(QString("MetricsWidget::setDevice called for %1 (%2), ID: %3")
                      .arg(device.hostname().isEmpty() ? "Unknown" : device.hostname())
                      .arg(device.ip())
                      .arg(device.getId().isEmpty() ? "empty" : device.getId()));

    // Clear charts when device changes
    clearCharts();

    viewModel->setDevice(device);

    Logger::debug(QString("MetricsWidget: Device set to %1 (%2)")
                      .arg(device.hostname())
                      .arg(device.ip()));
}

void MetricsWidget::startMonitoring(int intervalMs) {
    if (!viewModel) {
        Logger::error("MetricsWidget::startMonitoring: viewModel is null");
        return;
    }

    Logger::debug(QString("MetricsWidget::startMonitoring called with interval %1ms").arg(intervalMs));
    currentMonitoringInterval = intervalMs;
    viewModel->startMonitoring(intervalMs);
}

void MetricsWidget::stopMonitoring() {
    if (!viewModel) {
        return;
    }

    viewModel->stopMonitoring();
}

void MetricsWidget::setDevices(const QList<Device>& deviceList) {
    devices = deviceList;

    // Block signals while updating combo box
    ui->deviceComboBox->blockSignals(true);
    ui->deviceComboBox->clear();

    for (const Device& device : devices) {
        QString displayText = QString("%1 (%2)")
                                  .arg(device.hostname().isEmpty() ? "Unknown" : device.hostname())
                                  .arg(device.ip());
        ui->deviceComboBox->addItem(displayText);
    }

    ui->deviceComboBox->blockSignals(false);

    Logger::debug(QString("MetricsWidget: %1 devices loaded").arg(devices.size()));
}

void MetricsWidget::clearCharts() {
    if (latencyChart) {
        latencyChart->clearData();
    }
    if (packetLossChart) {
        packetLossChart->clearData();
    }
    if (jitterChart) {
        jitterChart->clearData();
    }

    // Reset summary labels
    ui->currentLatencyLabel->setText("--");
    ui->currentPacketLossLabel->setText("--");
    ui->currentJitterLabel->setText("--");
    ui->qualityScoreLabel->setText("--");

    Logger::debug("MetricsWidget: Charts cleared");
}

void MetricsWidget::onDeviceSelected(int index) {
    if (index < 0 || index >= devices.size()) {
        return;
    }

    const Device& device = devices[index];
    setDevice(device);

    Logger::debug(QString("MetricsWidget: Device selected - %1 (%2)")
                      .arg(device.hostname())
                      .arg(device.ip()));
}

void MetricsWidget::onMetricsUpdated(const NetworkMetrics& metrics) {
    updateSummaryLabels(metrics);
}

void MetricsWidget::onStartMonitoringClicked() {
    // Check if device is selected
    if (ui->deviceComboBox->currentIndex() < 0) {
        Logger::warn("MetricsWidget: No device selected for monitoring");
        return;
    }

    startMonitoring(currentMonitoringInterval);
}

void MetricsWidget::onStopMonitoringClicked() {
    stopMonitoring();
}

void MetricsWidget::onMonitoringStarted() {
    ui->startButton->setEnabled(false);
    ui->stopButton->setEnabled(true);
    ui->deviceComboBox->setEnabled(false);

    Logger::info("MetricsWidget: Monitoring started");
}

void MetricsWidget::onMonitoringStopped() {
    ui->startButton->setEnabled(true);
    ui->stopButton->setEnabled(false);
    ui->deviceComboBox->setEnabled(true);

    Logger::info("MetricsWidget: Monitoring stopped");
}

void MetricsWidget::updateSummaryLabels(const NetworkMetrics& metrics) {
    // Update latency
    ui->currentLatencyLabel->setText(QString("%1 ms").arg(metrics.latencyAvg(), 0, 'f', 2));

    // Update packet loss with color
    QString packetLossText = QString("%1%").arg(metrics.packetLoss(), 0, 'f', 2);
    ui->currentPacketLossLabel->setText(packetLossText);

    if (metrics.packetLoss() < 1.0) {
        ui->currentPacketLossLabel->setStyleSheet("color: #4CAF50; font-size: 14pt; font-weight: bold;"); // Green
    } else if (metrics.packetLoss() < 5.0) {
        ui->currentPacketLossLabel->setStyleSheet("color: #FF9800; font-size: 14pt; font-weight: bold;"); // Orange
    } else {
        ui->currentPacketLossLabel->setStyleSheet("color: #F44336; font-size: 14pt; font-weight: bold;"); // Red
    }

    // Update jitter
    ui->currentJitterLabel->setText(QString("%1 ms").arg(metrics.jitter(), 0, 'f', 2));

    // Update quality score with color
    ui->qualityScoreLabel->setText(metrics.qualityScoreString());
    QColor color = getQualityColor(metrics.qualityScoreString());
    ui->qualityScoreLabel->setStyleSheet(
        QString("color: %1; font-size: 14pt; font-weight: bold;").arg(color.name())
    );
}

QColor MetricsWidget::getQualityColor(const QString& qualityScore) const {
    if (qualityScore == "Excellent") {
        return QColor("#4CAF50"); // Green
    } else if (qualityScore == "Good") {
        return QColor("#8BC34A"); // Light green
    } else if (qualityScore == "Fair") {
        return QColor("#FF9800"); // Orange
    } else if (qualityScore == "Poor") {
        return QColor("#F44336"); // Red
    } else {
        return QColor("#9E9E9E"); // Grey for unknown
    }
}
