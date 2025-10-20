#include "views/MainWindow.h"
#include "ui_mainwindow.h"
#include "views/DeviceTableWidget.h"
#include "views/ScanConfigDialog.h"
#include "views/MetricsWidget.h"
#include "views/DeviceDetailDialog.h"
#include "views/SettingsDialog.h"
#include "views/AboutDialog.h"
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
#include "config/SettingsManager.h"
#include "../utils/Logger.h"
#include "utils/IconLoader.h"
#include "managers/ThemeManager.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QProgressBar>
#include <QLabel>
#include <QDockWidget>
#include <QSettings>
#include <QCloseEvent>

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
    , trayIcon(nullptr)
    , trayMenu(nullptr)
    , minimizeToTray(false)
    , closeToTray(false)
    , enableAlerts(true)
    , alertSound(false)
    , systemNotifications(true)
    , latencyThreshold(100)
    , packetLossThreshold(5)
    , jitterThreshold(10)
{
    ui->setupUi(this);

    setupMenuBar();
    setupToolBar();
    setupStatusBar();
    setupDeviceTable();
    setupMetricsWidget();
    setupSystemTray();
    setupConnections();

    // Load initial devices
    deviceTableViewModel->loadDevices();

    // Load settings
    loadTraySettings();
    loadAlertSettings();

    // Restore window geometry and state
    QByteArray geometry = SettingsManager::instance()->getWindowGeometry();
    if (!geometry.isEmpty()) {
        restoreGeometry(geometry);
        Logger::info("Window geometry restored");
    }

    QByteArray state = SettingsManager::instance()->getWindowState();
    if (!state.isEmpty()) {
        restoreState(state);
        Logger::info("Window state restored");
    }

    Logger::info("MainWindow initialized");
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::setupMenuBar() {
    // File Menu
    QMenu* fileMenu = menuBar()->addMenu(tr("&File"));

    QAction* newScanMenuAction = fileMenu->addAction(IconLoader::loadIcon("scan"), tr("&New Scan..."), this, &MainWindow::onNewScanTriggered, QKeySequence::New);
    newScanMenuAction->setProperty("iconName", "scan");

    QAction* exportMenuAction = fileMenu->addAction(IconLoader::loadIcon("export"), tr("&Export..."), this, &MainWindow::onExportTriggered, QKeySequence::Save);
    exportMenuAction->setProperty("iconName", "export");

    fileMenu->addSeparator();

    QAction* exitMenuAction = fileMenu->addAction(IconLoader::loadIcon("power"), tr("E&xit"), this, &QWidget::close, QKeySequence::Quit);
    exitMenuAction->setProperty("iconName", "power");

    // Scan Menu
    QMenu* scanMenu = menuBar()->addMenu(tr("&Scan"));

    QAction* quickScanMenuAction = scanMenu->addAction(IconLoader::loadIcon("scan"), tr("&Quick Scan"), this, &MainWindow::onQuickScan, QKeySequence(Qt::CTRL | Qt::Key_Q));
    quickScanMenuAction->setProperty("iconName", "scan");

    QAction* deepScanMenuAction = scanMenu->addAction(IconLoader::loadIcon("scan"), tr("&Deep Scan"), this, &MainWindow::onDeepScan, QKeySequence(Qt::CTRL | Qt::Key_D));
    deepScanMenuAction->setProperty("iconName", "scan");

    QAction* stopScanMenuAction = scanMenu->addAction(IconLoader::loadIcon("stop"), tr("&Stop Scan"), this, &MainWindow::onStopScanTriggered, QKeySequence(Qt::CTRL | Qt::Key_S));
    stopScanMenuAction->setProperty("iconName", "stop");

    // View Menu
    QMenu* viewMenu = menuBar()->addMenu(tr("&View"));

    QAction* refreshMenuAction = viewMenu->addAction(IconLoader::loadIcon("refresh"), tr("&Refresh"), this, &MainWindow::onRefresh, QKeySequence::Refresh);
    refreshMenuAction->setProperty("iconName", "refresh");

    QAction* clearMenuAction = viewMenu->addAction(IconLoader::loadIcon("clear"), tr("&Clear Results"), this, &MainWindow::onClearResults);
    clearMenuAction->setProperty("iconName", "clear");

    viewMenu->addSeparator();

    // Tools Menu
    QMenu* toolsMenu = menuBar()->addMenu(tr("&Tools"));

    QAction* settingsMenuAction = toolsMenu->addAction(IconLoader::loadIcon("settings"), tr("&Settings..."), this, &MainWindow::onSettingsTriggered, QKeySequence::Preferences);
    settingsMenuAction->setProperty("iconName", "settings");

    // Help Menu
    QMenu* helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(tr("&About"), this, &MainWindow::onAboutTriggered);
    helpMenu->addAction(tr("About &Qt"), qApp, &QApplication::aboutQt);
}

void MainWindow::setupToolBar() {
    QToolBar* toolbar = ui->toolBar;
    toolbar->setIconSize(QSize(24, 24));

    // Scan actions
    QAction* newScanAction = toolbar->addAction(IconLoader::loadIcon("scan"), tr("New Scan"));
    newScanAction->setProperty("iconName", "scan");
    connect(newScanAction, &QAction::triggered, this, &MainWindow::onNewScanTriggered);

    QAction* stopScanAction = toolbar->addAction(IconLoader::loadIcon("stop"), tr("Stop Scan"));
    stopScanAction->setProperty("iconName", "stop");
    connect(stopScanAction, &QAction::triggered, this, &MainWindow::onStopScanTriggered);

    toolbar->addSeparator();

    // Export action
    QAction* exportAction = toolbar->addAction(IconLoader::loadIcon("export"), tr("Export"));
    exportAction->setProperty("iconName", "export");
    connect(exportAction, &QAction::triggered, this, &MainWindow::onExportTriggered);

    toolbar->addSeparator();

    // Settings action
    QAction* settingsAction = toolbar->addAction(IconLoader::loadIcon("settings"), tr("Settings"));
    settingsAction->setProperty("iconName", "settings");
    connect(settingsAction, &QAction::triggered, this, &MainWindow::onSettingsTriggered);
}

void MainWindow::setupStatusBar() {
    statusLabel = new QLabel(tr("Ready"), this);
    deviceCountLabel = new QLabel(tr("Devices: 0"), this);

    // Use GradientProgressBar instead of QProgressBar
    progressBar = new GradientProgressBar(this);
    progressBar->setVisible(false);
    progressBar->setMaximumWidth(200);
    progressBar->setAnimated(true);
    progressBar->setAnimationDuration(300);

    // Add NetworkActivityIndicator
    activityIndicator = new NetworkActivityIndicator(this);
    activityIndicator->setState(NetworkActivityIndicator::Off);
    activityIndicator->setToolTip(tr("Network Activity"));

    ui->statusBar->addWidget(statusLabel, 1);
    ui->statusBar->addWidget(progressBar);
    ui->statusBar->addPermanentWidget(activityIndicator);
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

    // Fixed dock - no close button, not movable, not floatable (only closable via View menu)
    metricsDock->setFeatures(QDockWidget::NoDockWidgetFeatures);

    addDockWidget(Qt::RightDockWidgetArea, metricsDock);

    // Initially hidden
    metricsDock->hide();

    // Create custom toggle action for the dock (since NoDockWidgetFeatures disables the built-in one)
    QAction* toggleMetricsAction = new QAction(IconLoader::loadIcon("details"), tr("Device &Metrics"), this);
    toggleMetricsAction->setProperty("iconName", "details");
    toggleMetricsAction->setCheckable(true);
    toggleMetricsAction->setChecked(false); // Initially hidden

    connect(toggleMetricsAction, &QAction::triggered, this, [this, toggleMetricsAction](bool checked) {
        if (checked) {
            // When opening dock from menu, use currently selected device
            Device selectedDevice = deviceTable->getSelectedDevice();
            if (!selectedDevice.getIp().isEmpty()) {
                // Set device and start monitoring
                QList<Device> devices = {selectedDevice};
                metricsWidget->setDevices(devices);
                metricsWidget->setDevice(selectedDevice);
                metricsWidget->startMonitoring(1000);
                Logger::info("Metrics dock opened from menu - monitoring device: " + selectedDevice.getIp());
            } else {
                Logger::info("Metrics dock opened from menu - no device selected");
            }
            metricsDock->show();
        } else {
            metricsDock->hide();
        }
    });

    // Update action state when dock visibility changes
    connect(metricsDock, &QDockWidget::visibilityChanged, toggleMetricsAction, &QAction::setChecked);

    // Add toggle action to View menu
    QList<QAction*> actions = menuBar()->actions();
    for (QAction* action : actions) {
        if (action->text() == tr("&View")) {
            QMenu* viewMenu = action->menu();
            if (viewMenu) {
                viewMenu->addAction(toggleMetricsAction);
            }
            break;
        }
    }

    // Connect visibility changed to stop monitoring when dock is hidden
    connect(metricsDock, &QDockWidget::visibilityChanged, this, [this](bool visible) {
        if (!visible) {
            // Stop monitoring when dock is hidden
            metricsWidget->stopMonitoring();
            Logger::info("Metrics dock hidden - monitoring stopped");
        }
    });

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

    // Theme manager signal
    connect(&ThemeManager::instance(), &ThemeManager::themeChanged,
            this, &MainWindow::onThemeChanged);

    // MonitoringService signals
    connect(monitoringService, &MonitoringService::alertTriggered,
            this, &MainWindow::onAlertTriggered);
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
    connect(&dialog, &SettingsDialog::settingsApplied, this, &MainWindow::onSettingsApplied);

    dialog.exec();
}

void MainWindow::onAboutTriggered() {
    AboutDialog dialog(this);
    dialog.exec();
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

    // Set activity indicator to blinking during scan
    activityIndicator->setState(NetworkActivityIndicator::Blinking);

    Logger::info(QString("Scan starting - marked all devices as offline"));
}

void MainWindow::onScanStatusChanged(const QString& status) {
    updateStatusMessage(status);

    // Update activity indicator based on status
    if (status.contains("completed", Qt::CaseInsensitive) ||
        status.contains("stopped", Qt::CaseInsensitive) ||
        status.contains("Error", Qt::CaseInsensitive)) {
        // Scan finished - turn off indicator
        activityIndicator->setState(NetworkActivityIndicator::Off);
        progressBar->setVisible(false);
    }
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

void MainWindow::setupSystemTray() {
    // Check if system tray is available
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        Logger::warn("System tray not available on this platform");
        return;
    }

    // Create tray icon
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(IconLoader::loadIcon("scan", QSize(22, 22)));
    trayIcon->setToolTip(tr("LanScan - Network Scanner"));

    // Create tray menu
    trayMenu = new QMenu(this);

    QAction* showHideAction = trayMenu->addAction(IconLoader::loadIcon("scan"), tr("Show/Hide"));
    showHideAction->setProperty("iconName", "scan");
    connect(showHideAction, &QAction::triggered, this, &MainWindow::onShowHideAction);

    trayMenu->addSeparator();

    QAction* quickScanAction = trayMenu->addAction(IconLoader::loadIcon("scan"), tr("Quick Scan"));
    quickScanAction->setProperty("iconName", "scan");
    connect(quickScanAction, &QAction::triggered, this, &MainWindow::onTrayQuickScan);

    trayMenu->addSeparator();

    QAction* exitAction = trayMenu->addAction(IconLoader::loadIcon("power"), tr("Exit"));
    exitAction->setProperty("iconName", "power");
    connect(exitAction, &QAction::triggered, qApp, &QApplication::quit);

    trayIcon->setContextMenu(trayMenu);

    // Connect tray icon activation
    connect(trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::onTrayIconActivated);

    // Show tray icon
    trayIcon->show();

    Logger::info("System tray initialized");
}

void MainWindow::loadTraySettings() {
    QSettings settings;
    minimizeToTray = settings.value("general/minimizeToTray", false).toBool();
    closeToTray = settings.value("general/closeToTray", false).toBool();

    Logger::info(QString("Tray settings: minimize=%1, close=%2")
        .arg(minimizeToTray).arg(closeToTray));
}

void MainWindow::loadAlertSettings() {
    QSettings settings;
    enableAlerts = settings.value("Notifications/EnableAlerts", true).toBool();
    alertSound = settings.value("Notifications/AlertSound", false).toBool();
    systemNotifications = settings.value("Notifications/SystemNotifications", true).toBool();
    latencyThreshold = settings.value("Notifications/LatencyThreshold", 100).toInt();
    packetLossThreshold = settings.value("Notifications/PacketLossThreshold", 5).toInt();
    jitterThreshold = settings.value("Notifications/JitterThreshold", 10).toInt();

    Logger::info(QString("Alert settings loaded: enabled=%1, sound=%2, sysNotif=%3, latency=%4ms, loss=%5%, jitter=%6ms")
        .arg(enableAlerts).arg(alertSound).arg(systemNotifications)
        .arg(latencyThreshold).arg(packetLossThreshold).arg(jitterThreshold));
}

void MainWindow::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason) {
    if (reason == QSystemTrayIcon::Trigger || reason == QSystemTrayIcon::DoubleClick) {
        onShowHideAction();
    }
}

