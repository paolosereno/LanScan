#include "views/DeviceDetailDialog.h"
#include "ui_devicedetaildialog.h"
#include "views/MetricsWidget.h"
#include "viewmodels/MetricsViewModel.h"
#include "controllers/MetricsController.h"
#include "utils/Logger.h"
#include "utils/TimeFormatter.h"
#include <QDateTime>
#include <QMessageBox>
#include <QVBoxLayout>

DeviceDetailDialog::DeviceDetailDialog(
    const Device& device,
    MetricsController* metricsController,
    MonitoringService* monitoringService,
    HistoryService* historyService,
    TraceRouteService* tracerouteService,
    MtuDiscovery* mtuDiscovery,
    BandwidthTester* bandwidthTester,
    DnsDiagnostics* dnsDiagnostics,
    QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DeviceDetailDialog)
    , m_device(device)
    , m_metricsController(metricsController)
    , m_monitoringService(monitoringService)
    , m_historyService(historyService)
    , m_tracerouteService(tracerouteService)
    , m_mtuDiscovery(mtuDiscovery)
    , m_bandwidthTester(bandwidthTester)
    , m_dnsDiagnostics(dnsDiagnostics)
    , m_metricsWidget(nullptr)
    , m_metricsViewModel(nullptr)
{
    ui->setupUi(this);
    setupUi();
    setupConnections();

    // Load device data
    loadDeviceInfo();
    loadPorts();
    loadHistory();

    Logger::info(QString("DeviceDetailDialog: Opened for device %1").arg(m_device.getIp()));
}

DeviceDetailDialog::~DeviceDetailDialog()
{
    // Cleanup
    if (m_metricsViewModel) {
        m_metricsViewModel->stopMonitoring();
    }

    delete ui;
    Logger::info("DeviceDetailDialog: Closed");
}

void DeviceDetailDialog::setupUi()
{
    // Set window title
    setWindowTitle(QString("Device Details - %1").arg(m_device.getIp()));

    // Setup all tabs
    setupOverviewTab();
    setupPortsTab();
    setupMetricsTab();
    setupHistoryTab();
    setupDiagnosticsTab();
}

void DeviceDetailDialog::setupOverviewTab()
{
    // Header information
    ui->ipAddressLabel->setText(m_device.getIp());
    ui->hostnameLabel->setText(m_device.hostname().isEmpty() ?
                              tr("No hostname") : m_device.hostname());

    // Status indicator
    bool online = m_device.isOnline();
    ui->statusLabel->setText(online ? tr("● Online") : tr("● Offline"));
    ui->statusLabel->setStyleSheet(online ?
        "QLabel { color: #00AA00; }" : "QLabel { color: #AA0000; }");

    // Basic information
    ui->valueIpAddress->setText(m_device.getIp());
    ui->valueHostname->setText(m_device.hostname().isEmpty() ?
                              tr("Unknown") : m_device.hostname());
    ui->valueMacAddress->setText(m_device.macAddress().isEmpty() ?
                                tr("Unknown") : m_device.macAddress());
    ui->valueVendor->setText(m_device.vendor().isEmpty() ?
                            tr("Unknown") : m_device.vendor());

    // Device type - simplified since Device model doesn't have type enum
    ui->valueDeviceType->setText(tr("Network Device"));

    // Network information
    ui->valueFirstSeen->setText(formatTimestamp(m_device.lastSeen()));  // Use lastSeen as Device doesn't have firstSeen
    ui->valueLastSeen->setText(formatTimestamp(m_device.lastSeen()));
    ui->valueOpenPorts->setText(QString::number(m_device.openPorts().size()));
}

void DeviceDetailDialog::setupPortsTab()
{
    // Configure table
    ui->portsTableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->portsTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    loadPorts();
}

