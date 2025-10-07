# Phase 7: Advanced Diagnostics

**Timeline**: Week 13-14 (Updated 2025-10-07)
**Objective**: Implement advanced diagnostic tools (traceroute, MTU discovery, bandwidth testing, continuous monitoring)

---

## 7.0 MetricsWidget Integration (Phase 6 Completion) ✅

**Priority**: HIGH - Complete Phase 6 before starting Phase 7
**Status**: ✅ **COMPLETED** (2025-10-07)

### MainWindow Integration

Integrate MetricsWidget into MainWindow to enable ping/monitoring from context menu.

**Tasks**:
- [x] Add MetricsWidget as dock widget or dialog to MainWindow
- [x] Connect "Ping Device" context menu action to MetricsWidget
- [x] Pass MetricsViewModel and controllers to MetricsWidget
- [x] Update MainWindow constructor to create MetricsViewModel
- [x] Implement `onPingDevice()` in MainWindow to show MetricsWidget
- [x] Remove Phase 6 placeholder messages from DeviceTableWidget
- [x] Test ping functionality from context menu

### Implementation Summary

**Files Modified**:
1. **include/views/MainWindow.h**
   - Added MetricsWidget* and MetricsViewModel* members
   - Added QDockWidget* metricsDock member
   - Added DeviceRepository* member
   - Added setupMetricsWidget() private method
   - Added onPingDevice(const Device&) slot

2. **src/views/MainWindow.cpp**
   - Implemented setupMetricsWidget() to create dock widget
   - Dock widget positioned on right side, initially hidden
   - Implemented onPingDevice() to show dock and auto-start monitoring
   - Connected pingDeviceRequested signal from DeviceTableWidget

3. **include/views/DeviceTableWidget.h**
   - Added signal: `void pingDeviceRequested(const Device& device)`

4. **src/views/DeviceTableWidget.cpp**
   - Updated onPingDevice() to emit pingDeviceRequested signal
   - Removed QMessageBox placeholder

### Testing Results
- ✅ Context menu "Ping Device" opens MetricsWidget in dock
- ✅ Device is automatically selected in MetricsWidget
- ✅ Charts display real-time metrics (Latency, Packet Loss, Jitter)
- ✅ Start/Stop monitoring buttons work correctly
- ✅ Summary panel updates with metrics
- ✅ Dock widget can be hidden/shown and repositioned
- ✅ No Phase 6 placeholder messages remain
- ✅ Build successful (LanScan.exe created)
- ✅ Tests passing: 20/21 (95%)

### Bug Fixes Applied
- ✅ Fixed MetricsViewModel to use IP address as fallback when Device.ID is empty
- ✅ Updated setDevice(), startMonitoring(), onMonitoringTimerTimeout(), onMetricsCollected()
- ✅ Added comprehensive logging for debugging

### Log Verification (2025-10-07 13:25:21)
```
[INFO] MetricsViewModel: Monitoring started for device  (192.168.1.1) with interval 1000ms
[INFO] MetricsWidget: Monitoring started
[DEBUG] MetricsViewModel: Collecting metrics for device  (192.168.1.1)  // Every 1 second
[INFO] Status: Monitoring device: 192.168.1.1
```
- ✅ Monitoring initializes correctly
- ✅ Metrics collection triggered every 1 second as expected
- ✅ Button states managed correctly (Start disabled, Stop enabled when active)

### Recent Improvements (2025-10-07)

**MetricsController Enhancements**:
- ✅ Added `currentMonitoringDevice` tracking for proper device context
- ✅ Enhanced `onMetricsUpdated()` to emit `metricsCollected` signal with device ID
- ✅ Improved cleanup logic to stop collection only for current device
- ✅ Added comprehensive logging for device monitoring state

**PingService Multi-Language Support**:
- ✅ Enhanced Windows ping output parsing for internationalization
- ✅ Added support for Italian ("Risposta da", "durata", "byte")
- ✅ Added support for German ("Antwort von", "Zeit", "Bytes")
- ✅ Added support for French ("Réponse de", "temps", "octets")
- ✅ Added support for Spanish ("Respuesta desde", "tiempo", "bytes")
- ✅ Improved error message detection for timeout/unreachable states

**Code Changes**:
1. **include/controllers/MetricsController.h**
   - Added `QString currentMonitoringDevice` member
2. **src/controllers/MetricsController.cpp**
   - Enhanced `onMetricsUpdated()` with device context emission
   - Improved `collectMetricsForDevice()` with device tracking
   - Updated `cleanupTimer()` with proper device cleanup
3. **src/network/diagnostics/PingService.cpp**
   - Enhanced `parseWindowsPing()` with multi-language regex patterns
   - Added comprehensive error keyword detection

### Implementation Options