void MainWindow::onShowHideAction() {
    if (isVisible()) {
        hide();
        Logger::info("Window hidden to tray");
    } else {
        show();
        raise();
        activateWindow();
        Logger::info("Window restored from tray");
    }
}

void MainWindow::onTrayQuickScan() {
    // Show window if hidden
    if (!isVisible()) {
        show();
        raise();
        activateWindow();
    }

    // Trigger quick scan
    onQuickScan();

    // Show tray notification
    if (trayIcon && trayIcon->isVisible()) {
        trayIcon->showMessage(
            tr("LanScan"),
            tr("Quick scan started"),
            QSystemTrayIcon::Information,
            3000
        );
    }
}

void MainWindow::onAlertTriggered(const QString& deviceId, const Alert& alert) {
    // Check if alerts are enabled
    if (!enableAlerts) {
        Logger::debug(QString("Alert suppressed (alerts disabled): %1").arg(alert.toString()));
        return;
    }

    Logger::info(QString("Alert triggered: %1 - %2").arg(alert.typeToString()).arg(alert.message()));

    // Show system tray notification if enabled
    if (systemNotifications && trayIcon && trayIcon->isVisible()) {
        // Determine icon based on severity
        QSystemTrayIcon::MessageIcon icon;
        switch (alert.severity()) {
            case AlertSeverity::Info:
                icon = QSystemTrayIcon::Information;
                break;
            case AlertSeverity::Warning:
                icon = QSystemTrayIcon::Warning;
                break;
            case AlertSeverity::Critical:
                icon = QSystemTrayIcon::Critical;
                break;
            default:
                icon = QSystemTrayIcon::Information;
                break;
        }

        // Format the title with alert type
        QString title = QString("LanScan - %1").arg(alert.severityToString());

        // Format the message with device ID and alert message
        QString message = QString("%1\nDevice: %2")
            .arg(alert.message())
            .arg(deviceId);

        // Show tray message
        trayIcon->showMessage(title, message, icon, 5000);
    }

    // TODO: Play alert sound if enabled
    if (alertSound) {
        // Sound playback will be implemented in next step
        Logger::debug("Alert sound playback requested (not yet implemented)");
    }
}

