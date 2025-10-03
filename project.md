# LanScan - Network Scanner and Diagnostic Tool

## Panoramica Progetto
Applicazione desktop cross-platform sviluppata in C++ con Qt per la scansione, monitoraggio e diagnostica di reti LAN.

## Tecnologie
- **Framework**: Qt 6.x (Qt Widgets + Qt Network + Qt Charts)
- **UI Design**: Qt Designer (file .ui XML)
- **Linguaggio**: C++ (Standard C++17 o superiore)
- **Build System**: CMake
- **Librerie aggiuntive**:
  - libpcap/WinPcap (opzionale, per packet capture avanzato)
  - Boost.Asio (opzionale, per networking async)

---

## Funzionalità Core

### 1. Scansione Rete
- **IP Range Scanner**
  - Scansione CIDR (es. 192.168.1.0/24)
  - Scansione custom range (es. 192.168.1.10-192.168.1.50)
  - Auto-detect subnet locale
  - Scansione multi-threaded per performance

- **Rilevamento Dispositivi**
  - Hostname resolution (DNS lookup)
  - MAC address detection (ARP)
  - Vendor identification (OUI database)
  - Device type guessing (basato su porte/servizi)

- **Port Scanning**
  - Quick scan (porte comuni: 21, 22, 23, 25, 80, 443, 3389, ecc.)
  - Full scan (1-65535)
  - Custom port list
  - Service version detection

### 2. Metriche di Rete

#### Latenza
- **ICMP Ping**
  - RTT (Round Trip Time): min/avg/max/median
  - Packet loss percentage
  - Continuous ping con history
  - Configurable packet size
  - Configurable interval (ms)

#### Jitter
- Calcolo variazione latenza
- Standard deviation del RTT
- Visualizzazione grafico tempo reale

#### Packet Loss
- Percentuale pacchetti persi
- Pattern detection (burst vs random loss)
- Alert su soglie critiche

#### Throughput/Bandwidth
- TCP throughput test
- UDP bandwidth test
- Upload/Download speed test
- Concurrent connections test

### 3. Strumenti Diagnostici

#### Continuous Monitoring
- Ping continuo con grafici real-time
- Auto-refresh device list
- Network change detection
- Connection quality tracking

#### Traceroute
- Visual hop-by-hop path
- Latency per hop
- Geographic location (opzionale, con GeoIP)
- Multi-path detection

#### Path MTU Discovery
- Detect maximum transmission unit
- Fragmentation testing
- Optimal packet size suggestion

#### DNS Diagnostics
- DNS lookup times
- Multiple DNS server testing
- Reverse DNS lookup
- DNS cache analysis

#### Connection Quality Score
- Algoritmo scoring basato su:
  - Latency (peso: 30%)
  - Packet loss (peso: 40%)
  - Jitter (peso: 20%)
  - Availability (peso: 10%)
- Rating: Excellent/Good/Fair/Poor/Critical

### 4. Funzionalità Avanzate

#### Wake-on-LAN
- Send magic packet a dispositivi
- Saved devices per WoL rapido
- Verify device wake-up

#### Network Monitoring
- Real-time device list refresh
- New device alerts
- Device disconnection alerts
- Change tracking (IP/MAC/hostname changes)

#### Profili e Configurazioni
- Save/Load scan profiles
- Network presets (Home/Office/Custom)
- Favorite devices
- Custom naming/tagging devices

#### Export/Import
- Export risultati: CSV, JSON, XML, HTML
- Import device lists
- Report generation con statistiche

---

## Architettura Software

### Struttura Modulare (SRP-Compliant)

L'architettura segue il **Single Responsibility Principle (SRP)**: ogni classe ha una singola ragione di cambiamento.

```
LanScan/
├── src/
│   ├── main.cpp
│   │
│   ├── core/                          # Business Logic Layer
│   │   ├── scanning/
│   │   │   ├── IpScanner.h/cpp       # SRP: Scansione IP range
│   │   │   ├── PortScanner.h/cpp     # SRP: Scansione porte
│   │   │   ├── ScanCoordinator.h/cpp # SRP: Coordina scansioni multiple
│   │   │   └── ScanStrategy.h/cpp    # SRP: Strategia scansione (fast/deep)
│   │   │
│   │   ├── discovery/
│   │   │   ├── HostDiscovery.h/cpp   # SRP: Scoperta host (ping)
│   │   │   ├── ArpDiscovery.h/cpp    # SRP: Risoluzione ARP
│   │   │   └── DnsResolver.h/cpp     # SRP: Risoluzione DNS
│   │   │
│   │   ├── metrics/
│   │   │   ├── PingService.h/cpp     # SRP: Esecuzione ping
│   │   │   ├── LatencyCalculator.h/cpp    # SRP: Calcolo latenza
│   │   │   ├── JitterCalculator.h/cpp     # SRP: Calcolo jitter
│   │   │   ├── PacketLossCalculator.h/cpp # SRP: Calcolo packet loss
│   │   │   ├── QualityScoreCalculator.h/cpp # SRP: Calcolo quality score
│   │   │   └── MetricsAggregator.h/cpp    # SRP: Aggregazione metriche
│   │   │
│   │   ├── diagnostics/
│   │   │   ├── TraceRouteService.h/cpp    # SRP: Esecuzione traceroute
│   │   │   ├── MtuDiscovery.h/cpp         # SRP: Path MTU discovery
│   │   │   ├── BandwidthTester.h/cpp      # SRP: Test bandwidth
│   │   │   └── DnsDiagnostics.h/cpp       # SRP: Diagnostica DNS
│   │   │
│   │   └── management/
│   │       ├── DeviceRepository.h/cpp     # SRP: Persistenza dispositivi
│   │       ├── DeviceCache.h/cpp          # SRP: Cache in-memory dispositivi
│   │       ├── ProfileManager.h/cpp       # SRP: Gestione profili scan
│   │       └── FavoritesManager.h/cpp     # SRP: Gestione preferiti
│   │
│   ├── network/                       # Network Layer
│   │   ├── protocols/
│   │   │   ├── IcmpPacketBuilder.h/cpp    # SRP: Costruzione pacchetti ICMP
│   │   │   ├── ArpPacketBuilder.h/cpp     # SRP: Costruzione pacchetti ARP
│   │   │   └── WakeOnLanPacket.h/cpp      # SRP: Magic packet WoL
│   │   │
│   │   ├── sockets/
│   │   │   ├── TcpSocketManager.h/cpp     # SRP: Gestione socket TCP
│   │   │   ├── UdpSocketManager.h/cpp     # SRP: Gestione socket UDP
│   │   │   └── RawSocketManager.h/cpp     # SRP: Gestione raw socket
│   │   │
│   │   └── services/
│   │       ├── NetworkInterfaceDetector.h/cpp # SRP: Rilevamento interfacce
│   │       ├── SubnetCalculator.h/cpp     # SRP: Calcolo subnet/CIDR
│   │       ├── MacVendorLookup.h/cpp      # SRP: Lookup vendor da MAC
│   │       └── PortServiceMapper.h/cpp    # SRP: Mapping porta->servizio
│   │
│   ├── models/                        # Data Models
│   │   ├── Device.h/cpp               # SRP: Entità dispositivo
│   │   ├── NetworkMetrics.h/cpp       # SRP: Entità metriche
│   │   ├── ScanProfile.h/cpp          # SRP: Entità profilo scansione
│   │   ├── PortInfo.h/cpp             # SRP: Informazioni porta
│   │   ├── TraceRouteHop.h/cpp        # SRP: Hop traceroute
│   │   └── NetworkInterface.h/cpp     # SRP: Interfaccia di rete
│   │
│   ├── viewmodels/                    # Presentation Logic (MVVM)
│   │   ├── DeviceTableViewModel.h/cpp # SRP: Logic tabella dispositivi
│   │   ├── MetricsViewModel.h/cpp     # SRP: Logic widget metriche
│   │   ├── ScanConfigViewModel.h/cpp  # SRP: Logic config scansione
│   │   └── ChartViewModel.h/cpp       # SRP: Logic grafici
│   │
│   ├── ui/                            # View Layer
│   │   ├── MainWindow.h/cpp           # SRP: Finestra principale (view)
│   │   ├── DeviceTableWidget.h/cpp    # SRP: Widget tabella (view)
│   │   ├── MetricsWidget.h/cpp        # SRP: Widget metriche (view)
│   │   ├── ScanConfigDialog.h/cpp     # SRP: Dialog configurazione (view)
│   │   ├── DeviceDetailDialog.h/cpp   # SRP: Dialog dettagli (view)
│   │   ├── SettingsDialog.h/cpp       # SRP: Dialog impostazioni (view)
│   │   │
│   │   ├── charts/
│   │   │   ├── LatencyChart.h/cpp     # SRP: Rendering chart latenza
│   │   │   ├── PacketLossChart.h/cpp  # SRP: Rendering chart packet loss
│   │   │   ├── JitterChart.h/cpp      # SRP: Rendering chart jitter
│   │   │   └── ThroughputChart.h/cpp  # SRP: Rendering chart throughput
│   │   │
│   │   └── widgets/
│   │       ├── QualityGauge.h/cpp     # SRP: Widget gauge qualità
│   │       ├── StatusLed.h/cpp        # SRP: Widget LED status
│   │       ├── MiniSparkline.h/cpp    # SRP: Mini-grafico inline
│   │       └── IpRangePicker.h/cpp    # SRP: Selector range IP
│   │
│   ├── controllers/                   # Application Controllers
│   │   ├── ScanController.h/cpp       # SRP: Controlla operazioni scan
│   │   ├── MetricsController.h/cpp    # SRP: Controlla raccolta metriche
│   │   ├── ExportController.h/cpp     # SRP: Controlla export dati
│   │   └── NotificationController.h/cpp # SRP: Gestione notifiche/alert
│   │
│   ├── services/                      # Application Services
│   │   ├── ScanScheduler.h/cpp        # SRP: Scheduling scansioni
│   │   ├── MonitoringService.h/cpp    # SRP: Monitoring continuo rete
│   │   ├── AlertService.h/cpp         # SRP: Generazione alert
│   │   └── HistoryService.h/cpp       # SRP: Gestione storico eventi
│   │
│   ├── persistence/                   # Data Persistence Layer
│   │   ├── database/
│   │   │   ├── DatabaseManager.h/cpp  # SRP: Connessione database
│   │   │   ├── DeviceDao.h/cpp        # SRP: DAO dispositivi
│   │   │   ├── MetricsDao.h/cpp       # SRP: DAO metriche
│   │   │   └── HistoryDao.h/cpp       # SRP: DAO history
│   │   │
│   │   ├── export/
│   │   │   ├── CsvExporter.h/cpp      # SRP: Export CSV
│   │   │   ├── JsonExporter.h/cpp     # SRP: Export JSON
│   │   │   ├── XmlExporter.h/cpp      # SRP: Export XML
│   │   │   ├── HtmlReportGenerator.h/cpp # SRP: Generazione report HTML
│   │   │   └── ExportStrategy.h       # SRP: Interfaccia export
│   │   │
│   │   └── settings/
│   │       ├── SettingsManager.h/cpp  # SRP: Gestione impostazioni
│   │       ├── ThemeManager.h/cpp     # SRP: Gestione temi UI
│   │       └── ConfigValidator.h/cpp  # SRP: Validazione configurazioni
│   │
│   ├── utils/                         # Utilities
│   │   ├── Logger.h/cpp               # SRP: Logging
│   │   ├── IpAddressValidator.h/cpp   # SRP: Validazione IP
│   │   ├── StringFormatter.h/cpp      # SRP: Formattazione stringhe
│   │   ├── TimeFormatter.h/cpp        # SRP: Formattazione date/ore
│   │   ├── StatisticsCalculator.h/cpp # SRP: Calcoli statistici generici
│   │   └── ThreadPool.h/cpp           # SRP: Pool thread riutilizzabili
│   │
│   └── interfaces/                    # Interfaces/Abstract Classes
│       ├── IScanStrategy.h            # Interface: strategia scansione
│       ├── IMetricsCalculator.h       # Interface: calcolatore metriche
│       ├── IExporter.h                # Interface: esportatore dati
│       ├── IDeviceRepository.h        # Interface: repository dispositivi
│       └── INotificationObserver.h    # Interface: observer notifiche
│
├── ui/                                # Qt Designer Files (XML)
│   ├── mainwindow.ui
│   ├── scanconfigdialog.ui
│   ├── devicedetaildialog.ui
│   ├── settingsdialog.ui
│   ├── metricswidget.ui
│   └── devicetablewidget.ui
│
├── resources/
│   ├── icons/
│   ├── styles/
│   ├── resources.qrc
│   └── database/
│       └── oui.txt
│
├── tests/
│   ├── unit/
│   │   ├── core/
│   │   ├── network/
│   │   └── utils/
│   └── integration/
│
├── CMakeLists.txt
└── README.md
```

