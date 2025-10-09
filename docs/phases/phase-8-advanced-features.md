# Phase 8: Advanced Features

**Timeline**: Week 15-16
**Status**: 🔄 IN PROGRESS (2/5 modules - 40%)
**Objective**: Implement advanced features (Wake-on-LAN, advanced export, profiles, favorites, settings)

---

## 8.1 Wake-on-LAN ✅ (COMPLETED 2025-10-09)

**Status**: ✅ **COMPLETED**
**Files Created**: 3 (WakeOnLanService.h/cpp, WakeOnLanServiceTest.cpp)
**Lines of Code**: 551
**Test Coverage**: 12 test cases, all passing
**Integration**: DeviceTableWidget context menu, MainWindow dependency injection

### WakeOnLanPacket.h/cpp
Magic packet builder and sender

```cpp
class WakeOnLanPacket {
public:
    WakeOnLanPacket();

    static QByteArray createMagicPacket(const QString& macAddress);
    static bool isValidMacAddress(const QString& macAddress);

private:
    static QByteArray parseMacAddress(const QString& macAddress);
};

class WakeOnLanService : public QObject {
    Q_OBJECT

public:
    WakeOnLanService(QObject* parent = nullptr);

    void sendWakeOnLan(const QString& macAddress, const QString& broadcastAddress = "255.255.255.255");
    void sendWakeOnLanToDevice(const Device& device);

signals:
    void packetSent(const QString& macAddress);
    void sendError(const QString& error);

private:
    QUdpSocket* udpSocket;
    const int WOL_PORT = 9; // Standard WoL port

    bool sendMagicPacket(const QByteArray& packet, const QString& broadcastAddress);
};
```

### Implementation

```cpp
QByteArray WakeOnLanPacket::createMagicPacket(const QString& macAddress) {
    if (!isValidMacAddress(macAddress)) {
        return QByteArray();
    }

    QByteArray mac = parseMacAddress(macAddress);

    // Magic packet structure:
    // 6 bytes of 0xFF followed by 16 repetitions of target MAC address
    QByteArray packet;

    // Add header (6 bytes of 0xFF)
    for (int i = 0; i < 6; i++) {
        packet.append(static_cast<char>(0xFF));
    }

    // Add MAC address 16 times
    for (int i = 0; i < 16; i++) {
        packet.append(mac);
    }

    return packet;
}

QByteArray WakeOnLanPacket::parseMacAddress(const QString& macAddress) {
    QByteArray mac;

    // Remove separators (: or -)
    QString cleaned = macAddress;
    cleaned.remove(':');
    cleaned.remove('-');

    // Convert hex string to bytes
    for (int i = 0; i < cleaned.length(); i += 2) {
        QString hexByte = cleaned.mid(i, 2);
        bool ok;
        quint8 byte = hexByte.toUInt(&ok, 16);
        if (ok) {
            mac.append(static_cast<char>(byte));
        }
    }

    return mac;
}

bool WakeOnLanPacket::isValidMacAddress(const QString& macAddress) {
    // MAC address regex: XX:XX:XX:XX:XX:XX or XX-XX-XX-XX-XX-XX
    QRegularExpression regex(
        "^([0-9A-Fa-f]{2}[:-]){5}([0-9A-Fa-f]{2})$"
    );
    return regex.match(macAddress).hasMatch();
}

void WakeOnLanService::sendWakeOnLan(const QString& macAddress, const QString& broadcastAddress) {
    QByteArray packet = WakeOnLanPacket::createMagicPacket(macAddress);

    if (packet.isEmpty()) {
        emit sendError(tr("Invalid MAC address: %1").arg(macAddress));
        return;
    }

    if (sendMagicPacket(packet, broadcastAddress)) {
        emit packetSent(macAddress);
        Logger::info("WoL packet sent to " + macAddress);
    } else {
        emit sendError(tr("Failed to send WoL packet to %1").arg(macAddress));
    }
}

bool WakeOnLanService::sendMagicPacket(const QByteArray& packet, const QString& broadcastAddress) {
    udpSocket = new QUdpSocket(this);

    // Enable broadcast
    udpSocket->bind(QHostAddress::Any, 0, QAbstractSocket::ShareAddress);

    qint64 bytesSent = udpSocket->writeDatagram(
        packet,
        QHostAddress(broadcastAddress),
        WOL_PORT
    );

    bool success = (bytesSent == packet.size());

    udpSocket->deleteLater();

    return success;
}
```

