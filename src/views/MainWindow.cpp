#include "views/MainWindow.h"
#include "ui_mainwindow.h"
#include "views/DeviceTableWidget.h"
#include "views/ScanConfigDialog.h"
#include "views/MetricsWidget.h"
#include "views/DeviceDetailDialog.h"
#include "views/SettingsDialog.h"
#include "viewmodels/DeviceTableViewModel.h"
#include "viewmodels/ScanConfigViewModel.h"
#include "viewmodels/MetricsViewModel.h"
#include "controllers/ScanController.h"
#include "controllers/MetricsController.h"
#include "controllers/ExportController.h"
#include "database/DeviceRepository.h"
#include "services/MonitoringService.h"
#include "services/HistoryService.h"
#include "diagnostics/TraceRouteService.h"
#include "diagnostics/MtuDiscovery.h"
#include "diagnostics/BandwidthTester.h"
#include "diagnostics/DnsDiagnostics.h"
#include "services/WakeOnLanService.h"
#include "../utils/Logger.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QProgressBar>
#include <QLabel>
#include <QDockWidget>

MainWindow::MainWindow(
    ScanController* scanController,
    MetricsController* metricsController,
    ExportController* exportController,
    DeviceRepository* deviceRepository,
    MonitoringService* monitoringService,
    HistoryService* historyService,
    TraceRouteService* tracerouteService,
    MtuDiscovery* mtuDiscovery,
    BandwidthTester* bandwidthTester,
    DnsDiagnostics* dnsDiagnostics,
    WakeOnLanService* wolService,
    QWidget* parent
)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , scanController(scanController)
    , metricsController(metricsController)
    , exportController(exportController)
    , deviceRepository(deviceRepository)
    , monitoringService(monitoringService)
    , historyService(historyService)
    , tracerouteService(tracerouteService)
    , mtuDiscovery(mtuDiscovery)
    , bandwidthTester(bandwidthTester)
    , dnsDiagnostics(dnsDiagnostics)
    , wolService(wolService)
    , deviceTableViewModel(new DeviceTableViewModel(deviceRepository, this))
    , deviceTable(nullptr)
    , progressBar(nullptr)
    , statusLabel(nullptr)
    , deviceCountLabel(nullptr)
    , metricsWidget(nullptr)
    , metricsViewModel(nullptr)
    , metricsDock(nullptr)
{
    ui->setupUi(this);

    setupMenuBar();
    setupToolBar();
    setupStatusBar();
    setupDeviceTable();
    setupMetricsWidget();
    setupConnections();

    // Load initial devices
    deviceTableViewModel->loadDevices();

    Logger::info("MainWindow initialized");
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::setupMenuBar() {
    // File Menu
    QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(tr("&New Scan..."), this, &MainWindow::onNewScanTriggered, QKeySequence::New);
    fileMenu->addAction(tr("&Export..."), this, &MainWindow::onExportTriggered, QKeySequence::Save);
    fileMenu->addSeparator();
    fileMenu->addAction(tr("E&xit"), this, &QWidget::close, QKeySequence::Quit);

    // Scan Menu
    QMenu* scanMenu = menuBar()->addMenu(tr("&Scan"));
    scanMenu->addAction(tr("&Quick Scan"), this, &MainWindow::onQuickScan, QKeySequence(Qt::CTRL | Qt::Key_Q));
    scanMenu->addAction(tr("&Deep Scan"), this, &MainWindow::onDeepScan, QKeySequence(Qt::CTRL | Qt::Key_D));
    scanMenu->addAction(tr("&Stop Scan"), this, &MainWindow::onStopScanTriggered, QKeySequence(Qt::CTRL | Qt::Key_S));

    // View Menu
    QMenu* viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(tr("&Refresh"), this, &MainWindow::onRefresh, QKeySequence::Refresh);
    viewMenu->addAction(tr("&Clear Results"), this, &MainWindow::onClearResults);

    // Help Menu
    QMenu* helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(tr("&About"), this, &MainWindow::onAboutTriggered);
}

void MainWindow::setupToolBar() {
    QToolBar* toolbar = ui->toolBar;
    toolbar->setIconSize(QSize(24, 24));

    // Scan actions
    QAction* newScanAction = toolbar->addAction(tr("New Scan"));
    connect(newScanAction, &QAction::triggered, this, &MainWindow::onNewScanTriggered);

    QAction* stopScanAction = toolbar->addAction(tr("Stop Scan"));
    connect(stopScanAction, &QAction::triggered, this, &MainWindow::onStopScanTriggered);

    toolbar->addSeparator();

    // Export action
    QAction* exportAction = toolbar->addAction(tr("Export"));
    connect(exportAction, &QAction::triggered, this, &MainWindow::onExportTriggered);
}

void MainWindow::setupStatusBar() {
    statusLabel = new QLabel(tr("Ready"), this);
    deviceCountLabel = new QLabel(tr("Devices: 0"), this);

    progressBar = new QProgressBar(this);
    progressBar->setVisible(false);
    progressBar->setMaximumWidth(200);

    ui->statusBar->addWidget(statusLabel, 1);
    ui->statusBar->addWidget(progressBar);
    ui->statusBar->addPermanentWidget(deviceCountLabel);
}

void MainWindow::setupDeviceTable() {
    deviceTable = new DeviceTableWidget(deviceTableViewModel, this);

    // Set Wake-on-LAN service
    deviceTable->setWakeOnLanService(wolService);

    // Add to main layout
    QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(ui->deviceTableContainer->layout());
    if (layout) {
        layout->addWidget(deviceTable);
    }
}

void MainWindow::setupMetricsWidget() {
    // Create MetricsViewModel
    metricsViewModel = new MetricsViewModel(metricsController, deviceRepository, this);

    // Create MetricsWidget
    metricsWidget = new MetricsWidget(metricsViewModel, this);

    // Create dock widget
    metricsDock = new QDockWidget(tr("Device Metrics"), this);
    metricsDock->setWidget(metricsWidget);
    metricsDock->setAllowedAreas(Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);

    addDockWidget(Qt::RightDockWidgetArea, metricsDock);

    // Initially hidden
    metricsDock->hide();

    Logger::info("MetricsWidget setup completed");
}

void MainWindow::setupConnections() {
    // ScanController signals
    connect(scanController, &ScanController::scanStarted,
            this, &MainWindow::onScanStarted);
    connect(scanController, &ScanController::scanStatusChanged,
            this, &MainWindow::onScanStatusChanged);
    connect(scanController, &ScanController::devicesUpdated,
            this, &MainWindow::onDevicesUpdated);
    connect(scanController, &ScanController::deviceDiscovered,
            this, &MainWindow::onDeviceDiscovered);
    connect(scanController, &ScanController::scanProgressUpdated,
            this, &MainWindow::onScanProgressUpdated);

    // DeviceTableWidget signals
    connect(deviceTable, &DeviceTableWidget::deviceDoubleClicked,
            this, &MainWindow::onShowDeviceDetails);
    connect(deviceTable, &DeviceTableWidget::pingDeviceRequested,
            this, &MainWindow::onPingDevice);

    // DeviceTableViewModel signals
    connect(deviceTableViewModel, &DeviceTableViewModel::deviceCountChanged,
            [this](int count) {
                deviceCountLabel->setText(tr("Devices: %1").arg(count));
            });
}

void MainWindow::onNewScanTriggered() {
    ScanConfigViewModel* viewModel = new ScanConfigViewModel(this);
    ScanConfigDialog dialog(viewModel, this);

    if (dialog.exec() == QDialog::Accepted) {
        ScanCoordinator::ScanConfig config = dialog.getConfiguration();
        scanController->executeCustomScan(config);
    }
}

void MainWindow::onStopScanTriggered() {
    scanController->stopCurrentScan();
    updateStatusMessage(tr("Scan stopped"));
}

void MainWindow::onExportTriggered() {
    QString fileName = QFileDialog::getSaveFileName(
        this,
        tr("Export Devices"),
        QString(),
        tr("CSV Files (*.csv);;JSON Files (*.json);;XML Files (*.xml);;HTML Report (*.html)")
    );

    if (!fileName.isEmpty()) {
        ExportController::ExportFormat format;

        if (fileName.endsWith(".json")) {
            format = ExportController::JSON;
        } else if (fileName.endsWith(".xml")) {
            format = ExportController::XML;
        } else if (fileName.endsWith(".html")) {
            format = ExportController::HTML;
        } else {
            format = ExportController::CSV;
        }

        exportController->exportDevices(format, fileName);
        updateStatusMessage(tr("Exported to %1").arg(fileName));
    }
}

void MainWindow::onSettingsTriggered() {
    SettingsDialog dialog(this);

    // Connect settings applied signal to refresh application settings
    connect(&dialog, &SettingsDialog::settingsApplied, this, [this]() {
        Logger::info("Settings have been updated");
        // TODO: Reload application settings if needed (theme, font, etc.)
    });

    dialog.exec();
}

void MainWindow::onAboutTriggered() {
    QMessageBox::about(this, tr("About LanScan"),
        tr("<h3>LanScan - Network Scanner</h3>"
           "<p>Version 0.5.0-phase5</p>"
           "<p>Cross-platform network scanning and diagnostic tool.</p>"
           "<p>Built with Qt 6.9.1 and C++17</p>"
           "<p>Copyright © 2025 Paolo Sereno</p>"));
}

void MainWindow::onQuickScan() {
    ScanConfigViewModel* viewModel = new ScanConfigViewModel(this);
    viewModel->loadQuickScanPreset();

    // Auto-detect network
    QStringList networks = viewModel->detectLocalNetworks();
    if (!networks.isEmpty()) {
        viewModel->setSubnet(networks.first());
    }

    ScanConfigDialog dialog(viewModel, this);
    if (dialog.exec() == QDialog::Accepted) {
        scanController->executeCustomScan(dialog.getConfiguration());
    }
}

void MainWindow::onDeepScan() {
    ScanConfigViewModel* viewModel = new ScanConfigViewModel(this);
    viewModel->loadDeepScanPreset();

    // Auto-detect network
    QStringList networks = viewModel->detectLocalNetworks();
    if (!networks.isEmpty()) {
        viewModel->setSubnet(networks.first());
    }

    ScanConfigDialog dialog(viewModel, this);
    if (dialog.exec() == QDialog::Accepted) {
        scanController->executeCustomScan(dialog.getConfiguration());
    }
}

void MainWindow::onRefresh() {
    deviceTableViewModel->loadDevices();
    updateStatusMessage(tr("Devices refreshed"));
}

void MainWindow::onClearResults() {
    deviceTableViewModel->clear();
    scanController->clearAllDevices();
    updateStatusMessage(tr("Results cleared"));
}

void MainWindow::onScanStarted(int totalHosts) {
    // Mark all existing devices as offline before scan starts
    // Devices that are found during scan will be updated to online
    deviceTableViewModel->markAllDevicesOffline();
    Logger::info(QString("Scan starting - marked all devices as offline"));
}

void MainWindow::onScanStatusChanged(const QString& status) {
    updateStatusMessage(status);
}

void MainWindow::onDevicesUpdated() {
    deviceTableViewModel->loadDevices();
}

void MainWindow::onDeviceDiscovered(const Device& device) {
    // Update or add device directly to the ViewModel
    // This is more efficient than reloading all devices
    deviceTableViewModel->addDevice(device);  // addDevice internally handles update if exists
}

void MainWindow::onScanProgressUpdated(int current, int total, double percentage) {
    progressBar->setVisible(true);
    progressBar->setRange(0, total);
    progressBar->setValue(current);
    progressBar->setFormat(QString("%1/%2 (%p%)").arg(current).arg(total));
}

void MainWindow::onDeviceDoubleClicked(const Device& device) {
    // Deprecated - now using onShowDeviceDetails
    onShowDeviceDetails(device);
}

void MainWindow::onShowDeviceDetails(const Device& device) {
    Logger::info(QString("MainWindow: Opening device details for %1").arg(device.getIp()));

    // Create DeviceDetailDialog with all services
    DeviceDetailDialog* dialog = new DeviceDetailDialog(
        device,
        metricsController,
        monitoringService,
        historyService,
        tracerouteService,
        mtuDiscovery,
        bandwidthTester,
        dnsDiagnostics,
        this
    );

    // Show modal dialog
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->exec();
}

void MainWindow::onPingDevice(const Device& device) {
    if (device.getIp().isEmpty()) {
        Logger::warn("Cannot ping device: empty IP address");
        return;
    }

    // Show metrics dock
    metricsDock->show();
    metricsDock->raise();

    // Set device in MetricsWidget
    QList<Device> devices = {device};
    metricsWidget->setDevices(devices);
    metricsWidget->setDevice(device);

    // Auto-start monitoring with 1 second interval
    metricsWidget->startMonitoring(1000);

    updateStatusMessage(tr("Monitoring device: %1").arg(device.getIp()));
    Logger::info("Started monitoring device: " + device.getIp());
}

void MainWindow::updateStatusMessage(const QString& message) {
    statusLabel->setText(message);
    Logger::info("Status: " + message);
}