### Principi SRP Applicati

#### 1. **Separazione delle Responsabilità**

Ogni classe ha **una sola responsabilità** ben definita:

**❌ Cattivo Design (Viola SRP):**
```cpp
class NetworkScanner {
    void scanNetwork();           // Scansione
    void pingHost();             // Ping
    void calculateMetrics();     // Calcolo metriche
    void saveToDatabase();       // Persistenza
    void exportToCsv();          // Export
    void updateUI();             // UI update
};
// ^ Troppe responsabilità!
```

**✅ Buon Design (Rispetta SRP):**
```cpp
// Ogni classe ha UNA responsabilità
class IpScanner {
    void scanIpRange(QString cidr);  // SOLO: scansione IP
};

class PingService {
    PingResult ping(QString host);   // SOLO: esecuzione ping
};

class LatencyCalculator {
    double calculate(QVector<double> rtt);  // SOLO: calcolo latenza
};

class DeviceRepository {
    void save(Device device);        // SOLO: persistenza
};

class CsvExporter {
    void exportData(QList<Device>);  // SOLO: export CSV
};
```

#### 2. **Layer Architecture**

**Presentation Layer (UI):**
- Responsabilità: Rendering, input utente
- Non contiene business logic
- Comunica con ViewModel/Controller

**Application Layer (Controllers/Services):**
- Responsabilità: Orchestrazione, workflow
- Coordina operazioni tra layer
- Gestisce transazioni applicative

**Domain Layer (Core):**
- Responsabilità: Business logic
- Indipendente da UI e infrastruttura
- Contiene regole di dominio

**Infrastructure Layer (Network/Persistence):**
- Responsabilità: I/O, comunicazione esterna
- Database, file system, network
- Implementa interfacce definite dal dominio

#### 3. **Dependency Injection**

Le classi dipendono da **interfacce**, non da implementazioni concrete:

```cpp
// Interface
class IMetricsCalculator {
public:
    virtual double calculate(const QVector<double>& data) = 0;
    virtual ~IMetricsCalculator() = default;
};

// Implementazioni concrete
class LatencyCalculator : public IMetricsCalculator {
public:
    double calculate(const QVector<double>& rtt) override {
        return std::accumulate(rtt.begin(), rtt.end(), 0.0) / rtt.size();
    }
};

class JitterCalculator : public IMetricsCalculator {
public:
    double calculate(const QVector<double>& rtt) override {
        // Calcola standard deviation
    }
};

// Aggregator dipende dall'interfaccia
class MetricsAggregator {
private:
    IMetricsCalculator* latencyCalc;
    IMetricsCalculator* jitterCalc;

public:
    MetricsAggregator(IMetricsCalculator* latency,
                      IMetricsCalculator* jitter)
        : latencyCalc(latency), jitterCalc(jitter) {}

    NetworkMetrics aggregate(const QVector<double>& rtt) {
        NetworkMetrics metrics;
        metrics.latency = latencyCalc->calculate(rtt);
        metrics.jitter = jitterCalc->calculate(rtt);
        return metrics;
    }
};
```

#### 4. **MVVM Pattern per UI**

Separazione vista-logica tramite **Model-View-ViewModel**:

```cpp
// Model (dati puri)
class Device {
    QString ip;
    QString hostname;
    // ... solo dati
};

// ViewModel (presentation logic)
class DeviceTableViewModel : public QAbstractTableModel {
    Q_OBJECT
private:
    QList<Device> devices;
    IDeviceRepository* repository;

public:
    // QAbstractTableModel interface
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;

    // Business logic presentation
    void loadDevices() {
        devices = repository->findAll();
        emit dataChanged();
    }

    QString getStatusColor(int row) const {
        return devices[row].isOnline ? "#4CAF50" : "#F44336";
    }
};

// View (solo UI)
class DeviceTableWidget : public QWidget {
    Q_OBJECT
private:
    Ui::DeviceTableWidget *ui;
    DeviceTableViewModel* viewModel;

public:
    DeviceTableWidget(QWidget *parent = nullptr)
        : QWidget(parent), ui(new Ui::DeviceTableWidget) {
        ui->setupUi(this);
        ui->tableView->setModel(viewModel);
        // NESSUNA business logic qui
    }
};
```

### Design Patterns (SRP-Compliant)

- **Strategy Pattern**: Algoritmi scansione intercambiabili
- **Observer Pattern**: Notifiche eventi (nuovi dispositivi, alert)
- **Repository Pattern**: Astrazione persistenza dati
- **Factory Pattern**: Creazione oggetti complessi
- **Dependency Injection**: Disaccoppiamento componenti
- **MVVM**: Separazione UI-logic
- **Service Layer**: Coordinazione operazioni complesse

---

## Interfaccia Grafica Moderna

### Stile UI
- **Design**: Material Design o Fluent Design inspired
- **Dark/Light Mode**: Supporto entrambi i temi
- **Responsive**: Layout adattivo a diverse risoluzioni
- **Animazioni**: Transizioni smooth per feedback utente

### Layout Principale

```
┌─────────────────────────────────────────────────────────────┐
│ [LanScan]                                    [-] [□] [×]    │
├─────────────────────────────────────────────────────────────┤
│ File  Scan  Tools  View  Help                               │
├───────────┬─────────────────────────────────────────────────┤
│           │  ┌──────────────────────────────────────────┐   │
│  Network  │  │ Subnet: [192.168.1.0/24 ▼] [Start Scan] │   │
│  ───────  │  └──────────────────────────────────────────┘   │
│           │                                                  │
│  Devices  │  ┌────────────────────────────────────────────┐ │
│  ───────  │  │ IP         │ Name   │ MAC   │ Vendor │ ●  │ │
│           │  ├────────────────────────────────────────────┤ │
│  Favorites│  │ 192.168.1.1│ Router │ AA... │ Cisco  │ 🟢 │ │
│  ────────  │  │ 192.168.1.5│ NAS    │ BB... │ Synol. │ 🟢 │ │
│           │  │ ...                                        │ │
│  Monitor  │  └────────────────────────────────────────────┘ │
│  ───────  │                                                  │
│           │  ┌──────────────┬───────────────────────────┐   │
│  Settings │  │ Latency Graph│  Device Details          │   │
│  ────────  │  │              │  IP: 192.168.1.1         │   │
│           │  │   [Chart]    │  MAC: AA:BB:CC:DD:EE:FF  │   │
│           │  │              │  Latency: 5ms            │   │
│           │  │              │  Loss: 0%                │   │
│           │  └──────────────┴───────────────────────────┘   │
└───────────┴──────────────────────────────────────────────────┘
│ Status: Ready | Devices: 15 | Last scan: 10:30:45          │
└─────────────────────────────────────────────────────────────┘
```

### Componenti UI Principali

#### 1. Toolbar
- Quick scan button (icona grande)
- Stop scan button
- Refresh
- Export
- Settings
- Theme switcher (sun/moon icon)

#### 2. Sidebar (pannello laterale collassabile)
- Network overview (info subnet, gateway, DNS)
- Device categories (All/Active/Offline/Favorites)
- Quick actions (WoL, Ping, Traceroute)
- Recent scans history

#### 3. Device Table (QTableView con model custom)
**Colonne:**
- Status indicator (LED colorato)
- IP Address
- Hostname
- MAC Address
- Vendor
- Open Ports (badge count)
- Latency (ms, con color coding)
- Packet Loss (%)
- Quality Score (stars o gauge)
- Last Seen (timestamp)
- Actions (context menu: Details/Ping/WoL/Remove)

**Features:**
- Sortable columns
- Filterable (search box)
- Multi-select
- Drag & drop per favorites
- Custom delegates per rendering avanzato
- Tooltips con info dettagliate

#### 4. Metrics Dashboard (QDockWidget)

**Panels:**
- **Real-time Latency Chart** (QLineSeries con QtCharts)
  - Ultimo minuto di dati
  - Gridlines per leggibilità
  - Min/Max/Avg annotations

- **Packet Loss Chart** (QBarSeries)
  - Distribuzione temporale
  - Soglie warning (giallo >1%, rosso >5%)

- **Jitter Graph** (QSplineSeries)
  - Smoothed jitter values

- **Quality Score Gauge** (Custom widget circolare)
  - Stile speedometer
  - Color zones (verde/giallo/rosso)

#### 5. Device Detail Panel
Quando selezionato un device:
- Header con icona device type
- Tabs:
  - **Overview**: info generali
  - **Ports**: lista porte aperte con servizi
  - **Metrics**: grafici storici latency/loss
  - **History**: log eventi (connect/disconnect/changes)
  - **Actions**: buttons per WoL, custom ping test, ecc.

#### 6. Dialogs

**Scan Configuration Dialog:**
- IP range selector (visual subnet picker)
- Scan type: Quick/Standard/Deep/Custom
- Threads slider (1-100)
- Timeout settings
- Port scan options (checkbox list porte comuni)
- Advanced options (collapsible)

**Device Detail Dialog:**
- Modal window con full device info
- Tabs per organizzare info
- Live metrics updates
- History graph zoom/pan

**Settings Dialog:**
- Tabs: General, Network, Appearance, Notifications, Advanced
- Theme selector con preview
- Scan defaults
- Alert thresholds
- Auto-save options
- Export preferences

#### 7. Status Bar
- Current operation status
- Progress bar (durante scan)
- Device count badges
- Network stats summary
- Timestamp ultimo scan

### Elementi Visivi Moderni

#### Color Scheme (Light Mode)
- Primary: #2196F3 (Blue)
- Secondary: #FF5722 (Deep Orange)
- Success: #4CAF50 (Green)
- Warning: #FFC107 (Amber)
- Error: #F44336 (Red)
- Background: #FAFAFA
- Surface: #FFFFFF
- Text: #212121

#### Color Scheme (Dark Mode)
- Primary: #42A5F5
- Secondary: #FF7043
- Success: #66BB6A
- Warning: #FFCA28
- Error: #EF5350
- Background: #121212
- Surface: #1E1E1E
- Text: #E0E0E0

#### Typography
- Headers: Roboto Bold, 16-24pt
- Body: Roboto Regular, 12-14pt
- Monospace (IP/MAC): Roboto Mono, 11pt

#### Icons
- Material Design Icons o Feather Icons
- Size: 16x16, 24x24, 48x48
- SVG per scalabilità

#### Animazioni
- Fade in/out per pannelli
- Slide per sidebar
- Pulse per device status changes
- Progress indicators durante operazioni lunghe
- Smooth scrolling

#### Custom Widgets
- **QualityGauge**: Widget circolare gauge per quality score
- **MiniSparkline**: Piccoli grafici inline nella tabella
- **StatusLED**: Indicatore colorato animato
- **NetworkVisualization**: Vista grafica rete (opzionale, fase 2)

---

## Tecnologie Qt Utilizzate

### Qt Modules
- **Qt Core**: Fondamentale
- **Qt Widgets**: UI components
- **Qt Network**: QTcpSocket, QUdpSocket, QHostInfo, QNetworkInterface
- **Qt Charts**: Grafici e visualizzazioni
- **Qt Concurrent**: Multi-threading per scansioni
- **Qt SQL**: Database locale per cache/history (opzionale)

### Qt Features
- **Signals & Slots**: Event handling
- **Model/View**: Device table con QAbstractTableModel custom
- **QThread**: Background operations
- **QTimer**: Periodic tasks (auto-refresh, ping monitoring)
- **QSettings**: Persistenza configurazione
- **QStyle**: Custom styling con QSS
- **QStateMachine**: Gestione stati applicazione (opzionale)

---

## Qt Designer - Sviluppo UI con file .ui (XML)

### Workflow Sviluppo UI

L'interfaccia utente viene progettata con **Qt Designer**, che genera file `.ui` in formato XML. Questi file vengono automaticamente compilati in codice C++ durante il build process.

#### 1. Creazione UI in Qt Designer