**Option A: Dock Widget** (Recommended)
```cpp
// In MainWindow.h
class MainWindow : public QMainWindow {
    // ...
private:
    MetricsWidget* metricsWidget;
    MetricsViewModel* metricsViewModel;
    QDockWidget* metricsDock;
};

// In MainWindow.cpp constructor
void MainWindow::setupMetricsWidget() {
    // Create MetricsViewModel
    metricsViewModel = new MetricsViewModel(
        metricsController,
        deviceRepository,
        this
    );

    // Create MetricsWidget
    metricsWidget = new MetricsWidget(metricsViewModel, this);

    // Create dock widget
    metricsDock = new QDockWidget(tr("Device Metrics"), this);
    metricsDock->setWidget(metricsWidget);
    metricsDock->setAllowedAreas(Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);

    addDockWidget(Qt::RightDockWidgetArea, metricsDock);

    // Initially hidden
    metricsDock->hide();
}
```

**Option B: Modal Dialog**
```cpp
void MainWindow::onPingDevice(const Device& device) {
    // Create dialog with MetricsWidget
    QDialog* dialog = new QDialog(this);
    dialog->setWindowTitle(tr("Monitor Device: %1").arg(device.hostname()));

    QVBoxLayout* layout = new QVBoxLayout(dialog);

    MetricsViewModel* viewModel = new MetricsViewModel(
        metricsController, deviceRepository, dialog
    );

    MetricsWidget* widget = new MetricsWidget(viewModel, dialog);
    widget->setDevices({device});
    widget->setDevice(device);
    layout->addWidget(widget);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Close, dialog
    );
    connect(buttonBox, &QDialogButtonBox::rejected, dialog, &QDialog::accept);
    layout->addWidget(buttonBox);

    dialog->resize(800, 600);
    dialog->exec();
}
```

### DeviceTableWidget Updates

Remove placeholder and implement real ping functionality:

```cpp
// In DeviceTableWidget.cpp
void DeviceTableWidget::onPingDevice() {
    Device device = getSelectedDevice();
    if (device.getIp().isEmpty()) {
        return;
    }

    // Emit signal to MainWindow to show MetricsWidget
    emit pingDeviceRequested(device);
}
```

### Signal/Slot Connections

```cpp
// In MainWindow.cpp
void MainWindow::setupConnections() {
    // ... existing connections ...

    // Connect ping request from device table
    connect(deviceTable, &DeviceTableWidget::pingDeviceRequested,
            this, &MainWindow::onPingDevice);
}

void MainWindow::onPingDevice(const Device& device) {
    // Show metrics dock
    metricsDock->show();

    // Set device in MetricsWidget
    metricsWidget->setDevices({device});
    metricsWidget->setDevice(device);

    // Auto-start monitoring
    metricsWidget->startMonitoring(1000); // 1 second interval
}
```

### Files to Modify

1. **include/views/MainWindow.h**
   - Add MetricsWidget* and MetricsViewModel* members
   - Add QDockWidget* metricsDock member
   - Add setupMetricsWidget() private method
   - Add onPingDevice(const Device&) slot

2. **src/views/MainWindow.cpp**
   - Implement setupMetricsWidget()
   - Call setupMetricsWidget() in constructor
   - Implement onPingDevice() to show dock and set device
   - Connect signals from device table

3. **include/views/DeviceTableWidget.h**
   - Add signal: `void pingDeviceRequested(const Device& device)`

4. **src/views/DeviceTableWidget.cpp**
   - Update onPingDevice() to emit pingDeviceRequested signal
   - Remove QMessageBox placeholder

5. **src/main.cpp**
   - Ensure MetricsController is available for MainWindow

### Testing Checklist

- [ ] Context menu "Ping Device" opens MetricsWidget
- [ ] Device is automatically selected in MetricsWidget
- [ ] Charts display real-time metrics
- [ ] Start/Stop monitoring buttons work
- [ ] Summary panel updates with metrics
- [ ] Multiple devices can be monitored sequentially
- [ ] Dock widget can be hidden/shown from View menu
- [ ] No Phase 6 placeholder messages remain

---

## 7.1 Traceroute Service

### TraceRouteService.h/cpp
Execute and parse traceroute operations

```cpp
class TraceRouteService : public QObject {
    Q_OBJECT

public:
    TraceRouteService(QObject* parent = nullptr);

    void executeTraceroute(const QString& targetIp, int maxHops = 30, int timeout = 5000);
    void stop();

    QList<TraceRouteHop> getHops() const;

signals:
    void hopDiscovered(const TraceRouteHop& hop);
    void traceCompleted(const QList<TraceRouteHop>& hops, qint64 durationMs);
    void traceError(const QString& error);
    void progress(int currentHop, int maxHops);

private:
    QProcess* process;
    QString targetIp;
    int maxHops;
    int timeout;

    QList<TraceRouteHop> hops;

    void parseTracerouteOutput(const QString& output);
    QString buildTracerouteCommand(const QString& target, int maxHops, int timeout);

#ifdef Q_OS_WIN
    void parseWindowsTraceroute(const QString& output);
#else
    void parseLinuxTraceroute(const QString& output);
#endif

private slots:
    void onProcessFinished(int exitCode, QProcess::ExitStatus status);
    void onProcessReadyRead();
};
```

