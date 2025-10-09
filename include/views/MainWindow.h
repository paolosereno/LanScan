#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProgressBar>
#include <QLabel>
#include <QDockWidget>
#include "../models/Device.h"

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

    // Controller signals
    void onScanStatusChanged(const QString& status);
    void onDevicesUpdated();
    void onScanProgressUpdated(int current, int total, double percentage);

    // Device table signals
    void onDeviceDoubleClicked(const Device& device);
    void onPingDevice(const Device& device);
    void onShowDeviceDetails(const Device& device);

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
    QProgressBar* progressBar;
    QLabel* statusLabel;
    QLabel* deviceCountLabel;

    // Metrics widgets
    MetricsWidget* metricsWidget;
    MetricsViewModel* metricsViewModel;
    QDockWidget* metricsDock;

    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
    void setupConnections();
    void setupDeviceTable();
    void setupMetricsWidget();
    void updateStatusMessage(const QString& message);
};

#endif // MAINWINDOW_H