void DeviceDetailDialog::setupMetricsTab()
{
    // Update the UI labels with actual metrics data when available
    // The basic UI structure is already defined in the .ui file

    // Create and setup QualityGauge
    m_qualityGauge = new QualityGauge(this);
    m_qualityGauge->setValue(0);

    // Insert the QualityGauge into the container widget
    QVBoxLayout* gaugeContainerLayout = new QVBoxLayout(ui->qualityGaugeContainer);
    gaugeContainerLayout->setContentsMargins(0, 0, 0, 0);
    gaugeContainerLayout->addWidget(m_qualityGauge, 0, Qt::AlignCenter);

    // Create MetricsViewModel for data updates
    m_metricsViewModel = new MetricsViewModel(
        m_metricsController,
        nullptr,  // DeviceRepository not needed here
        this
    );

    // Connect metrics updates to UI labels and QualityGauge
    connect(m_metricsViewModel, &MetricsViewModel::metricsUpdated,
            this, [this](const NetworkMetrics& metrics) {
                // Update latency (using average)
                ui->valueLatency->setText(QString("%1 ms").arg(metrics.latencyAvg(), 0, 'f', 1));

                // Update packet loss
                ui->valuePacketLoss->setText(QString("%1%").arg(metrics.packetLoss(), 0, 'f', 1));

                // Update jitter
                ui->valueJitter->setText(QString("%1 ms").arg(metrics.jitter(), 0, 'f', 1));

                // Update quality score text
                QString qualityText;
                switch (metrics.qualityScore()) {
                    case NetworkMetrics::Excellent:
                        qualityText = tr("Excellent");
                        break;
                    case NetworkMetrics::Good:
                        qualityText = tr("Good");
                        break;
                    case NetworkMetrics::Fair:
                        qualityText = tr("Fair");
                        break;
                    case NetworkMetrics::Poor:
                        qualityText = tr("Poor");
                        break;
                    case NetworkMetrics::Critical:
                        qualityText = tr("Critical");
                        break;
                }
                ui->valueQualityScore->setText(qualityText);

                // Update QualityGauge with numerical value (0-100)
                int gaugeValue = convertQualityScoreToValue(metrics.qualityScore());
                m_qualityGauge->setValue(gaugeValue);

                Logger::debug(QString("DeviceDetailDialog: Updated QualityGauge to %1 (%2)")
                             .arg(gaugeValue).arg(qualityText));
            });

    // Set device and start monitoring
    m_metricsViewModel->setDevice(m_device);
    m_metricsViewModel->startMonitoring();

    Logger::info("DeviceDetailDialog: Metrics monitoring started with UI integration and QualityGauge");
}

void DeviceDetailDialog::setupHistoryTab()
{
    // Configure history table
    ui->historyTableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->historyTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    // Set default time range (Last 24 Hours)
    ui->timeRangeComboBox->setCurrentIndex(2);

    loadHistory();
}

void DeviceDetailDialog::setupDiagnosticsTab()
{
    // Initial state
    ui->cancelTracerouteButton->setEnabled(false);
    ui->tracerouteOutput->clear();
    ui->mtuResultLabel->setText(tr("MTU: -"));
    ui->bandwidthResultLabel->setText(tr("Speed: -"));
    ui->dnsResultLabel->setText(tr("Records: -"));
}

void DeviceDetailDialog::setupConnections()
{
    // Traceroute connections
    connect(ui->runTracerouteButton, &QPushButton::clicked,
            this, &DeviceDetailDialog::onRunTracerouteClicked);
    connect(ui->cancelTracerouteButton, &QPushButton::clicked,
            this, &DeviceDetailDialog::onCancelTracerouteClicked);

    if (m_tracerouteService) {
        connect(m_tracerouteService, &TraceRouteService::hopDiscovered,
                this, &DeviceDetailDialog::onHopDiscovered);
        connect(m_tracerouteService, &TraceRouteService::traceCompleted,
                this, &DeviceDetailDialog::onTraceCompleted);
        connect(m_tracerouteService, &TraceRouteService::traceError,
                this, &DeviceDetailDialog::onTraceError);
    }

    // MTU Discovery connections
    connect(ui->discoverMtuButton, &QPushButton::clicked,
            this, &DeviceDetailDialog::onDiscoverMtuClicked);

    if (m_mtuDiscovery) {
        connect(m_mtuDiscovery, &MtuDiscovery::mtuDiscovered,
                this, &DeviceDetailDialog::onMtuDiscovered);
        connect(m_mtuDiscovery, &MtuDiscovery::discoveryError,
                this, &DeviceDetailDialog::onMtuDiscoveryError);
    }

    // Bandwidth Test connections
    connect(ui->testBandwidthButton, &QPushButton::clicked,
            this, &DeviceDetailDialog::onTestBandwidthClicked);

    if (m_bandwidthTester) {
        connect(m_bandwidthTester, &BandwidthTester::testCompleted,
                this, &DeviceDetailDialog::onBandwidthTestCompleted);
        connect(m_bandwidthTester, &BandwidthTester::testError,
                this, &DeviceDetailDialog::onBandwidthTestError);
    }

    // DNS Diagnostics connections
    connect(ui->dnsLookupButton, &QPushButton::clicked,
            this, &DeviceDetailDialog::onDnsLookupClicked);

    if (m_dnsDiagnostics) {
        connect(m_dnsDiagnostics, &DnsDiagnostics::lookupCompleted,
                this, &DeviceDetailDialog::onDnsLookupCompleted);
        connect(m_dnsDiagnostics, &DnsDiagnostics::lookupError,
                this, &DeviceDetailDialog::onDnsLookupError);
    }

    // History connections
    connect(ui->refreshHistoryButton, &QPushButton::clicked,
            this, &DeviceDetailDialog::onRefreshHistoryClicked);
    connect(ui->timeRangeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &DeviceDetailDialog::onTimeRangeChanged);
}