### TraceRouteHop.h/cpp
Model for traceroute hop

```cpp
class TraceRouteHop {
public:
    int hopNumber;
    QString ipAddress;
    QString hostname;
    double rtt1;        // First ping RTT
    double rtt2;        // Second ping RTT
    double rtt3;        // Third ping RTT
    double avgRtt;      // Average RTT
    bool timeout;       // If hop timed out
    QString asNumber;   // AS number (optional)

    TraceRouteHop();

    bool isValid() const;
    double getMinRtt() const;
    double getMaxRtt() const;
    QString toString() const;
};
```

### Implementation

```cpp
void TraceRouteService::executeTraceroute(const QString& targetIp, int maxHops, int timeout) {
    this->targetIp = targetIp;
    this->maxHops = maxHops;
    this->timeout = timeout;

    hops.clear();

    process = new QProcess(this);
    connect(process, &QProcess::readyReadStandardOutput,
            this, &TraceRouteService::onProcessReadyRead);
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &TraceRouteService::onProcessFinished);

    QString command = buildTracerouteCommand(targetIp, maxHops, timeout);

    Logger::info("Executing traceroute: " + command);
    process->start(command);
}

QString TraceRouteService::buildTracerouteCommand(const QString& target, int maxHops, int timeout) {
#ifdef Q_OS_WIN
    // Windows: tracert -h maxHops -w timeout target
    return QString("tracert -h %1 -w %2 %3")
        .arg(maxHops)
        .arg(timeout)
        .arg(target);
#else
    // Linux: traceroute -m maxHops -w timeout target
    return QString("traceroute -m %1 -w %2 %3")
        .arg(maxHops)
        .arg(timeout / 1000) // Convert ms to seconds
        .arg(target);
#endif
}

void TraceRouteService::onProcessReadyRead() {
    QString output = process->readAllStandardOutput();
    parseTracerouteOutput(output);
}

void TraceRouteService::parseTracerouteOutput(const QString& output) {
#ifdef Q_OS_WIN
    parseWindowsTraceroute(output);
#else
    parseLinuxTraceroute(output);
#endif
}

#ifdef Q_OS_WIN
void TraceRouteService::parseWindowsTraceroute(const QString& output) {
    // Windows tracert output format:
    // 1    <1 ms    <1 ms    <1 ms  192.168.1.1
    // 2    10 ms    11 ms    12 ms  10.0.0.1 [gateway.local]

    QStringList lines = output.split('\n', Qt::SkipEmptyParts);

    for (const QString& line : lines) {
        QString trimmed = line.trimmed();

        // Skip header lines
        if (trimmed.startsWith("Tracing") || trimmed.startsWith("over")) {
            continue;
        }

        // Regex to parse: hop_number rtt1 rtt2 rtt3 ip [hostname]
        QRegularExpression regex(
            R"(^\s*(\d+)\s+(?:<1 ms|(\d+) ms|\*)\s+(?:<1 ms|(\d+) ms|\*)\s+(?:<1 ms|(\d+) ms|\*)\s+([^\s]+)(?:\s+\[([^\]]+)\])?)"
        );
        QRegularExpressionMatch match = regex.match(trimmed);

        if (match.hasMatch()) {
            TraceRouteHop hop;
            hop.hopNumber = match.captured(1).toInt();

            // Parse RTTs (handle "*" as timeout)
            hop.rtt1 = match.captured(2).isEmpty() ? -1 : match.captured(2).toDouble();
            hop.rtt2 = match.captured(3).isEmpty() ? -1 : match.captured(3).toDouble();
            hop.rtt3 = match.captured(4).isEmpty() ? -1 : match.captured(4).toDouble();

            hop.ipAddress = match.captured(5);
            hop.hostname = match.captured(6);

            hop.timeout = (hop.rtt1 < 0 && hop.rtt2 < 0 && hop.rtt3 < 0);

            if (!hop.timeout) {
                hop.avgRtt = (hop.rtt1 + hop.rtt2 + hop.rtt3) / 3.0;
            }

            hops.append(hop);
            emit hopDiscovered(hop);
            emit progress(hop.hopNumber, maxHops);
        }
    }
}
#endif

void TraceRouteService::onProcessFinished(int exitCode, QProcess::ExitStatus status) {
    if (status == QProcess::NormalExit && exitCode == 0) {
        qint64 duration = 0; // Calculate from start time
        emit traceCompleted(hops, duration);
    } else {
        emit traceError("Traceroute failed with exit code " + QString::number(exitCode));
    }

    process->deleteLater();
}
```