### UI Integration

```cpp
// Add context menu action in DeviceTableWidget
void DeviceTableWidget::setupContextMenu() {
    // ... existing actions ...

    QAction* wolAction = contextMenu->addAction(
        QIcon(":/icons/power.svg"),
        tr("Wake on LAN"),
        this,
        &DeviceTableWidget::onWakeOnLan
    );
}

void DeviceTableWidget::onWakeOnLan() {
    Device device = getSelectedDevice();

    if (device.macAddress.isEmpty()) {
        QMessageBox::warning(this, tr("Wake on LAN"),
            tr("Cannot send WoL: MAC address unknown"));
        return;
    }

    int result = QMessageBox::question(this, tr("Wake on LAN"),
        tr("Send Wake-on-LAN packet to %1 (%2)?")
            .arg(device.hostname)
            .arg(device.macAddress));

    if (result == QMessageBox::Yes) {
        wolService->sendWakeOnLanToDevice(device);

        QMessageBox::information(this, tr("Wake on LAN"),
            tr("Wake-on-LAN packet sent to %1").arg(device.macAddress));
    }
}
```

### Tests
- ✅ Test magic packet creation (12 test cases, all passing)
- ✅ Test MAC address validation
- ✅ Test packet sending

---

## 8.2 Advanced Export ✅ (COMPLETED 2025-10-09)

**Status**: ✅ **COMPLETED**
**Files Created**: 6 (XmlExporter.h/cpp, HtmlReportGenerator.h/cpp, XmlExporterTest.cpp, HtmlReportGeneratorTest.cpp)
**Lines of Code**: ~800
**Test Coverage**: 14 test cases (XmlExporter: 6, HtmlReportGenerator: 8), all passing
**Integration**: ExportController, MainWindow with all 4 formats (CSV, JSON, XML, HTML)

### XmlExporter.h/cpp
Structured XML export with QXmlStreamWriter

**Header:**
```cpp
class XmlExporter : public IExporter {
public:
    XmlExporter() = default;
    ~XmlExporter() override = default;

    bool exportData(const QList<Device>& devices, const QString& filepath) override;
    QString getFormatName() const override { return "XML"; }
    QString getFileExtension() const override { return ".xml"; }

private:
    void writeDevice(QXmlStreamWriter& writer, const Device& device);
    void writeMetrics(QXmlStreamWriter& writer, const NetworkMetrics& metrics);
    void writePorts(QXmlStreamWriter& writer, const QList<PortInfo>& ports);
    void writePort(QXmlStreamWriter& writer, const PortInfo& port);
};
```

**Key Features:**
- Structured XML hierarchy: `LanScanExport > Devices > Device`
- Device metadata as XML attributes (id)
- Detailed metrics with unit attributes (ms, %)
- Port information with protocol, service, and state
- Auto-formatted output with 2-space indentation
- Proper XML encoding (UTF-8)

**XML Output Structure:**
```xml
<?xml version="1.0" encoding="UTF-8"?>
<LanScanExport version="1.0" exportDate="2025-10-09T..." totalDevices="2">
  <Devices>
    <Device id="device-uuid">
      <IP>192.168.1.1</IP>
      <Hostname>router.local</Hostname>
      <MacAddress>AA:BB:CC:DD:EE:FF</MacAddress>
      <Vendor>Cisco</Vendor>
      <Status>Online</Status>
      <LastSeen>2025-10-09T...</LastSeen>
      <Metrics>
        <Latency unit="ms">
          <Min>4.50</Min>
          <Avg>5.20</Avg>
          <Max>6.80</Max>
          <Median>5.10</Median>
        </Latency>
        <Jitter unit="ms">
          <Value>1.20</Value>
        </Jitter>
        <PacketLoss unit="%">
          <Value>0.00</Value>
        </PacketLoss>
        <Quality>
          <Score>95</Score>
          <Rating>Excellent</Rating>
        </Quality>
      </Metrics>
      <OpenPorts count="2">
        <Port number="80">
          <Protocol>TCP</Protocol>
          <Service>HTTP</Service>
          <State>Open</State>
        </Port>
        <Port number="443">
          <Protocol>TCP</Protocol>
          <Service>HTTPS</Service>
          <State>Open</State>
        </Port>
      </OpenPorts>
    </Device>
  </Devices>
</LanScanExport>
```

