#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProgressBar>
#include <QLabel>
#include <QDockWidget>
#include <QSystemTrayIcon>
#include <QMenu>
#include "../models/Device.h"
#include "../widgets/NetworkActivityIndicator.h"
#include "../widgets/GradientProgressBar.h"

class ScanController;
class MetricsController;
class ExportController;
class DeviceTableWidget;
class DeviceTableViewModel;
class DeviceRepository;
class MetricsWidget;
class MetricsViewModel;
class MonitoringService;
class HistoryService;
class TraceRouteService;
class MtuDiscovery;
class BandwidthTester;
class DnsDiagnostics;
class WakeOnLanService;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/**
 * @brief Main application window (View)
 *
 * Implements the main UI with menu bar, toolbar, status bar, and device table.
 * Uses MVVM pattern with dependency injection of controllers.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(
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
        QWidget* parent = nullptr
    );
    ~MainWindow();

private slots:
    // Menu actions
    void onNewScanTriggered();
    void onStopScanTriggered();
    void onExportTriggered();
    void onSettingsTriggered();
    void onAboutTriggered();
    void onQuickScan();
    void onDeepScan();
    void onRefresh();
    void onClearResults();

    // Theme management
    void onThemeChanged();

    // Controller signals
    void onScanStarted(int totalHosts);
    void onScanStatusChanged(const QString& status);
    void onDevicesUpdated();
    void onDeviceDiscovered(const Device& device);
    void onScanProgressUpdated(int current, int total, double percentage);

    // Device table signals
    void onDeviceDoubleClicked(const Device& device);
    void onPingDevice(const Device& device);
    void onShowDeviceDetails(const Device& device);

    // System tray slots
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void onShowHideAction();
    void onTrayQuickScan();

protected:
    void closeEvent(QCloseEvent* event) override;
    void changeEvent(QEvent* event) override;

private:
    Ui::MainWindow* ui;

    // Controllers
    ScanController* scanController;
    MetricsController* metricsController;
    ExportController* exportController;

    // Repository
    DeviceRepository* deviceRepository;

    // Services
    MonitoringService* monitoringService;
    HistoryService* historyService;
    TraceRouteService* tracerouteService;
    MtuDiscovery* mtuDiscovery;
    BandwidthTester* bandwidthTester;
    DnsDiagnostics* dnsDiagnostics;
    WakeOnLanService* wolService;

    // Widgets
    DeviceTableWidget* deviceTable;
    DeviceTableViewModel* deviceTableViewModel;
    GradientProgressBar* progressBar;
    QLabel* statusLabel;
    QLabel* deviceCountLabel;
    NetworkActivityIndicator* activityIndicator;

    // Metrics widgets
    MetricsWidget* metricsWidget;
    MetricsViewModel* metricsViewModel;
    QDockWidget* metricsDock;

    // System tray
    QSystemTrayIcon* trayIcon;
    QMenu* trayMenu;
    bool minimizeToTray;
    bool closeToTray;

    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
    void setupConnections();
    void setupDeviceTable();
    void setupMetricsWidget();
    void setupSystemTray();
    void updateStatusMessage(const QString& message);
    void loadTraySettings();
    void updateIcons();
};

#endif // MAINWINDOW_H