### Tests
- [ ] TraceRouteServiceTest
- [ ] Test Windows and Linux output parsing

---

## 7.2 Advanced Diagnostics

### MtuDiscovery.h/cpp
Path MTU discovery

```cpp
class MtuDiscovery : public QObject {
    Q_OBJECT

public:
    MtuDiscovery(QObject* parent = nullptr);

    void discoverMtu(const QString& targetIp);
    int getDiscoveredMtu() const;

signals:
    void mtuDiscovered(int mtu);
    void progress(int currentMtu, int minMtu, int maxMtu);
    void discoveryError(const QString& error);

private:
    QString targetIp;
    int discoveredMtu = 1500;

    void binarySearchMtu(int minMtu, int maxMtu);
    bool testMtuSize(int mtuSize);
    bool sendPingWithSize(const QString& target, int packetSize, bool dontFragment);
};
```

### MTU Discovery Implementation

```cpp
void MtuDiscovery::discoverMtu(const QString& targetIp) {
    this->targetIp = targetIp;

    // Binary search between 576 (IPv4 minimum) and 9000 (jumbo frames)
    binarySearchMtu(576, 9000);
}

void MtuDiscovery::binarySearchMtu(int minMtu, int maxMtu) {
    while (minMtu <= maxMtu) {
        int midMtu = (minMtu + maxMtu) / 2;

        emit progress(midMtu, minMtu, maxMtu);

        if (testMtuSize(midMtu)) {
            // Success: MTU is at least midMtu
            discoveredMtu = midMtu;
            minMtu = midMtu + 1;
        } else {
            // Failure: MTU is less than midMtu
            maxMtu = midMtu - 1;
        }
    }

    emit mtuDiscovered(discoveredMtu);
}

bool MtuDiscovery::testMtuSize(int mtuSize) {
    // Send ICMP ping with specific packet size and DF (Don't Fragment) flag
    int icmpHeaderSize = 28; // IP header (20) + ICMP header (8)
    int dataSize = mtuSize - icmpHeaderSize;

    return sendPingWithSize(targetIp, dataSize, true);
}

bool MtuDiscovery::sendPingWithSize(const QString& target, int packetSize, bool dontFragment) {
#ifdef Q_OS_WIN
    QString command = QString("ping -n 1 -l %1 -f %2")
                        .arg(packetSize)
                        .arg(target);
#else
    QString command = QString("ping -c 1 -s %1 -M do %2")
                        .arg(packetSize)
                        .arg(target);
#endif

    QProcess process;
    process.start(command);
    process.waitForFinished(5000);

    QString output = process.readAllStandardOutput();

    // Check if ping succeeded (no "Packet needs to be fragmented" error)
    return !output.contains("needs to be fragmented", Qt::CaseInsensitive) &&
           !output.contains("Message too long", Qt::CaseInsensitive) &&
           process.exitCode() == 0;
}
```

### BandwidthTester.h/cpp
Bandwidth testing

```cpp
class BandwidthTester : public QObject {
    Q_OBJECT

public:
    struct BandwidthResult {
        double downloadSpeedMbps;
        double uploadSpeedMbps;
        qint64 latency;
        double jitter;
        QDateTime timestamp;
    };

    BandwidthTester(QObject* parent = nullptr);

    void testBandwidth(const QString& targetIp, int durationSec = 10);
    void stop();

    BandwidthResult getResult() const;

signals:
    void testStarted();
    void progress(int percentage, double currentSpeedMbps);
    void testCompleted(const BandwidthResult& result);
    void testError(const QString& error);

private:
    QTcpSocket* socket;
    QString targetIp;
    int port = 8080;
    int durationSec;

    BandwidthResult result;

    QElapsedTimer timer;
    qint64 bytesTransferred = 0;

    void testDownloadSpeed();
    void testUploadSpeed();
    void calculateSpeed();

private slots:
    void onConnected();
    void onReadyRead();
    void onBytesWritten(qint64 bytes);
    void onError(QAbstractSocket::SocketError error);
};
```

### DnsDiagnostics.h/cpp
Advanced DNS diagnostics

```cpp
class DnsDiagnostics : public QObject {
    Q_OBJECT

public:
    struct DnsRecord {
        QString hostname;
        QString recordType; // A, AAAA, MX, CNAME, etc.
        QString value;
        int ttl;
    };

    DnsDiagnostics(QObject* parent = nullptr);

    void lookupHost(const QString& hostname);
    void reverseLookup(const QString& ipAddress);
    void queryDnsServer(const QString& hostname, const QString& dnsServer);

    QList<DnsRecord> getRecords() const;

signals:
    void lookupCompleted(const QList<DnsRecord>& records);
    void lookupError(const QString& error);

private:
    QDnsLookup* dnsLookup;
    QList<DnsRecord> records;

    void parseDnsResponse(QDnsLookup* lookup);

private slots:
    void onDnsLookupFinished();
};
```

