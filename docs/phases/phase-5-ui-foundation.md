# Phase 5: UI Foundation & Basic Views ✅

**Timeline**: Week 10-11 ✅ **COMPLETED**
**Objective**: Functional base UI with Qt Designer, MVVM pattern implementation
**Status**: 100% Complete (5/5 modules completed)
**Completion Date**: 2025-10-04
**Git Tag**: v0.5.0-phase5

---

## 5.1 Qt Designer Files (.ui)

### UI Files to Create

#### mainwindow.ui
Main application window layout
```xml
<?xml version="1.0" encoding="UTF-8"?>
<ui version="4.0">
 <class>MainWindow</class>
 <widget class="QMainWindow" name="MainWindow">
  <property name="windowTitle">
   <string>LanScan - Network Scanner</string>
  </property>
  <widget class="QWidget" name="centralWidget">
   <layout class="QVBoxLayout">
    <!-- Device table widget -->
    <!-- Metrics panel -->
    <!-- Status bar -->
   </layout>
  </widget>
  <widget class="QMenuBar" name="menuBar"/>
  <widget class="QToolBar" name="toolBar"/>
  <widget class="QStatusBar" name="statusBar"/>
 </widget>
</ui>
```

#### scanconfigdialog.ui
Scan configuration dialog
```xml
<!-- Fields for:
- IP range/CIDR input
- Scan type (Quick/Deep/Custom)
- Thread count
- Timeout settings
- Port scan options
- DNS/ARP resolution toggles
-->
```

#### devicetablewidget.ui
Device table widget layout
```xml
<!-- QTableView with columns:
- Status (online/offline)
- IP Address
- Hostname
- MAC Address
- Vendor
- Open Ports
- Latency
- Quality Score
-->
```

### Design Guidelines
- Use placeholder widgets for charts (will be implemented in Phase 6)
- Follow Material Design principles
- Responsive layouts with QSplitter
- Consistent spacing and margins

### Tasks
- [x] Create mainwindow.ui ✅
- [x] Create scanconfigdialog.ui ✅
- [x] Create devicetablewidget.ui ✅
- [x] Setup Qt Designer custom widgets ✅

---

## 5.2 ViewModels (Presentation Logic)

### DeviceTableViewModel.h/cpp
ViewModel for device table (QAbstractTableModel)

```cpp
class DeviceTableViewModel : public QAbstractTableModel {
    Q_OBJECT

public:
    enum Column {
        Status = 0,
        IpAddress,
        Hostname,
        MacAddress,
        Vendor,
        OpenPorts,
        Latency,
        QualityScore,
        ColumnCount
    };

    DeviceTableViewModel(DeviceRepository* repository, QObject* parent = nullptr);

    // QAbstractTableModel interface
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    // Custom methods
    void loadDevices();
    void addDevice(const Device& device);
    void updateDevice(const Device& device);
    void removeDevice(const QString& ip);
    void clear();

    Device getDeviceAt(int row) const;

signals:
    void deviceCountChanged(int count);

private:
    DeviceRepository* repository;
    QList<Device> devices;

    QString getStatusIcon(bool isOnline) const;
    QColor getQualityColor(const QString& quality) const;
};
```

### Custom Delegates

#### StatusDelegate.h/cpp
Custom delegate for status column

```cpp
class StatusDelegate : public QStyledItemDelegate {
    Q_OBJECT

public:
    StatusDelegate(QObject* parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionViewItem& option,
               const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem& option,
                   const QModelIndex& index) const override;

private:
    void paintStatusLed(QPainter* painter, const QRect& rect, bool isOnline) const;
};
```

#### QualityScoreDelegate.h/cpp
Custom delegate for quality score column

```cpp
class QualityScoreDelegate : public QStyledItemDelegate {
    Q_OBJECT

public:
    QualityScoreDelegate(QObject* parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionViewItem& option,
               const QModelIndex& index) const override;

private:
    void paintQualityBar(QPainter* painter, const QRect& rect,
                        const QString& quality) const;
    QColor getColorForQuality(const QString& quality) const;
};
```

### Implementation Status
- [x] DeviceTableViewModel.h/cpp implemented ✅
- [x] StatusDelegate.h/cpp implemented ✅
- [x] QualityScoreDelegate.h/cpp implemented ✅
- [x] Custom rendering with QPainter ✅
- [x] LED indicator for status column ✅
- [x] Color-coded progress bar for quality score ✅