### HtmlReportGenerator.h/cpp
Professional HTML reports with embedded CSS

**Header:**
```cpp
class HtmlReportGenerator : public IExporter {
public:
    HtmlReportGenerator() = default;
    ~HtmlReportGenerator() override = default;

    bool exportData(const QList<Device>& devices, const QString& filepath) override;
    QString getFormatName() const override { return "HTML"; }
    QString getFileExtension() const override { return ".html"; }

private:
    QString generateHtml(const QList<Device>& devices);
    QString generateCss();
    QString generateHeader();
    QString generateSummary(const QList<Device>& devices);
    QString generateDeviceTable(const QList<Device>& devices);
    QString generateDeviceRow(const Device& device);
    QString generateFooter();
    QString getQualityColorClass(const QString& qualityString);
    QString formatPortsList(const QList<PortInfo>& ports);
    int countOnlineDevices(const QList<Device>& devices);
};
```

**Key Features:**
- Professional HTML5 structure with DOCTYPE
- Embedded CSS for self-contained reports
- Responsive design with CSS Grid
- Modern UI with gradient cards
- Color-coded quality indicators:
  - 🟢 Excellent (green)
  - 🟢 Good (light green)
  - 🟡 Fair (orange)
  - 🟠 Poor (orange-red)
  - 🔴 Bad (red)
- Status badges (Online/Offline)
- Hover effects on table rows
- Mobile-friendly layout
- Professional typography (Segoe UI, system fonts)

**HTML Features:**
```html
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>LanScan Network Report</title>
    <style>
        /* Embedded CSS with modern styling */
        /* Gradient cards, responsive grid, quality colors */
    </style>
</head>
<body>
    <div class="container">
        <h1>🔍 LanScan Network Report</h1>
        <p class="subtitle">Generated on October 09, 2025 at 07:40 PM</p>

        <!-- Summary Cards with Gradients -->
        <div class="summary">
            <div class="summary-card">
                <h3>Total Devices</h3>
                <div class="value">42</div>
            </div>
            <div class="summary-card online">
                <h3>Online Devices</h3>
                <div class="value">38</div>
            </div>
            <div class="summary-card offline">
                <h3>Offline Devices</h3>
                <div class="value">4</div>
            </div>
        </div>

        <!-- Device Table -->
        <table>
            <thead>
                <tr>
                    <th>IP Address</th>
                    <th>Hostname</th>
                    <th>Status</th>
                    <th>Latency</th>
                    <th>Quality</th>
                    <th>Open Ports</th>
                </tr>
            </thead>
            <tbody>
                <!-- Color-coded rows with status badges -->
            </tbody>
        </table>

        <div class="footer">
            <p>Report generated by LanScan v1.0</p>
        </div>
    </div>
</body>
</html>
```

### Integration

**ExportController Enhancement:**
```cpp
void ExportController::initializeExporters() {
    exporters[CSV] = new CsvExporter();
    exporters[JSON] = new JsonExporter();
    exporters[XML] = new XmlExporter();        // ✅ NEW
    exporters[HTML] = new HtmlReportGenerator(); // ✅ NEW

    Logger::debug("Exporters initialized: CSV, JSON, XML, HTML");
}
```

**MainWindow UI Integration:**
```cpp
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
            format = ExportController::XML;      // ✅ NEW
        } else if (fileName.endsWith(".html")) {
            format = ExportController::HTML;     // ✅ NEW
        } else {
            format = ExportController::CSV;
        }

        exportController->exportDevices(format, fileName);
    }
}
```

### Tests

**XmlExporterTest.cpp** (6 test cases):
- ✅ testExportData - Basic export functionality
- ✅ testExportEmptyList - Edge case: empty device list
- ✅ testXmlStructure - Root element and attributes validation
- ✅ testDeviceElements - Device information verification
- ✅ testMetricsElements - Metrics structure with units
- ✅ testPortsElements - Ports information and count