### Tests
- [ ] MtuDiscoveryTest
- [ ] BandwidthTesterTest
- [ ] DnsDiagnosticsTest

---

## 7.3 Monitoring Service

### MonitoringService.h/cpp
Continuous monitoring service

```cpp
class MonitoringService : public QObject {
    Q_OBJECT

public:
    struct MonitoringConfig {
        QString deviceId;
        int intervalMs = 1000;      // Monitoring interval
        bool enableAlerts = true;
        double alertLatencyThreshold = 100.0;   // ms
        double alertPacketLossThreshold = 5.0;  // %
        double alertJitterThreshold = 30.0;     // ms
    };

    MonitoringService(
        MetricsAggregator* metricsAggregator,
        AlertService* alertService,
        HistoryService* historyService,
        QObject* parent = nullptr
    );

    void startMonitoring(const MonitoringConfig& config);
    void stopMonitoring(const QString& deviceId);
    void stopAllMonitoring();

    bool isMonitoring(const QString& deviceId) const;
    QList<QString> getMonitoredDevices() const;

signals:
    void monitoringStarted(const QString& deviceId);
    void monitoringStopped(const QString& deviceId);
    void metricsCollected(const QString& deviceId, const NetworkMetrics& metrics);
    void alertTriggered(const QString& deviceId, const Alert& alert);

private:
    MetricsAggregator* metricsAggregator;
    AlertService* alertService;
    HistoryService* historyService;

    QMap<QString, QTimer*> monitoringTimers;
    QMap<QString, MonitoringConfig> monitoringConfigs;

    void collectMetrics(const QString& deviceId);
    void checkThresholds(const QString& deviceId, const NetworkMetrics& metrics);

private slots:
    void onTimerTimeout();
};
```

### AlertService.h/cpp
Alert generation and management

```cpp
class Alert {
public:
    enum Severity {
        Info,
        Warning,
        Critical
    };

    enum Type {
        HighLatency,
        PacketLoss,
        HighJitter,
        DeviceOffline,
        DeviceOnline
    };

    QString id;
    Type type;
    Severity severity;
    QString deviceId;
    QString message;
    QDateTime timestamp;
    bool acknowledged = false;

    QString toString() const;
    QColor getSeverityColor() const;
};

class AlertService : public QObject {
    Q_OBJECT

public:
    AlertService(QObject* parent = nullptr);

    void createAlert(Alert::Type type, Alert::Severity severity,
                    const QString& deviceId, const QString& message);

    QList<Alert> getAlerts() const;
    QList<Alert> getUnacknowledgedAlerts() const;
    QList<Alert> getAlertsForDevice(const QString& deviceId) const;

    void acknowledgeAlert(const QString& alertId);
    void acknowledgeAllAlerts();
    void clearAlerts();

signals:
    void alertCreated(const Alert& alert);
    void alertAcknowledged(const QString& alertId);

private:
    QList<Alert> alerts;
    int maxAlerts = 1000;

    QString generateAlertId();
    void pruneOldAlerts();
};
```

### HistoryService.h/cpp
Historical data management

```cpp
class HistoryService : public QObject {
    Q_OBJECT

public:
    HistoryService(DatabaseManager* dbManager, QObject* parent = nullptr);

    void saveMetrics(const QString& deviceId, const NetworkMetrics& metrics);
    void saveEvent(const QString& deviceId, const QString& eventType,
                   const QString& description);

    QList<NetworkMetrics> getMetricsHistory(const QString& deviceId,
                                           const QDateTime& start,
                                           const QDateTime& end);

    QList<HistoryEvent> getEventHistory(const QString& deviceId,
                                       const QDateTime& start,
                                       const QDateTime& end);

    void pruneOldData(int daysToKeep = 30);

signals:
    void metricsStored(const QString& deviceId);
    void eventStored(const QString& deviceId);

private:
    DatabaseManager* dbManager;

    void createTables();
};
```

### Monitoring Implementation