### Tests
- [ ] DeviceTableViewModelTest (deferred to Phase 10)
- [ ] Test model updates with mock repository (deferred to Phase 10)

---

## 5.3 Main Window & Basic UI

### MainWindow.h/cpp
Main application window (view)

```cpp
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(
        ScanController* scanController,
        MetricsController* metricsController,
        ExportController* exportController,
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

    // Controller signals
    void onScanStarted(int totalHosts);
    void onDeviceDiscovered(const Device& device);
    void onScanProgress(int current, int total, const QString& currentIp);
    void onScanCompleted(int devicesFound, qint64 durationMs);
    void onScanError(const QString& error);

    // UI events
    void onDeviceSelectionChanged(const QModelIndex& index);
    void onDeviceDoubleClicked(const QModelIndex& index);

private:
    Ui::MainWindow* ui;

    // Controllers
    ScanController* scanController;
    MetricsController* metricsController;
    ExportController* exportController;

    // Widgets
    DeviceTableWidget* deviceTable;
    QProgressBar* progressBar;
    QLabel* statusLabel;

    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
    void setupConnections();
    void updateStatusMessage(const QString& message);
};
```

### Menu Structure

```cpp
void MainWindow::setupMenuBar() {
    // File Menu
    QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(tr("&New Scan..."), this, &MainWindow::onNewScanTriggered, QKeySequence::New);
    fileMenu->addAction(tr("&Export..."), this, &MainWindow::onExportTriggered, QKeySequence::Save);
    fileMenu->addSeparator();
    fileMenu->addAction(tr("&Settings"), this, &MainWindow::onSettingsTriggered, QKeySequence::Preferences);
    fileMenu->addSeparator();
    fileMenu->addAction(tr("E&xit"), this, &QWidget::close, QKeySequence::Quit);

    // Scan Menu
    QMenu* scanMenu = menuBar()->addMenu(tr("&Scan"));
    scanMenu->addAction(tr("&Quick Scan"), this, &MainWindow::onQuickScan);
    scanMenu->addAction(tr("&Deep Scan"), this, &MainWindow::onDeepScan);
    scanMenu->addAction(tr("&Stop Scan"), this, &MainWindow::onStopScanTriggered);

    // View Menu
    QMenu* viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(tr("&Refresh"), this, &MainWindow::onRefresh, QKeySequence::Refresh);
    viewMenu->addAction(tr("&Clear Results"), this, &MainWindow::onClearResults);

    // Help Menu
    QMenu* helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(tr("&Documentation"), this, &MainWindow::onDocumentation, QKeySequence::HelpContents);
    helpMenu->addAction(tr("&About"), this, &MainWindow::onAboutTriggered);
}
```

### Toolbar Setup

```cpp
void MainWindow::setupToolBar() {
    QToolBar* toolbar = addToolBar(tr("Main Toolbar"));
    toolbar->setIconSize(QSize(24, 24));

    // Scan actions
    QAction* newScanAction = toolbar->addAction(QIcon(":/icons/scan.svg"), tr("New Scan"));
    connect(newScanAction, &QAction::triggered, this, &MainWindow::onNewScanTriggered);

    QAction* stopScanAction = toolbar->addAction(QIcon(":/icons/stop.svg"), tr("Stop Scan"));
    connect(stopScanAction, &QAction::triggered, this, &MainWindow::onStopScanTriggered);
    stopScanAction->setEnabled(false);

    toolbar->addSeparator();

    // Export action
    QAction* exportAction = toolbar->addAction(QIcon(":/icons/export.svg"), tr("Export"));
    connect(exportAction, &QAction::triggered, this, &MainWindow::onExportTriggered);

    toolbar->addSeparator();

    // Settings action
    QAction* settingsAction = toolbar->addAction(QIcon(":/icons/settings.svg"), tr("Settings"));
    connect(settingsAction, &QAction::triggered, this, &MainWindow::onSettingsTriggered);
}
```

### Implementation Status
- [x] MainWindow.h/cpp implemented ✅
- [x] Menu bar with File/Scan/View/Help menus ✅
- [x] Toolbar with scan/export/settings actions ✅
- [x] Status bar with progress tracking ✅
- [x] Signal/slot connections to controllers ✅
- [x] Dependency injection in constructor ✅

### Tests
- [ ] MainWindowTest (deferred to Phase 10)

---

## 5.4 Device Table Widget

### DeviceTableWidget.h/cpp
Device table widget (view)