void DeviceDetailDialog::loadDeviceInfo()
{
    // Already done in setupOverviewTab
}

void DeviceDetailDialog::loadPorts()
{
    ui->portsTableWidget->setRowCount(0);

    QList<PortInfo> ports = m_device.openPorts();
    ui->portsTableWidget->setRowCount(ports.size());

    for (int i = 0; i < ports.size(); ++i) {
        const PortInfo& port = ports[i];

        // Port number
        QTableWidgetItem* portItem = new QTableWidgetItem(QString::number(port.getPort()));
        portItem->setTextAlignment(Qt::AlignCenter);
        ui->portsTableWidget->setItem(i, 0, portItem);

        // Protocol
        QString protocol = (port.protocol() == PortInfo::Protocol::TCP) ? "TCP" : "UDP";
        QTableWidgetItem* protocolItem = new QTableWidgetItem(protocol);
        protocolItem->setTextAlignment(Qt::AlignCenter);
        ui->portsTableWidget->setItem(i, 1, protocolItem);

        // State
        QString state;
        switch (port.state()) {
            case PortInfo::State::Open:
                state = tr("Open");
                break;
            case PortInfo::State::Closed:
                state = tr("Closed");
                break;
            case PortInfo::State::Filtered:
                state = tr("Filtered");
                break;
            default:
                state = tr("Unknown");
                break;
        }
        QTableWidgetItem* stateItem = new QTableWidgetItem(state);
        stateItem->setTextAlignment(Qt::AlignCenter);
        ui->portsTableWidget->setItem(i, 2, stateItem);

        // Service
        QTableWidgetItem* serviceItem = new QTableWidgetItem(
            port.service().isEmpty() ? tr("Unknown") : port.service()
        );
        ui->portsTableWidget->setItem(i, 3, serviceItem);
    }

    Logger::debug(QString("DeviceDetailDialog: Loaded %1 ports").arg(ports.size()));
}

void DeviceDetailDialog::loadHistory()
{
    if (!m_historyService) {
        Logger::warn("DeviceDetailDialog: HistoryService is null, cannot load history");
        return;
    }

    ui->historyTableWidget->setRowCount(0);

    // Get time range
    QDateTime endTime = QDateTime::currentDateTime();
    QDateTime startTime = getStartTimeForRange(ui->timeRangeComboBox->currentIndex());

    // Get events from history service
    QList<HistoryEvent> events = m_historyService->getEventHistory(
        m_device.getIp(),
        startTime,
        endTime
    );

    ui->historyTableWidget->setRowCount(events.size());

    for (int i = 0; i < events.size(); ++i) {
        const HistoryEvent& event = events[i];

        // Timestamp
        QTableWidgetItem* timeItem = new QTableWidgetItem(formatTimestamp(event.timestamp));
        ui->historyTableWidget->setItem(i, 0, timeItem);

        // Event type
        QTableWidgetItem* typeItem = new QTableWidgetItem(event.eventType);
        ui->historyTableWidget->setItem(i, 1, typeItem);

        // Description
        QTableWidgetItem* descItem = new QTableWidgetItem(event.description);
        ui->historyTableWidget->setItem(i, 2, descItem);
    }

    Logger::debug(QString("DeviceDetailDialog: Loaded %1 history events").arg(events.size()));
}

// Traceroute slots

void DeviceDetailDialog::onRunTracerouteClicked()
{
    if (!m_tracerouteService) {
        QMessageBox::warning(this, tr("Error"),
                           tr("Traceroute service not available"));
        return;
    }

    ui->tracerouteOutput->clear();
    ui->tracerouteOutput->append(QString(tr("Running traceroute to %1...\n"))
                                 .arg(m_device.getIp()));

    ui->runTracerouteButton->setEnabled(false);
    ui->cancelTracerouteButton->setEnabled(true);

    bool started = m_tracerouteService->traceRoute(m_device.getIp());
    if (!started) {
        ui->tracerouteOutput->append(tr("Failed to start traceroute"));
        ui->runTracerouteButton->setEnabled(true);
        ui->cancelTracerouteButton->setEnabled(false);
    }

    Logger::info(QString("DeviceDetailDialog: Started traceroute to %1")
                 .arg(m_device.getIp()));
}