**Passi:**
1. Apri Qt Designer
2. Scegli template (es. "Main Window", "Dialog", "Widget")
3. Trascina widget dalla palette al form
4. Configura properties (objectName, geometry, text, ecc.)
5. Crea layouts (QVBoxLayout, QHBoxLayout, QGridLayout)
6. Connetti signals/slots visualmente (opzionale)
7. Salva come `.ui` file nella directory `ui/`

**Esempio: mainwindow.ui**
```xml
<?xml version="1.0" encoding="UTF-8"?>
<ui version="4.0">
 <class>MainWindow</class>
 <widget class="QMainWindow" name="MainWindow">
  <property name="geometry">
   <rect>
    <x>0</x>
    <y>0</y>
    <width>1200</width>
    <height>800</height>
   </rect>
  </property>
  <property name="windowTitle">
   <string>LanScan - Network Scanner</string>
  </property>
  <widget class="QWidget" name="centralwidget">
   <layout class="QVBoxLayout" name="verticalLayout">
    <item>
     <widget class="QTableView" name="deviceTableView"/>
    </item>
   </layout>
  </widget>
  <widget class="QMenuBar" name="menubar"/>
  <widget class="QStatusBar" name="statusbar"/>
 </widget>
 <resources/>
 <connections/>
</ui>
```

#### 2. Integrazione nel Codice C++

**File Header (MainWindow.h):**
```cpp
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

// Forward declaration del namespace Ui
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onScanButtonClicked();
    void onDeviceSelected(const QModelIndex &index);

private:
    Ui::MainWindow *ui;  // Puntatore all'interfaccia generata

    void setupConnections();
    void initializeUI();
};

#endif // MAINWINDOW_H
```

**File Implementation (MainWindow.cpp):**
```cpp
#include "MainWindow.h"
#include "ui_mainwindow.h"  // Header auto-generato da mainwindow.ui

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)  // Allocazione interfaccia
{
    ui->setupUi(this);  // Setup dell'UI dal file .ui

    initializeUI();
    setupConnections();
}

MainWindow::~MainWindow()
{
    delete ui;  // Cleanup
}

void MainWindow::initializeUI()
{
    // Configura widget non gestibili da Designer
    ui->deviceTableView->setModel(deviceModel);
    ui->statusbar->showMessage("Ready");
}

void MainWindow::setupConnections()
{
    // Connetti signals/slots
    connect(ui->scanButton, &QPushButton::clicked,
            this, &MainWindow::onScanButtonClicked);

    connect(ui->deviceTableView, &QTableView::clicked,
            this, &MainWindow::onDeviceSelected);
}

void MainWindow::onScanButtonClicked()
{
    // Accesso ai widget tramite ui->nomeWidget
    QString subnet = ui->subnetLineEdit->text();
    ui->progressBar->setValue(0);
    // ... logica scan
}
```

#### 3. CMake Configuration

Il file `CMakeLists.txt` deve abilitare l'auto-generazione del codice dai file `.ui`:

```cmake
cmake_minimum_required(VERSION 3.16)
project(LanScan VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Abilita auto-generazione per Qt
set(CMAKE_AUTOMOC ON)      # Meta-Object Compiler
set(CMAKE_AUTOUIC ON)      # User Interface Compiler (per .ui files)
set(CMAKE_AUTORCC ON)      # Resource Compiler (per .qrc files)

# Directory dove cercare i file .ui
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_SOURCE_DIR}/ui)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Widgets
    Network
    Charts
)

# File sorgenti
set(PROJECT_SOURCES
    src/main.cpp

    # UI Classes
    src/ui/MainWindow.h
    src/ui/MainWindow.cpp

    src/ui/ScanConfigDialog.h
    src/ui/ScanConfigDialog.cpp

    src/ui/DeviceDetailDialog.h
    src/ui/DeviceDetailDialog.cpp

    src/ui/SettingsDialog.h
    src/ui/SettingsDialog.cpp

    src/ui/MetricsWidget.h
    src/ui/MetricsWidget.cpp

    # Core classes
    src/core/NetworkScanner.h
    src/core/NetworkScanner.cpp

    # ... altri file
)

# File .ui (opzionale, vengono trovati automaticamente)
set(PROJECT_UI
    ui/mainwindow.ui
    ui/scanconfigdialog.ui
    ui/devicedetaildialog.ui
    ui/settingsdialog.ui
    ui/metricswidget.ui
)

# File resources (icone, stili, ecc.)
set(PROJECT_RESOURCES
    resources/resources.qrc
)

add_executable(LanScan
    ${PROJECT_SOURCES}
    ${PROJECT_UI}
    ${PROJECT_RESOURCES}
)

target_link_libraries(LanScan
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
    Qt6::Charts
)

# Include directory per i file generati
target_include_directories(LanScan PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}  # Per ui_*.h files
)

install(TARGETS LanScan
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
```

#### 4. Build Process

Quando compili il progetto:

1. **CMake** rileva i file `.ui` nella directory `ui/`
2. **UIC (User Interface Compiler)** converte `mainwindow.ui` → `ui_mainwindow.h`
3. Il file `ui_mainwindow.h` viene generato in `build/` directory
4. Il tuo codice C++ include `ui_mainwindow.h` e lo compila

**File generato automaticamente (ui_mainwindow.h):**
```cpp
// Auto-generato - NON MODIFICARE!
#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>
// ... altri includes

QT_BEGIN_NAMESPACE

namespace Ui {
    class MainWindow: public QMainWindow
    {
    public:
        QWidget *centralwidget;
        QVBoxLayout *verticalLayout;
        QTableView *deviceTableView;
        QMenuBar *menubar;
        QStatusBar *statusbar;

        void setupUi(QMainWindow *MainWindow)
        {
            // ... codice setup generato automaticamente
        }

        void retranslateUi(QMainWindow *MainWindow)
        {
            MainWindow->setWindowTitle(
                QCoreApplication::translate("MainWindow",
                    "LanScan - Network Scanner", nullptr));
        }
    };
}

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
```

### Best Practices Qt Designer

#### Naming Conventions
- **objectName**: usa nomi descrittivi
  - Button: `scanButton`, `stopButton`, `exportButton`
  - LineEdit: `subnetLineEdit`, `hostnameLineEdit`
  - Table: `deviceTableView`, `portTableWidget`
  - Label: `statusLabel`, `infoLabel`

#### Layout Management
- Usa sempre **layouts** (non posizioni assolute)
- Layout principali:
  - `QVBoxLayout`: verticale
  - `QHBoxLayout`: orizzontale
  - `QGridLayout`: griglia
  - `QFormLayout`: form label-field
- Imposta **size policies** appropriate
- Usa **spacers** per allineamenti

#### Separazione Responsabilità
- **Designer**: layout, dimensioni, testi statici, tab order
- **Codice C++**: logica business, popolamento dati, stili dinamici, connessioni complesse

#### Signals/Slots
- Connessioni semplici: usa Designer (clic destro → Go to slot)
- Connessioni complesse: nel codice C++ (più flessibile)

### File .ui per LanScan

#### mainwindow.ui
**Componenti principali:**
- QMenuBar: File, Scan, Tools, View, Help
- QToolBar: Quick access buttons
- QSplitter: Sidebar + Area centrale
- QTableView: Device list
- QDockWidget: Metrics dashboard
- QStatusBar: Status info

**Widgets:**
- `scanButton` (QPushButton)
- `stopButton` (QPushButton)
- `subnetComboBox` (QComboBox)
- `deviceTableView` (QTableView)
- `progressBar` (QProgressBar)
- `statusLabel` (QLabel)

#### scanconfigdialog.ui
**Componenti:**
- QTabWidget: Basic/Advanced tabs
- IP range inputs (QLineEdit)
- Scan type (QComboBox)
- Thread count (QSlider + QSpinBox)
- Port selection (QListWidget con checkboxes)
- OK/Cancel buttons (QDialogButtonBox)

#### devicedetaildialog.ui
**Componenti:**
- QTabWidget: Overview, Ports, Metrics, History
- Device info labels (QLabel)
- Port table (QTableWidget)
- Charts (widget placeholder, popolato in codice)
- Action buttons (QPushButton)

#### settingsdialog.ui
**Componenti:**
- QTabWidget: General, Network, Appearance, Notifications
- Theme selector (QComboBox)
- Checkboxes per opzioni (QCheckBox)
- Spin boxes per valori (QSpinBox)
- QDialogButtonBox: OK/Cancel/Apply

#### metricswidget.ui
**Componenti:**
- QVBoxLayout principale
- Chart containers (QWidget, popolati in codice con QtCharts)
- Metric labels (QLabel)
- Refresh controls

### Resources (.qrc file)

Il file `resources/resources.qrc` contiene riferimenti a icone, stili, database:

```xml
<!DOCTYPE RCC>
<RCC version="1.0">
    <qresource prefix="/icons">
        <file>icons/scan.svg</file>
        <file>icons/stop.svg</file>
        <file>icons/export.svg</file>
        <file>icons/settings.svg</file>
        <file>icons/device.svg</file>
        <file>icons/network.svg</file>
    </qresource>
    <qresource prefix="/styles">
        <file>styles/dark.qss</file>
        <file>styles/light.qss</file>
    </qresource>
    <qresource prefix="/database">
        <file>database/oui.txt</file>
    </qresource>
</RCC>
```

**Uso nel codice:**
```cpp
// Carica icona da resources
ui->scanButton->setIcon(QIcon(":/icons/scan.svg"));

// Carica stylesheet
QFile styleFile(":/styles/dark.qss");
styleFile.open(QFile::ReadOnly);
QString styleSheet = QLatin1String(styleFile.readAll());
qApp->setStyleSheet(styleSheet);
```

### Vantaggi Qt Designer

✅ **WYSIWYG**: vedi come apparirà l'UI in tempo reale
✅ **Rapid prototyping**: modifica veloce layout senza ricompilare
✅ **Separazione**: UI design separato da logica business
✅ **Manutenibilità**: modifiche UI senza toccare codice C++
✅ **Collaborazione**: designer UI può lavorare separatamente
✅ **Preview**: anteprima su diverse piattaforme/stili

### Limitazioni Qt Designer

❌ **Widget custom complessi**: vanno aggiunti in codice
❌ **Logica dinamica**: popolazione dati runtime in C++
❌ **Animazioni**: devono essere implementate in codice
❌ **Stili avanzati**: QSS complessi meglio in file separati

---

## Implementazione Tecnica

### Network Operations

#### ICMP Ping Implementation
Opzioni:
1. **QProcess + system ping** (semplice, cross-platform)
   ```cpp
   QProcess ping;
   ping.start("ping", {"-n", "4", host});
   // Parse output
   ```

2. **Raw ICMP sockets** (richiede privilegi admin/root)
   ```cpp
   QUdpSocket socket;
   // Construct ICMP packet
   // Send and wait for reply
   // Calculate RTT
   ```

3. **Libreria Qt ICMP** (custom implementation)

#### Port Scanning
```cpp
QTcpSocket socket;
connect(&socket, &QTcpSocket::connected, [&](){
    // Port open
});
socket.connectToHost(ip, port);
socket.waitForConnected(timeout);
```

#### ARP Resolution
- Windows: system("arp -a")
- Linux: read /proc/net/arp
- Raw ARP packets (advanced)

#### Packet Loss Calculation
```cpp
sent = total_packets;
received = successful_responses;
loss_percentage = ((sent - received) / sent) * 100;
```

#### Jitter Calculation
```cpp
std::vector<double> rtt_values;
// Collect RTT samples
double mean = calculate_mean(rtt_values);
double variance = calculate_variance(rtt_values, mean);
double jitter = std::sqrt(variance);
```

### Multi-threading Strategy

```cpp
// Scan worker thread
class ScanWorker : public QObject {
    Q_OBJECT
public slots:
    void doScan(QString subnet);
signals:
    void deviceFound(Device device);
    void scanProgress(int percentage);
    void scanComplete();
};

// In main thread
QThread* thread = new QThread;
ScanWorker* worker = new ScanWorker;
worker->moveToThread(thread);
connect(thread, &QThread::started, worker, &ScanWorker::doScan);
connect(worker, &ScanWorker::deviceFound, this, &MainWindow::onDeviceFound);
thread->start();
```

### Data Models