```cpp
class DeviceTableWidget : public QWidget {
    Q_OBJECT

public:
    DeviceTableWidget(DeviceTableViewModel* viewModel, QWidget* parent = nullptr);

    void setViewModel(DeviceTableViewModel* viewModel);
    Device getSelectedDevice() const;
    QList<Device> getSelectedDevices() const;

signals:
    void deviceSelected(const Device& device);
    void deviceDoubleClicked(const Device& device);
    void contextMenuRequested(const Device& device, const QPoint& pos);

private slots:
    void onSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
    void onDoubleClicked(const QModelIndex& index);
    void onCustomContextMenuRequested(const QPoint& pos);

    // Context menu actions
    void onPingDevice();
    void onShowDetails();
    void onAddToFavorites();
    void onRemoveDevice();
    void onCopyIpAddress();

private:
    Ui::DeviceTableWidget* ui;
    QTableView* tableView;
    DeviceTableViewModel* viewModel;

    QMenu* contextMenu;

    void setupTableView();
    void setupContextMenu();
    void setupConnections();
    void setupSortingAndFiltering();
};
```

### Table View Configuration

```cpp
void DeviceTableWidget::setupTableView() {
    tableView = new QTableView(this);
    tableView->setModel(viewModel);

    // Set custom delegates
    tableView->setItemDelegateForColumn(DeviceTableViewModel::Status,
                                       new StatusDelegate(this));
    tableView->setItemDelegateForColumn(DeviceTableViewModel::QualityScore,
                                       new QualityScoreDelegate(this));

    // Configure view
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    tableView->setAlternatingRowColors(true);
    tableView->setSortingEnabled(true);
    tableView->setContextMenuPolicy(Qt::CustomContextMenu);

    // Column sizing
    tableView->horizontalHeader()->setStretchLastSection(true);
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    tableView->resizeColumnsToContents();

    // Row height
    tableView->verticalHeader()->setDefaultSectionSize(30);
    tableView->verticalHeader()->setVisible(false);
}
```

### Context Menu

```cpp
void DeviceTableWidget::setupContextMenu() {
    contextMenu = new QMenu(this);

    contextMenu->addAction(QIcon(":/icons/ping.svg"), tr("Ping Device"),
                          this, &DeviceTableWidget::onPingDevice);
    contextMenu->addAction(QIcon(":/icons/details.svg"), tr("Show Details"),
                          this, &DeviceTableWidget::onShowDetails);
    contextMenu->addSeparator();
    contextMenu->addAction(QIcon(":/icons/favorite.svg"), tr("Add to Favorites"),
                          this, &DeviceTableWidget::onAddToFavorites);
    contextMenu->addSeparator();
    contextMenu->addAction(QIcon(":/icons/copy.svg"), tr("Copy IP Address"),
                          this, &DeviceTableWidget::onCopyIpAddress);
    contextMenu->addSeparator();
    contextMenu->addAction(QIcon(":/icons/delete.svg"), tr("Remove Device"),
                          this, &DeviceTableWidget::onRemoveDevice);
}
```

### Sorting and Filtering

```cpp
void DeviceTableWidget::setupSortingAndFiltering() {
    // Sort proxy model for sorting/filtering
    QSortFilterProxyModel* proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(viewModel);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    proxyModel->setFilterKeyColumn(-1); // Search all columns

    tableView->setModel(proxyModel);

    // Add search box
    QLineEdit* searchBox = new QLineEdit(this);
    searchBox->setPlaceholderText(tr("Search devices..."));
    connect(searchBox, &QLineEdit::textChanged,
            proxyModel, &QSortFilterProxyModel::setFilterFixedString);
}
```

### Implementation Status
- [x] DeviceTableWidget.h/cpp implemented ✅
- [x] QTableView with custom delegates ✅
- [x] QSortFilterProxyModel for sorting/filtering ✅
- [x] Context menu with actions (ping, details, favorites, copy IP, remove) ✅
- [x] Search box integration ✅
- [x] Signal/slot connections ✅

### Tests
- [ ] DeviceTableWidgetTest (deferred to Phase 10)

---

## 5.5 Scan Config Dialog

### ScanConfigDialog.h/cpp
Scan configuration dialog (view)