void DeviceDetailDialog::onCancelTracerouteClicked()
{
    if (m_tracerouteService) {
        m_tracerouteService->cancel();
        ui->tracerouteOutput->append(tr("\nTraceroute cancelled by user"));
    }

    ui->runTracerouteButton->setEnabled(true);
    ui->cancelTracerouteButton->setEnabled(false);
}

void DeviceDetailDialog::onHopDiscovered(const TraceRouteHop& hop)
{
    QString hopText = QString("Hop %1: %2 (%3) - %4 ms")
        .arg(hop.hopNumber())
        .arg(hop.hostname().isEmpty() ? tr("Unknown") : hop.hostname())
        .arg(hop.ipAddress())
        .arg(hop.averageRtt(), 0, 'f', 2);

    ui->tracerouteOutput->append(hopText);
}

void DeviceDetailDialog::onTraceCompleted()
{
    ui->tracerouteOutput->append(tr("\nTraceroute completed"));
    ui->runTracerouteButton->setEnabled(true);
    ui->cancelTracerouteButton->setEnabled(false);

    Logger::info("DeviceDetailDialog: Traceroute completed");
}

void DeviceDetailDialog::onTraceError(const QString& error)
{
    ui->tracerouteOutput->append(QString(tr("\nError: %1")).arg(error));
    ui->runTracerouteButton->setEnabled(true);
    ui->cancelTracerouteButton->setEnabled(false);

    Logger::error(QString("DeviceDetailDialog: Traceroute error: %1").arg(error));
}

// MTU Discovery slots

void DeviceDetailDialog::onDiscoverMtuClicked()
{
    if (!m_mtuDiscovery) {
        QMessageBox::warning(this, tr("Error"),
                           tr("MTU Discovery service not available"));
        return;
    }

    ui->mtuResultLabel->setText(tr("MTU: Discovering..."));
    ui->discoverMtuButton->setEnabled(false);

    bool started = m_mtuDiscovery->discoverMtu(m_device.getIp());
    if (!started) {
        ui->mtuResultLabel->setText(tr("MTU: Error"));
        ui->discoverMtuButton->setEnabled(true);
    }

    Logger::info(QString("DeviceDetailDialog: Started MTU discovery for %1")
                 .arg(m_device.getIp()));
}

void DeviceDetailDialog::onMtuDiscovered(int mtu)
{
    ui->mtuResultLabel->setText(QString(tr("MTU: %1 bytes")).arg(mtu));
    ui->discoverMtuButton->setEnabled(true);

    Logger::info(QString("DeviceDetailDialog: MTU discovered: %1 bytes").arg(mtu));
}

void DeviceDetailDialog::onMtuDiscoveryError(const QString& error)
{
    ui->mtuResultLabel->setText(tr("MTU: Error"));
    ui->discoverMtuButton->setEnabled(true);

    QMessageBox::warning(this, tr("MTU Discovery Error"), error);
    Logger::error(QString("DeviceDetailDialog: MTU discovery error: %1").arg(error));
}

// Bandwidth Test slots

void DeviceDetailDialog::onTestBandwidthClicked()
{
    if (!m_bandwidthTester) {
        QMessageBox::warning(this, tr("Error"),
                           tr("Bandwidth tester not available"));
        return;
    }

    ui->bandwidthResultLabel->setText(tr("Speed: Testing..."));
    ui->testBandwidthButton->setEnabled(false);

    // Test download speed on port 80 (HTTP)
    bool started = m_bandwidthTester->testDownloadSpeed(
        m_device.getIp(), 80, 5, BandwidthTester::TCP
    );

    if (!started) {
        ui->bandwidthResultLabel->setText(tr("Speed: Error"));
        ui->testBandwidthButton->setEnabled(true);
    }

    Logger::info(QString("DeviceDetailDialog: Started bandwidth test for %1")
                 .arg(m_device.getIp()));
}

void DeviceDetailDialog::onBandwidthTestCompleted(double bandwidth)
{
    ui->bandwidthResultLabel->setText(QString(tr("Speed: %1 Mbps"))
                                     .arg(bandwidth, 0, 'f', 2));
    ui->testBandwidthButton->setEnabled(true);

    Logger::info(QString("DeviceDetailDialog: Bandwidth: %1 Mbps").arg(bandwidth));
}