```cpp
class Device {
public:
    QString ipAddress;
    QString hostname;
    QString macAddress;
    QString vendor;
    QList<int> openPorts;
    NetworkMetrics metrics;
    QDateTime lastSeen;
    bool isOnline;
    DeviceType type;
};

class NetworkMetrics {
public:
    double latency_min;
    double latency_avg;
    double latency_max;
    double jitter;
    double packetLoss;
    int qualityScore;
    QVector<double> latencyHistory;
};
```

---

## Roadmap Sviluppo (SRP-Based)

La roadmap è organizzata per **layer architetturali** e **priorità funzionali**, seguendo il principio bottom-up: prima i layer infrastrutturali, poi la business logic, infine la UI.

---

### Fase 0: Foundation & Infrastructure (Settimana 1-2)
**Obiettivo**: Setup progetto, infrastruttura base, layer fondamentali

#### 0.1 Project Setup
- [ ] Inizializzare repository Git
- [ ] Setup CMakeLists.txt base con Qt6
- [ ] Creare struttura directory (src/, ui/, resources/, tests/)
- [ ] Configurare Qt Designer workflow
- [ ] Setup CI/CD (opzionale: GitHub Actions)

#### 0.2 Models Layer (Data Entities)
- [ ] `Device.h/cpp` - Entità dispositivo
- [ ] `NetworkMetrics.h/cpp` - Entità metriche
- [ ] `PortInfo.h/cpp` - Info porta
- [ ] `NetworkInterface.h/cpp` - Interfaccia di rete
- [ ] **Test**: DeviceTest, NetworkMetricsTest

#### 0.3 Utils Layer
- [ ] `Logger.h/cpp` - Sistema logging
- [ ] `IpAddressValidator.h/cpp` - Validazione IP
- [ ] `StringFormatter.h/cpp` - Formattazione stringhe
- [ ] `TimeFormatter.h/cpp` - Formattazione date
- [ ] `StatisticsCalculator.h/cpp` - Statistiche generiche
- [ ] **Test**: IpAddressValidatorTest, StatisticsCalculatorTest

#### 0.4 Interfaces Layer
- [ ] `IScanStrategy.h` - Interface strategia scan
- [ ] `IMetricsCalculator.h` - Interface calcolatori
- [ ] `IExporter.h` - Interface export
- [ ] `IDeviceRepository.h` - Interface repository

**Deliverable Fase 0**: Infrastruttura base funzionante, modelli dati, utilities testate

---

### Fase 1: Network Layer & Core Discovery (Settimana 3-4)
**Obiettivo**: Implementare funzionalità networking di base e discovery dispositivi

#### 1.1 Network Services
- [ ] `SubnetCalculator.h/cpp` - Calcoli subnet/CIDR
- [ ] `NetworkInterfaceDetector.h/cpp` - Rilevamento interfacce locali
- [ ] `MacVendorLookup.h/cpp` - Lookup vendor (OUI database)
- [ ] `PortServiceMapper.h/cpp` - Mapping porta→servizio
- [ ] **Test**: SubnetCalculatorTest, NetworkInterfaceDetectorTest

#### 1.2 Socket Management
- [ ] `TcpSocketManager.h/cpp` - Gestione socket TCP
- [ ] `UdpSocketManager.h/cpp` - Gestione socket UDP
- [ ] **Test**: Mock socket tests

#### 1.3 Discovery Services
- [ ] `HostDiscovery.h/cpp` - Ping-based discovery (QProcess)
- [ ] `DnsResolver.h/cpp` - Risoluzione DNS
- [ ] `ArpDiscovery.h/cpp` - Risoluzione ARP
- [ ] **Test**: HostDiscoveryTest, DnsResolverTest

#### 1.4 Basic Scanning
- [ ] `IpScanner.h/cpp` - Scansione IP range
- [ ] `ScanStrategy.h/cpp` - Strategia quick scan
- [ ] **Test**: IpScannerTest

**Deliverable Fase 1**: Capacità di scansionare una subnet e scoprire dispositivi con IP, hostname, MAC

---

### Fase 2: Metrics & Diagnostics Core (Settimana 5-6)
**Obiettivo**: Implementare metriche di rete (latenza, jitter, packet loss)

#### 2.1 Ping Service
- [ ] `PingService.h/cpp` - Esecuzione ping (QProcess wrapper)
- [ ] Parsing output ping (cross-platform)
- [ ] **Test**: PingServiceTest con mock data

#### 2.2 Metrics Calculators
- [ ] `LatencyCalculator.h/cpp` - Calcolo latenza (min/avg/max/median)
- [ ] `JitterCalculator.h/cpp` - Calcolo jitter (std deviation)
- [ ] `PacketLossCalculator.h/cpp` - Calcolo packet loss %
- [ ] `QualityScoreCalculator.h/cpp` - Score qualità connessione
- [ ] **Test**: LatencyCalculatorTest, JitterCalculatorTest, PacketLossCalculatorTest, QualityScoreCalculatorTest

#### 2.3 Metrics Aggregation
- [ ] `MetricsAggregator.h/cpp` - Aggregazione metriche
- [ ] Continuous ping tracking (history)
- [ ] **Test**: MetricsAggregatorTest con DI

#### 2.4 Port Scanning
- [ ] `PortScanner.h/cpp` - Scansione porte TCP
- [ ] Quick scan (porte comuni)
- [ ] Custom port ranges
- [ ] **Test**: PortScannerTest

**Deliverable Fase 2**: Capacità di misurare latenza, jitter, packet loss per ogni dispositivo + port scan

---

### Fase 3: Persistence & Data Management (Settimana 7)
**Obiettivo**: Persistenza dati, cache, export

#### 3.1 Repository Pattern
- [ ] `DeviceRepository.h/cpp` - Persistenza dispositivi (SQLite)
- [ ] `DeviceCache.h/cpp` - Cache in-memory
- [ ] Database schema creation
- [ ] **Test**: DeviceRepositoryTest (in-memory DB)

#### 3.2 Export Functionality
- [ ] `ExportStrategy.h` - Interface export
- [ ] `CsvExporter.h/cpp` - Export CSV
- [ ] `JsonExporter.h/cpp` - Export JSON
- [ ] **Test**: CsvExporterTest, JsonExporterTest

#### 3.3 Settings Management
- [ ] `SettingsManager.h/cpp` - QSettings wrapper
- [ ] `ConfigValidator.h/cpp` - Validazione config
- [ ] **Test**: SettingsManagerTest

**Deliverable Fase 3**: Dati persistono, export CSV/JSON funzionante

---

### Fase 4: Application Layer & Controllers (Settimana 8-9)
**Obiettivo**: Orchestrazione business logic, coordinamento operazioni

#### 4.1 Core Coordinators
- [ ] `ScanCoordinator.h/cpp` - Coordina scansioni multiple
- [ ] Multi-threading con QThread
- [ ] Progress tracking
- [ ] **Test**: ScanCoordinatorTest

#### 4.2 Controllers
- [ ] `ScanController.h/cpp` - Controlla workflow scansione
- [ ] `MetricsController.h/cpp` - Controlla raccolta metriche
- [ ] `ExportController.h/cpp` - Controlla export
- [ ] **Test**: Integration tests per controllers

#### 4.3 Management Services
- [ ] `ProfileManager.h/cpp` - Gestione profili scan
- [ ] `FavoritesManager.h/cpp` - Gestione preferiti
- [ ] **Test**: ProfileManagerTest

**Deliverable Fase 4**: Business logic completa, workflow orchestrati, threading funzionante

---

### Fase 5: UI Foundation & Basic Views (Settimana 10-11)
**Obiettivo**: UI base funzionante con Qt Designer, pattern MVVM

#### 5.1 Qt Designer Files (.ui)
- [ ] `mainwindow.ui` - Layout finestra principale
- [ ] `scanconfigdialog.ui` - Dialog configurazione scan
- [ ] `devicetablewidget.ui` - Widget tabella dispositivi
- [ ] Design base con placeholder widgets

#### 5.2 ViewModels (Presentation Logic)
- [ ] `DeviceTableViewModel.h/cpp` - ViewModel tabella dispositivi (QAbstractTableModel)
- [ ] Custom delegates per rendering celle
- [ ] **Test**: DeviceTableViewModelTest con mock repository

#### 5.3 Main Window & Basic UI
- [ ] `MainWindow.h/cpp` - Finestra principale (view)
- [ ] Menu bar, toolbar, status bar
- [ ] Connessioni signals/slots
- [ ] Integration con ScanController

#### 5.4 Device Table Widget
- [ ] `DeviceTableWidget.h/cpp` - Widget tabella (view)
- [ ] QTableView setup
- [ ] Context menu (ping, dettagli, rimuovi)
- [ ] Sorting, filtering

#### 5.5 Scan Config Dialog
- [ ] `ScanConfigDialog.h/cpp` - Dialog config (view)
- [ ] `ScanConfigViewModel.h/cpp` - ViewModel
- [ ] Input IP range, scan type, threads

**Deliverable Fase 5**: UI base funzionante, scansione avviabile da GUI, tabella dispositivi popolata

---

### Fase 6: Charts & Metrics Visualization (Settimana 12)
**Obiettivo**: Visualizzazione grafici metriche con QtCharts

#### 6.1 QtCharts Integration
- [ ] Setup QtCharts in CMake
- [ ] `ChartViewModel.h/cpp` - ViewModel per grafici

#### 6.2 Chart Widgets
- [ ] `LatencyChart.h/cpp` - Grafico latenza (QLineSeries)
- [ ] `PacketLossChart.h/cpp` - Grafico packet loss (QBarSeries)
- [ ] `JitterChart.h/cpp` - Grafico jitter (QSplineSeries)
- [ ] Real-time update dei grafici

#### 6.3 Metrics Widget
- [ ] `metricswidget.ui` - Layout widget metriche
- [ ] `MetricsWidget.h/cpp` - Widget view
- [ ] `MetricsViewModel.h/cpp` - ViewModel
- [ ] Integration con MetricsController

**Deliverable Fase 6**: Grafici latenza/jitter/packet loss real-time funzionanti

---

### Fase 7: Advanced Diagnostics (Settimana 13-14)
**Obiettivo**: Funzionalità diagnostiche avanzate

#### 7.1 Traceroute
- [ ] `TraceRouteService.h/cpp` - Esecuzione traceroute
- [ ] `TraceRouteHop.h/cpp` - Model hop
- [ ] Parsing output traceroute
- [ ] **Test**: TraceRouteServiceTest

#### 7.2 Advanced Diagnostics
- [ ] `MtuDiscovery.h/cpp` - Path MTU discovery
- [ ] `BandwidthTester.h/cpp` - Test bandwidth
- [ ] `DnsDiagnostics.h/cpp` - Diagnostica DNS avanzata
- [ ] **Test**: MtuDiscoveryTest

#### 7.3 Monitoring Service
- [ ] `MonitoringService.h/cpp` - Monitoring continuo
- [ ] `AlertService.h/cpp` - Generazione alert
- [ ] `HistoryService.h/cpp` - Gestione storico
- [ ] **Test**: MonitoringServiceTest

#### 7.4 Device Detail Dialog
- [ ] `devicedetaildialog.ui` - Layout dialog dettagli
- [ ] `DeviceDetailDialog.h/cpp` - Dialog view
- [ ] Tabs: Overview, Ports, Metrics, History
- [ ] Live metrics update

**Deliverable Fase 7**: Traceroute, MTU discovery, bandwidth test, monitoring continuo

---

### Fase 8: Advanced Features (Settimana 15-16)
**Obiettivo**: Funzionalità avanzate (WoL, export avanzato, profili)

#### 8.1 Wake-on-LAN
- [ ] `WakeOnLanPacket.h/cpp` - Magic packet builder
- [ ] Send WoL da device context menu
- [ ] Saved devices per WoL rapido

#### 8.2 Advanced Export
- [ ] `XmlExporter.h/cpp` - Export XML
- [ ] `HtmlReportGenerator.h/cpp` - Report HTML completo
- [ ] Template HTML con statistiche

#### 8.3 Profile & Favorites
- [ ] `ScanProfile.h/cpp` - Model profilo scan
- [ ] Save/Load profili
- [ ] Favorites UI integration

#### 8.4 History & Database
- [ ] `HistoryDao.h/cpp` - DAO history eventi
- [ ] `MetricsDao.h/cpp` - DAO metriche storiche
- [ ] Visualizzazione trend temporali