**HtmlReportGeneratorTest.cpp** (8 test cases):
- ✅ testExportData - Basic HTML generation
- ✅ testExportEmptyList - Edge case: empty list
- ✅ testHtmlStructure - DOCTYPE, HTML5 structure, CSS
- ✅ testSummarySection - Summary cards with device counts
- ✅ testDeviceTable - Table headers and data rows
- ✅ testQualityColors - Quality color class mapping
- ✅ testOnlineOfflineStatus - Status badge generation
- ✅ testPortsFormatting - Port list truncation (5+ ports)

### Use Cases

**XML Export:**
- ✅ Enterprise integration (SOAP, REST APIs)
- ✅ Configuration management systems
- ✅ Data interchange with other network tools
- ✅ Structured data analysis
- ✅ Automated processing and parsing

**HTML Reports:**
- ✅ Executive summaries and presentations
- ✅ Email distribution (self-contained)
- ✅ Documentation and archiving
- ✅ Visual network status overview
- ✅ Printable network reports
- ✅ Mobile viewing (responsive design)

---

## 8.3 Profile & Favorites

### ScanProfile.h/cpp
Scan profile model (already defined in Phase 4)

Enhanced with additional features:

```cpp
class ProfileManager {
    // ... existing methods ...

public:
    // Export/Import profiles
    bool exportProfile(const ScanProfile& profile, const QString& filepath);
    ScanProfile importProfile(const QString& filepath);

    // Profile templates
    static ScanProfile createHomeNetworkProfile();
    static ScanProfile createEnterpriseNetworkProfile();
    static ScanProfile createSecurityAuditProfile();

    // Profile statistics
    QDateTime getLastUsed(const QString& profileId);
    int getUsageCount(const QString& profileId);

private:
    QMap<QString, int> usageCounts;
    QMap<QString, QDateTime> lastUsedTimes;

    void updateUsageStats(const QString& profileId);
};
```

### Profile UI

```cpp
class ProfileDialog : public QDialog {
    Q_OBJECT

public:
    ProfileDialog(ProfileManager* profileManager, QWidget* parent = nullptr);

    ScanProfile getSelectedProfile() const;

private slots:
    void onNewProfile();
    void onEditProfile();
    void onDeleteProfile();
    void onImportProfile();
    void onExportProfile();
    void onProfileSelected(const QModelIndex& index);

private:
    Ui::ProfileDialog* ui;
    ProfileManager* profileManager;

    QListWidget* profileList;
    QPushButton* newButton;
    QPushButton* editButton;
    QPushButton* deleteButton;
    QPushButton* importButton;
    QPushButton* exportButton;

    void setupUI();
    void loadProfiles();
    void displayProfileDetails(const ScanProfile& profile);
};
```

### FavoritesManager Enhancement

```cpp
class FavoritesManager {
    // ... existing methods ...

public:
    // Groups
    void createGroup(const QString& groupName);
    void deleteGroup(const QString& groupName);
    QList<QString> getGroups();
    void addToGroup(const QString& deviceId, const QString& groupName);
    void removeFromGroup(const QString& deviceId, const QString& groupName);
    QList<FavoriteDevice> getDevicesInGroup(const QString& groupName);

    // Notes
    void addNote(const QString& deviceId, const QString& note);
    QList<QString> getNotes(const QString& deviceId);

    // Custom icons
    void setCustomIcon(const QString& deviceId, const QString& iconPath);
    QString getCustomIcon(const QString& deviceId);

private:
    QMap<QString, QList<QString>> deviceGroups; // group -> [device IDs]
    QMap<QString, QList<QString>> deviceNotes;  // device ID -> [notes]
    QMap<QString, QString> customIcons;         // device ID -> icon path
};
```

### Favorites UI Integration

```cpp
class FavoritesWidget : public QWidget {
    Q_OBJECT

public:
    FavoritesWidget(FavoritesManager* manager, QWidget* parent = nullptr);

signals:
    void favoriteSelected(const FavoriteDevice& favorite);

private slots:
    void onAddFavorite();
    void onRemoveFavorite();
    void onEditFavorite();
    void onQuickConnect();
    void onGroupSelected(const QString& group);

private:
    FavoritesManager* manager;

    QTreeWidget* favoritesTree;
    QLineEdit* searchBox;
    QComboBox* groupFilter;

    void setupUI();
    void loadFavorites();
    void filterFavorites(const QString& searchText);
    void organizeFavoritesByGroup();
};
```

---

## 8.4 History & Database

### HistoryDao.h/cpp
Data Access Object for history events