```cpp
void MonitoringService::startMonitoring(const MonitoringConfig& config) {
    if (isMonitoring(config.deviceId)) {
        stopMonitoring(config.deviceId);
    }

    monitoringConfigs[config.deviceId] = config;

    QTimer* timer = new QTimer(this);
    timer->setInterval(config.intervalMs);
    timer->setProperty("deviceId", config.deviceId);

    connect(timer, &QTimer::timeout, this, &MonitoringService::onTimerTimeout);

    monitoringTimers[config.deviceId] = timer;
    timer->start();

    emit monitoringStarted(config.deviceId);

    Logger::info("Started monitoring device: " + config.deviceId);
}

void MonitoringService::onTimerTimeout() {
    QTimer* timer = qobject_cast<QTimer*>(sender());
    if (!timer) return;

    QString deviceId = timer->property("deviceId").toString();
    collectMetrics(deviceId);
}

void MonitoringService::collectMetrics(const QString& deviceId) {
    // Collect metrics using MetricsAggregator
    NetworkMetrics metrics = metricsAggregator->collectMetrics(deviceId);

    // Save to history
    historyService->saveMetrics(deviceId, metrics);

    // Check thresholds for alerts
    if (monitoringConfigs[deviceId].enableAlerts) {
        checkThresholds(deviceId, metrics);
    }

    emit metricsCollected(deviceId, metrics);
}

void MonitoringService::checkThresholds(const QString& deviceId, const NetworkMetrics& metrics) {
    const MonitoringConfig& config = monitoringConfigs[deviceId];

    // High latency alert
    if (metrics.latencyAvg > config.alertLatencyThreshold) {
        Alert alert;
        alert.type = Alert::HighLatency;
        alert.severity = Alert::Warning;
        alert.deviceId = deviceId;
        alert.message = QString("High latency detected: %1 ms").arg(metrics.latencyAvg);

        alertService->createAlert(alert.type, alert.severity, deviceId, alert.message);
        emit alertTriggered(deviceId, alert);
    }

    // Packet loss alert
    if (metrics.packetLoss > config.alertPacketLossThreshold) {
        Alert alert;
        alert.type = Alert::PacketLoss;
        alert.severity = Alert::Critical;
        alert.deviceId = deviceId;
        alert.message = QString("Packet loss detected: %1%").arg(metrics.packetLoss);

        alertService->createAlert(alert.type, alert.severity, deviceId, alert.message);
        emit alertTriggered(deviceId, alert);
    }

    // High jitter alert
    if (metrics.jitter > config.alertJitterThreshold) {
        Alert alert;
        alert.type = Alert::HighJitter;
        alert.severity = Alert::Warning;
        alert.deviceId = deviceId;
        alert.message = QString("High jitter detected: %1 ms").arg(metrics.jitter);

        alertService->createAlert(alert.type, alert.severity, deviceId, alert.message);
        emit alertTriggered(deviceId, alert);
    }
}
```

### Tests
- [ ] MonitoringServiceTest
- [ ] AlertServiceTest
- [ ] HistoryServiceTest

---

## 7.4 Device Detail Dialog with MetricsWidget Integration

### Overview

The Device Detail Dialog provides comprehensive device information with integrated real-time monitoring using the MetricsWidget created in Phase 6.

### devicedetaildialog.ui
Device detail dialog layout

```xml
<?xml version="1.0" encoding="UTF-8"?>
<ui version="4.0">
 <widget class="QDialog" name="DeviceDetailDialog">
  <property name="windowTitle">
   <string>Device Details</string>
  </property>
  <layout class="QVBoxLayout">
   <!-- Device header (IP, hostname, status) -->
   <widget class="QWidget" name="headerWidget"/>

   <!-- Tab widget -->
   <widget class="QTabWidget" name="tabWidget">
    <!-- Overview Tab -->
    <widget class="QWidget" name="overviewTab"/>

    <!-- Ports Tab -->
    <widget class="QWidget" name="portsTab"/>

    <!-- Metrics Tab -->
    <widget class="QWidget" name="metricsTab"/>

    <!-- History Tab -->
    <widget class="QWidget" name="historyTab"/>

    <!-- Diagnostics Tab -->
    <widget class="QWidget" name="diagnosticsTab"/>
   </widget>

   <!-- Button box (Close) -->
   <widget class="QDialogButtonBox" name="buttonBox"/>
  </layout>
 </widget>
</ui>
```

### DeviceDetailDialog.h/cpp
Device detail dialog

```cpp
class DeviceDetailDialog : public QDialog {
    Q_OBJECT

public:
    DeviceDetailDialog(
        const Device& device,
        MonitoringService* monitoringService,
        TraceRouteService* tracerouteService,
        MtuDiscovery* mtuDiscovery,
        QWidget* parent = nullptr
    );

private slots:
    void onStartMonitoringClicked();
    void onStopMonitoringClicked();
    void onRunTracerouteClicked();
    void onDiscoverMtuClicked();
    void onMetricsUpdated(const NetworkMetrics& metrics);
    void onHopDiscovered(const TraceRouteHop& hop);
    void onMtuDiscovered(int mtu);

private:
    Ui::DeviceDetailDialog* ui;
    Device device;

    // Services
    MonitoringService* monitoringService;
    TraceRouteService* tracerouteService;
    MtuDiscovery* mtuDiscovery;

    // Widgets
    MetricsWidget* metricsWidget;
    QTableWidget* portsTable;
    QTableWidget* tracerouteTable;
    QTableWidget* historyTable;

    void setupOverviewTab();
    void setupPortsTab();
    void setupMetricsTab();
    void setupHistoryTab();
    void setupDiagnosticsTab();

    void loadDeviceInfo();
    void loadPortsInfo();
    void loadHistory();
};
```