#### 8.5 Settings Dialog
- [ ] `settingsdialog.ui` - Layout impostazioni
- [ ] `SettingsDialog.h/cpp` - Dialog view
- [ ] Tabs: General, Network, Appearance, Notifications

**Deliverable Fase 8**: WoL, export completo, profili salvabili, history database

---

### Fase 9: UI Polish & Theming (Settimana 17)
**Obiettivo**: UI moderna, temi, custom widgets

#### 9.1 Theme System
- [ ] `ThemeManager.h/cpp` - Gestione temi
- [ ] `dark.qss` - Stylesheet dark mode
- [ ] `light.qss` - Stylesheet light mode
- [ ] Theme switcher in settings

#### 9.2 Custom Widgets
- [ ] `QualityGauge.h/cpp` - Widget gauge circolare
- [ ] `StatusLed.h/cpp` - LED status animato
- [ ] `MiniSparkline.h/cpp` - Mini-grafico inline
- [ ] `IpRangePicker.h/cpp` - Selector range IP

#### 9.3 UI Enhancements
- [ ] Icons SVG (Material Design)
- [ ] Animazioni smooth (QPropertyAnimation)
- [ ] Tooltips dettagliati
- [ ] Responsive layout migliorato

#### 9.4 Localization
- [ ] Setup Qt Linguist
- [ ] `lanscan_it.ts` - Traduzioni italiano
- [ ] `lanscan_en.ts` - Traduzioni inglese
- [ ] Runtime language switching

**Deliverable Fase 9**: UI moderna con dark/light theme, custom widgets, localizzazione

---

### Fase 10: Testing & Quality Assurance (Settimana 18-19)
**Obiettivo**: Test completi, quality assurance, bug fixing

#### 10.1 Complete Unit Tests
- [ ] Test coverage per tutti i calculator (100%)
- [ ] Test coverage network services (>80%)
- [ ] Test coverage repositories (100%)
- [ ] Mock objects per tutte le dependencies

#### 10.2 Integration Tests
- [ ] `ScanWorkflowTest` - Test completo workflow scan
- [ ] `MetricsCollectionTest` - Test raccolta metriche
- [ ] `ExportWorkflowTest` - Test export end-to-end
- [ ] `DatabaseIntegrationTest` - Test persistenza

#### 10.3 UI Tests
- [ ] Test ViewModels (DeviceTableViewModel, MetricsViewModel)
- [ ] Test signal/slot connections
- [ ] User interaction flows

#### 10.4 Performance & Optimization
- [ ] Profiling (Valgrind, Qt Creator Profiler)
- [ ] Memory leak detection
- [ ] Thread pool optimization
- [ ] Database query optimization
- [ ] Large network handling (1000+ devices)

#### 10.5 Cross-Platform Testing
- [ ] Test su Windows 10/11
- [ ] Test su Linux (Ubuntu, Fedora)
- [ ] Test su macOS (opzionale)
- [ ] Privilege handling (admin/root per raw sockets)

**Deliverable Fase 10**: Test coverage >85%, bug critici risolti, performance ottimizzate

---

### Fase 11: Documentation & Release (Settimana 20)
**Obiettivo**: Documentazione, packaging, release

#### 11.1 Code Documentation
- [ ] Doxygen comments per tutte le classi pubbliche
- [ ] Generazione documentazione API
- [ ] Architecture diagram (UML)

#### 11.2 User Documentation
- [ ] README.md completo
- [ ] User manual (Markdown o PDF)
- [ ] Quick start guide
- [ ] FAQ
- [ ] Screenshots e video tutorial

#### 11.3 Packaging
- [ ] Windows installer (NSIS o WiX)
- [ ] Linux AppImage
- [ ] macOS DMG (opzionale)
- [ ] Versioning e changelog

#### 11.4 Release
- [ ] Tag v1.0.0 su Git
- [ ] GitHub Release con binari
- [ ] Pubblicazione (opzionale: SourceForge, website)

**Deliverable Fase 11**: Release 1.0 pronta per distribuzione

---

## Priorità Sviluppo

### P0 - Critico (MVP)
- Fase 0, 1, 2: Foundation + Network + Metrics
- Fase 5 (UI base)
→ **Risultato**: App funzionante per scan+metriche base

### P1 - Alta (Core Features)
- Fase 3, 4: Persistence + Controllers
- Fase 6: Charts
→ **Risultato**: App completa per uso produttivo

### P2 - Media (Advanced)
- Fase 7, 8: Diagnostics avanzate + Features
→ **Risultato**: Funzionalità professionali

### P3 - Bassa (Polish)
- Fase 9: UI polish + Theming
→ **Risultato**: UX professionale

### P4 - Finale (Quality)
- Fase 10, 11: Testing + Release
→ **Risultato**: Prodotto production-ready

---

## Milestones

| Milestone | Fase | Settimana | Deliverable |
|-----------|------|-----------|-------------|
| **M1: Foundation** | 0 | 2 | Infrastruttura base |
| **M2: Discovery** | 1 | 4 | Scan subnet funzionante |
| **M3: Metrics** | 2 | 6 | Latenza/jitter/loss |
| **M4: Persistence** | 3 | 7 | Database + export |
| **M5: Business Logic** | 4 | 9 | Controllers + threading |
| **M6: MVP UI** | 5 | 11 | UI base funzionante |
| **M7: Visualization** | 6 | 12 | Grafici real-time |
| **M8: Diagnostics** | 7 | 14 | Traceroute + monitoring |
| **M9: Advanced** | 8 | 16 | WoL + profili + history |
| **M10: Polish** | 9 | 17 | Temi + custom widgets |
| **M11: Quality** | 10 | 19 | Test + ottimizzazioni |
| **M12: Release** | 11 | 20 | v1.0.0 pubblicata |

---

## Development Strategy

### Approccio Bottom-Up
1. **Infrastructure First**: Models, Utils, Interfaces
2. **Core Business Logic**: Network, Discovery, Metrics
3. **Data Layer**: Repository, Persistence, Export
4. **Application Layer**: Controllers, Services, Orchestration
5. **Presentation Layer**: ViewModels, Views, UI

### Test-Driven Development (TDD)
- Scrivere test **prima** dell'implementazione (quando possibile)
- Red-Green-Refactor cycle
- Unit test per ogni classe SRP
- Integration test per workflow

### Continuous Integration
- Commit frequenti su feature branches
- Pull request con code review
- Automated testing su ogni commit
- Build automatico

---

## Git Workflow & Jira Integration

### Git Repository Setup

#### Inizializzazione Repository

```bash
# Naviga nella directory del progetto
cd C:\Users\paolo\Documents\SwDev\LanScan

# Inizializza repository Git
git init

# Crea branch principale
git checkout -b main

# Configura informazioni utente (se non già fatto globalmente)
git config user.name "Paolo"
git config user.email "tua.email@example.com"
```

#### Struttura .gitignore

Crea file `.gitignore` nella root del progetto:

```gitignore
# Build directories
build/
build-*/
cmake-build-*/
out/

# Qt generated files
ui_*.h
moc_*.cpp
moc_*.h
qrc_*.cpp
*.autosave

# IDE files
.vscode/
.idea/
*.pro.user
*.pro.user.*

# Compiled files
*.o
*.obj
*.exe
*.dll
*.so
*.dylib
*.a
*.lib

# OS files
.DS_Store
Thumbs.db
desktop.ini

# Temporary files
*.tmp
*.log
*.swp
*.swo
*~

# Database files (development)
*.db
*.sqlite
*.sqlite3

# Test coverage
*.gcov
*.gcda
*.gcno
coverage/

# Package files
*.zip
*.tar.gz
*.dmg
*.deb
*.rpm

# User settings
.user.ini
settings.ini
```

### Git Branch Strategy (GitFlow)

#### Branch Types

```
main (o master)
├── develop
│   ├── feature/JIRA-123-ip-scanner
│   ├── feature/JIRA-124-latency-calculator
│   ├── feature/JIRA-125-device-table-ui
│   └── ...
├── release/v1.0.0
└── hotfix/JIRA-200-critical-bug
```

**Branch Naming Convention**:
- `feature/JIRA-XXX-short-description` - Nuove funzionalità
- `bugfix/JIRA-XXX-short-description` - Bug fix
- `hotfix/JIRA-XXX-short-description` - Fix critici su production
- `release/vX.Y.Z` - Preparazione release
- `develop` - Branch di sviluppo principale
- `main` - Branch production (solo release stabili)

#### Workflow Completo

```bash
# 1. Crea branch develop
git checkout -b develop

# 2. Per ogni nuova feature (es. JIRA-123)
git checkout develop
git pull origin develop
git checkout -b feature/JIRA-123-ip-scanner

# 3. Sviluppa e committa
git add src/core/scanning/IpScanner.h src/core/scanning/IpScanner.cpp
git commit -m "JIRA-123: Implement IpScanner class

- Add IpScanner.h/cpp with subnet scanning logic
- Implement CIDR notation parsing
- Add multithreading support
- Unit tests: IpScannerTest"

# 4. Push branch su remote
git push -u origin feature/JIRA-123-ip-scanner

# 5. Crea Pull Request (su GitHub/GitLab/Bitbucket)
# (via interfaccia web)

# 6. Dopo merge, elimina branch locale
git checkout develop
git pull origin develop
git branch -d feature/JIRA-123-ip-scanner
```

### Commit Message Convention

**Formato Standard** (collegato a Jira):

```
JIRA-XXX: Breve descrizione (max 50 caratteri)

Descrizione dettagliata del cambiamento (max 72 caratteri per riga):
- Cosa è stato fatto
- Perché è stato fatto
- Come funziona

Refs: JIRA-XXX
```

**Esempi**:

```bash
# Feature implementation
git commit -m "JIRA-45: Add LatencyCalculator class

Implements latency calculation with min/avg/max/median support.
Includes unit tests with 100% coverage.

Refs: JIRA-45"

# Bug fix
git commit -m "JIRA-78: Fix packet loss calculation rounding error

PacketLossCalculator was using integer division instead of float.
Changed calculation to use double precision.

Fixes: JIRA-78"

# Refactoring
git commit -m "JIRA-92: Refactor DeviceRepository to use dependency injection

Extracted IDeviceRepository interface to enable mocking in tests.
Updated all consumers to use interface instead of concrete class.

Refs: JIRA-92"

# Documentation
git commit -m "JIRA-105: Add Doxygen comments to network layer

Added comprehensive documentation for all public APIs in
network/services/ directory.

Refs: JIRA-105"
```

### Jira Project Setup

#### Struttura Progetto Jira

**Project Key**: `LANSCAN`

**Issue Types**:
- **Epic**: Funzionalità macro (es. "Network Scanning", "Metrics Dashboard")
- **Story**: User story (es. "As a user, I want to scan my LAN")
- **Task**: Compito tecnico (es. "Implement LatencyCalculator")
- **Bug**: Difetto da correggere
- **Sub-task**: Sotto-task di una story/task

#### Epics per Fase di Sviluppo

```
Epic: LANSCAN-1 - Foundation & Infrastructure (Fase 0)
Epic: LANSCAN-2 - Network Layer & Discovery (Fase 1)
Epic: LANSCAN-3 - Metrics & Diagnostics (Fase 2)
Epic: LANSCAN-4 - Persistence & Data Management (Fase 3)
Epic: LANSCAN-5 - Application Layer & Controllers (Fase 4)
Epic: LANSCAN-6 - UI Foundation (Fase 5)
Epic: LANSCAN-7 - Charts & Visualization (Fase 6)
Epic: LANSCAN-8 - Advanced Diagnostics (Fase 7)
Epic: LANSCAN-9 - Advanced Features (Fase 8)
Epic: LANSCAN-10 - UI Polish & Theming (Fase 9)
Epic: LANSCAN-11 - Testing & QA (Fase 10)
Epic: LANSCAN-12 - Documentation & Release (Fase 11)
```

#### Esempio Issue Structure per Fase 0

**Epic**: LANSCAN-1 - Foundation & Infrastructure