```cpp
class HistoryEvent {
public:
    QString id;
    QString deviceId;
    QString eventType;    // "scan", "status_change", "alert", "user_action"
    QString description;
    QJsonObject metadata; // Additional data
    QDateTime timestamp;
};

class HistoryDao {
public:
    HistoryDao(DatabaseManager* dbManager);

    void insert(const HistoryEvent& event);
    void insertBatch(const QList<HistoryEvent>& events);

    QList<HistoryEvent> findByDevice(const QString& deviceId);
    QList<HistoryEvent> findByType(const QString& eventType);
    QList<HistoryEvent> findByDateRange(const QDateTime& start, const QDateTime& end);
    QList<HistoryEvent> findByDeviceAndDateRange(const QString& deviceId,
                                                 const QDateTime& start,
                                                 const QDateTime& end);

    void deleteOlderThan(const QDateTime& cutoffDate);
    void deleteByDevice(const QString& deviceId);

private:
    DatabaseManager* dbManager;

    void createTable();
    HistoryEvent eventFromQuery(const QSqlQuery& query);
};
```

### MetricsDao.h/cpp
Data Access Object for historical metrics

```cpp
class MetricsDao {
public:
    MetricsDao(DatabaseManager* dbManager);

    void insert(const QString& deviceId, const NetworkMetrics& metrics);
    void insertBatch(const QString& deviceId, const QList<NetworkMetrics>& metricsList);

    QList<NetworkMetrics> findByDevice(const QString& deviceId, int limit = 100);
    QList<NetworkMetrics> findByDateRange(const QString& deviceId,
                                         const QDateTime& start,
                                         const QDateTime& end);

    // Aggregations
    NetworkMetrics getAverageMetrics(const QString& deviceId, const QDateTime& start,
                                    const QDateTime& end);
    double getMaxLatency(const QString& deviceId, const QDateTime& start,
                        const QDateTime& end);
    double getAveragePacketLoss(const QString& deviceId, const QDateTime& start,
                               const QDateTime& end);

    void deleteOlderThan(const QDateTime& cutoffDate);

private:
    DatabaseManager* dbManager;

    void createTable();
    NetworkMetrics metricsFromQuery(const QSqlQuery& query);
};
```

### Temporal Trends Visualization

```cpp
class TrendsWidget : public QWidget {
    Q_OBJECT

public:
    TrendsWidget(MetricsDao* metricsDao, QWidget* parent = nullptr);

    void setDevice(const QString& deviceId);
    void setDateRange(const QDateTime& start, const QDateTime& end);

private slots:
    void onLoadTrends();
    void onTimeRangeChanged(int index);

private:
    MetricsDao* metricsDao;
    QString currentDeviceId;
    QDateTime startDate;
    QDateTime endDate;

    LatencyChart* trendsChart;
    QComboBox* timeRangeCombo;
    QDateTimeEdit* startDateEdit;
    QDateTimeEdit* endDateEdit;

    void setupUI();
    void loadHistoricalData();
    void displayTrends(const QList<NetworkMetrics>& metrics);
};
```

### Tests
- [ ] HistoryDaoTest
- [ ] MetricsDaoTest
- [ ] Test batch inserts
- [ ] Test date range queries

---

## 8.5 Settings Dialog

### settingsdialog.ui
Settings dialog layout

```xml
<?xml version="1.0" encoding="UTF-8"?>
<ui version="4.0">
 <widget class="QDialog" name="SettingsDialog">
  <property name="windowTitle">
   <string>Settings</string>
  </property>
  <layout class="QVBoxLayout">
   <widget class="QTabWidget" name="tabWidget">
    <!-- General Tab -->
    <widget class="QWidget" name="generalTab"/>

    <!-- Network Tab -->
    <widget class="QWidget" name="networkTab"/>

    <!-- Appearance Tab -->
    <widget class="QWidget" name="appearanceTab"/>

    <!-- Notifications Tab -->
    <widget class="QWidget" name="notificationsTab"/>

    <!-- Advanced Tab -->
    <widget class="QWidget" name="advancedTab"/>
   </widget>

   <widget class="QDialogButtonBox" name="buttonBox"/>
  </layout>
 </widget>
</ui>
```

### SettingsDialog.h/cpp
Settings dialog implementation