```cpp
class ScanConfigDialog : public QDialog {
    Q_OBJECT

public:
    ScanConfigDialog(ScanConfigViewModel* viewModel, QWidget* parent = nullptr);

    ScanCoordinator::ScanConfig getConfiguration() const;

private slots:
    void onScanTypeChanged(int index);
    void onSubnetChanged(const QString& subnet);
    void onValidate();
    void onAccepted();
    void onDetectNetwork();

private:
    Ui::ScanConfigDialog* ui;
    ScanConfigViewModel* viewModel;

    void setupValidators();
    void setupConnections();
    void updateUiForScanType(const QString& scanType);
    bool validateInputs();
};
```

### ScanConfigViewModel.h/cpp
ViewModel for scan configuration

```cpp
class ScanConfigViewModel : public QObject {
    Q_OBJECT

public:
    enum ScanType {
        Quick,
        Deep,
        Custom
    };

    ScanConfigViewModel(QObject* parent = nullptr);

    // Getters
    QString getSubnet() const;
    ScanType getScanType() const;
    int getThreadCount() const;
    int getTimeout() const;
    bool isResolveDns() const;
    bool isResolveArp() const;
    bool isScanPorts() const;
    QList<int> getPortsToScan() const;

    // Setters
    void setSubnet(const QString& subnet);
    void setScanType(ScanType type);
    void setThreadCount(int count);
    void setTimeout(int timeout);
    void setResolveDns(bool resolve);
    void setResolveArp(bool resolve);
    void setScanPorts(bool scan);
    void setPortsToScan(const QList<int>& ports);

    // Validation
    bool isSubnetValid() const;
    QString getSubnetError() const;

    // Presets
    void loadQuickScanPreset();
    void loadDeepScanPreset();
    void loadCustomScanPreset();

    // Network detection
    QStringList detectLocalNetworks();

signals:
    void subnetChanged(const QString& subnet);
    void scanTypeChanged(ScanType type);
    void validationChanged(bool isValid);

private:
    QString subnet;
    ScanType scanType;
    int threadCount;
    int timeout;
    bool resolveDns;
    bool resolveArp;
    bool scanPorts;
    QList<int> portsToScan;

    IpAddressValidator validator;
};
```

### Dialog Layout

```cpp
// scanconfigdialog.ui structure:
// - Scan Type combo box (Quick/Deep/Custom)
// - Subnet input (QLineEdit with validator)
// - Detect Network button
// - Thread count spin box
// - Timeout spin box
// - DNS resolution checkbox
// - ARP resolution checkbox
// - Port scan checkbox
// - Ports to scan input (comma-separated)
// - OK/Cancel buttons
```

### Input Validation

```cpp
void ScanConfigDialog::setupValidators() {
    // Subnet validator
    QRegularExpression subnetRegex(
        "^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}"
        "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)/(3[0-2]|[12]?[0-9])$"
    );
    QRegularExpressionValidator* subnetValidator =
        new QRegularExpressionValidator(subnetRegex, this);
    ui->subnetLineEdit->setValidator(subnetValidator);

    // Thread count (1-64)
    ui->threadSpinBox->setRange(1, 64);
    ui->threadSpinBox->setValue(QThread::idealThreadCount());

    // Timeout (100ms - 10000ms)
    ui->timeoutSpinBox->setRange(100, 10000);
    ui->timeoutSpinBox->setValue(1000);
    ui->timeoutSpinBox->setSuffix(" ms");
}
```

### Network Detection

```cpp
void ScanConfigDialog::onDetectNetwork() {
    QStringList networks = viewModel->detectLocalNetworks();

    if (networks.isEmpty()) {
        QMessageBox::warning(this, tr("Network Detection"),
                            tr("No active network interfaces found."));
        return;
    }

    // Show selection dialog if multiple networks
    if (networks.size() == 1) {
        ui->subnetLineEdit->setText(networks.first());
    } else {
        bool ok;
        QString selected = QInputDialog::getItem(this,
            tr("Select Network"),
            tr("Multiple networks detected. Select one:"),
            networks, 0, false, &ok);

        if (ok && !selected.isEmpty()) {
            ui->subnetLineEdit->setText(selected);
        }
    }
}
```

### Implementation Status
- [x] ScanConfigDialog.h/cpp implemented ✅
- [x] ScanConfigViewModel.h/cpp implemented ✅
- [x] Input validation with regex validators ✅
- [x] Scan type presets (Quick/Deep/Custom) ✅
- [x] Network detection functionality ✅
- [x] toScanConfig() conversion method ✅
- [x] Signal/slot connections for live validation ✅

### Tests
- [ ] ScanConfigViewModelTest (deferred to Phase 10)
- [ ] Test preset configurations (deferred to Phase 10)

---

