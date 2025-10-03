# LanScan - TODO List

## Legend
- `[ ]` TODO
- `[>]` IN PROGRESS
- `[x]` DONE
- `[!]` BLOCKED

---

## Phase 0: Foundation & Infrastructure (Week 1-2)

### 0.1 Project Setup
- [x] Initialize Git repository
- [x] Create .gitignore
- [x] Create README.md
- [x] Create project.md documentation
- [ ] Setup CMakeLists.txt base
- [ ] Configure Qt6 in CMake
- [ ] Create project directory structure

### 0.2 Models Layer
- [ ] Device.h/cpp - Entità dispositivo
- [ ] NetworkMetrics.h/cpp - Entità metriche
- [ ] PortInfo.h/cpp - Info porta
- [ ] NetworkInterface.h/cpp - Interfaccia di rete
- [ ] Test: DeviceTest
- [ ] Test: NetworkMetricsTest

### 0.3 Utils Layer
- [ ] Logger.h/cpp - Sistema logging (DEBUG, INFO, WARN, ERROR)
- [ ] IpAddressValidator.h/cpp - Validazione IP/CIDR
- [ ] StringFormatter.h/cpp - Formattazione stringhe
- [ ] TimeFormatter.h/cpp - Formattazione date/ore
- [ ] StatisticsCalculator.h/cpp - Calcoli statistici
- [ ] Test: IpAddressValidatorTest
- [ ] Test: StatisticsCalculatorTest

### 0.4 Interfaces Layer
- [ ] IScanStrategy.h - Interface strategia scan
- [ ] IMetricsCalculator.h - Interface calcolatori
- [ ] IExporter.h - Interface export
- [ ] IDeviceRepository.h - Interface repository

**Deliverable**: Infrastruttura base funzionante, modelli dati, utilities testate

---

## Phase 1: Network Layer & Core Discovery (Week 3-4)

### 1.1 Network Services
- [ ] SubnetCalculator.h/cpp - Calcoli subnet/CIDR
- [ ] NetworkInterfaceDetector.h/cpp - Rilevamento interfacce
- [ ] MacVendorLookup.h/cpp - Lookup vendor (OUI database)
- [ ] PortServiceMapper.h/cpp - Mapping porta→servizio
- [ ] Test: SubnetCalculatorTest
- [ ] Test: NetworkInterfaceDetectorTest

### 1.2 Socket Management
- [ ] TcpSocketManager.h/cpp - Gestione socket TCP
- [ ] UdpSocketManager.h/cpp - Gestione socket UDP
- [ ] Test: Socket mock tests

### 1.3 Discovery Services
- [ ] HostDiscovery.h/cpp - Ping-based discovery (QProcess)
- [ ] DnsResolver.h/cpp - Risoluzione DNS
- [ ] ArpDiscovery.h/cpp - Risoluzione ARP
- [ ] Test: HostDiscoveryTest
- [ ] Test: DnsResolverTest

### 1.4 Basic Scanning
- [ ] IpScanner.h/cpp - Scansione IP range
- [ ] ScanStrategy.h/cpp - Strategia quick scan
- [ ] Test: IpScannerTest

**Deliverable**: Scansione subnet e discovery dispositivi con IP, hostname, MAC

---

## Phase 2: Metrics & Diagnostics Core (Week 5-6)

### 2.1 Ping Service
- [ ] PingService.h/cpp - Esecuzione ping (QProcess wrapper)
- [ ] Parsing output ping (cross-platform)
- [ ] Test: PingServiceTest

### 2.2 Metrics Calculators
- [ ] LatencyCalculator.h/cpp - Calcolo latenza (min/avg/max/median)
- [ ] JitterCalculator.h/cpp - Calcolo jitter (std deviation)
- [ ] PacketLossCalculator.h/cpp - Calcolo packet loss %
- [ ] QualityScoreCalculator.h/cpp - Score qualità
- [ ] Test: LatencyCalculatorTest
- [ ] Test: JitterCalculatorTest
- [ ] Test: PacketLossCalculatorTest
- [ ] Test: QualityScoreCalculatorTest

### 2.3 Metrics Aggregation
- [ ] MetricsAggregator.h/cpp - Aggregazione metriche
- [ ] Continuous ping tracking (history)
- [ ] Test: MetricsAggregatorTest

### 2.4 Port Scanning
- [ ] PortScanner.h/cpp - Scansione porte TCP
- [ ] Quick scan (porte comuni)
- [ ] Custom port ranges
- [ ] Test: PortScannerTest

**Deliverable**: Metriche latenza, jitter, packet loss per ogni dispositivo + port scan