**Stories/Tasks**:
```
LANSCAN-10: Setup Git repository and CMake project
  Type: Task
  Priority: Highest
  Story Points: 2
  Sprint: Sprint 1
  Description: Initialize Git repo, create CMakeLists.txt, configure Qt6

LANSCAN-11: Create project directory structure
  Type: Task
  Priority: Highest
  Story Points: 1
  Sprint: Sprint 1

LANSCAN-12: Implement Device model class
  Type: Task
  Priority: High
  Story Points: 3
  Sprint: Sprint 1
  Acceptance Criteria:
    - Device.h/cpp created
    - Properties: ipAddress, hostname, macAddress, etc.
    - Unit test: DeviceTest with 100% coverage

LANSCAN-13: Implement NetworkMetrics model class
  Type: Task
  Priority: High
  Story Points: 2
  Sprint: Sprint 1

LANSCAN-14: Implement IpAddressValidator utility
  Type: Task
  Priority: High
  Story Points: 3
  Sprint: Sprint 1
  Acceptance Criteria:
    - Validates IPv4 addresses
    - Validates CIDR notation
    - Unit tests: IpAddressValidatorTest

LANSCAN-15: Implement Logger utility
  Type: Task
  Priority: Medium
  Story Points: 3
  Sprint: Sprint 1
  Acceptance Criteria:
    - Multiple log levels (DEBUG, INFO, WARN, ERROR)
    - File and console output
    - Thread-safe implementation
```

#### Custom Fields in Jira

- **Git Branch**: Campo testo per tracciare nome branch
- **Test Coverage**: Campo numerico per % coverage
- **Component**: Componente architetturale (Core, Network, UI, Persistence, etc.)
- **Phase**: Fase di sviluppo (0-11)

#### Workflow Jira

```
TODO → IN PROGRESS → CODE REVIEW → TESTING → DONE
  ↓         ↓              ↓           ↓
BLOCKED   BLOCKED      BLOCKED     BLOCKED
```

**Transizioni**:
1. **TODO → IN PROGRESS**: Quando inizi a lavorare, crei branch Git
2. **IN PROGRESS → CODE REVIEW**: Quando pushhi e crei PR
3. **CODE REVIEW → TESTING**: Dopo merge, inizia testing
4. **TESTING → DONE**: Test passano, issue completata
5. **ANY → BLOCKED**: Quando trovi un impedimento

### Git-Jira Integration

#### Smart Commits

Jira riconosce automaticamente riferimenti nei commit:

```bash
# Transizione automatica a "In Progress"
git commit -m "LANSCAN-45 #in-progress Implement LatencyCalculator"

# Aggiungere commento
git commit -m "LANSCAN-45 #comment Fixed edge case with empty data"

# Transizione a "Done" con tempo loggato
git commit -m "LANSCAN-45 #done #time 3h Completed LatencyCalculator with tests"

# Combinazioni multiple
git commit -m "LANSCAN-45 #in-progress #time 2h Started implementation
LANSCAN-46 #comment Added test data for validation"
```

#### Branch Linking

In Jira, aggiungi campo "Git Branch" e popolalo:

```
Issue: LANSCAN-123
Git Branch: feature/LANSCAN-123-ip-scanner
```

Questo permette di linkare direttamente issue→branch.

#### Pull Request Template

Crea file `.github/pull_request_template.md` (per GitHub):

```markdown
## Description
Brief description of changes

## Jira Issue
Closes LANSCAN-XXX

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Refactoring
- [ ] Documentation
- [ ] Tests

## Checklist
- [ ] Code follows SRP principles
- [ ] Unit tests added/updated
- [ ] All tests passing
- [ ] Documentation updated
- [ ] No compiler warnings
- [ ] Self-review completed

## Test Coverage
- Unit Tests: X%
- Integration Tests: (if applicable)

## Screenshots (if UI changes)
(add screenshots)
```

### Sprint Planning con Jira

#### Sprint Structure (2 settimane)

**Sprint 1 (Settimana 1-2)**: Fase 0 - Foundation
- LANSCAN-10: Setup progetto
- LANSCAN-11: Struttura directory
- LANSCAN-12: Device model
- LANSCAN-13: NetworkMetrics model
- LANSCAN-14: IpAddressValidator
- LANSCAN-15: Logger
- LANSCAN-16: Interfaces

**Sprint 2 (Settimana 3-4)**: Fase 1 - Network Layer
- LANSCAN-20: SubnetCalculator
- LANSCAN-21: NetworkInterfaceDetector
- LANSCAN-22: MacVendorLookup
- LANSCAN-23: TcpSocketManager
- LANSCAN-24: HostDiscovery
- LANSCAN-25: DnsResolver
- LANSCAN-26: IpScanner

... e così via

#### Story Points Estimation

**Fibonacci Sequence**: 1, 2, 3, 5, 8, 13

- **1 pt**: Molto semplice, <2h (es. aggiungere un campo al model)
- **2 pts**: Semplice, 2-4h (es. utility class con pochi metodi)
- **3 pts**: Media, 4-8h (es. classe SRP completa con test)
- **5 pts**: Complessa, 1-2 giorni (es. coordinator con threading)
- **8 pts**: Molto complessa, 2-3 giorni (es. UI completa con ViewModel)
- **13 pts**: Da suddividere in task più piccole

### Git Hooks per Jira

Crea file `.git/hooks/commit-msg` per validare commit messages:

```bash
#!/bin/bash

commit_msg_file=$1
commit_msg=$(cat "$commit_msg_file")

# Verifica che il commit inizi con LANSCAN-XXX
if ! echo "$commit_msg" | grep -qE "^LANSCAN-[0-9]+:"; then
    echo "ERROR: Commit message must start with 'LANSCAN-XXX:'"
    echo "Example: LANSCAN-45: Add LatencyCalculator class"
    exit 1
fi

# Verifica lunghezza prima riga (max 72 caratteri)
first_line=$(echo "$commit_msg" | head -n1)
if [ ${#first_line} -gt 72 ]; then
    echo "ERROR: First line must be ≤72 characters"
    exit 1
fi

exit 0
```

Rendi eseguibile:
```bash
chmod +x .git/hooks/commit-msg
```

### Jira Automation Rules

**Automazione 1: Auto-transition su PR creation**
```
Trigger: Commit created (con smart commit)
Condition: Commit message contains "#in-progress"
Action: Transition issue to "In Progress"
```

**Automazione 2: Notify su blocco**
```
Trigger: Issue transitioned to "Blocked"
Action: Send Slack/Email notification to team
```

**Automazione 3: Close issue su merge**
```
Trigger: Pull request merged
Condition: Branch name contains "LANSCAN-XXX"
Action: Transition issue to "Done"
```

### Jira Dashboard

**Dashboard Components**:

1. **Sprint Burndown Chart**: Progresso sprint corrente
2. **Velocity Chart**: Story points completati per sprint
3. **Cumulative Flow Diagram**: Distribuzione issue per stato
4. **Created vs Resolved**: Issue create vs risolte
5. **Epic Progress**: % completamento per epic/fase
6. **Component Breakdown**: Issue per componente architetturale
7. **Bug Trend**: Andamento bug nel tempo

### Reports Utili

**Jira Reports**:
- **Sprint Report**: Dettaglio sprint completato
- **Version Report**: Progresso verso release
- **Time Tracking Report**: Ore lavorate per issue
- **Issue Statistics**: Distribuzione per tipo/priorità/componente

### Backup & Repository Remote

#### Setup Remote Repository (GitHub/GitLab/Bitbucket)

```bash
# Aggiungi remote (esempio GitHub)
git remote add origin https://github.com/username/LanScan.git

# Push iniziale
git push -u origin main
git push -u origin develop

# Verifica remote
git remote -v
```

#### Protezione Branch

Su GitHub/GitLab, proteggi branch `main` e `develop`:
- Require pull request reviews (almeno 1 review)
- Require status checks to pass (CI/CD)
- Require linear history
- No force push
- No deletion

### Comandi Git Essenziali per Workflow

```bash
# Clona repository
git clone https://github.com/username/LanScan.git

# Vedi stato
git status

# Vedi differenze
git diff

# Vedi log con grafo
git log --oneline --graph --all

# Aggiorna develop
git checkout develop
git pull origin develop

# Crea feature branch
git checkout -b feature/LANSCAN-45-latency-calc

# Stage modifiche
git add src/core/metrics/LatencyCalculator.h
git add src/core/metrics/LatencyCalculator.cpp
git add tests/unit/core/metrics/LatencyCalculatorTest.cpp

# Commit con messaggio Jira
git commit -m "LANSCAN-45: Implement LatencyCalculator

Add LatencyCalculator class with min/avg/max/median calculation.
Includes comprehensive unit tests with 100% coverage.

Refs: LANSCAN-45"

# Push
git push -u origin feature/LANSCAN-45-latency-calc

# Merge develop in feature (se serve)
git checkout feature/LANSCAN-45-latency-calc
git merge develop

# Rebase su develop (alternativa a merge)
git rebase develop

# Squash commits prima del merge
git rebase -i HEAD~3

# Elimina branch dopo merge
git branch -d feature/LANSCAN-45-latency-calc
git push origin --delete feature/LANSCAN-45-latency-calc
```

### Template Issue Jira

#### Task Template

```
Summary: Implement [ClassName]

Description:
Implement [ClassName] following SRP principles.

Acceptance Criteria:
- [ ] Class created in correct directory
- [ ] Header (.h) and implementation (.cpp) files
- [ ] Public API documented with Doxygen comments
- [ ] Unit tests created with ≥90% coverage
- [ ] All tests passing
- [ ] Code reviewed

Technical Details:
- Location: src/[layer]/[module]/
- Dependencies: [list dependencies]
- Interfaces: [implements which interface?]

Definition of Done:
- Code committed to feature branch
- Pull request created and approved
- Tests passing in CI
- Merged to develop
```

#### Bug Template

```
Summary: [Brief description of bug]

Environment:
- OS: Windows 11 / Ubuntu 22.04
- Qt Version: 6.x
- Build: Debug / Release

Steps to Reproduce:
1.
2.
3.

Expected Behavior:
[What should happen]

Actual Behavior:
[What actually happens]

Logs/Screenshots:
[Attach relevant logs or screenshots]

Severity: Critical / High / Medium / Low

Acceptance Criteria:
- [ ] Bug reproduced
- [ ] Root cause identified
- [ ] Fix implemented
- [ ] Regression test added
- [ ] Verified on affected platforms
```

---

### Quick Start Guide

**Setup Completo in 10 Passi**:

1. **Inizializza Git**:
   ```bash
   cd C:\Users\paolo\Documents\SwDev\LanScan
   git init
   git checkout -b main
   ```

2. **Crea .gitignore** (copia template sopra)

3. **Primo commit**:
   ```bash
   git add .gitignore
   git commit -m "LANSCAN-1: Initial commit with .gitignore"
   ```

4. **Crea branch develop**:
   ```bash
   git checkout -b develop
   ```

5. **Setup remote** (dopo aver creato repo su GitHub/GitLab):
   ```bash
   git remote add origin <URL>
   git push -u origin main
   git push -u origin develop
   ```

6. **Crea progetto Jira** con key `LANSCAN`

7. **Crea Epics** per ogni fase (LANSCAN-1 a LANSCAN-12)

8. **Popola backlog** con task per Fase 0

9. **Crea Sprint 1** e assegna task

10. **Inizia sviluppo**:
    ```bash
    git checkout develop
    git checkout -b feature/LANSCAN-10-setup-project
    # ... sviluppa ...
    git commit -m "LANSCAN-10: Setup CMake project"
    git push -u origin feature/LANSCAN-10-setup-project
    ```

---

## Build & Deployment

### CMakeLists.txt Completo

```cmake
cmake_minimum_required(VERSION 3.16)
project(LanScan VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Abilita auto-generazione per Qt
set(CMAKE_AUTOMOC ON)      # Meta-Object Compiler
set(CMAKE_AUTOUIC ON)      # User Interface Compiler (per .ui files)
set(CMAKE_AUTORCC ON)      # Resource Compiler (per .qrc files)

# Directory dove cercare i file .ui
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_SOURCE_DIR}/ui)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Widgets
    Network
    Charts
)

# File sorgenti
set(PROJECT_SOURCES
    src/main.cpp

    # UI Classes
    src/ui/MainWindow.h
    src/ui/MainWindow.cpp

    src/ui/ScanConfigDialog.h
    src/ui/ScanConfigDialog.cpp

    src/ui/DeviceDetailDialog.h
    src/ui/DeviceDetailDialog.cpp

    src/ui/SettingsDialog.h
    src/ui/SettingsDialog.cpp

    src/ui/MetricsWidget.h
    src/ui/MetricsWidget.cpp

    # Core classes
    src/core/NetworkScanner.h
    src/core/NetworkScanner.cpp

    src/core/DeviceManager.h
    src/core/DeviceManager.cpp

    src/core/PingEngine.h
    src/core/PingEngine.cpp

    # ... altri file
)

# File .ui (opzionale, vengono trovati automaticamente da AUTOUIC)
set(PROJECT_UI
    ui/mainwindow.ui
    ui/scanconfigdialog.ui
    ui/devicedetaildialog.ui
    ui/settingsdialog.ui
    ui/metricswidget.ui
)

# File resources (icone, stili, database)
set(PROJECT_RESOURCES
    resources/resources.qrc
)

add_executable(LanScan
    ${PROJECT_SOURCES}
    ${PROJECT_UI}
    ${PROJECT_RESOURCES}
)

target_link_libraries(LanScan
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
    Qt6::Charts
)

# Include directory per i file generati
target_include_directories(LanScan PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}  # Per ui_*.h files auto-generati
)

install(TARGETS LanScan
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
```

