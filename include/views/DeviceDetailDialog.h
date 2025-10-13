#ifndef DEVICEDETAILDIALOG_H
#define DEVICEDETAILDIALOG_H

#include <QDialog>
#include <QTimer>
#include "models/Device.h"
#include "models/NetworkMetrics.h"
#include "models/TraceRouteHop.h"
#include "services/MonitoringService.h"
#include "services/HistoryService.h"
#include "diagnostics/TraceRouteService.h"
#include "diagnostics/MtuDiscovery.h"
#include "diagnostics/BandwidthTester.h"
#include "diagnostics/DnsDiagnostics.h"
#include "widgets/QualityGauge.h"

namespace Ui {
class DeviceDetailDialog;
}

class MetricsWidget;
class MetricsViewModel;
class MetricsController;

/**
 * @brief Device detail dialog with comprehensive device information and diagnostics
 *
 * Provides a tabbed interface with:
 * - Overview: Basic device information
 * - Ports: Open ports list
 * - Metrics: Real-time monitoring with MetricsWidget
 * - History: Historical events and metrics
 * - Diagnostics: Traceroute, MTU, Bandwidth, DNS tools
 */
class DeviceDetailDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DeviceDetailDialog(
        const Device& device,
        MetricsController* metricsController,
        MonitoringService* monitoringService,
        HistoryService* historyService,
        TraceRouteService* tracerouteService,
        MtuDiscovery* mtuDiscovery,
        BandwidthTester* bandwidthTester,
        DnsDiagnostics* dnsDiagnostics,
        QWidget *parent = nullptr
    );
    ~DeviceDetailDialog();

private slots:
    // Traceroute slots
    void onRunTracerouteClicked();
    void onCancelTracerouteClicked();
    void onHopDiscovered(const TraceRouteHop& hop);
    void onTraceCompleted();
    void onTraceError(const QString& error);

    // MTU Discovery slots
    void onDiscoverMtuClicked();
    void onMtuDiscovered(int mtu);
    void onMtuDiscoveryError(const QString& error);

    // Bandwidth Test slots
    void onTestBandwidthClicked();
    void onBandwidthTestCompleted(double bandwidth);
    void onBandwidthTestError(const QString& error);

    // DNS Diagnostics slots
    void onDnsLookupClicked();
    void onDnsLookupCompleted(const QList<DnsDiagnostics::DnsRecord>& records);
    void onDnsLookupError(const QString& error);

private:
    Ui::DeviceDetailDialog *ui;
    Device m_device;

    // Services
    MetricsController* m_metricsController;
    MonitoringService* m_monitoringService;
    HistoryService* m_historyService;
    TraceRouteService* m_tracerouteService;
    MtuDiscovery* m_mtuDiscovery;
    BandwidthTester* m_bandwidthTester;
    DnsDiagnostics* m_dnsDiagnostics;

    // Widgets
    MetricsWidget* m_metricsWidget;
    MetricsViewModel* m_metricsViewModel;
    QualityGauge* m_qualityGauge;

    // UI setup methods
    void setupUi();
    void setupOverviewTab();
    void setupPortsTab();
    void setupMetricsTab();
    void setupDiagnosticsTab();
    void setupConnections();

    // Data loading methods
    void loadDeviceInfo();
    void loadPorts();

    // Utility methods
    QString formatTimestamp(const QDateTime& timestamp);
    int convertQualityScoreToValue(NetworkMetrics::QualityScore score) const;
};

#endif // DEVICEDETAILDIALOG_H