---

## Phase 3: Persistence & Data Management (Week 7)

### 3.1 Repository Pattern
- [ ] DeviceRepository.h/cpp - Persistenza dispositivi (SQLite)
- [ ] DeviceCache.h/cpp - Cache in-memory
- [ ] Database schema creation
- [ ] Test: DeviceRepositoryTest

### 3.2 Export Functionality
- [ ] ExportStrategy.h - Interface export
- [ ] CsvExporter.h/cpp - Export CSV
- [ ] JsonExporter.h/cpp - Export JSON
- [ ] Test: CsvExporterTest
- [ ] Test: JsonExporterTest

### 3.3 Settings Management
- [ ] SettingsManager.h/cpp - QSettings wrapper
- [ ] ConfigValidator.h/cpp - Validazione config
- [ ] Test: SettingsManagerTest

**Deliverable**: Dati persistono, export CSV/JSON funzionante

---

## Phase 4: Application Layer & Controllers (Week 8-9)

### 4.1 Core Coordinators
- [ ] ScanCoordinator.h/cpp - Coordina scansioni multiple
- [ ] Multi-threading con QThread
- [ ] Progress tracking
- [ ] Test: ScanCoordinatorTest

### 4.2 Controllers
- [ ] ScanController.h/cpp - Controlla workflow scansione
- [ ] MetricsController.h/cpp - Controlla raccolta metriche
- [ ] ExportController.h/cpp - Controlla export
- [ ] Test: Integration tests

### 4.3 Management Services
- [ ] ProfileManager.h/cpp - Gestione profili scan
- [ ] FavoritesManager.h/cpp - Gestione preferiti
- [ ] Test: ProfileManagerTest

**Deliverable**: Business logic completa, workflow orchestrati, threading

---

## Phase 5: UI Foundation & Basic Views (Week 10-11)

### 5.1 Qt Designer Files (.ui)
- [ ] mainwindow.ui - Layout finestra principale
- [ ] scanconfigdialog.ui - Dialog configurazione scan
- [ ] devicetablewidget.ui - Widget tabella dispositivi

### 5.2 ViewModels
- [ ] DeviceTableViewModel.h/cpp - ViewModel tabella (QAbstractTableModel)
- [ ] Custom delegates per rendering
- [ ] Test: DeviceTableViewModelTest

### 5.3 Main Window & Basic UI
- [ ] MainWindow.h/cpp - Finestra principale
- [ ] Menu bar, toolbar, status bar
- [ ] Connessioni signals/slots
- [ ] Integration con ScanController

### 5.4 Device Table Widget
- [ ] DeviceTableWidget.h/cpp - Widget tabella
- [ ] Context menu (ping, dettagli, rimuovi)
- [ ] Sorting, filtering

### 5.5 Scan Config Dialog
- [ ] ScanConfigDialog.h/cpp - Dialog config
- [ ] ScanConfigViewModel.h/cpp - ViewModel
- [ ] Input IP range, scan type, threads

**Deliverable**: UI base funzionante, scansione da GUI, tabella dispositivi

---

## Phase 6: Charts & Metrics Visualization (Week 12)

### 6.1 QtCharts Integration
- [ ] Setup QtCharts in CMake
- [ ] ChartViewModel.h/cpp - ViewModel grafici

### 6.2 Chart Widgets
- [ ] LatencyChart.h/cpp - Grafico latenza (QLineSeries)
- [ ] PacketLossChart.h/cpp - Grafico packet loss (QBarSeries)
- [ ] JitterChart.h/cpp - Grafico jitter (QSplineSeries)
- [ ] Real-time update dei grafici

### 6.3 Metrics Widget
- [ ] metricswidget.ui - Layout widget metriche
- [ ] MetricsWidget.h/cpp - Widget view
- [ ] MetricsViewModel.h/cpp - ViewModel
- [ ] Integration con MetricsController

**Deliverable**: Grafici latenza/jitter/packet loss real-time

---

## Phase 7-11: Advanced Features (Week 13-20)

_Vedere project.md per task dettagliate delle fasi successive_

---

## Current Focus

**Current Phase**: Phase 0 - Foundation & Infrastructure
**Current Week**: Week 1-2
**Next Milestone**: Complete foundation layer (models, utils, interfaces)

---

## Notes & Blockers

### Active Blockers
- None

### Technical Decisions
- Using Qt 6.x framework
- CMake build system
- SQLite for local database
- Qt Test framework for testing

### Resources Needed
- Qt 6.2+ installed
- CMake 3.16+
- C++17 compiler

---

Last Updated: 2025-01-03