```cpp
class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    SettingsDialog(QSettings* settings, QWidget* parent = nullptr);

private slots:
    void onApply();
    void onOk();
    void onCancel();
    void onRestoreDefaults();

    // General
    void onStartWithSystemChanged(bool checked);
    void onMinimizeToTrayChanged(bool checked);

    // Network
    void onDefaultTimeoutChanged(int value);
    void onMaxThreadsChanged(int value);
    void onDefaultSubnetChanged(const QString& subnet);

    // Appearance
    void onThemeChanged(int index);
    void onFontSizeChanged(int value);

    // Notifications
    void onEnableAlertsChanged(bool checked);
    void onAlertSoundChanged(bool checked);

private:
    Ui::SettingsDialog* ui;
    QSettings* settings;

    void setupGeneralTab();
    void setupNetworkTab();
    void setupAppearanceTab();
    void setupNotificationsTab();
    void setupAdvancedTab();

    void loadSettings();
    void saveSettings();
    void applySettings();
};
```

### Settings Implementation

```cpp
void SettingsDialog::setupGeneralTab() {
    QVBoxLayout* layout = new QVBoxLayout(ui->generalTab);

    // Startup
    QGroupBox* startupGroup = new QGroupBox(tr("Startup"));
    QVBoxLayout* startupLayout = new QVBoxLayout(startupGroup);

    QCheckBox* startWithSystem = new QCheckBox(tr("Start with system"));
    connect(startWithSystem, &QCheckBox::toggled,
            this, &SettingsDialog::onStartWithSystemChanged);
    startupLayout->addWidget(startWithSystem);

    QCheckBox* minimizeToTray = new QCheckBox(tr("Minimize to system tray"));
    connect(minimizeToTray, &QCheckBox::toggled,
            this, &SettingsDialog::onMinimizeToTrayChanged);
    startupLayout->addWidget(minimizeToTray);

    layout->addWidget(startupGroup);

    // Language
    QGroupBox* languageGroup = new QGroupBox(tr("Language"));
    QVBoxLayout* languageLayout = new QVBoxLayout(languageGroup);

    QComboBox* languageCombo = new QComboBox();
    languageCombo->addItem(tr("English"), "en");
    languageCombo->addItem(tr("Italiano"), "it");
    languageLayout->addWidget(languageCombo);

    layout->addWidget(languageGroup);

    layout->addStretch();
}

void SettingsDialog::setupNetworkTab() {
    QFormLayout* layout = new QFormLayout(ui->networkTab);

    // Scan defaults
    QGroupBox* scanGroup = new QGroupBox(tr("Scan Defaults"));
    QFormLayout* scanLayout = new QFormLayout(scanGroup);

    QSpinBox* timeoutSpin = new QSpinBox();
    timeoutSpin->setRange(100, 10000);
    timeoutSpin->setSuffix(" ms");
    scanLayout->addRow(tr("Timeout:"), timeoutSpin);

    QSpinBox* threadsSpin = new QSpinBox();
    threadsSpin->setRange(1, 64);
    threadsSpin->setValue(QThread::idealThreadCount());
    scanLayout->addRow(tr("Max Threads:"), threadsSpin);

    QLineEdit* subnetEdit = new QLineEdit();
    subnetEdit->setPlaceholderText("192.168.1.0/24");
    scanLayout->addRow(tr("Default Subnet:"), subnetEdit);

    layout->addRow(scanGroup);

    // Network interface
    QGroupBox* interfaceGroup = new QGroupBox(tr("Network Interface"));
    QVBoxLayout* interfaceLayout = new QVBoxLayout(interfaceGroup);

    QComboBox* interfaceCombo = new QComboBox();
    // Populate with available network interfaces
    interfaceLayout->addWidget(interfaceCombo);

    layout->addRow(interfaceGroup);
}

void SettingsDialog::setupAppearanceTab() {
    QVBoxLayout* layout = new QVBoxLayout(ui->appearanceTab);

    // Theme
    QGroupBox* themeGroup = new QGroupBox(tr("Theme"));
    QVBoxLayout* themeLayout = new QVBoxLayout(themeGroup);

    QComboBox* themeCombo = new QComboBox();
    themeCombo->addItem(tr("Light"), "light");
    themeCombo->addItem(tr("Dark"), "dark");
    themeCombo->addItem(tr("System"), "system");
    connect(themeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SettingsDialog::onThemeChanged);
    themeLayout->addWidget(themeCombo);

    layout->addWidget(themeGroup);

    // Font
    QGroupBox* fontGroup = new QGroupBox(tr("Font"));
    QFormLayout* fontLayout = new QFormLayout(fontGroup);

    QSpinBox* fontSizeSpin = new QSpinBox();
    fontSizeSpin->setRange(8, 24);
    fontSizeSpin->setValue(10);
    fontLayout->addRow(tr("Font Size:"), fontSizeSpin);

    layout->addWidget(fontGroup);

    layout->addStretch();
}

void SettingsDialog::loadSettings() {
    // General
    ui->startWithSystemCheck->setChecked(
        settings->value("General/StartWithSystem", false).toBool()
    );
    ui->minimizeToTrayCheck->setChecked(
        settings->value("General/MinimizeToTray", true).toBool()
    );

    // Network
    ui->timeoutSpin->setValue(
        settings->value("Network/Timeout", 1000).toInt()
    );
    ui->maxThreadsSpin->setValue(
        settings->value("Network/MaxThreads", QThread::idealThreadCount()).toInt()
    );
    ui->defaultSubnetEdit->setText(
        settings->value("Network/DefaultSubnet", "192.168.1.0/24").toString()
    );

    // Appearance
    QString theme = settings->value("Appearance/Theme", "system").toString();
    int themeIndex = ui->themeCombo->findData(theme);
    ui->themeCombo->setCurrentIndex(themeIndex);

    ui->fontSizeSpin->setValue(
        settings->value("Appearance/FontSize", 10).toInt()
    );

    // Notifications
    ui->enableAlertsCheck->setChecked(
        settings->value("Notifications/EnableAlerts", true).toBool()
    );
    ui->alertSoundCheck->setChecked(
        settings->value("Notifications/AlertSound", false).toBool()
    );
}

void SettingsDialog::saveSettings() {
    // General
    settings->setValue("General/StartWithSystem",
        ui->startWithSystemCheck->isChecked());
    settings->setValue("General/MinimizeToTray",
        ui->minimizeToTrayCheck->isChecked());

    // Network
    settings->setValue("Network/Timeout",
        ui->timeoutSpin->value());
    settings->setValue("Network/MaxThreads",
        ui->maxThreadsSpin->value());
    settings->setValue("Network/DefaultSubnet",
        ui->defaultSubnetEdit->text());

    // Appearance
    settings->setValue("Appearance/Theme",
        ui->themeCombo->currentData().toString());
    settings->setValue("Appearance/FontSize",
        ui->fontSizeSpin->value());

    // Notifications
    settings->setValue("Notifications/EnableAlerts",
        ui->enableAlertsCheck->isChecked());
    settings->setValue("Notifications/AlertSound",
        ui->alertSoundCheck->isChecked());

    settings->sync();
}
```