**Note:**
- `CMAKE_AUTOUIC=ON` abilita la compilazione automatica dei file `.ui`
- I file `ui_*.h` vengono generati nella directory `build/`
- CMake rileva automaticamente le dipendenze tra `.ui` e `.cpp`

### Requisiti Sistema
- **Windows**: Windows 10/11, Visual Studio 2019+ o MinGW
- **Linux**: Ubuntu 20.04+, GCC 9+
- **macOS**: macOS 11+, Xcode 12+
- **Qt**: Qt 6.2 o superiore
- **RAM**: 512 MB minimo
- **Disk**: 50 MB

---

## Note Sicurezza

⚠️ **Privilegi Amministratore**
- Raw socket operations richiedono privilegi elevati
- Port scanning può triggare firewall/IDS
- Implementare rate limiting per evitare flooding
- Disclaimer uso responsabile

⚠️ **Privacy**
- Non salvare dati sensibili in chiaro
- Opzione per disabilitare logging
- Conformità GDPR per deployment enterprise

---

## Testing Strategy

Con l'architettura SRP-compliant, i test di unità sono **essenziali e semplici** da scrivere poiché ogni classe ha una responsabilità ben definita.

### Framework di Testing
- **Qt Test**: Framework ufficiale Qt per unit/integration/UI tests
- **Google Test** (opzionale): Alternativa più moderna con rich assertions
- **Google Mock** (opzionale): Per mocking dipendenze

### Struttura Directory Test

```
tests/
├── unit/                           # Test di unità
│   ├── core/
│   │   ├── scanning/
│   │   │   ├── IpScannerTest.h/cpp
│   │   │   ├── PortScannerTest.h/cpp
│   │   │   └── ScanCoordinatorTest.h/cpp
│   │   ├── discovery/
│   │   │   ├── HostDiscoveryTest.h/cpp
│   │   │   ├── ArpDiscoveryTest.h/cpp
│   │   │   └── DnsResolverTest.h/cpp
│   │   ├── metrics/
│   │   │   ├── LatencyCalculatorTest.h/cpp
│   │   │   ├── JitterCalculatorTest.h/cpp
│   │   │   ├── PacketLossCalculatorTest.h/cpp
│   │   │   └── QualityScoreCalculatorTest.h/cpp
│   │   └── diagnostics/
│   │       ├── TraceRouteServiceTest.h/cpp
│   │       └── MtuDiscoveryTest.h/cpp
│   ├── network/
│   │   ├── SubnetCalculatorTest.h/cpp
│   │   ├── IpAddressValidatorTest.h/cpp
│   │   ├── MacVendorLookupTest.h/cpp
│   │   └── PortServiceMapperTest.h/cpp
│   ├── models/
│   │   ├── DeviceTest.h/cpp
│   │   └── NetworkMetricsTest.h/cpp
│   ├── persistence/
│   │   ├── CsvExporterTest.h/cpp
│   │   ├── JsonExporterTest.h/cpp
│   │   └── DeviceRepositoryTest.h/cpp
│   └── utils/
│       ├── StatisticsCalculatorTest.h/cpp
│       └── StringFormatterTest.h/cpp
│
├── integration/                    # Test di integrazione
│   ├── ScanWorkflowTest.h/cpp     # Test workflow completo scansione
│   ├── MetricsCollectionTest.h/cpp
│   ├── DatabaseIntegrationTest.h/cpp
│   └── ExportWorkflowTest.h/cpp
│
├── ui/                            # Test UI
│   ├── MainWindowTest.h/cpp
│   ├── DeviceTableViewModelTest.h/cpp
│   └── ScanConfigDialogTest.h/cpp
│
├── mocks/                         # Mock objects
│   ├── MockDeviceRepository.h
│   ├── MockPingService.h
│   ├── MockNetworkInterface.h
│   └── MockExporter.h
│
└── testdata/                      # Dati di test
    ├── sample_devices.json
    ├── mock_ping_responses.txt
    └── oui_sample.txt
```

### Unit Tests - Esempi Concreti

#### 1. Test Calcolatori Metriche (Core Logic)

**LatencyCalculatorTest.cpp**
```cpp
#include <QtTest/QtTest>
#include "core/metrics/LatencyCalculator.h"

class LatencyCalculatorTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase() {
        // Setup eseguito una volta prima di tutti i test
        calculator = new LatencyCalculator();
    }

    void cleanupTestCase() {
        // Cleanup dopo tutti i test
        delete calculator;
    }

    void testCalculateAverage() {
        // Arrange
        QVector<double> rttValues = {10.5, 12.3, 11.8, 10.2, 13.1};

        // Act
        double result = calculator->calculateAverage(rttValues);

        // Assert
        QCOMPARE(result, 11.58);
    }

    void testCalculateMin() {
        QVector<double> rttValues = {10.5, 12.3, 11.8, 10.2, 13.1};
        QCOMPARE(calculator->calculateMin(rttValues), 10.2);
    }

    void testCalculateMax() {
        QVector<double> rttValues = {10.5, 12.3, 11.8, 10.2, 13.1};
        QCOMPARE(calculator->calculateMax(rttValues), 13.1);
    }

    void testEmptyVector() {
        QVector<double> emptyValues;
        QCOMPARE(calculator->calculateAverage(emptyValues), 0.0);
    }

    void testSingleValue() {
        QVector<double> singleValue = {15.5};
        QCOMPARE(calculator->calculateAverage(singleValue), 15.5);
    }

private:
    LatencyCalculator* calculator;
};

QTEST_MAIN(LatencyCalculatorTest)
#include "LatencyCalculatorTest.moc"
```

**JitterCalculatorTest.cpp**
```cpp
#include <QtTest/QtTest>
#include "core/metrics/JitterCalculator.h"

class JitterCalculatorTest : public QObject
{
    Q_OBJECT

private slots:
    void testCalculateJitter() {
        // Arrange
        JitterCalculator calculator;
        QVector<double> rttValues = {10.0, 12.0, 11.0, 13.0, 10.5};

        // Act
        double jitter = calculator.calculate(rttValues);

        // Assert - jitter è la std deviation
        QVERIFY(jitter > 1.0 && jitter < 2.0);
    }

    void testZeroJitter() {
        JitterCalculator calculator;
        QVector<double> constantValues = {10.0, 10.0, 10.0, 10.0};

        double jitter = calculator.calculate(constantValues);

        QCOMPARE(jitter, 0.0);
    }

    void testHighJitter() {
        JitterCalculator calculator;
        QVector<double> volatileValues = {5.0, 50.0, 10.0, 45.0, 8.0};

        double jitter = calculator.calculate(volatileValues);

        QVERIFY(jitter > 15.0);  // Aspettati alta variabilità
    }
};

QTEST_MAIN(JitterCalculatorTest)
#include "JitterCalculatorTest.moc"
```

**PacketLossCalculatorTest.cpp**
```cpp
#include <QtTest/QtTest>
#include "core/metrics/PacketLossCalculator.h"

class PacketLossCalculatorTest : public QObject
{
    Q_OBJECT

private slots:
    void testNoPacketLoss() {
        PacketLossCalculator calculator;

        int sent = 100;
        int received = 100;

        double loss = calculator.calculate(sent, received);

        QCOMPARE(loss, 0.0);
    }

    void testPartialPacketLoss() {
        PacketLossCalculator calculator;

        int sent = 100;
        int received = 95;

        double loss = calculator.calculate(sent, received);

        QCOMPARE(loss, 5.0);  // 5% loss
    }

    void testCompletePacketLoss() {
        PacketLossCalculator calculator;

        int sent = 100;
        int received = 0;

        double loss = calculator.calculate(sent, received);

        QCOMPARE(loss, 100.0);
    }

    void testInvalidInput() {
        PacketLossCalculator calculator;

        // Sent = 0 dovrebbe gestire errore
        double loss = calculator.calculate(0, 0);

        QCOMPARE(loss, 0.0);  // o throw exception
    }
};

QTEST_MAIN(PacketLossCalculatorTest)
#include "PacketLossCalculatorTest.moc"
```

**QualityScoreCalculatorTest.cpp**
```cpp
#include <QtTest/QtTest>
#include "core/metrics/QualityScoreCalculator.h"

class QualityScoreCalculatorTest : public QObject
{
    Q_OBJECT

private slots:
    void testExcellentQuality() {
        QualityScoreCalculator calculator;

        // Latency bassa, no packet loss, no jitter
        NetworkMetrics metrics;
        metrics.latency_avg = 5.0;
        metrics.packetLoss = 0.0;
        metrics.jitter = 0.5;

        int score = calculator.calculate(metrics);

        QVERIFY(score >= 90);  // Excellent: 90-100
    }

    void testPoorQuality() {
        QualityScoreCalculator calculator;

        // Alta latenza, packet loss elevato
        NetworkMetrics metrics;
        metrics.latency_avg = 200.0;
        metrics.packetLoss = 10.0;
        metrics.jitter = 50.0;

        int score = calculator.calculate(metrics);

        QVERIFY(score < 50);  // Poor: 0-50
    }

    void testMediumQuality() {
        QualityScoreCalculator calculator;

        NetworkMetrics metrics;
        metrics.latency_avg = 50.0;
        metrics.packetLoss = 2.0;
        metrics.jitter = 10.0;

        int score = calculator.calculate(metrics);

        QVERIFY(score >= 50 && score < 80);  // Fair/Good
    }
};

QTEST_MAIN(QualityScoreCalculatorTest)
#include "QualityScoreCalculatorTest.moc"
```

#### 2. Test Network Utilities

**SubnetCalculatorTest.cpp**
```cpp
#include <QtTest/QtTest>
#include "network/services/SubnetCalculator.h"

class SubnetCalculatorTest : public QObject
{
    Q_OBJECT

private slots:
    void testCidrToRange() {
        SubnetCalculator calculator;

        auto range = calculator.cidrToRange("192.168.1.0/24");

        QCOMPARE(range.first, QString("192.168.1.0"));
        QCOMPARE(range.last, QString("192.168.1.255"));
        QCOMPARE(range.count, 256);
    }

    void testSmallSubnet() {
        SubnetCalculator calculator;

        auto range = calculator.cidrToRange("192.168.1.0/30");

        QCOMPARE(range.count, 4);
        QCOMPARE(range.first, QString("192.168.1.0"));
        QCOMPARE(range.last, QString("192.168.1.3"));
    }

    void testIsInSubnet() {
        SubnetCalculator calculator;

        QVERIFY(calculator.isInSubnet("192.168.1.50", "192.168.1.0/24"));
        QVERIFY(!calculator.isInSubnet("192.168.2.50", "192.168.1.0/24"));
    }

    void testGetNetworkAddress() {
        SubnetCalculator calculator;

        QString network = calculator.getNetworkAddress("192.168.1.50", "255.255.255.0");

        QCOMPARE(network, QString("192.168.1.0"));
    }

    void testGetBroadcastAddress() {
        SubnetCalculator calculator;

        QString broadcast = calculator.getBroadcastAddress("192.168.1.0/24");

        QCOMPARE(broadcast, QString("192.168.1.255"));
    }
};

QTEST_MAIN(SubnetCalculatorTest)
#include "SubnetCalculatorTest.moc"
```