## Implementation Details

### MVVM Pattern

```
View (UI)
   ↓ (bindings/signals)
ViewModel (Presentation Logic)
   ↓ (data operations)
Model/Repository (Data)
   ↓
Database/Network
```

### Signal/Slot Connections

```cpp
void MainWindow::setupConnections() {
    // ScanController signals
    connect(scanController, &ScanController::scanStarted,
            this, &MainWindow::onScanStarted);
    connect(scanController, &ScanController::deviceDiscovered,
            this, &MainWindow::onDeviceDiscovered);
    connect(scanController, &ScanController::scanProgress,
            this, &MainWindow::onScanProgress);
    connect(scanController, &ScanController::scanCompleted,
            this, &MainWindow::onScanCompleted);
    connect(scanController, &ScanController::scanError,
            this, &MainWindow::onScanError);

    // DeviceTable signals
    connect(deviceTable, &DeviceTableWidget::deviceDoubleClicked,
            this, &MainWindow::onDeviceDoubleClicked);
    connect(deviceTable, &DeviceTableWidget::contextMenuRequested,
            this, &MainWindow::onContextMenuRequested);
}
```

### UI State Management

```cpp
void MainWindow::onScanStarted(int totalHosts) {
    // Disable scan button
    ui->actionNewScan->setEnabled(false);
    ui->actionStopScan->setEnabled(true);

    // Show progress bar
    progressBar->setRange(0, totalHosts);
    progressBar->setValue(0);
    progressBar->setVisible(true);

    // Update status
    updateStatusMessage(tr("Scanning %1 hosts...").arg(totalHosts));
}

void MainWindow::onScanCompleted(int devicesFound, qint64 durationMs) {
    // Enable scan button
    ui->actionNewScan->setEnabled(true);
    ui->actionStopScan->setEnabled(false);

    // Hide progress bar
    progressBar->setVisible(false);

    // Update status
    QString message = tr("Scan completed: %1 devices found in %2ms")
                        .arg(devicesFound)
                        .arg(durationMs);
    updateStatusMessage(message);

    // Show notification
    QMessageBox::information(this, tr("Scan Complete"), message);
}
```

---

## Deliverable

**Phase 5 Completion Criteria**: ✅ **ALL COMPLETED**
- ✅ Base UI functional with Qt Designer layouts
- ✅ MainWindow with menu bar, toolbar, status bar
- ✅ Device table displaying discovered devices
- ✅ Scan configuration dialog working
- ✅ MVVM pattern implemented correctly
- ✅ Custom delegates for status and quality columns
- ✅ Context menu with basic actions
- ✅ Sorting and filtering functional
- ✅ Scan can be started from GUI
- ✅ Device table populated with scan results
- ✅ Progress tracking displayed in UI

**Implementation Summary**:
- **19 new files created** (7 headers, 7 cpp files, 3 UI files, 2 modified files)
- **ViewModels**: DeviceTableViewModel, ScanConfigViewModel
- **Delegates**: StatusDelegate (LED indicator), QualityScoreDelegate (progress bar)
- **Views**: MainWindow, DeviceTableWidget, ScanConfigDialog
- **UI Files**: mainwindow.ui, scanconfigdialog.ui, devicetablewidget.ui
- **Build**: Successful compilation, LanScan.exe (34 MB)
- **Git**: Committed as v0.5.0-phase5
- **Documentation**: TODO.md updated with 50% project completion

---

## Integration Points

### With Phase 0-4
- Uses Device, NetworkMetrics models
- Uses ScanController, MetricsController
- Uses DeviceRepository for data access
- Uses IpAddressValidator for input validation

### For Phase 6
- UI structure ready for charts integration
- ViewModels will provide data for charts
- Placeholder widgets will be replaced with QtCharts

---

## Best Practices

### Qt Designer Workflow
- Keep .ui files simple and clean
- Use layout managers (not fixed positions)
- Name widgets descriptively (deviceTableView, not tableView1)
- Use Qt property system for custom properties

### MVVM Guidelines
- ViewModels contain NO Qt Widgets code
- Views contain NO business logic
- Use signals/slots for communication
- ViewModels are testable without UI

### Performance
- Use QSortFilterProxyModel for sorting/filtering (efficient)
- Update table in batches, not row-by-row
- Use custom delegates only when necessary
- Lazy load data for large device lists

---

## Next Phase

After completing Phase 5, proceed to:
→ **Phase 6: Charts & Metrics Visualization** (Week 12)