void DeviceDetailDialog::onBandwidthTestError(const QString& error)
{
    ui->bandwidthResultLabel->setText(tr("Speed: Error"));
    ui->testBandwidthButton->setEnabled(true);

    QMessageBox::warning(this, tr("Bandwidth Test Error"), error);
    Logger::error(QString("DeviceDetailDialog: Bandwidth test error: %1").arg(error));
}

// DNS Diagnostics slots

void DeviceDetailDialog::onDnsLookupClicked()
{
    if (!m_dnsDiagnostics) {
        QMessageBox::warning(this, tr("Error"),
                           tr("DNS diagnostics not available"));
        return;
    }

    ui->dnsResultLabel->setText(tr("Records: Looking up..."));
    ui->dnsLookupButton->setEnabled(false);

    // If device has hostname, do forward lookup; otherwise do reverse lookup
    bool started;
    if (!m_device.hostname().isEmpty()) {
        started = m_dnsDiagnostics->forwardLookup(m_device.hostname());
    } else {
        started = m_dnsDiagnostics->reverseLookup(m_device.getIp());
    }

    if (!started) {
        ui->dnsResultLabel->setText(tr("Records: Error"));
        ui->dnsLookupButton->setEnabled(true);
    }

    Logger::info(QString("DeviceDetailDialog: Started DNS lookup for %1")
                 .arg(m_device.getIp()));
}

void DeviceDetailDialog::onDnsLookupCompleted(const QList<DnsDiagnostics::DnsRecord>& records)
{
    ui->dnsResultLabel->setText(QString(tr("Records: %1 found")).arg(records.size()));
    ui->dnsLookupButton->setEnabled(true);

    // Show records in a message box
    QString recordsText;
    for (const auto& record : records) {
        recordsText += record.toString() + "\n";
    }

    if (!recordsText.isEmpty()) {
        QMessageBox::information(this, tr("DNS Records"), recordsText);
    }

    Logger::info(QString("DeviceDetailDialog: DNS lookup completed, %1 records found")
                 .arg(records.size()));
}

void DeviceDetailDialog::onDnsLookupError(const QString& error)
{
    ui->dnsResultLabel->setText(tr("Records: Error"));
    ui->dnsLookupButton->setEnabled(true);

    QMessageBox::warning(this, tr("DNS Lookup Error"), error);
    Logger::error(QString("DeviceDetailDialog: DNS lookup error: %1").arg(error));
}

// History slots

void DeviceDetailDialog::onRefreshHistoryClicked()
{
    loadHistory();
    Logger::info("DeviceDetailDialog: History refreshed");
}

void DeviceDetailDialog::onTimeRangeChanged(int index)
{
    Q_UNUSED(index);
    loadHistory();
}

// Utility methods

QString DeviceDetailDialog::formatTimestamp(const QDateTime& timestamp)
{
    if (!timestamp.isValid()) {
        return tr("Never");
    }

    return TimeFormatter::formatRelativeTime(timestamp);
}

QDateTime DeviceDetailDialog::getStartTimeForRange(int rangeIndex)
{
    QDateTime now = QDateTime::currentDateTime();

    switch (rangeIndex) {
        case 0: // Last Hour
            return now.addSecs(-3600);
        case 1: // Last 6 Hours
            return now.addSecs(-6 * 3600);
        case 2: // Last 24 Hours
            return now.addDays(-1);
        case 3: // Last 7 Days
            return now.addDays(-7);
        case 4: // Last 30 Days
            return now.addDays(-30);
        default:
            return now.addDays(-1);
    }
}

int DeviceDetailDialog::convertQualityScoreToValue(NetworkMetrics::QualityScore score) const
{
    // Convert QualityScore enum to a numerical value (0-100) for the gauge
    // The mapping is designed to show visual differences between quality levels
    switch (score) {
        case NetworkMetrics::Excellent:
            return 95;  // 90-100 range - show near top
        case NetworkMetrics::Good:
            return 80;  // 70-89 range - show in upper-middle
        case NetworkMetrics::Fair:
            return 60;  // 50-69 range - show in middle
        case NetworkMetrics::Poor:
            return 35;  // 0-49 range - show in lower area
        case NetworkMetrics::Critical:
            return 15;  // Very poor - show near bottom
        default:
            return 0;   // Unknown/Invalid
    }
}