### Overview Tab

```cpp
void DeviceDetailDialog::setupOverviewTab() {
    QFormLayout* layout = new QFormLayout(ui->overviewTab);

    // Basic info
    layout->addRow(tr("IP Address:"), new QLabel(device.ipAddress));
    layout->addRow(tr("Hostname:"), new QLabel(device.hostname));
    layout->addRow(tr("MAC Address:"), new QLabel(device.macAddress));
    layout->addRow(tr("Vendor:"), new QLabel(device.vendor));

    // Status
    QString status = device.isOnline ? tr("Online") : tr("Offline");
    QLabel* statusLabel = new QLabel(status);
    statusLabel->setStyleSheet(device.isOnline ?
        "color: green; font-weight: bold;" :
        "color: red; font-weight: bold;");
    layout->addRow(tr("Status:"), statusLabel);

    // Last seen
    layout->addRow(tr("Last Seen:"),
        new QLabel(device.lastSeen.toString("yyyy-MM-dd hh:mm:ss")));

    // Actions
    QPushButton* pingButton = new QPushButton(tr("Ping"));
    QPushButton* favoriteButton = new QPushButton(tr("Add to Favorites"));

    connect(pingButton, &QPushButton::clicked, this, &DeviceDetailDialog::onPingClicked);
    connect(favoriteButton, &QPushButton::clicked, this, &DeviceDetailDialog::onAddToFavoritesClicked);

    QHBoxLayout* actionsLayout = new QHBoxLayout();
    actionsLayout->addWidget(pingButton);
    actionsLayout->addWidget(favoriteButton);

    layout->addRow(tr("Actions:"), actionsLayout);
}
```

### Metrics Tab with Live Updates

```cpp
void DeviceDetailDialog::setupMetricsTab() {
    QVBoxLayout* layout = new QVBoxLayout(ui->metricsTab);

    // Monitoring controls
    QHBoxLayout* controlsLayout = new QHBoxLayout();
    QPushButton* startButton = new QPushButton(tr("Start Monitoring"));
    QPushButton* stopButton = new QPushButton(tr("Stop Monitoring"));

    connect(startButton, &QPushButton::clicked, this, &DeviceDetailDialog::onStartMonitoringClicked);
    connect(stopButton, &QPushButton::clicked, this, &DeviceDetailDialog::onStopMonitoringClicked);

    controlsLayout->addWidget(startButton);
    controlsLayout->addWidget(stopButton);
    layout->addLayout(controlsLayout);

    // Metrics widget with live charts
    metricsWidget = new MetricsWidget(metricsViewModel, this);
    metricsWidget->setDevice(device);
    layout->addWidget(metricsWidget);

    // Connect to monitoring service
    connect(monitoringService, &MonitoringService::metricsCollected,
            this, &DeviceDetailDialog::onMetricsUpdated);
}
```

### Diagnostics Tab

```cpp
void DeviceDetailDialog::setupDiagnosticsTab() {
    QVBoxLayout* layout = new QVBoxLayout(ui->diagnosticsTab);

    // Traceroute section
    QGroupBox* tracerouteBox = new QGroupBox(tr("Traceroute"));
    QVBoxLayout* tracerouteLayout = new QVBoxLayout(tracerouteBox);

    QPushButton* runTracerouteButton = new QPushButton(tr("Run Traceroute"));
    connect(runTracerouteButton, &QPushButton::clicked,
            this, &DeviceDetailDialog::onRunTracerouteClicked);
    tracerouteLayout->addWidget(runTracerouteButton);

    tracerouteTable = new QTableWidget(0, 5, this);
    tracerouteTable->setHorizontalHeaderLabels({
        tr("Hop"), tr("IP Address"), tr("Hostname"), tr("RTT"), tr("Status")
    });
    tracerouteLayout->addWidget(tracerouteTable);

    layout->addWidget(tracerouteBox);

    // MTU Discovery section
    QGroupBox* mtuBox = new QGroupBox(tr("MTU Discovery"));
    QVBoxLayout* mtuLayout = new QVBoxLayout(mtuBox);

    QPushButton* discoverMtuButton = new QPushButton(tr("Discover Path MTU"));
    connect(discoverMtuButton, &QPushButton::clicked,
            this, &DeviceDetailDialog::onDiscoverMtuClicked);
    mtuLayout->addWidget(discoverMtuButton);

    QLabel* mtuResultLabel = new QLabel(tr("MTU: --"));
    mtuResultLabel->setObjectName("mtuResultLabel");
    mtuLayout->addWidget(mtuResultLabel);

    layout->addWidget(mtuBox);
}

void DeviceDetailDialog::onRunTracerouteClicked() {
    tracerouteTable->setRowCount(0);

    connect(tracerouteService, &TraceRouteService::hopDiscovered,
            this, &DeviceDetailDialog::onHopDiscovered);
    connect(tracerouteService, &TraceRouteService::traceCompleted,
            this, [this]() {
                QMessageBox::information(this, tr("Traceroute"),
                    tr("Traceroute completed successfully."));
            });

    tracerouteService->executeTraceroute(device.ipAddress);
}

void DeviceDetailDialog::onHopDiscovered(const TraceRouteHop& hop) {
    int row = tracerouteTable->rowCount();
    tracerouteTable->insertRow(row);

    tracerouteTable->setItem(row, 0, new QTableWidgetItem(QString::number(hop.hopNumber)));
    tracerouteTable->setItem(row, 1, new QTableWidgetItem(hop.ipAddress));
    tracerouteTable->setItem(row, 2, new QTableWidgetItem(hop.hostname));
    tracerouteTable->setItem(row, 3, new QTableWidgetItem(
        QString("%1 ms").arg(hop.avgRtt, 0, 'f', 2)
    ));
    tracerouteTable->setItem(row, 4, new QTableWidgetItem(
        hop.timeout ? tr("Timeout") : tr("OK")
    ));
}

void DeviceDetailDialog::onDiscoverMtuClicked() {
    connect(mtuDiscovery, &MtuDiscovery::mtuDiscovered,
            this, &DeviceDetailDialog::onMtuDiscovered);

    mtuDiscovery->discoverMtu(device.ipAddress);
}

void DeviceDetailDialog::onMtuDiscovered(int mtu) {
    QLabel* label = findChild<QLabel*>("mtuResultLabel");
    if (label) {
        label->setText(tr("MTU: %1 bytes").arg(mtu));
    }
}
```