---

## Deliverable

**Phase 8 Completion Criteria**:
- ✅ Wake-on-LAN functional (COMPLETED 2025-10-09)
- ✅ XML export working (COMPLETED 2025-10-09)
- ✅ HTML report generator producing formatted reports (COMPLETED 2025-10-09)
- ⏳ Profile management with save/load/import/export (PENDING)
- ⏳ Favorites system with groups and notes (PENDING)
- ⏳ History DAO storing events to database (PENDING)
- ⏳ Metrics DAO storing metrics with temporal queries (PENDING)
- ⏳ Trends visualization showing historical data (PENDING)
- ⏳ Settings dialog with all tabs functional (PENDING)
- ⏳ Settings persistence working (PENDING)
- ✅ All tests passing for completed modules (26/26 tests)

---

## Integration Points

### With All Previous Phases
- Uses all models, services, controllers
- Enhances UI with advanced features
- Extends database with history tables

### For Phase 9
- Settings will control theme and appearance
- Custom icons for favorites
- UI polish will enhance all dialogs

---

## Best Practices

### Data Persistence
- Use QSettings for application settings
- Use JSON for profiles and favorites
- Use SQLite for historical data

### User Experience
- Confirmation dialogs for destructive actions
- Progress indicators for long operations
- Tooltips for all settings

### Performance
- Batch database operations
- Cache frequently accessed settings
- Lazy load historical data

---

## Next Phase

After completing Phase 8, proceed to:
→ **Phase 9: UI Polish & Theming** (Week 17)