void MainWindow::onSettingsApplied() {
    Logger::info("Settings have been applied - reloading alert settings");

    // Reload tray settings
    loadTraySettings();

    // Reload alert settings
    loadAlertSettings();
}

void MainWindow::closeEvent(QCloseEvent* event) {
    if (closeToTray && trayIcon && trayIcon->isVisible()) {
        hide();
        event->ignore();

        if (trayIcon->supportsMessages()) {
            trayIcon->showMessage(
                tr("LanScan"),
                tr("Application minimized to tray. Right-click the tray icon for options."),
                QSystemTrayIcon::Information,
                3000
            );
        }

        Logger::info("Window closed to tray");
    } else {
        // Save window geometry and state before closing
        SettingsManager::instance()->setWindowGeometry(saveGeometry());
        SettingsManager::instance()->setWindowState(saveState());
        SettingsManager::instance()->save();
        Logger::info("Window geometry and state saved");

        event->accept();
        Logger::info("Application closing");
    }
}

void MainWindow::changeEvent(QEvent* event) {
    if (event->type() == QEvent::WindowStateChange) {
        if (isMinimized() && minimizeToTray && trayIcon && trayIcon->isVisible()) {
            hide();
            event->ignore();
            Logger::info("Window minimized to tray");
            return;
        }
    }

    QMainWindow::changeEvent(event);
}

void MainWindow::onThemeChanged() {
    Logger::info("Theme changed - updating icons");
    updateIcons();
}

void MainWindow::updateIcons() {
    // Find all QActions in the window and update their icons
    QList<QAction*> actions = findChildren<QAction*>();

    for (QAction* action : actions) {
        // Check if action has iconName property
        QVariant iconNameVar = action->property("iconName");
        if (iconNameVar.isValid()) {
            QString iconName = iconNameVar.toString();
            action->setIcon(IconLoader::loadIcon(iconName));
        }
    }

    // Update tray icon
    if (trayIcon) {
        trayIcon->setIcon(IconLoader::loadIcon("scan", QSize(22, 22)));
    }

    // Update application icon
    qApp->setWindowIcon(QIcon(":/icons/lanscan-app-icon.svg"));

    Logger::info("Icons updated for current theme");
}