---

## Deliverable

**Phase 7 Completion Criteria**:

### Module 7.0: MetricsWidget Integration (Phase 6 Completion) ✅
- [x] MetricsWidget integrated into MainWindow (dock widget)
- [x] "Ping Device" context menu functional
- [x] MetricsViewModel properly connected to MetricsController
- [x] Real-time monitoring working from context menu
- [x] Phase 6 placeholder messages removed
- [x] Device selection populates MetricsWidget automatically
- [x] Start/Stop monitoring controls functional
- [x] Enhanced MetricsController with currentMonitoringDevice tracking
- [x] Improved metrics collection with device context emission
- [x] PingService multi-language support (Italian, German, French, Spanish)
- [x] Enhanced error detection for timeout/unreachable states

### Module 7.1: Traceroute Service
- [ ] TraceRouteService working on Windows and Linux
- [ ] Cross-platform output parsing (tracert/traceroute)
- [ ] Real-time hop discovery with progress tracking
- [ ] TraceRouteHop model with RTT statistics
- [ ] TraceRouteServiceTest passing

### Module 7.2: Advanced Diagnostics
- [ ] MTU discovery functional with binary search
- [ ] Bandwidth testing implemented
- [ ] Advanced DNS diagnostics working
- [ ] MtuDiscoveryTest passing
- [ ] BandwidthTesterTest passing
- [ ] DnsDiagnosticsTest passing

### Module 7.3: Monitoring Service
- [ ] Continuous monitoring service running
- [ ] Alert system triggering on thresholds
- [ ] History service storing metrics to database
- [ ] Alert severity levels (Info/Warning/Critical)
- [ ] Alert acknowledgment system
- [ ] MonitoringServiceTest passing
- [ ] AlertServiceTest passing
- [ ] HistoryServiceTest passing

### Module 7.4: Device Detail Dialog
- [ ] DeviceDetailDialog with all tabs functional
- [ ] Overview tab with device information
- [ ] Ports tab with open ports display
- [ ] Metrics tab with integrated MetricsWidget
- [ ] History tab with historical data
- [ ] Diagnostics tab with traceroute and MTU discovery
- [ ] Live metrics updates in detail dialog
- [ ] Traceroute results displayed in UI table
- [ ] MTU discovery results displayed

**Overall Phase 7 Status**: 1/5 modules completed (20%) 🔄

---

## Integration Points

### With Phase 6
- Metrics from monitoring displayed in charts
- Real-time chart updates during monitoring
- Historical data loaded into charts

### With Phase 3
- HistoryService stores data in database
- Historical queries for trends
- Data pruning for old metrics

### For Phase 8
- Alert notifications will be enhanced
- History data will be exported
- Monitoring profiles will be saved

---

## Best Practices

### Platform Compatibility
- Abstract system commands (traceroute/tracert)
- Handle different output formats
- Test on Windows and Linux

### Performance
- Limit concurrent monitoring sessions
- Batch database writes
- Prune old data regularly

### Error Handling
- Graceful handling of command failures
- Timeout handling for long operations
- User notifications for errors

---

## Next Phase

After completing Phase 7, proceed to:
→ **Phase 8: Advanced Features** (Week 15-16)