**IpAddressValidatorTest.cpp**
```cpp
#include <QtTest/QtTest>
#include "utils/IpAddressValidator.h"

class IpAddressValidatorTest : public QObject
{
    Q_OBJECT

private slots:
    void testValidIpv4() {
        IpAddressValidator validator;

        QVERIFY(validator.isValidIpv4("192.168.1.1"));
        QVERIFY(validator.isValidIpv4("10.0.0.1"));
        QVERIFY(validator.isValidIpv4("255.255.255.255"));
        QVERIFY(validator.isValidIpv4("0.0.0.0"));
    }

    void testInvalidIpv4() {
        IpAddressValidator validator;

        QVERIFY(!validator.isValidIpv4("256.1.1.1"));  // > 255
        QVERIFY(!validator.isValidIpv4("192.168.1"));  // Incompleto
        QVERIFY(!validator.isValidIpv4("192.168.1.1.1"));  // Troppi ottetti
        QVERIFY(!validator.isValidIpv4("abc.def.ghi.jkl"));  // Non numerico
        QVERIFY(!validator.isValidIpv4(""));  // Vuoto
    }

    void testValidCidr() {
        IpAddressValidator validator;

        QVERIFY(validator.isValidCidr("192.168.1.0/24"));
        QVERIFY(validator.isValidCidr("10.0.0.0/8"));
        QVERIFY(validator.isValidCidr("172.16.0.0/12"));
    }

    void testInvalidCidr() {
        IpAddressValidator validator;

        QVERIFY(!validator.isValidCidr("192.168.1.0/33"));  // > 32
        QVERIFY(!validator.isValidCidr("192.168.1.0"));  // Manca /
        QVERIFY(!validator.isValidCidr("192.168.1.0/"));  // Manca numero
    }
};

QTEST_MAIN(IpAddressValidatorTest)
#include "IpAddressValidatorTest.moc"
```

#### 3. Test con Mock Objects

**MetricsAggregatorTest.cpp** (usa Dependency Injection)
```cpp
#include <QtTest/QtTest>
#include "core/metrics/MetricsAggregator.h"
#include "tests/mocks/MockMetricsCalculator.h"

// Mock Calculator
class MockLatencyCalculator : public IMetricsCalculator {
public:
    double calculate(const QVector<double>& data) override {
        return 10.5;  // Valore fisso per test
    }
};

class MockJitterCalculator : public IMetricsCalculator {
public:
    double calculate(const QVector<double>& data) override {
        return 2.3;  // Valore fisso per test
    }
};

class MetricsAggregatorTest : public QObject
{
    Q_OBJECT

private slots:
    void testAggregate() {
        // Arrange: usa mock invece di implementazioni reali
        MockLatencyCalculator* latencyCalc = new MockLatencyCalculator();
        MockJitterCalculator* jitterCalc = new MockJitterCalculator();

        MetricsAggregator aggregator(latencyCalc, jitterCalc);

        QVector<double> rttData = {10.0, 11.0, 12.0};

        // Act
        NetworkMetrics result = aggregator.aggregate(rttData);

        // Assert: verifica che aggregator usi correttamente i calculator
        QCOMPARE(result.latency, 10.5);  // Dal mock
        QCOMPARE(result.jitter, 2.3);    // Dal mock

        delete latencyCalc;
        delete jitterCalc;
    }
};

QTEST_MAIN(MetricsAggregatorTest)
#include "MetricsAggregatorTest.moc"
```

#### 4. Test Repository Pattern

**DeviceRepositoryTest.cpp**
```cpp
#include <QtTest/QtTest>
#include "core/management/DeviceRepository.h"

class DeviceRepositoryTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase() {
        // Setup database di test
        repository = new DeviceRepository(":memory:");  // SQLite in-memory
    }

    void cleanupTestCase() {
        delete repository;
    }

    void testSaveAndFind() {
        // Arrange
        Device device;
        device.ipAddress = "192.168.1.100";
        device.hostname = "test-device";
        device.macAddress = "AA:BB:CC:DD:EE:FF";

        // Act
        repository->save(device);
        Device found = repository->findByIp("192.168.1.100");

        // Assert
        QCOMPARE(found.ipAddress, device.ipAddress);
        QCOMPARE(found.hostname, device.hostname);
        QCOMPARE(found.macAddress, device.macAddress);
    }

    void testFindAll() {
        repository->clear();

        // Salva 3 dispositivi
        for (int i = 1; i <= 3; i++) {
            Device device;
            device.ipAddress = QString("192.168.1.%1").arg(i);
            repository->save(device);
        }

        QList<Device> devices = repository->findAll();

        QCOMPARE(devices.size(), 3);
    }

    void testDelete() {
        Device device;
        device.ipAddress = "192.168.1.50";

        repository->save(device);
        QVERIFY(repository->exists("192.168.1.50"));

        repository->deleteByIp("192.168.1.50");
        QVERIFY(!repository->exists("192.168.1.50"));
    }

private:
    DeviceRepository* repository;
};

QTEST_MAIN(DeviceRepositoryTest)
#include "DeviceRepositoryTest.moc"
```

#### 5. Test Export/Import

**CsvExporterTest.cpp**
```cpp
#include <QtTest/QtTest>
#include "persistence/export/CsvExporter.h"
#include <QTemporaryFile>

class CsvExporterTest : public QObject
{
    Q_OBJECT

private slots:
    void testExport() {
        // Arrange
        CsvExporter exporter;

        QList<Device> devices;
        Device dev1;
        dev1.ipAddress = "192.168.1.1";
        dev1.hostname = "router";
        dev1.macAddress = "AA:BB:CC:DD:EE:FF";
        devices.append(dev1);

        QTemporaryFile tempFile;
        tempFile.open();

        // Act
        bool success = exporter.exportData(devices, tempFile.fileName());

        // Assert
        QVERIFY(success);
        QVERIFY(tempFile.size() > 0);

        // Verifica contenuto
        QString content = QString(tempFile.readAll());
        QVERIFY(content.contains("192.168.1.1"));
        QVERIFY(content.contains("router"));
    }

    void testExportEmpty() {
        CsvExporter exporter;
        QList<Device> empty;

        QTemporaryFile tempFile;
        tempFile.open();

        bool success = exporter.exportData(empty, tempFile.fileName());

        // Dovrebbe creare file con solo header
        QVERIFY(success);
    }
};

QTEST_MAIN(CsvExporterTest)
#include "CsvExporterTest.moc"
```

### Integration Tests

**ScanWorkflowTest.cpp**
```cpp
#include <QtTest/QtTest>
#include "controllers/ScanController.h"
#include "core/scanning/ScanCoordinator.h"

class ScanWorkflowTest : public QObject
{
    Q_OBJECT

private slots:
    void testFullScanWorkflow() {
        // Test workflow completo: scan -> discovery -> metrics
        ScanController controller;

        QSignalSpy spy(&controller, &ScanController::scanComplete);

        controller.startScan("192.168.1.0/29");  // Small subnet per test veloce

        // Aspetta completamento (max 10 secondi)
        QVERIFY(spy.wait(10000));

        QList<Device> devices = controller.getDiscoveredDevices();

        QVERIFY(devices.size() > 0);

        // Verifica che tutti i device abbiano IP validi
        for (const Device& dev : devices) {
            QVERIFY(!dev.ipAddress.isEmpty());
        }
    }
};

QTEST_MAIN(ScanWorkflowTest)
#include "ScanWorkflowTest.moc"
```

### UI Tests (ViewModel)

**DeviceTableViewModelTest.cpp**
```cpp
#include <QtTest/QtTest>
#include "viewmodels/DeviceTableViewModel.h"
#include "tests/mocks/MockDeviceRepository.h"

class DeviceTableViewModelTest : public QObject
{
    Q_OBJECT

private slots:
    void testRowCount() {
        MockDeviceRepository mockRepo;
        mockRepo.addMockDevices(5);

        DeviceTableViewModel viewModel(&mockRepo);
        viewModel.loadDevices();

        QCOMPARE(viewModel.rowCount(QModelIndex()), 5);
    }

    void testDataRetrieval() {
        MockDeviceRepository mockRepo;
        Device device;
        device.ipAddress = "192.168.1.1";
        device.hostname = "test";
        mockRepo.addDevice(device);

        DeviceTableViewModel viewModel(&mockRepo);
        viewModel.loadDevices();

        QModelIndex index = viewModel.index(0, 0);  // Prima riga, colonna IP
        QVariant data = viewModel.data(index, Qt::DisplayRole);

        QCOMPARE(data.toString(), QString("192.168.1.1"));
    }

    void testStatusColor() {
        MockDeviceRepository mockRepo;
        DeviceTableViewModel viewModel(&mockRepo);

        QString colorOnline = viewModel.getStatusColor(true);
        QString colorOffline = viewModel.getStatusColor(false);

        QCOMPARE(colorOnline, QString("#4CAF50"));  // Green
        QCOMPARE(colorOffline, QString("#F44336")); // Red
    }
};

QTEST_MAIN(DeviceTableViewModelTest)
#include "DeviceTableViewModelTest.moc"
```

### CMakeLists.txt per Tests

```cmake
# Abilita testing
enable_testing()

find_package(Qt6 REQUIRED COMPONENTS Test)

# Macro per creare test
function(add_lanscan_test test_name)
    add_executable(${test_name} ${ARGN})
    target_link_libraries(${test_name}
        Qt6::Test
        Qt6::Core
        LanScanLib  # Libreria condivisa con codice principale
    )
    add_test(NAME ${test_name} COMMAND ${test_name})
endfunction()

# Unit Tests - Core
add_lanscan_test(LatencyCalculatorTest tests/unit/core/metrics/LatencyCalculatorTest.cpp)
add_lanscan_test(JitterCalculatorTest tests/unit/core/metrics/JitterCalculatorTest.cpp)
add_lanscan_test(PacketLossCalculatorTest tests/unit/core/metrics/PacketLossCalculatorTest.cpp)
add_lanscan_test(QualityScoreCalculatorTest tests/unit/core/metrics/QualityScoreCalculatorTest.cpp)

# Unit Tests - Network
add_lanscan_test(SubnetCalculatorTest tests/unit/network/SubnetCalculatorTest.cpp)
add_lanscan_test(IpAddressValidatorTest tests/unit/utils/IpAddressValidatorTest.cpp)

# Unit Tests - Persistence
add_lanscan_test(CsvExporterTest tests/unit/persistence/CsvExporterTest.cpp)
add_lanscan_test(DeviceRepositoryTest tests/unit/persistence/DeviceRepositoryTest.cpp)

# Integration Tests
add_lanscan_test(ScanWorkflowTest tests/integration/ScanWorkflowTest.cpp)

# UI Tests
add_lanscan_test(DeviceTableViewModelTest tests/ui/DeviceTableViewModelTest.cpp)
```

### Vantaggi Testing con SRP

✅ **Test Isolati**: Ogni test verifica UNA responsabilità
✅ **Facili da Scrivere**: Classi piccole = test semplici
✅ **Manutenibili**: Modifiche a una classe = aggiorna solo i suoi test
✅ **Veloci**: Test unitari eseguono in millisecondi
✅ **Affidabili**: Mock dependencies = test deterministici
✅ **Coverage Alto**: Classi piccole = facile raggiungere 100% coverage

### Best Practices

1. **AAA Pattern**: Arrange-Act-Assert in ogni test
2. **Un assertion per test**: Testa una cosa alla volta
3. **Nomi descrittivi**: `testCalculateAverageWithEmptyVector`
4. **Test data-driven**: Usa `QFETCH` per test parametrici
5. **Mock dependencies**: Isola la classe sotto test
6. **Setup/Teardown**: `initTestCase()` e `cleanupTestCase()`

---

## Risorse Utili

### Documentation
- Qt Documentation: https://doc.qt.io/qt-6/
- Qt Network: https://doc.qt.io/qt-6/qtnetwork-index.html
- Qt Charts: https://doc.qt.io/qt-6/qtcharts-index.html

### Database
- IEEE OUI Database: https://standards.ieee.org/develop/regauth/oui/oui.txt
- Common ports: https://www.iana.org/assignments/service-names-port-numbers/

### Libraries
- libpcap: https://www.tcpdump.org/
- Boost.Asio: https://www.boost.org/doc/libs/release/libs/asio/

### Design
- Material Design: https://material.io/design
- Qt Style Sheets: https://doc.qt.io/qt-6/stylesheet-examples.html

---

## License
Da definire (MIT, GPL, proprietary)

---

## Contributors
Paolo - Lead Developer

---

## Changelog
- v0.1.0 - Project initialization
