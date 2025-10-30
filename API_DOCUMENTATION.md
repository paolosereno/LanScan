# LanScan API Documentation
## Developer Guide and Architecture Reference

**Version**: 1.0.0
**Last Updated**: 2025-10-30
**Target Audience**: Developers, Contributors, Integration Partners

---

## Table of Contents

1. [Architecture Overview](#1-architecture-overview)
2. [Core Models](#2-core-models)
3. [Network Layer](#3-network-layer)
4. [Diagnostics](#4-diagnostics)
5. [Services](#5-services)
6. [Database Layer](#6-database-layer)
7. [Controllers](#7-controllers)
8. [ViewModels & Views](#8-viewmodels--views)
9. [Managers](#9-managers)
10. [Utilities](#10-utilities)
11. [Extension Points](#11-extension-points)
12. [Build & Dependencies](#12-build--dependencies)

---

## 1. Architecture Overview

### 1.1 Design Patterns

LanScan employs several industry-standard design patterns for maintainability, testability, and scalability:

#### MVVM (Model-View-ViewModel)
**Purpose**: Separation of UI logic from business logic

```
┌──────────┐      ┌──────────────┐      ┌───────────┐      ┌────────────┐
│  View    │─────▶│  ViewModel   │─────▶│   Model   │─────▶│  Database  │
│  (UI)    │◀─────│  (Logic)     │◀─────│  (Data)   │◀─────│  (Storage) │
└──────────┘      └──────────────┘      └───────────┘      └────────────┘
     │                    │                     │
     │                    │                     │
  Qt Widgets       Qt Signals/Slots      Business Logic
  Qt Designer       Data Binding         Domain Models
```

**Example Flow**:
1. User clicks "Start Scan" button in **MainWindow** (View)
2. View emits signal to **ScanController** (ViewModel/Controller)
3. Controller orchestrates **ScanCoordinator** and **IpScanner** (Model/Service)
4. Scanner updates **Device** models (Model)
5. Models saved to database via **DeviceRepository** (Repository)
6. Repository updates **DeviceCache** (Cache)
7. ViewModel emits signals back to View for UI updates

#### Repository Pattern
**Purpose**: Abstract data access layer, provide consistent interface

```
┌───────────────┐
│  Controller   │
└───────┬───────┘
        │
        ▼
┌───────────────┐      ┌──────────────┐
│  Repository   │─────▶│  Database    │
│  (Interface)  │      │  (SQLite)    │
└───────────────┘      └──────────────┘
        │
        ▼
┌───────────────┐
│    Cache      │
│  (LRU Memory) │
└───────────────┘
```

**Implementations**:
- `IDeviceRepository` (interface)
- `DeviceRepository` (SQLite implementation)
- `DeviceCache` (In-memory LRU cache)

#### Dependency Injection
**Purpose**: Loose coupling, testability, flexibility

```cpp
// Constructor injection example
class ScanController : public QObject {
public:
    ScanController(
        ScanCoordinator* coordinator,
        IDeviceRepository* repository,
        QObject* parent = nullptr
    ) : QObject(parent),
        m_coordinator(coordinator),
        m_repository(repository)
    {
        // Dependencies injected, not created internally
        connect(m_coordinator, &ScanCoordinator::deviceDiscovered,
                this, &ScanController::onDeviceDiscovered);
    }

private:
    ScanCoordinator* m_coordinator;  // Injected dependency
    IDeviceRepository* m_repository;  // Injected dependency
};
```

**Benefits**:
- Easy to mock dependencies for unit testing
- Swap implementations without changing consumers
- Clear dependency graph

#### Singleton Pattern
**Purpose**: Single global instance for stateful services

**Singletons in LanScan**:
- `DatabaseManager` - Single database connection
- `ThemeManager` - Application-wide theme state
- `LanguageManager` - Global language settings
- `MacVendorLookup` - Shared OUI database (1.2 MB, loaded once)

```cpp
// Singleton implementation (Meyer's Singleton)
class ThemeManager : public QObject {
public:
    static ThemeManager* instance() {
        static ThemeManager instance;
        return &instance;
    }

    // Delete copy/move constructors
    ThemeManager(const ThemeManager&) = delete;
    ThemeManager& operator=(const ThemeManager&) = delete;

private:
    ThemeManager() = default;  // Private constructor
};
```

### 1.2 Component Diagram

```
┌─────────────────────────────────────────────────────────────────────┐
│                          LanScan Application                        │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  ┌──────────────────────────────────────────────────────────────┐ │
│  │                     Presentation Layer                        │ │
│  │  ┌────────────┐  ┌──────────────┐  ┌──────────────────┐     │ │
│  │  │ MainWindow │  │ SettingsDialog│  │ DeviceDetailDlg  │     │ │
│  │  └────────────┘  └──────────────┘  └──────────────────┘     │ │
│  │  ┌─────────────────┐  ┌──────────────────┐                  │ │
│  │  │ MetricsWidget   │  │ FavoritesWidget  │                  │ │
│  │  └─────────────────┘  └──────────────────┘                  │ │
│  └──────────────────────────────────────────────────────────────┘ │
│                              │                                     │
│                              ▼                                     │
│  ┌──────────────────────────────────────────────────────────────┐ │
│  │                     ViewModel Layer                           │ │
│  │  ┌──────────────────┐  ┌─────────────────┐                  │ │
│  │  │DeviceTableVM     │  │ MetricsViewModel│                  │ │
│  │  └──────────────────┘  └─────────────────┘                  │ │
│  │  ┌──────────────────┐  ┌─────────────────┐                  │ │
│  │  │ChartViewModel    │  │ LatencyChart    │                  │ │
│  │  └──────────────────┘  └─────────────────┘                  │ │
│  └──────────────────────────────────────────────────────────────┘ │
│                              │                                     │
│                              ▼                                     │
│  ┌──────────────────────────────────────────────────────────────┐ │
│  │                     Controller Layer                          │ │
│  │  ┌──────────────┐  ┌──────────────┐  ┌─────────────────┐   │ │
│  │  │ScanController│  │ExportCtrlr   │  │MetricsController│   │ │
│  │  └──────────────┘  └──────────────┘  └─────────────────┘   │ │
│  └──────────────────────────────────────────────────────────────┘ │
│                              │                                     │
│                              ▼                                     │
│  ┌──────────────────────────────────────────────────────────────┐ │
│  │                     Business Logic Layer                      │ │
│  │  ┌──────────────┐  ┌──────────────┐  ┌─────────────────┐   │ │
│  │  │IpScanner     │  │PortScanner   │  │HostDiscovery    │   │ │
│  │  └──────────────┘  └──────────────┘  └─────────────────┘   │ │
│  │  ┌──────────────┐  ┌──────────────┐  ┌─────────────────┐   │ │
│  │  │PingService   │  │TraceRoute    │  │MonitoringService│   │ │
│  │  └──────────────┘  └──────────────┘  └─────────────────┘   │ │
│  └──────────────────────────────────────────────────────────────┘ │
│                              │                                     │
│                              ▼                                     │
│  ┌──────────────────────────────────────────────────────────────┐ │
│  │                     Data Access Layer                         │ │
│  │  ┌──────────────┐  ┌──────────────┐  ┌─────────────────┐   │ │
│  │  │DeviceRepo    │  │HistoryDao    │  │MetricsDao       │   │ │
│  │  └──────────────┘  └──────────────┘  └─────────────────┘   │ │
│  │  ┌──────────────┐  ┌──────────────┐                        │ │
│  │  │DeviceCache   │  │DatabaseMgr   │                        │ │
│  │  └──────────────┘  └──────────────┘                        │ │
│  └──────────────────────────────────────────────────────────────┘ │
│                              │                                     │
│                              ▼                                     │
│  ┌──────────────────────────────────────────────────────────────┐ │
│  │                     Persistence Layer                         │ │
│  │  ┌──────────────────────────────────────────────────────┐   │ │
│  │  │                SQLite Database                        │   │ │
│  │  │  Tables: devices, ports, metrics, alerts, history    │   │ │
│  │  └──────────────────────────────────────────────────────┘   │ │
│  └──────────────────────────────────────────────────────────────┘ │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

### 1.3 Technology Stack

**Core Framework**: Qt 6.9.1
- **Qt Widgets**: UI framework (MainWindow, dialogs, custom widgets)
- **Qt Network**: Networking (TCP/UDP sockets, DNS, HTTP)
- **Qt Sql**: SQLite database integration
- **Qt Charts**: Metrics visualization (line, bar, spline charts)
- **Qt Concurrent**: Multi-threading (QThreadPool, QFuture)
- **Qt Svg**: SVG icon rendering
- **Qt LinguistTools**: Internationalization (i18n/l10n)

**Language**: C++17
- Modern C++ features: `auto`, range-based for, smart pointers, `std::optional`
- STL containers: `std::vector`, `std::map`, `std::unordered_map`
- Qt containers: `QList`, `QString`, `QHash`, `QMap`

**Build System**: CMake 3.16+
- Cross-platform build configuration
- Automatic MOC, UIC, RCC generation
- Test integration with CTest
- Qt6 package management

**Database**: SQLite 3
- Embedded relational database
- Zero-configuration
- ACID-compliant transactions
- Full-text search (FTS5) support

**Testing**: Qt Test Framework
- 40 test suites with 160+ test cases
- Unit tests for all core components
- Integration tests for end-to-end workflows
- Performance benchmarks with QBENCHMARK

---

## 2. Core Models

### 2.1 Device Model

**Purpose**: Represents a network device with all discovered information

**File**: `src/models/Device.h`, `src/models/Device.cpp`

**Class Definition**:
```cpp
class Device {
public:
    Device();

    // Getters
    QString getIp() const;
    QString getHostname() const;
    QString getMacAddress() const;
    QString getVendor() const;
    bool isOnline() const;
    QDateTime getFirstSeen() const;
    QDateTime getLastSeen() const;
    QList<PortInfo> getOpenPorts() const;
    NetworkMetrics getMetrics() const;

    // Setters
    void setIp(const QString& ip);
    void setHostname(const QString& hostname);
    void setMacAddress(const QString& mac);
    void setVendor(const QString& vendor);
    void setOnline(bool online);
    void setFirstSeen(const QDateTime& dateTime);
    void setLastSeen(const QDateTime& dateTime);
    void addPort(const PortInfo& port);
    void setMetrics(const NetworkMetrics& metrics);

    // Utility
    bool operator==(const Device& other) const;
    QString toString() const;

private:
    QString m_ip;
    QString m_hostname;
    QString m_macAddress;
    QString m_vendor;
    bool m_online;
    QDateTime m_firstSeen;
    QDateTime m_lastSeen;
    QList<PortInfo> m_openPorts;
    NetworkMetrics m_metrics;
};
```

**Usage Example**:
```cpp
Device device;
device.setIp("192.168.1.100");
device.setHostname("server-01");
device.setMacAddress("00:11:22:33:44:55");
device.setVendor("Dell Inc.");
device.setOnline(true);
device.setFirstSeen(QDateTime::currentDateTime());
device.setLastSeen(QDateTime::currentDateTime());

PortInfo port(80, PortInfo::TCP);
port.setService("HTTP");
port.setState(PortInfo::Open);
device.addPort(port);

NetworkMetrics metrics;
metrics.setLatencyAvg(12.5);
metrics.setJitter(2.3);
metrics.setPacketLoss(0.5);
metrics.calculateQualityScore();
device.setMetrics(metrics);
```

**Database Mapping**:
```sql
CREATE TABLE devices (
    ip TEXT PRIMARY KEY,
    hostname TEXT,
    mac TEXT,
    vendor TEXT,
    status TEXT,
    first_seen DATETIME,
    last_seen DATETIME
);
```

### 2.2 PortInfo Model

**Purpose**: Represents an open/closed port with service information

**File**: `src/models/PortInfo.h`, `src/models/PortInfo.cpp`

**Class Definition**:
```cpp
class PortInfo {
public:
    enum Protocol {
        TCP,
        UDP
    };

    enum State {
        Open,
        Closed,
        Filtered
    };

    PortInfo();
    PortInfo(int port, Protocol protocol);

    // Getters
    int getPort() const;
    Protocol getProtocol() const;
    QString getService() const;
    State getState() const;

    // Setters
    void setPort(int port);
    void setProtocol(Protocol protocol);
    void setService(const QString& service);
    void setState(State state);

    // Utility
    QString protocolToString() const;
    QString stateToString() const;

private:
    int m_port;
    Protocol m_protocol;
    QString m_service;
    State m_state;
};
```

**Usage Example**:
```cpp
// HTTP port
PortInfo httpPort(80, PortInfo::TCP);
httpPort.setService("HTTP");
httpPort.setState(PortInfo::Open);

// SSH port
PortInfo sshPort(22, PortInfo::TCP);
sshPort.setService("SSH");
sshPort.setState(PortInfo::Open);

// DNS port (UDP)
PortInfo dnsPort(53, PortInfo::UDP);
dnsPort.setService("DNS");
dnsPort.setState(PortInfo::Open);
```

**Common Services**:
- 80 → HTTP
- 443 → HTTPS
- 22 → SSH
- 21 → FTP
- 25 → SMTP
- 3306 → MySQL
- 5432 → PostgreSQL
- 3389 → RDP

### 2.3 NetworkMetrics Model

**Purpose**: Stores network quality metrics (latency, jitter, packet loss, quality score)

**File**: `src/models/NetworkMetrics.h`, `src/models/NetworkMetrics.cpp`

**Class Definition**:
```cpp
class NetworkMetrics {
public:
    enum QualityScore {
        Unknown = 0,
        Poor = 1,      // 0-49
        Fair = 2,      // 50-69
        Good = 3,      // 70-89
        Excellent = 4  // 90-100
    };

    NetworkMetrics();

    // Getters
    double latencyAvg() const;
    double latencyMin() const;
    double latencyMax() const;
    double jitter() const;
    double packetLoss() const;
    int qualityScore() const;
    QualityScore qualityLevel() const;
    QDateTime timestamp() const;

    // Setters
    void setLatencyAvg(double latency);
    void setLatencyMin(double latency);
    void setLatencyMax(double latency);
    void setJitter(double jitter);
    void setPacketLoss(double loss);
    void setQualityScore(QualityScore score);
    void setTimestamp(const QDateTime& timestamp);

    // Calculation
    void calculateQualityScore();  // Auto-calculate from latency/jitter/loss

    // Utility
    QString qualityLevelToString() const;

private:
    double m_latencyAvg;
    double m_latencyMin;
    double m_latencyMax;
    double m_jitter;
    double m_packetLoss;
    int m_qualityScore;
    QDateTime m_timestamp;
};
```

**Quality Score Calculation**:
```cpp
void NetworkMetrics::calculateQualityScore() {
    // Weights
    const double W_LATENCY = 0.4;
    const double W_JITTER = 0.3;
    const double W_PACKET_LOSS = 0.3;

    // Normalize factors
    double latencyFactor = std::min(m_latencyAvg / 200.0, 1.0) * 100.0;
    double jitterFactor = std::min(m_jitter / 50.0, 1.0) * 100.0;
    double packetLossFactor = m_packetLoss;  // Already 0-100

    // Calculate weighted score
    double score = 100.0 - (W_LATENCY * latencyFactor +
                            W_JITTER * jitterFactor +
                            W_PACKET_LOSS * packetLossFactor);

    // Clamp to [0, 100]
    m_qualityScore = std::max(0, std::min(100, static_cast<int>(score)));
}
```

**Usage Example**:
```cpp
NetworkMetrics metrics;
metrics.setLatencyAvg(15.5);
metrics.setLatencyMin(12.1);
metrics.setLatencyMax(18.9);
metrics.setJitter(2.3);
metrics.setPacketLoss(0.5);
metrics.calculateQualityScore();  // Auto-calculates based on above values

qDebug() << "Quality Score:" << metrics.qualityScore();  // e.g., 92
qDebug() << "Quality Level:" << metrics.qualityLevelToString();  // "Excellent"
```

### 2.4 Alert Model

**Purpose**: Represents a network alert (high latency, packet loss, device offline, etc.)

**File**: `src/models/Alert.h`, `src/models/Alert.cpp`

**Class Definition**:
```cpp
class Alert {
public:
    enum Type {
        HighLatency,
        PacketLoss,
        HighJitter,
        DeviceOffline,
        DeviceOnline
    };

    enum Severity {
        Info,
        Warning,
        Critical
    };

    Alert();
    Alert(Type type, Severity severity, const QString& deviceId, const QString& description);

    // Getters
    int getId() const;
    Type getType() const;
    Severity getSeverity() const;
    QString getDeviceId() const;
    QString getDescription() const;
    QDateTime getTimestamp() const;
    bool isAcknowledged() const;

    // Setters
    void setId(int id);
    void setType(Type type);
    void setSeverity(Severity severity);
    void setDeviceId(const QString& deviceId);
    void setDescription(const QString& description);
    void setTimestamp(const QDateTime& timestamp);
    void setAcknowledged(bool acknowledged);

    // Utility
    QString typeToString() const;
    QString severityToString() const;
    QIcon getSeverityIcon() const;

private:
    int m_id;
    Type m_type;
    Severity m_severity;
    QString m_deviceId;
    QString m_description;
    QDateTime m_timestamp;
    bool m_acknowledged;
};
```

**Usage Example**:
```cpp
// High latency alert
Alert alert(Alert::HighLatency,
            Alert::Warning,
            "192.168.1.100",
            "Latency exceeded threshold (125ms > 100ms)");
alert.setTimestamp(QDateTime::currentDateTime());

// Device offline alert
Alert offlineAlert(Alert::DeviceOffline,
                   Alert::Critical,
                   "192.168.1.50",
                   "Device Server-01 is now OFFLINE");
offlineAlert.setTimestamp(QDateTime::currentDateTime());
```

### 2.5 Profile Model

**Purpose**: Stores scan configuration profiles (port lists, timeouts, etc.)

**File**: `src/models/Profile.h`, `src/models/Profile.cpp`

**Class Definition**:
```cpp
class Profile {
public:
    Profile();

    // Getters
    QString getId() const;
    QString getName() const;
    QString getDescription() const;
    QList<int> getPortList() const;
    int getTimeout() const;
    int getThreadCount() const;
    QDateTime getLastUsed() const;
    int getUsageCount() const;

    // Setters
    void setId(const QString& id);
    void setName(const QString& name);
    void setDescription(const QString& description);
    void setPortList(const QList<int>& ports);
    void setTimeout(int timeout);
    void setThreadCount(int count);
    void setLastUsed(const QDateTime& dateTime);
    void setUsageCount(int count);

    // Utility
    void incrementUsageCount();
    bool isFrequentlyUsed() const;  // > 10 uses
    QJsonObject toJson() const;
    static Profile fromJson(const QJsonObject& json);

private:
    QString m_id;
    QString m_name;
    QString m_description;
    QList<int> m_portList;
    int m_timeout;
    int m_threadCount;
    QDateTime m_lastUsed;
    int m_usageCount;
};
```

**JSON Format**:
```json
{
  "id": "home-network-001",
  "name": "Home Network",
  "description": "Standard home network scan profile",
  "ports": [80, 443, 22, 21, 445, 3389],
  "timeout": 2000,
  "threadCount": 4,
  "lastUsed": "2025-10-30T15:30:00Z",
  "usageCount": 15
}
```

---

## 3. Network Layer

### 3.1 IpScanner

**Purpose**: Multi-threaded IP range scanner with strategy pattern

**File**: `src/network/scanner/IpScanner.h`, `src/network/scanner/IpScanner.cpp`

**Class Definition**:
```cpp
class IpScanner : public QObject {
    Q_OBJECT

public:
    explicit IpScanner(QObject* parent = nullptr);

    void setStrategy(IScanStrategy* strategy);
    void scan(const QString& subnet);
    void stop();
    bool isScanning() const;

signals:
    void deviceDiscovered(const Device& device);
    void scanProgress(int current, int total);
    void scanComplete();
    void scanError(const QString& error);

private:
    IScanStrategy* m_strategy;
    QThreadPool* m_threadPool;
    std::atomic<bool> m_scanning;
};
```

**Scan Strategies**:

1. **QuickScanStrategy**: Fast scan with 20 common ports
2. **DeepScanStrategy**: Thorough scan with 1000 ports
3. **CustomScanStrategy**: User-defined port list from Profile

**Usage Example**:
```cpp
IpScanner scanner;
QuickScanStrategy quickStrategy;
scanner.setStrategy(&quickStrategy);

connect(&scanner, &IpScanner::deviceDiscovered, [](const Device& device) {
    qDebug() << "Found device:" << device.getIp() << device.getHostname();
});

connect(&scanner, &IpScanner::scanProgress, [](int current, int total) {
    qDebug() << "Progress:" << current << "/" << total;
});

connect(&scanner, &IpScanner::scanComplete, []() {
    qDebug() << "Scan complete!";
});

scanner.scan("192.168.1.0/24");  // Scan subnet
```

**Thread Safety**:
- Uses `QThreadPool` for parallel scanning
- Thread count configurable (1-16 threads)
- `std::atomic<bool>` for cancellation flag
- Thread-safe signal emissions via Qt's queued connections

### 3.2 PortScanner

**Purpose**: TCP/UDP port scanning with timeout and state detection

**File**: `src/network/scanner/PortScanner.h`, `src/network/scanner/PortScanner.cpp`

**Class Definition**:
```cpp
class PortScanner : public QObject {
    Q_OBJECT

public:
    explicit PortScanner(QObject* parent = nullptr);

    void scanPorts(const QString& ip,
                   const QList<int>& ports,
                   int timeout = 1000);
    void stop();

signals:
    void portFound(const QString& ip, const PortInfo& port);
    void scanProgress(int current, int total);
    void scanComplete();

private:
    void scanPort(const QString& ip, int port, int timeout);
    bool isPortOpen(const QString& ip, int port, int timeout);

    QThreadPool* m_threadPool;
    std::atomic<bool> m_scanning;
};
```

**Port Scanning Methods**:

1. **TCP Connect Scan**: Attempts full TCP handshake
   - Reliable, detects open/closed/filtered
   - Logged by firewalls (not stealthy)

2. **TCP SYN Scan**: Half-open scan (requires raw sockets, admin privileges)
   - Faster, more stealthy
   - Not fully implemented (future feature)

**Usage Example**:
```cpp
PortScanner scanner;
QList<int> commonPorts = {80, 443, 22, 21, 25, 3389};

connect(&scanner, &PortScanner::portFound, [](const QString& ip, const PortInfo& port) {
    qDebug() << "Open port found on" << ip << ":" << port.getPort()
             << "(" << port.getService() << ")";
});

scanner.scanPorts("192.168.1.100", commonPorts, 2000);
```

### 3.3 HostDiscovery

**Purpose**: Discover active hosts using ping (ICMP Echo Request)

**File**: `src/network/discovery/HostDiscovery.h`, `src/network/discovery/HostDiscovery.cpp`

**Class Definition**:
```cpp
class HostDiscovery : public QObject {
    Q_OBJECT

public:
    explicit HostDiscovery(QObject* parent = nullptr);

    void discover(const QString& subnet);
    void ping(const QString& ip, int count = 4);
    void stop();

signals:
    void hostDiscovered(const QString& ip, double latency);
    void hostUnreachable(const QString& ip);
    void discoveryComplete();

private:
    void executePing(const QString& ip, int count);
    double parsePingOutput(const QString& output);

    QProcess* m_process;
};
```

**Platform-Specific Commands**:
- **Windows**: `ping -n <count> -w <timeout> <ip>`
- **Linux**: `ping -c <count> -W <timeout_sec> <ip>`
- **macOS**: `ping -c <count> -W <timeout_ms> <ip>`

**Ping Output Parsing**:
```cpp
double HostDiscovery::parsePingOutput(const QString& output) {
    // Multi-language regex patterns for parsing ping output

    // English: "Average = 12ms"
    QRegularExpression enPattern(R"(Average = (\d+)ms)");

    // Italian: "Media = 12ms"
    QRegularExpression itPattern(R"(Media = (\d+)ms)");

    // German: "Mittelwert = 12ms"
    QRegularExpression dePattern(R"(Mittelwert = (\d+)ms)");

    // Try each pattern
    for (const auto& pattern : {enPattern, itPattern, dePattern}) {
        QRegularExpressionMatch match = pattern.match(output);
        if (match.hasMatch()) {
            return match.captured(1).toDouble();
        }
    }

    return -1.0;  // Parsing failed
}
```

### 3.4 DnsResolver

**Purpose**: Asynchronous DNS resolution (forward and reverse lookups)

**File**: `src/network/discovery/DnsResolver.h`, `src/network/discovery/DnsResolver.cpp`

**Class Definition**:
```cpp
class DnsResolver : public QObject {
    Q_OBJECT

public:
    explicit DnsResolver(QObject* parent = nullptr);

    void resolve(const QString& hostname);
    void reverseResolve(const QString& ip);
    void cancel();

signals:
    void resolved(const QString& hostname, const QString& ip);
    void resolveError(const QString& hostname, const QString& error);

private:
    QDnsLookup* m_dnsLookup;
    QHash<QString, QString> m_cache;  // hostname -> IP cache
};
```

**DNS Caching**:
- In-memory cache with max 1000 entries
- LRU eviction policy
- Speeds up repeated scans by 10-50x

**Usage Example**:
```cpp
DnsResolver resolver;

// Forward lookup
connect(&resolver, &DnsResolver::resolved, [](const QString& hostname, const QString& ip) {
    qDebug() << hostname << "resolves to" << ip;
});

resolver.resolve("google.com");  // Result: "142.250.185.46"

// Reverse lookup
resolver.reverseResolve("8.8.8.8");  // Result: "dns.google"
```

### 3.5 ArpDiscovery

**Purpose**: MAC address and vendor identification via ARP table parsing

**File**: `src/network/discovery/ArpDiscovery.h`, `src/network/discovery/ArpDiscovery.cpp`

**Class Definition**:
```cpp
class ArpDiscovery : public QObject {
    Q_OBJECT

public:
    explicit ArpDiscovery(QObject* parent = nullptr);

    void discover();
    QString getMacAddress(const QString& ip);

signals:
    void arpEntryFound(const QString& ip, const QString& mac);
    void discoveryComplete();

private:
    void parseArpTable(const QString& output);

    QProcess* m_process;
    QHash<QString, QString> m_arpCache;  // IP -> MAC mapping
};
```

**Platform-Specific Commands**:
- **Windows**: `arp -a`
- **Linux**: `arp -n` or `ip neigh show`
- **macOS**: `arp -a`

**ARP Table Parsing Example (Windows)**:
```
Internet Address      Physical Address      Type
192.168.1.1           00-11-22-33-44-55     dynamic
192.168.1.100         AA-BB-CC-DD-EE-FF     dynamic
```

**Vendor Identification**:
```cpp
MacVendorLookup* lookup = MacVendorLookup::instance();
QString vendor = lookup->getVendor("00:11:22:33:44:55");
// Returns: "Cisco Systems, Inc."
```

**OUI Database**:
- 38,169+ manufacturers
- IEEE OUI database (1.2 MB)
- Loaded once on startup (Singleton pattern)
- Fast hash table lookup (O(1))

---

## 4. Diagnostics

### 4.1 PingService

**Purpose**: Advanced ping with metrics calculation (latency, jitter, packet loss)

**File**: `src/network/diagnostics/PingService.h`, `src/network/diagnostics/PingService.cpp`

**Class Definition**:
```cpp
class PingService : public QObject {
    Q_OBJECT

public:
    explicit PingService(QObject* parent = nullptr);

    void ping(const QString& ip, int count = 10, int interval = 1000);
    void stop();
    bool isRunning() const;

signals:
    void pingResult(double latency);
    void pingComplete(const NetworkMetrics& metrics);
    void pingError(const QString& error);

private:
    void executePing();
    void calculateMetrics();

    QString m_targetIp;
    int m_count;
    int m_interval;
    QVector<double> m_latencies;
    QVector<bool> m_responses;
    QProcess* m_process;
    bool m_running;
};
```

**Metrics Calculation**:
```cpp
void PingService::calculateMetrics() {
    NetworkMetrics metrics;

    // Filter successful pings
    QVector<double> validLatencies;
    for (int i = 0; i < m_latencies.size(); ++i) {
        if (m_responses[i]) {
            validLatencies.append(m_latencies[i]);
        }
    }

    if (validLatencies.isEmpty()) {
        emit pingComplete(metrics);  // Empty metrics
        return;
    }

    // Calculate latency stats
    double sum = 0.0;
    double min = validLatencies.first();
    double max = validLatencies.first();

    for (double latency : validLatencies) {
        sum += latency;
        min = std::min(min, latency);
        max = std::max(max, latency);
    }

    double avg = sum / validLatencies.size();

    // Calculate jitter (consecutive packet latency variation)
    double jitterSum = 0.0;
    for (int i = 1; i < validLatencies.size(); ++i) {
        jitterSum += std::abs(validLatencies[i] - validLatencies[i-1]);
    }
    double jitter = (validLatencies.size() > 1) ?
                    (jitterSum / (validLatencies.size() - 1)) : 0.0;

    // Calculate packet loss
    int totalPackets = m_count;
    int lostPackets = totalPackets - validLatencies.size();
    double packetLoss = (lostPackets * 100.0) / totalPackets;

    // Set metrics
    metrics.setLatencyAvg(avg);
    metrics.setLatencyMin(min);
    metrics.setLatencyMax(max);
    metrics.setJitter(jitter);
    metrics.setPacketLoss(packetLoss);
    metrics.calculateQualityScore();
    metrics.setTimestamp(QDateTime::currentDateTime());

    emit pingComplete(metrics);
}
```

**Usage Example**:
```cpp
PingService pingService;

connect(&pingService, &PingService::pingResult, [](double latency) {
    qDebug() << "Ping:" << latency << "ms";
});

connect(&pingService, &PingService::pingComplete, [](const NetworkMetrics& metrics) {
    qDebug() << "Average latency:" << metrics.latencyAvg() << "ms";
    qDebug() << "Jitter:" << metrics.jitter() << "ms";
    qDebug() << "Packet loss:" << metrics.packetLoss() << "%";
    qDebug() << "Quality score:" << metrics.qualityScore();
});

pingService.ping("192.168.1.100", 10, 1000);  // 10 pings, 1s interval
```

### 4.2 TraceRouteService

**Purpose**: Discover network path (hops) to target device

**File**: `src/network/diagnostics/TraceRouteService.h`, `src/network/diagnostics/TraceRouteService.cpp`

**Class Definition**:
```cpp
class TraceRouteService : public QObject {
    Q_OBJECT

public:
    explicit TraceRouteService(QObject* parent = nullptr);

    void trace(const QString& target);
    void stop();

signals:
    void hopDiscovered(const TraceRouteHop& hop);
    void traceComplete();
    void traceError(const QString& error);

private:
    void executeTrace();
    TraceRouteHop parseHopLine(const QString& line);

    QString m_target;
    QProcess* m_process;
    int m_currentHop;
};
```

**TraceRouteHop Model**:
```cpp
class TraceRouteHop {
public:
    int hopNumber;
    QString ip;
    QString hostname;
    double rttMin;
    double rttAvg;
    double rttMax;
    bool timeout;
};
```

**Platform-Specific Commands**:
- **Windows**: `tracert <target>`
- **Linux**: `traceroute <target>`
- **macOS**: `traceroute <target>`

**Output Parsing Example (Windows)**:
```
Tracing route to google.com [142.250.185.46]
over a maximum of 30 hops:

  1    <1 ms    <1 ms    <1 ms  192.168.1.1
  2     8 ms     9 ms    10 ms  10.0.0.1 [gateway.isp.com]
  3    15 ms    16 ms    17 ms  203.0.113.1 [border.isp.com]
  4    22 ms    23 ms    25 ms  142.250.185.46 [google.com]
```

### 4.3 MtuDiscovery

**Purpose**: Find optimal Maximum Transmission Unit using binary search

**File**: `src/network/diagnostics/MtuDiscovery.h`, `src/network/diagnostics/MtuDiscovery.cpp`

**Class Definition**:
```cpp
class MtuDiscovery : public QObject {
    Q_OBJECT

public:
    explicit MtuDiscovery(QObject* parent = nullptr);

    void discover(const QString& target);
    void cancel();

signals:
    void progressUpdate(int currentMtu, int minMtu, int maxMtu);
    void discoveryComplete(int optimalMtu);
    void discoveryError(const QString& error);

private:
    void binarySearch();
    bool testMtu(const QString& target, int mtu);

    QString m_target;
    int m_minMtu;  // 576 bytes
    int m_maxMtu;  // 9000 bytes
    int m_currentMtu;
    bool m_running;
};
```

**Binary Search Algorithm**:
```cpp
void MtuDiscovery::binarySearch() {
    m_minMtu = 576;   // IPv4 minimum
    m_maxMtu = 9000;  // Jumbo frames

    while (m_minMtu < m_maxMtu && m_running) {
        m_currentMtu = (m_minMtu + m_maxMtu + 1) / 2;

        emit progressUpdate(m_currentMtu, m_minMtu, m_maxMtu);

        if (testMtu(m_target, m_currentMtu)) {
            // MTU is acceptable, try larger
            m_minMtu = m_currentMtu;
        } else {
            // MTU too large, fragmentation occurred
            m_maxMtu = m_currentMtu - 1;
        }
    }

    // Optimal MTU found
    int optimalMtu = m_minMtu;
    emit discoveryComplete(optimalMtu);
}
```

**MTU Testing**:
```cpp
bool MtuDiscovery::testMtu(const QString& target, int mtu) {
    // Windows: ping -f -l <size> <target>
    // Linux: ping -M do -s <size> <target>

    int packetSize = mtu - 28;  // Subtract IP (20) + ICMP (8) headers

    QProcess process;
    QStringList args;

#ifdef Q_OS_WIN
    args << "-f" << "-l" << QString::number(packetSize)
         << "-n" << "1" << "-w" << "2000" << target;
    process.start("ping", args);
#else
    args << "-M" << "do" << "-s" << QString::number(packetSize)
         << "-c" << "1" << "-W" << "2" << target;
    process.start("ping", args);
#endif

    process.waitForFinished(3000);
    QString output = process.readAllStandardOutput();

    // Check for fragmentation error messages
    if (output.contains("Packet needs to be fragmented") ||
        output.contains("Message too long") ||
        output.contains("fragmentation needed")) {
        return false;  // MTU too large
    }

    return true;  // MTU is acceptable
}
```

### 4.4 BandwidthTester

**Purpose**: Measure network throughput (download/upload speed)

**File**: `src/network/diagnostics/BandwidthTester.h`, `src/network/diagnostics/BandwidthTester.cpp`

**Class Definition**:
```cpp
class BandwidthTester : public QObject {
    Q_OBJECT

public:
    enum Protocol {
        TCP,
        UDP
    };

    enum Direction {
        Download,
        Upload
    };

    explicit BandwidthTester(QObject* parent = nullptr);

    void test(const QString& serverIp,
              int port,
              Protocol protocol,
              Direction direction,
              int durationSeconds,
              int packetSizeKB);
    void cancel();

signals:
    void progressUpdate(double currentBandwidth);
    void testComplete(double bandwidthMbps, qint64 totalBytes);
    void testError(const QString& error);

private:
    void runTest();
    void sendHandshake();
    bool receiveHandshake();
    void runDownloadTest();
    void runUploadTest();
    void receiveResults();

    QTcpSocket* m_socket;
    QString m_serverIp;
    int m_port;
    Protocol m_protocol;
    Direction m_direction;
    int m_duration;
    int m_packetSize;
    bool m_running;
};
```

**LanScan Bandwidth Test Protocol**:

1. **Handshake Phase**:
```
Client → Server:
LANSCAN_BW_TEST
VERSION:1.0
PROTOCOL:TCP
DIRECTION:DOWNLOAD
DURATION:10
PACKET_SIZE:65536
END

Server → Client:
LANSCAN_BW_OK
VERSION:1.0
READY
```

2. **Data Transfer Phase**:
- **Download**: Server sends test data to client as fast as possible
- **Upload**: Client sends test data to server as fast as possible

3. **Results Phase**:
```
Server → Client:
LANSCAN_BW_RESULTS
BYTES:4833214464
THROUGHPUT_MBPS:3866.57
DURATION_MS:10000
END
```

**Bandwidth Calculation** (Server-side):
```cpp
double calculateBandwidth(qint64 bytes, qint64 durationMs) {
    // Convert to Megabits per second
    double bits = bytes * 8.0;
    double seconds = durationMs / 1000.0;
    double mbps = bits / seconds / 1000000.0;
    return mbps;
}
```

**Important**: Requires **LanScan-BandwidthServer** running on target device (NOT compatible with iperf3).

### 4.5 DnsDiagnostics

**Purpose**: Advanced DNS queries (A, AAAA, MX, NS, TXT, CNAME, PTR, SRV)

**File**: `src/network/diagnostics/DnsDiagnostics.h`, `src/network/diagnostics/DnsDiagnostics.cpp`

**Class Definition**:
```cpp
class DnsDiagnostics : public QObject {
    Q_OBJECT

public:
    explicit DnsDiagnostics(QObject* parent = nullptr);

    void lookup(const QString& query,
                QDnsLookup::Type recordType,
                const QString& dnsServer = QString());
    void cancel();

signals:
    void recordReceived(const DnsRecord& record);
    void lookupComplete(qint64 queryTimeMs);
    void lookupError(const QString& error);

private:
    void handleResults();

    QDnsLookup* m_dnsLookup;
    QElapsedTimer m_timer;
};
```

**DnsRecord Model**:
```cpp
class DnsRecord {
public:
    QString name;
    QString value;
    QDnsLookup::Type type;
    quint32 ttl;
    quint16 priority;  // For MX and SRV records
};
```

**Supported Record Types**:
- **A**: IPv4 address (e.g., 192.0.2.1)
- **AAAA**: IPv6 address (e.g., 2001:db8::1)
- **MX**: Mail exchange servers with priority
- **NS**: Name servers
- **TXT**: Text records (SPF, DKIM, etc.)
- **CNAME**: Canonical name (alias)
- **PTR**: Pointer (reverse DNS)
- **SRV**: Service locator

**Usage Example**:
```cpp
DnsDiagnostics dns;

// Forward lookup (A record)
connect(&dns, &DnsDiagnostics::recordReceived, [](const DnsRecord& record) {
    qDebug() << "DNS Record:" << record.name << "->" << record.value
             << "TTL:" << record.ttl;
});

dns.lookup("google.com", QDnsLookup::A);
// Result: google.com -> 142.250.185.46 (TTL: 300)

// MX record lookup
dns.lookup("example.com", QDnsLookup::MX);
// Results: mail1.example.com (priority: 10), mail2.example.com (priority: 20)

// Reverse DNS (PTR)
dns.lookup("8.8.8.8", QDnsLookup::PTR);
// Result: 8.8.8.8 -> dns.google
```

**Custom DNS Server**:
```cpp
// Use Google Public DNS
dns.lookup("example.com", QDnsLookup::A, "8.8.8.8");

// Use Cloudflare DNS
dns.lookup("example.com", QDnsLookup::A, "1.1.1.1");
```

---

## 5. Services

### 5.1 MonitoringService

**Purpose**: Continuous device monitoring with alert generation

**File**: `src/services/MonitoringService.h`, `src/services/MonitoringService.cpp`

**Class Definition**:
```cpp
class MonitoringService : public QObject {
    Q_OBJECT

public:
    explicit MonitoringService(
        PingService* pingService,
        AlertService* alertService,
        HistoryService* historyService,
        IDeviceRepository* repository,
        QObject* parent = nullptr
    );

    void startMonitoring(const QString& deviceId, int intervalSeconds = 5);
    void stopMonitoring(const QString& deviceId);
    void stopAll();
    bool isMonitoring(const QString& deviceId) const;

    // Configuration
    void setLatencyThreshold(int ms);
    void setPacketLossThreshold(double percent);
    void setJitterThreshold(int ms);

signals:
    void metricsCollected(const QString& deviceId, const NetworkMetrics& metrics);
    void deviceStatusChanged(const QString& deviceId, bool online);
    void alertTriggered(const QString& deviceId, const Alert& alert);

private slots:
    void onMonitoringTimeout();
    void onPingComplete(const NetworkMetrics& metrics);

private:
    void checkThresholds(const QString& deviceId, const NetworkMetrics& metrics);
    void generateAlert(Alert::Type type, Alert::Severity severity,
                       const QString& deviceId, const QString& description);

    PingService* m_pingService;
    AlertService* m_alertService;
    HistoryService* m_historyService;
    IDeviceRepository* m_repository;

    QHash<QString, QTimer*> m_monitoringTimers;
    QHash<QString, bool> m_deviceStatus;  // IP -> online status

    int m_latencyThreshold;      // Default: 100ms
    double m_packetLossThreshold;  // Default: 5%
    int m_jitterThreshold;       // Default: 10ms
};
```

**Alert Generation Logic**:
```cpp
void MonitoringService::checkThresholds(const QString& deviceId,
                                        const NetworkMetrics& metrics) {
    // High latency alert
    if (metrics.latencyAvg() > m_latencyThreshold) {
        QString desc = QString("High latency detected (%1ms > %2ms)")
                       .arg(metrics.latencyAvg())
                       .arg(m_latencyThreshold);
        generateAlert(Alert::HighLatency, Alert::Warning, deviceId, desc);
    }

    // Packet loss alert
    if (metrics.packetLoss() > m_packetLossThreshold) {
        QString desc = QString("Packet loss detected (%1% > %2%)")
                       .arg(metrics.packetLoss())
                       .arg(m_packetLossThreshold);
        generateAlert(Alert::PacketLoss, Alert::Warning, deviceId, desc);
    }

    // High jitter alert
    if (metrics.jitter() > m_jitterThreshold) {
        QString desc = QString("High jitter detected (%1ms > %2ms)")
                       .arg(metrics.jitter())
                       .arg(m_jitterThreshold);
        generateAlert(Alert::HighJitter, Alert::Warning, deviceId, desc);
    }

    // Device status change
    bool currentlyOnline = (metrics.packetLoss() < 100.0);
    bool previouslyOnline = m_deviceStatus.value(deviceId, true);

    if (currentlyOnline != previouslyOnline) {
        m_deviceStatus[deviceId] = currentlyOnline;

        if (currentlyOnline) {
            QString desc = QString("Device %1 is now ONLINE").arg(deviceId);
            generateAlert(Alert::DeviceOnline, Alert::Info, deviceId, desc);
        } else {
            QString desc = QString("Device %1 is now OFFLINE").arg(deviceId);
            generateAlert(Alert::DeviceOffline, Alert::Critical, deviceId, desc);
        }

        emit deviceStatusChanged(deviceId, currentlyOnline);
    }
}
```

**Usage Example**:
```cpp
MonitoringService monitoringService(pingService, alertService,
                                    historyService, repository);

// Configure thresholds
monitoringService.setLatencyThreshold(100);      // 100ms
monitoringService.setPacketLossThreshold(5.0);   // 5%
monitoringService.setJitterThreshold(10);        // 10ms

// Start monitoring device
connect(&monitoringService, &MonitoringService::metricsCollected,
        [](const QString& deviceId, const NetworkMetrics& metrics) {
    qDebug() << "Metrics for" << deviceId
             << "- Latency:" << metrics.latencyAvg() << "ms"
             << "Quality:" << metrics.qualityScore();
});

connect(&monitoringService, &MonitoringService::alertTriggered,
        [](const QString& deviceId, const Alert& alert) {
    qDebug() << "ALERT for" << deviceId << ":" << alert.getDescription();
});

monitoringService.startMonitoring("192.168.1.100", 5);  // Monitor every 5 seconds
```

### 5.2 AlertService

**Purpose**: Alert management (create, acknowledge, filter, prune)

**File**: `src/services/AlertService.h`, `src/services/AlertService.cpp`

**Class Definition**:
```cpp
class AlertService : public QObject {
    Q_OBJECT

public:
    explicit AlertService(QObject* parent = nullptr);

    void addAlert(const Alert& alert);
    void acknowledgeAlert(int alertId);
    void clearAlerts(const QString& deviceId = QString());

    QList<Alert> getAlerts(const QString& deviceId = QString(),
                           Alert::Severity minSeverity = Alert::Info,
                           bool includeAcknowledged = false) const;

    int getUnacknowledgedCount() const;
    void pruneOldAlerts(int maxAlerts = 1000);  // LRU pruning

signals:
    void alertAdded(const Alert& alert);
    void alertAcknowledged(int alertId);
    void alertsCleared();

private:
    QList<Alert> m_alerts;
    mutable QMutex m_mutex;  // Thread-safe
};
```

**LRU Pruning** (Least Recently Used):
```cpp
void AlertService::pruneOldAlerts(int maxAlerts) {
    QMutexLocker locker(&m_mutex);

    if (m_alerts.size() <= maxAlerts) {
        return;  // No pruning needed
    }

    // Sort by timestamp (oldest first)
    std::sort(m_alerts.begin(), m_alerts.end(),
              [](const Alert& a, const Alert& b) {
        return a.getTimestamp() < b.getTimestamp();
    });

    // Remove oldest alerts
    int toRemove = m_alerts.size() - maxAlerts;
    m_alerts.erase(m_alerts.begin(), m_alerts.begin() + toRemove);

    qDebug() << "Pruned" << toRemove << "old alerts";
}
```

### 5.3 HistoryService

**Purpose**: Historical event tracking (scans, status changes, user actions)

**File**: `src/services/HistoryService.h`, `src/services/HistoryService.cpp`

**Class Definition**:
```cpp
class HistoryService : public QObject {
    Q_OBJECT

public:
    explicit HistoryService(HistoryDao* dao, QObject* parent = nullptr);

    void recordEvent(const QString& deviceId,
                     const QString& eventType,
                     const QString& description,
                     const QJsonObject& metadata = QJsonObject());

    QList<HistoryEvent> getEvents(const QString& deviceId = QString(),
                                   const QString& eventType = QString(),
                                   const QDateTime& from = QDateTime(),
                                   const QDateTime& to = QDateTime()) const;

    void clearHistory(const QString& deviceId = QString());
    void pruneOldEvents(int daysToKeep = 30);

signals:
    void eventRecorded(const HistoryEvent& event);

private:
    HistoryDao* m_dao;
};
```

**Event Types**:
- **"scan"**: Network scan performed
- **"status_change"**: Device went online/offline
- **"alert"**: Alert generated
- **"user_action"**: User performed action (export, Wake-on-LAN, etc.)

**Usage Example**:
```cpp
HistoryService historyService(historyDao);

// Record scan event
QJsonObject scanMetadata;
scanMetadata["subnet"] = "192.168.1.0/24";
scanMetadata["scanType"] = "Quick Scan";
scanMetadata["devicesFound"] = 15;

historyService.recordEvent("", "scan", "Network scan completed", scanMetadata);

// Record device status change
QJsonObject statusMetadata;
statusMetadata["previousStatus"] = "Online";
statusMetadata["newStatus"] = "Offline";

historyService.recordEvent("192.168.1.100", "status_change",
                           "Device went offline", statusMetadata);
```

### 5.4 WakeOnLanService

**Purpose**: Send magic packets to wake sleeping devices

**File**: `src/services/WakeOnLanService.h`, `src/services/WakeOnLanService.cpp`

**Class Definition**:
```cpp
class WakeOnLanService : public QObject {
    Q_OBJECT

public:
    explicit WakeOnLanService(QObject* parent = nullptr);

    void sendMagicPacket(const QString& macAddress,
                         const QString& broadcastAddress = "255.255.255.255",
                         int port = 9);

signals:
    void packetSent(const QString& macAddress);
    void sendError(const QString& error);

private:
    QByteArray buildMagicPacket(const QString& macAddress);
    QByteArray macStringToBytes(const QString& mac);

    QUdpSocket* m_socket;
};
```

**Magic Packet Format**:
```
┌────────────────────────────────────────┐
│  6 bytes of 0xFF                       │  Synchronization stream
├────────────────────────────────────────┤
│  Target MAC address (6 bytes)          │  Repeated 16 times
│  Target MAC address (6 bytes)          │
│  ... (14 more repetitions)             │
│  Target MAC address (6 bytes)          │
└────────────────────────────────────────┘
Total: 102 bytes
```

**Magic Packet Builder**:
```cpp
QByteArray WakeOnLanService::buildMagicPacket(const QString& macAddress) {
    QByteArray packet;

    // 6 bytes of 0xFF
    for (int i = 0; i < 6; ++i) {
        packet.append(static_cast<char>(0xFF));
    }

    // MAC address repeated 16 times
    QByteArray mac = macStringToBytes(macAddress);
    for (int i = 0; i < 16; ++i) {
        packet.append(mac);
    }

    return packet;
}

QByteArray WakeOnLanService::macStringToBytes(const QString& mac) {
    // Convert "AA:BB:CC:DD:EE:FF" or "AA-BB-CC-DD-EE-FF" to bytes
    QString cleanMac = mac;
    cleanMac.remove(':').remove('-');

    QByteArray bytes;
    for (int i = 0; i < cleanMac.length(); i += 2) {
        QString hex = cleanMac.mid(i, 2);
        bytes.append(static_cast<char>(hex.toInt(nullptr, 16)));
    }

    return bytes;
}
```

**Usage Example**:
```cpp
WakeOnLanService wolService;

connect(&wolService, &WakeOnLanService::packetSent,
        [](const QString& macAddress) {
    qDebug() << "Magic packet sent to" << macAddress;
});

connect(&wolService, &WakeOnLanService::sendError,
        [](const QString& error) {
    qDebug() << "WoL error:" << error;
});

wolService.sendMagicPacket("00:11:22:33:44:55");  // Wake device
```

---

## 6. Database Layer

### 6.1 DatabaseManager

**Purpose**: SQLite database connection management (Singleton)

**File**: `src/database/DatabaseManager.h`, `src/database/DatabaseManager.cpp`

**Class Definition**:
```cpp
class DatabaseManager {
public:
    static DatabaseManager* instance();

    bool open(const QString& dbPath = QString());
    void close();
    bool isOpen() const;

    QSqlDatabase& database();  // For transactions

    bool createTables();
    bool executeMigrations();

    // Delete copy/move
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

private:
    DatabaseManager();
    ~DatabaseManager();

    QSqlDatabase m_database;
    QString m_connectionName;
};
```

**Database Schema**:
```sql
-- Devices table
CREATE TABLE IF NOT EXISTS devices (
    ip TEXT PRIMARY KEY,
    hostname TEXT,
    mac TEXT,
    vendor TEXT,
    status TEXT,
    first_seen DATETIME,
    last_seen DATETIME
);

-- Ports table
CREATE TABLE IF NOT EXISTS ports (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    device_ip TEXT NOT NULL,
    port INTEGER NOT NULL,
    protocol TEXT,
    service TEXT,
    state TEXT,
    FOREIGN KEY (device_ip) REFERENCES devices(ip) ON DELETE CASCADE
);

-- Metrics table (historical)
CREATE TABLE IF NOT EXISTS metrics (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    device_ip TEXT NOT NULL,
    latency_avg REAL,
    latency_min REAL,
    latency_max REAL,
    jitter REAL,
    packet_loss REAL,
    quality_score INTEGER,
    timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (device_ip) REFERENCES devices(ip) ON DELETE CASCADE
);

-- Alerts table
CREATE TABLE IF NOT EXISTS alerts (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    device_ip TEXT,
    alert_type TEXT NOT NULL,
    severity TEXT NOT NULL,
    description TEXT,
    timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
    acknowledged INTEGER DEFAULT 0,
    FOREIGN KEY (device_ip) REFERENCES devices(ip) ON DELETE SET NULL
);

-- History/Events table
CREATE TABLE IF NOT EXISTS history (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    device_ip TEXT,
    event_type TEXT NOT NULL,
    description TEXT,
    metadata TEXT,  -- JSON
    timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (device_ip) REFERENCES devices(ip) ON DELETE SET NULL
);

-- Indices for performance
CREATE INDEX IF NOT EXISTS idx_metrics_device_time
    ON metrics(device_ip, timestamp);
CREATE INDEX IF NOT EXISTS idx_alerts_device
    ON alerts(device_ip);
CREATE INDEX IF NOT EXISTS idx_history_device_type
    ON history(device_ip, event_type);
```

**Usage Example**:
```cpp
DatabaseManager* dbMgr = DatabaseManager::instance();

if (!dbMgr->open("lanscan.db")) {
    qCritical() << "Failed to open database!";
    return;
}

if (!dbMgr->createTables()) {
    qCritical() << "Failed to create tables!";
    return;
}

// Use database via repositories
DeviceRepository deviceRepo(dbMgr);
```

### 6.2 DeviceRepository

**Purpose**: CRUD operations for Device model with caching

**File**: `src/database/DeviceRepository.h`, `src/database/DeviceRepository.cpp`

**Interface** (`IDeviceRepository`):
```cpp
class IDeviceRepository {
public:
    virtual ~IDeviceRepository() = default;

    virtual Device findById(const QString& ip) = 0;
    virtual QList<Device> findAll() = 0;
    virtual void save(const Device& device) = 0;
    virtual void remove(const QString& ip) = 0;
    virtual void clear() = 0;
    virtual int count() const = 0;
};
```

**Implementation**:
```cpp
class DeviceRepository : public IDeviceRepository {
public:
    explicit DeviceRepository(DatabaseManager* dbManager);

    Device findById(const QString& ip) override;
    QList<Device> findAll() override;
    void save(const Device& device) override;
    void remove(const QString& ip) override;
    void clear() override;
    int count() const override;

    // Additional queries
    QList<Device> findByStatus(bool online);
    QList<Device> findByVendor(const QString& vendor);

private:
    void savePorts(const QString& deviceIp, const QList<PortInfo>& ports);
    QList<PortInfo> loadPorts(const QString& deviceIp);

    DatabaseManager* m_dbManager;
    DeviceCache* m_cache;
};
```

**Save Operation (with cache)**:
```cpp
void DeviceRepository::save(const Device& device) {
    QSqlQuery query(m_dbManager->database());

    // Check if device exists
    query.prepare("SELECT ip FROM devices WHERE ip = ?");
    query.addBindValue(device.getIp());
    query.exec();

    if (query.next()) {
        // Update existing device
        query.prepare(R"(
            UPDATE devices
            SET hostname = ?, mac = ?, vendor = ?,
                status = ?, last_seen = ?
            WHERE ip = ?
        )");
        query.addBindValue(device.getHostname());
        query.addBindValue(device.getMacAddress());
        query.addBindValue(device.getVendor());
        query.addBindValue(device.isOnline() ? "Online" : "Offline");
        query.addBindValue(device.getLastSeen());
        query.addBindValue(device.getIp());
    } else {
        // Insert new device
        query.prepare(R"(
            INSERT INTO devices
            (ip, hostname, mac, vendor, status, first_seen, last_seen)
            VALUES (?, ?, ?, ?, ?, ?, ?)
        )");
        query.addBindValue(device.getIp());
        query.addBindValue(device.getHostname());
        query.addBindValue(device.getMacAddress());
        query.addBindValue(device.getVendor());
        query.addBindValue(device.isOnline() ? "Online" : "Offline");
        query.addBindValue(device.getFirstSeen());
        query.addBindValue(device.getLastSeen());
    }

    if (!query.exec()) {
        qWarning() << "Failed to save device:" << query.lastError().text();
        return;
    }

    // Save ports
    savePorts(device.getIp(), device.getOpenPorts());

    // Update cache
    m_cache->put(device.getIp(), device);
}
```

### 6.3 DeviceCache

**Purpose**: In-memory LRU cache for fast device lookups

**File**: `src/database/DeviceCache.h`, `src/database/DeviceCache.cpp`

**Class Definition**:
```cpp
class DeviceCache {
public:
    explicit DeviceCache(int maxSize = 1000);

    void put(const QString& key, const Device& device);
    Device get(const QString& key) const;
    bool contains(const QString& key) const;
    void remove(const QString& key);
    void clear();

    int size() const;
    int maxSize() const;

private:
    void evictLRU();

    int m_maxSize;
    QHash<QString, Device> m_cache;
    QList<QString> m_lruList;  // Least recently used order
    mutable QMutex m_mutex;    // Thread-safe
};
```

**LRU Eviction**:
```cpp
void DeviceCache::evictLRU() {
    if (m_lruList.isEmpty()) {
        return;
    }

    // Remove least recently used (front of list)
    QString keyToRemove = m_lruList.takeFirst();
    m_cache.remove(keyToRemove);
}

void DeviceCache::put(const QString& key, const Device& device) {
    QMutexLocker locker(&m_mutex);

    // If exists, remove from LRU list
    m_lruList.removeAll(key);

    // Add to end of LRU list (most recently used)
    m_lruList.append(key);

    // Add/update in cache
    m_cache[key] = device;

    // Evict if over capacity
    while (m_cache.size() > m_maxSize) {
        evictLRU();
    }
}
```

**Performance**:
- **Cache hit**: O(1) hash lookup + list reordering
- **Cache miss**: Database query (slower)
- **Typical hit rate**: 85-95% for repeated scans

### 6.4 HistoryDao & MetricsDao

**Purpose**: Data Access Objects for historical data

**HistoryDao**:
```cpp
class HistoryDao {
public:
    explicit HistoryDao(DatabaseManager* dbManager);

    void insert(const HistoryEvent& event);
    void insertBatch(const QList<HistoryEvent>& events);  // Transaction

    QList<HistoryEvent> findByDevice(const QString& deviceIp);
    QList<HistoryEvent> findByType(const QString& eventType);
    QList<HistoryEvent> findByDateRange(const QDateTime& from, const QDateTime& to);

    void deleteOlderThan(const QDateTime& cutoffDate);
    void deleteByDevice(const QString& deviceIp);

private:
    DatabaseManager* m_dbManager;
};
```

**MetricsDao**:
```cpp
class MetricsDao {
public:
    explicit MetricsDao(DatabaseManager* dbManager);

    void insert(const QString& deviceIp, const NetworkMetrics& metrics);
    void insertBatch(const QString& deviceIp, const QList<NetworkMetrics>& metrics);

    QList<NetworkMetrics> findByDevice(const QString& deviceIp);
    QList<NetworkMetrics> findByDateRange(const QDateTime& from, const QDateTime& to);

    // Statistical aggregation
    NetworkMetrics getAverageMetrics(const QString& deviceIp,
                                     const QDateTime& from,
                                     const QDateTime& to);
    double getMaxLatency(const QString& deviceIp, const QDateTime& from, const QDateTime& to);
    double getMinLatency(const QString& deviceIp, const QDateTime& from, const QDateTime& to);

    void deleteOlderThan(const QDateTime& cutoffDate);

private:
    DatabaseManager* m_dbManager;
};
```

**Statistical Aggregation Example**:
```cpp
NetworkMetrics MetricsDao::getAverageMetrics(const QString& deviceIp,
                                              const QDateTime& from,
                                              const QDateTime& to) {
    QSqlQuery query(m_dbManager->database());
    query.prepare(R"(
        SELECT
            AVG(latency_avg) as avg_latency,
            AVG(jitter) as avg_jitter,
            AVG(packet_loss) as avg_packet_loss
        FROM metrics
        WHERE device_ip = ? AND timestamp BETWEEN ? AND ?
    )");
    query.addBindValue(deviceIp);
    query.addBindValue(from);
    query.addBindValue(to);

    NetworkMetrics avgMetrics;
    if (query.exec() && query.next()) {
        avgMetrics.setLatencyAvg(query.value("avg_latency").toDouble());
        avgMetrics.setJitter(query.value("avg_jitter").toDouble());
        avgMetrics.setPacketLoss(query.value("avg_packet_loss").toDouble());
        avgMetrics.calculateQualityScore();
    }

    return avgMetrics;
}
```

---

## 7. Controllers

### 7.1 ScanController

**Purpose**: Orchestrate network scanning operations

**File**: `src/controllers/ScanController.h`, `src/controllers/ScanController.cpp`

**Class Definition**:
```cpp
class ScanController : public QObject {
    Q_OBJECT

public:
    explicit ScanController(
        ScanCoordinator* coordinator,
        IDeviceRepository* repository,
        QObject* parent = nullptr
    );

    void startQuickScan(const QString& subnet);
    void startDeepScan(const QString& subnet);
    void startCustomScan(const QString& subnet, const Profile& profile);
    void stopScan();

    bool isScanning() const;

signals:
    void deviceDiscovered(const Device& device);
    void scanProgress(int current, int total);
    void scanComplete(int devicesFound);
    void scanError(const QString& error);

private slots:
    void onDeviceDiscovered(const Device& device);
    void onScanComplete();

private:
    ScanCoordinator* m_coordinator;
    IDeviceRepository* m_repository;
    int m_devicesFound;
};
```

**Usage Example**:
```cpp
// Dependency injection
ScanCoordinator* coordinator = new ScanCoordinator(ipScanner, portScanner, ...);
DeviceRepository* repository = new DeviceRepository(dbManager);
ScanController* scanController = new ScanController(coordinator, repository);

// Connect signals
connect(scanController, &ScanController::deviceDiscovered,
        [](const Device& device) {
    qDebug() << "Discovered:" << device.getIp() << device.getHostname();
});

connect(scanController, &ScanController::scanComplete,
        [](int devicesFound) {
    qDebug() << "Scan complete!" << devicesFound << "devices found";
});

// Start scan
scanController->startQuickScan("192.168.1.0/24");
```

### 7.2 ExportController

**Purpose**: Manage export operations (CSV, JSON, XML, HTML)

**File**: `src/controllers/ExportController.h`, `src/controllers/ExportController.cpp`

**Class Definition**:
```cpp
class ExportController : public QObject {
    Q_OBJECT

public:
    explicit ExportController(
        IDeviceRepository* repository,
        QObject* parent = nullptr
    );

    void exportToCsv(const QString& filePath);
    void exportToJson(const QString& filePath);
    void exportToXml(const QString& filePath);
    void exportToHtml(const QString& filePath);

signals:
    void exportProgress(int current, int total);
    void exportComplete(const QString& filePath);
    void exportError(const QString& error);

private:
    QList<Device> getDevices();

    IDeviceRepository* m_repository;
    CsvExporter* m_csvExporter;
    JsonExporter* m_jsonExporter;
    XmlExporter* m_xmlExporter;
    HtmlReportGenerator* m_htmlGenerator;
};
```

**Auto-format Detection**:
```cpp
void MainWindow::onExportClicked() {
    QString filePath = QFileDialog::getSaveFileName(
        this,
        tr("Export Scan Results"),
        QDir::homePath(),
        tr("CSV Files (*.csv);;JSON Files (*.json);;XML Files (*.xml);;HTML Files (*.html)")
    );

    if (filePath.isEmpty()) {
        return;
    }

    // Auto-detect format from extension
    if (filePath.endsWith(".csv", Qt::CaseInsensitive)) {
        m_exportController->exportToCsv(filePath);
    } else if (filePath.endsWith(".json", Qt::CaseInsensitive)) {
        m_exportController->exportToJson(filePath);
    } else if (filePath.endsWith(".xml", Qt::CaseInsensitive)) {
        m_exportController->exportToXml(filePath);
    } else if (filePath.endsWith(".html", Qt::CaseInsensitive) ||
               filePath.endsWith(".htm", Qt::CaseInsensitive)) {
        m_exportController->exportToHtml(filePath);
    }
}
```

### 7.3 MetricsController

**Purpose**: Manage real-time metrics collection and monitoring

**File**: `src/controllers/MetricsController.h`, `src/controllers/MetricsController.cpp`

**Class Definition**:
```cpp
class MetricsController : public QObject {
    Q_OBJECT

public:
    explicit MetricsController(
        MetricsAggregator* aggregator,
        MonitoringService* monitoringService,
        QObject* parent = nullptr
    );

    void startMonitoring(const QString& deviceIp, int intervalSeconds = 5);
    void stopMonitoring(const QString& deviceIp);
    void collectMetrics(const QString& deviceIp, int pingCount = 10);

signals:
    void metricsCollected(const QString& deviceIp, const NetworkMetrics& metrics);
    void metricsError(const QString& error);
    void monitoringStarted(const QString& deviceIp);
    void monitoringStopped(const QString& deviceIp);

private:
    MetricsAggregator* m_aggregator;
    MonitoringService* m_monitoringService;
};
```

---

## 8. ViewModels & Views

### 8.1 DeviceTableViewModel

**Purpose**: Data model for device table with Qt Model/View architecture

**File**: `src/viewmodels/DeviceTableViewModel.h`, `src/viewmodels/DeviceTableViewModel.cpp`

**Class Definition**:
```cpp
class DeviceTableViewModel : public QAbstractTableModel {
    Q_OBJECT

public:
    enum Column {
        Status = 0,
        Ip,
        Hostname,
        Mac,
        Vendor,
        Ports,
        Latency,
        Quality,
        ColumnCount
    };

    explicit DeviceTableViewModel(QObject* parent = nullptr);

    // QAbstractTableModel interface
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    // Data management
    void addDevice(const Device& device);
    void updateDevice(const Device& device);
    void removeDevice(const QString& ip);
    void clear();

    Device getDevice(int row) const;
    QList<Device> getAllDevices() const;

signals:
    void deviceAdded(const Device& device);
    void deviceUpdated(const Device& device);
    void deviceRemoved(const QString& ip);

private:
    QList<Device> m_devices;
};
```

**Custom Data Roles**:
```cpp
QVariant DeviceTableViewModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= m_devices.size()) {
        return QVariant();
    }

    const Device& device = m_devices[index.row()];

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case Column::Ip:
            return device.getIp();
        case Column::Hostname:
            return device.getHostname();
        case Column::Mac:
            return device.getMacAddress();
        case Column::Vendor:
            return device.getVendor();
        case Column::Ports:
            return device.getOpenPorts().size();
        case Column::Latency:
            return QString::number(device.getMetrics().latencyAvg(), 'f', 1) + " ms";
        case Column::Quality:
            return device.getMetrics().qualityScore();
        }
    }
    else if (role == Qt::ForegroundRole && index.column() == Column::Quality) {
        // Color-code quality score
        int quality = device.getMetrics().qualityScore();
        if (quality >= 90) return QColor(Qt::darkGreen);      // Excellent
        else if (quality >= 70) return QColor(Qt::darkYellow); // Good
        else if (quality >= 50) return QColor(Qt::darkOrange); // Fair
        else return QColor(Qt::darkRed);                        // Poor
    }
    else if (role == Qt::TextAlignmentRole) {
        if (index.column() == Column::Ports ||
            index.column() == Column::Latency ||
            index.column() == Column::Quality) {
            return Qt::AlignRight | Qt::AlignVCenter;
        }
    }

    return QVariant();
}
```

### 8.2 MetricsViewModel

**Purpose**: Manage real-time metrics visualization and chart updates

**File**: `src/viewmodels/MetricsViewModel.h`, `src/viewmodels/MetricsViewModel.cpp`

**Class Definition**:
```cpp
class MetricsViewModel : public QObject {
    Q_OBJECT

public:
    explicit MetricsViewModel(
        IDeviceRepository* repository,
        MonitoringService* monitoringService,
        QObject* parent = nullptr
    );

    void startMonitoring(const QString& deviceIp);
    void stopMonitoring();

    QString getCurrentDevice() const;
    bool isMonitoring() const;

signals:
    void metricsUpdated(const NetworkMetrics& metrics);
    void deviceChanged(const QString& deviceIp);
    void monitoringStarted();
    void monitoringStopped();

private slots:
    void onMetricsCollected(const QString& deviceIp, const NetworkMetrics& metrics);

private:
    IDeviceRepository* m_repository;
    MonitoringService* m_monitoringService;
    QString m_currentDevice;
};
```

### 8.3 Chart ViewModels

**LatencyChart, PacketLossChart, JitterChart**

**Base Class** (`ChartViewModel`):
```cpp
class ChartViewModel : public QObject {
    Q_OBJECT

public:
    explicit ChartViewModel(QObject* parent = nullptr);

    virtual void addDataPoint(const NetworkMetrics& metrics) = 0;
    virtual void clear() = 0;

    QChart* chart() const;

protected:
    void setMaxDataPoints(int max);
    void pruneOldData();

    QChart* m_chart;
    int m_maxDataPoints;
};
```

**LatencyChart**:
```cpp
class LatencyChart : public ChartViewModel {
    Q_OBJECT

public:
    explicit LatencyChart(QObject* parent = nullptr);

    void addDataPoint(const NetworkMetrics& metrics) override;
    void clear() override;

private:
    QLineSeries* m_minSeries;
    QLineSeries* m_avgSeries;
    QLineSeries* m_maxSeries;
    QValueAxis* m_axisX;
    QValueAxis* m_axisY;
    int m_dataPointCount;
};
```

**Usage in View**:
```cpp
// In MetricsWidget or DeviceDetailDialog
LatencyChart* latencyChart = new LatencyChart(this);
PacketLossChart* packetLossChart = new PacketLossChart(this);
JitterChart* jitterChart = new JitterChart(this);

QChartView* latencyChartView = new QChartView(latencyChart->chart());
QChartView* packetLossChartView = new QChartView(packetLossChart->chart());
QChartView* jitterChartView = new QChartView(jitterChart->chart());

// Update charts on metrics update
connect(metricsViewModel, &MetricsViewModel::metricsUpdated,
        [=](const NetworkMetrics& metrics) {
    latencyChart->addDataPoint(metrics);
    packetLossChart->addDataPoint(metrics);
    jitterChart->addDataPoint(metrics);
});
```

---

## 9. Managers

### 9.1 ThemeManager

**Purpose**: Application-wide theme management (Singleton)

**File**: `src/managers/ThemeManager.h`, `src/managers/ThemeManager.cpp`

**Class Definition**:
```cpp
class ThemeManager : public QObject {
    Q_OBJECT

public:
    enum Theme {
        Light,
        Dark,
        System  // Auto-detect from OS
    };

    static ThemeManager* instance();

    void setTheme(Theme theme);
    Theme currentTheme() const;

    void setFontSize(int pointSize);
    int fontSize() const;

signals:
    void themeChanged(Theme theme);
    void fontSizeChanged(int pointSize);

private:
    ThemeManager();
    ~ThemeManager() = default;

    void applyTheme(Theme theme);
    void loadStyleSheet(const QString& resourcePath);
    Theme detectSystemTheme() const;

    Theme m_currentTheme;
    int m_fontSize;
};
```

**System Theme Detection** (Windows):
```cpp
ThemeManager::Theme ThemeManager::detectSystemTheme() const {
#ifdef Q_OS_WIN
    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                       QSettings::NativeFormat);
    int appsUseLightTheme = settings.value("AppsUseLightTheme", 1).toInt();
    return (appsUseLightTheme == 0) ? Dark : Light;
#elif defined(Q_OS_LINUX)
    // Check GTK theme or KDE theme (implementation varies)
    return Light;  // Default
#elif defined(Q_OS_MACOS)
    // Check macOS appearance mode
    return Light;  // Default
#endif
}
```

**Dynamic Font Size Replacement**:
```cpp
void ThemeManager::setFontSize(int pointSize) {
    m_fontSize = pointSize;

    // Reload current stylesheet with new font size
    QString styleSheet = qApp->styleSheet();

    // Replace all "font-size: XXpt" with new size using regex
    QRegularExpression fontSizePattern(R"(font-size:\s*\d+pt)");
    QString newFontSize = QString("font-size: %1pt").arg(pointSize);
    styleSheet.replace(fontSizePattern, newFontSize);

    qApp->setStyleSheet(styleSheet);
    emit fontSizeChanged(pointSize);
}
```

### 9.2 LanguageManager

**Purpose**: Multi-language support with Qt Linguist (Singleton)

**File**: `src/managers/LanguageManager.h`, `src/managers/LanguageManager.cpp`

**Class Definition**:
```cpp
class LanguageManager : public QObject {
    Q_OBJECT

public:
    enum Language {
        English,
        Italian,
        Spanish,
        French,
        German
    };

    static LanguageManager* instance();

    void setLanguage(Language language);
    Language currentLanguage() const;

    static QString languageToCode(Language language);
    static Language codeToLanguage(const QString& code);
    static QString languageToName(Language language);

signals:
    void languageChanged(Language language);

private:
    LanguageManager();
    ~LanguageManager() = default;

    void loadTranslation(Language language);
    void removeTranslation();

    Language m_currentLanguage;
    QTranslator* m_translator;
};
```

**Translation Loading**:
```cpp
void LanguageManager::loadTranslation(Language language) {
    // Remove existing translation
    if (m_translator) {
        qApp->removeTranslator(m_translator);
        delete m_translator;
        m_translator = nullptr;
    }

    // Load new translation
    QString code = languageToCode(language);
    QString translationFile = QString("lanscan_%1").arg(code);

    m_translator = new QTranslator(this);

    // Try loading from build directory first
    QString buildPath = QCoreApplication::applicationDirPath() + "/translations";
    if (m_translator->load(translationFile, buildPath)) {
        qApp->installTranslator(m_translator);
        m_currentLanguage = language;
        emit languageChanged(language);
        Logger::info(QString("Loaded translation: %1").arg(translationFile));
        return;
    }

    // Try loading from Qt resource system
    if (m_translator->load(translationFile, ":/translations")) {
        qApp->installTranslator(m_translator);
        m_currentLanguage = language;
        emit languageChanged(language);
        Logger::info(QString("Loaded translation from resources: %1").arg(translationFile));
        return;
    }

    Logger::warning(QString("Failed to load translation: %1").arg(translationFile));
}
```

### 9.3 ProfileManager

**Purpose**: Scan profile management with templates and statistics

**File**: `src/managers/ProfileManager.h`, `src/managers/ProfileManager.cpp`

**Class Definition**:
```cpp
class ProfileManager : public QObject {
    Q_OBJECT

public:
    explicit ProfileManager(const QString& profilesPath = QString(),
                           QObject* parent = nullptr);

    // CRUD
    void saveProfile(const Profile& profile);
    Profile loadProfile(const QString& profileId) const;
    QList<Profile> getAllProfiles() const;
    void deleteProfile(const QString& profileId);

    // Templates
    Profile createHomeNetworkProfile();
    Profile createEnterpriseNetworkProfile();
    Profile createSecurityAuditProfile();

    // Import/Export
    bool exportProfile(const QString& profileId, const QString& filePath);
    Profile importProfile(const QString& filePath);

    // Usage tracking
    void updateUsageStats(const QString& profileId);
    QDateTime getLastUsed(const QString& profileId) const;
    int getUsageCount(const QString& profileId) const;

signals:
    void profileAdded(const Profile& profile);
    void profileUpdated(const Profile& profile);
    void profileDeleted(const QString& profileId);

private:
    void loadFromFile();
    void saveToFile();

    QString m_profilesPath;
    QHash<QString, Profile> m_profiles;
};
```

**Template Profiles**:
```cpp
Profile ProfileManager::createHomeNetworkProfile() {
    Profile profile;
    profile.setId(QUuid::createUuid().toString());
    profile.setName("Home Network");
    profile.setDescription("Standard home network scan with common ports");

    // 6 common home network ports
    QList<int> ports = {
        80,    // HTTP
        443,   // HTTPS
        22,    // SSH
        21,    // FTP
        445,   // SMB
        3389   // RDP
    };
    profile.setPortList(ports);
    profile.setTimeout(2000);
    profile.setThreadCount(4);

    return profile;
}

Profile ProfileManager::createEnterpriseNetworkProfile() {
    Profile profile;
    profile.setId(QUuid::createUuid().toString());
    profile.setName("Enterprise Network");
    profile.setDescription("Comprehensive enterprise scan with database and management ports");

    // 25 enterprise ports
    QList<int> ports = {
        // Web
        80, 443, 8080, 8443,
        // Remote access
        22, 23, 3389,
        // File sharing
        21, 445, 139, 135,
        // Email
        25, 110, 143, 993, 995,
        // Database
        3306, 5432, 1433, 27017, 1521,
        // Management
        161, 162, 389, 636
    };
    profile.setPortList(ports);
    profile.setTimeout(2000);
    profile.setThreadCount(8);

    return profile;
}
```

### 9.4 FavoritesManager

**Purpose**: Favorite devices management with groups, notes, icons

**File**: `src/managers/FavoritesManager.h`, `src/managers/FavoritesManager.cpp`

**Class Definition**:
```cpp
class FavoritesManager : public QObject {
    Q_OBJECT

public:
    explicit FavoritesManager(const QString& favoritesPath = QString(),
                             QObject* parent = nullptr);

    // Basic operations
    void addFavorite(const QString& deviceIp);
    void removeFavorite(const QString& deviceIp);
    bool isFavorite(const QString& deviceIp) const;
    QStringList getAllFavorites() const;

    // Groups
    void createGroup(const QString& groupName);
    void deleteGroup(const QString& groupName);
    QStringList getGroups() const;
    void addToGroup(const QString& deviceIp, const QString& groupName);
    void removeFromGroup(const QString& deviceIp, const QString& groupName);
    QString getGroup(const QString& deviceIp) const;

    // Notes
    void addNote(const QString& deviceIp, const QString& note);
    QString getNote(const QString& deviceIp) const;
    void removeNote(const QString& deviceIp);

    // Custom icons
    void setCustomIcon(const QString& deviceIp, const QString& iconPath);
    QString getCustomIcon(const QString& deviceIp) const;
    void removeCustomIcon(const QString& deviceIp);

signals:
    void favoriteAdded(const QString& deviceIp);
    void favoriteRemoved(const QString& deviceIp);
    void groupCreated(const QString& groupName);
    void groupDeleted(const QString& groupName);

private:
    void loadFromFile();
    void saveToFile();

    struct FavoriteData {
        QString deviceIp;
        QString group;
        QString note;
        QString customIcon;
    };

    QString m_favoritesPath;
    QHash<QString, FavoriteData> m_favorites;
    QSet<QString> m_groups;
};
```

**JSON Persistence Format**:
```json
{
  "favorites": [
    {
      "deviceIp": "192.168.1.100",
      "group": "Servers",
      "note": "Production web server (Apache 2.4)",
      "customIcon": "/path/to/custom/icon.png"
    },
    {
      "deviceIp": "192.168.1.200",
      "group": "Printers",
      "note": "HP LaserJet in Room 201",
      "customIcon": ""
    }
  ],
  "groups": ["Servers", "Printers", "IoT Devices", "VMs"]
}
```

---

## 10. Utilities

### 10.1 Logger

**Purpose**: Application-wide logging with levels and file output

**File**: `src/utils/Logger.h`, `src/utils/Logger.cpp`

**Class Definition**:
```cpp
class Logger {
public:
    enum Level {
        Debug,
        Info,
        Warning,
        Error
    };

    static void setLogLevel(Level level);
    static Level logLevel();

    static void enableFileLogging(bool enable, const QString& filePath = QString());
    static void enableConsoleOutput(bool enable);

    static void debug(const QString& message);
    static void info(const QString& message);
    static void warning(const QString& message);
    static void error(const QString& message);

private:
    static void log(Level level, const QString& message);
    static QString levelToString(Level level);

    static Level s_level;
    static bool s_fileLoggingEnabled;
    static bool s_consoleOutputEnabled;
    static QString s_logFilePath;
    static QMutex s_mutex;
};
```

**Log Format**:
```
[2025-10-30 15:42:18.123] [INFO] Application started
[2025-10-30 15:42:19.456] [DEBUG] Database opened: lanscan.db
[2025-10-30 15:42:20.789] [INFO] Starting quick scan on 192.168.1.0/24
[2025-10-30 15:42:25.012] [INFO] Device discovered: 192.168.1.100 (server-01)
[2025-10-30 15:42:30.345] [WARNING] DNS lookup failed for 192.168.1.150
[2025-10-30 15:42:35.678] [ERROR] Failed to connect to device 192.168.1.200
```

**Usage Example**:
```cpp
// Configure logger at startup
Logger::setLogLevel(Logger::Info);
Logger::enableFileLogging(true, "lanscan.log");
Logger::enableConsoleOutput(true);

// Use throughout application
Logger::info("Application started");
Logger::debug("Scanning subnet: 192.168.1.0/24");
Logger::warning("Device not responding");
Logger::error("Failed to open database");
```

### 10.2 IpAddressValidator

**Purpose**: Validate IP addresses and CIDR notation

**File**: `src/utils/IpAddressValidator.h`, `src/utils/IpAddressValidator.cpp`

**Class Definition**:
```cpp
class IpAddressValidator {
public:
    static bool isValidIp(const QString& ip);
    static bool isValidSubnet(const QString& subnet);
    static bool isValidCidr(const QString& cidr);

    static QPair<QString, int> parseCidr(const QString& cidr);
    static QList<QString> generateIpRange(const QString& cidr);

    static int cidrToHostCount(int cidrBits);
    static QString getNetworkAddress(const QString& ip, int cidrBits);
    static QString getBroadcastAddress(const QString& ip, int cidrBits);
};
```

**IP Validation**:
```cpp
bool IpAddressValidator::isValidIp(const QString& ip) {
    QRegularExpression ipPattern(
        R"(^((25[0-5]|(2[0-4]|1\d|[1-9]|)\d)\.){3}(25[0-5]|(2[0-4]|1\d|[1-9]|)\d)$)"
    );
    return ipPattern.match(ip).hasMatch();
}

bool IpAddressValidator::isValidCidr(const QString& cidr) {
    QStringList parts = cidr.split('/');
    if (parts.size() != 2) {
        return false;
    }

    if (!isValidIp(parts[0])) {
        return false;
    }

    bool ok;
    int bits = parts[1].toInt(&ok);
    return ok && bits >= 0 && bits <= 32;
}
```

**CIDR IP Range Generation**:
```cpp
QList<QString> IpAddressValidator::generateIpRange(const QString& cidr) {
    QPair<QString, int> parsed = parseCidr(cidr);
    QString networkIp = parsed.first;
    int cidrBits = parsed.second;

    QList<QString> ips;

    // Parse network address
    QStringList octets = networkIp.split('.');
    quint32 networkAddr = (octets[0].toUInt() << 24) |
                          (octets[1].toUInt() << 16) |
                          (octets[2].toUInt() << 8) |
                          octets[3].toUInt();

    // Calculate host count
    int hostBits = 32 - cidrBits;
    int hostCount = (1 << hostBits) - 2;  // Exclude network and broadcast

    // Generate all host IPs
    for (int i = 1; i <= hostCount; ++i) {
        quint32 hostAddr = networkAddr + i;
        QString ip = QString("%1.%2.%3.%4")
                     .arg((hostAddr >> 24) & 0xFF)
                     .arg((hostAddr >> 16) & 0xFF)
                     .arg((hostAddr >> 8) & 0xFF)
                     .arg(hostAddr & 0xFF);
        ips.append(ip);
    }

    return ips;
}
```

### 10.3 StatisticsCalculator

**Purpose**: Calculate network statistics (mean, median, stddev, percentiles)

**File**: `src/utils/StatisticsCalculator.h`, `src/utils/StatisticsCalculator.cpp`

**Class Definition**:
```cpp
class StatisticsCalculator {
public:
    static double mean(const QVector<double>& values);
    static double median(const QVector<double>& values);
    static double standardDeviation(const QVector<double>& values);
    static double percentile(const QVector<double>& values, double percent);

    static double min(const QVector<double>& values);
    static double max(const QVector<double>& values);
    static double range(const QVector<double>& values);

    static QVector<double> movingAverage(const QVector<double>& values, int windowSize);
};
```

**Statistical Calculations**:
```cpp
double StatisticsCalculator::mean(const QVector<double>& values) {
    if (values.isEmpty()) return 0.0;
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    return sum / values.size();
}

double StatisticsCalculator::median(const QVector<double>& values) {
    if (values.isEmpty()) return 0.0;

    QVector<double> sorted = values;
    std::sort(sorted.begin(), sorted.end());

    int n = sorted.size();
    if (n % 2 == 0) {
        return (sorted[n/2 - 1] + sorted[n/2]) / 2.0;
    } else {
        return sorted[n/2];
    }
}

double StatisticsCalculator::standardDeviation(const QVector<double>& values) {
    if (values.size() < 2) return 0.0;

    double avg = mean(values);
    double sumSquaredDiff = 0.0;

    for (double value : values) {
        double diff = value - avg;
        sumSquaredDiff += diff * diff;
    }

    return std::sqrt(sumSquaredDiff / (values.size() - 1));
}
```

---

## 11. Extension Points

### 11.1 Creating Custom Scan Strategies

Implement `IScanStrategy` interface:

```cpp
class CustomScanStrategy : public IScanStrategy {
public:
    void scan(const QString& subnet) override {
        // Your custom scanning logic

        QList<QString> ips = IpAddressValidator::generateIpRange(subnet);

        for (const QString& ip : ips) {
            // Custom host discovery
            // Custom port scanning
            // Emit deviceDiscovered(device)
        }

        emit scanComplete();
    }
};
```

### 11.2 Adding Custom Exporters

Implement `IExporter` interface:

```cpp
class CustomExporter : public IExporter {
public:
    void exportDevices(const QList<Device>& devices, const QString& filePath) override {
        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly)) {
            emit exportError("Failed to open file");
            return;
        }

        QTextStream stream(&file);

        // Your custom export format
        for (const Device& device : devices) {
            stream << formatDevice(device) << "\n";
        }

        file.close();
        emit exportComplete(filePath);
    }

private:
    QString formatDevice(const Device& device) {
        // Your custom formatting logic
        return QString("%1|%2|%3").arg(device.getIp(), device.getHostname(), device.getMac());
    }
};
```

### 11.3 Creating Custom Widgets

Extend `QWidget` for custom visualizations:

```cpp
class CustomMetricsWidget : public QWidget {
    Q_OBJECT

public:
    explicit CustomMetricsWidget(QWidget* parent = nullptr)
        : QWidget(parent)
    {
        setupUi();
    }

    void updateMetrics(const NetworkMetrics& metrics) {
        // Update your custom visualization
        update();  // Trigger repaint
    }

protected:
    void paintEvent(QPaintEvent* event) override {
        QPainter painter(this);

        // Your custom painting logic
        painter.drawText(rect(), Qt::AlignCenter, "Custom Visualization");
    }

private:
    void setupUi() {
        setMinimumSize(400, 300);
    }
};
```

### 11.4 Adding New Diagnostic Tools

Extend diagnostic services:

```cpp
class CustomDiagnosticTool : public QObject {
    Q_OBJECT

public:
    explicit CustomDiagnosticTool(QObject* parent = nullptr);

    void runDiagnostic(const QString& target);

signals:
    void diagnosticComplete(const QString& result);
    void diagnosticError(const QString& error);

private:
    void executeDiagnostic();

    QString m_target;
    QProcess* m_process;
};
```

---

## 12. Build & Dependencies

### 12.1 CMake Configuration

**CMakeLists.txt Structure**:
```cmake
cmake_minimum_required(VERSION 3.16)
project(LanScan VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt6 packages
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Widgets
    Network
    Sql
    Charts
    Concurrent
    Svg
    LinguistTools
    Test
)

# Auto-generation
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Source files
set(SOURCES
    src/main.cpp
    src/models/Device.cpp
    src/models/PortInfo.cpp
    src/models/NetworkMetrics.cpp
    # ... (all source files)
)

set(HEADERS
    include/models/Device.h
    include/models/PortInfo.h
    include/models/NetworkMetrics.h
    # ... (all header files)
)

set(UI_FILES
    ui/mainwindow.ui
    ui/settingsdialog.ui
    # ... (all UI files)
)

set(RESOURCES
    resources/resources.qrc
)

# Executable
add_executable(LanScan ${SOURCES} ${HEADERS} ${UI_FILES} ${RESOURCES})

# Link libraries
target_link_libraries(LanScan PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
    Qt6::Sql
    Qt6::Charts
    Qt6::Concurrent
    Qt6::Svg
)

# Include directories
target_include_directories(LanScan PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/models
    ${CMAKE_SOURCE_DIR}/include/network
    # ... (all include directories)
)

# Enable testing
enable_testing()
add_subdirectory(tests)
```

### 12.2 Dependencies

**Required Qt6 Components**:
- **Qt6::Core** - Core non-GUI functionality
- **Qt6::Widgets** - GUI widgets (buttons, tables, dialogs)
- **Qt6::Network** - TCP/UDP sockets, DNS, HTTP
- **Qt6::Sql** - SQLite database integration
- **Qt6::Charts** - Metrics charts (line, bar, spline)
- **Qt6::Concurrent** - Multi-threading utilities
- **Qt6::Svg** - SVG icon rendering
- **Qt6::LinguistTools** - Translation tools (lupdate, lrelease)
- **Qt6::Test** - Unit testing framework

**System Libraries**:
- **SQLite 3** - Embedded database (bundled with Qt)
- **OpenSSL** (optional) - SSL/TLS support for secure connections

**Build Tools**:
- **CMake 3.16+** - Build system generator
- **C++17 Compiler** - MSVC 2019+, GCC 9+, Clang 10+
- **Qt 6.2+** - Framework (Qt 6.9.1 recommended)

**Platform-Specific Tools**:
- **Windows**: `ping.exe`, `tracert.exe`, `arp.exe` (built-in)
- **Linux**: `ping`, `traceroute`, `arp` or `ip neigh` (install via package manager)
- **macOS**: `ping`, `traceroute`, `arp` (built-in)

### 12.3 Compilation

**Debug Build**:
```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . -j8
```

**Release Build**:
```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release -j8
```

**With Tests**:
```bash
cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON ..
cmake --build . -j8
ctest --output-on-failure
```

### 12.4 Deployment

**Windows (windeployqt)**:
```bash
cd build/Release
windeployqt.exe LanScan.exe --release --no-compiler-runtime
```

**Linux (linuxdeployqt)**:
```bash
cd build
linuxdeployqt LanScan -appimage
```

**macOS (macdeployqt)**:
```bash
cd build
macdeployqt LanScan.app -dmg
```

---

## Appendix A: Class Hierarchy

```
QObject
├── IpScanner
├── PortScanner
├── HostDiscovery
├── DnsResolver
├── ArpDiscovery
├── PingService
├── TraceRouteService
├── MtuDiscovery
├── BandwidthTester
├── DnsDiagnostics
├── MonitoringService
├── AlertService
├── HistoryService
├── WakeOnLanService
├── ScanController
├── ExportController
├── MetricsController
├── ThemeManager
├── LanguageManager
├── ProfileManager
├── FavoritesManager
├── Logger (static utility)
├── IpAddressValidator (static utility)
├── StatisticsCalculator (static utility)
├── QAbstractTableModel
│   └── DeviceTableViewModel
├── ChartViewModel
│   ├── LatencyChart
│   ├── PacketLossChart
│   └── JitterChart
└── QWidget
    ├── MainWindow
    ├── SettingsDialog
    ├── DeviceDetailDialog
    ├── MetricsWidget
    ├── FavoritesWidget
    ├── QualityGauge
    ├── NetworkActivityIndicator
    └── GradientProgressBar
```

---

## Appendix B: Signal/Slot Connections

**Common Signal/Slot Patterns**:

```cpp
// Device discovery
connect(ipScanner, &IpScanner::deviceDiscovered,
        scanController, &ScanController::onDeviceDiscovered);

// Scan progress
connect(scanController, &ScanController::scanProgress,
        progressBar, &QProgressBar::setValue);

// Metrics update
connect(monitoringService, &MonitoringService::metricsCollected,
        metricsViewModel, &MetricsViewModel::onMetricsCollected);

// Alert notification
connect(monitoringService, &MonitoringService::alertTriggered,
        mainWindow, &MainWindow::onAlertTriggered);

// Theme change
connect(themeManager, &ThemeManager::themeChanged,
        mainWindow, &MainWindow::onThemeChanged);

// Export completion
connect(exportController, &ExportController::exportComplete,
        mainWindow, &MainWindow::onExportComplete);
```

---

## Appendix C: Performance Optimization Tips

**1. Use Caching**:
- DeviceCache for frequently accessed devices
- DNS cache for repeated lookups
- MAC vendor database loaded once (Singleton)

**2. Thread Pool Management**:
- Configure thread count based on CPU cores (4-8 recommended)
- Use `QThreadPool` for scan operations
- Avoid creating too many threads (overhead)

**3. Database Optimization**:
- Use indices on frequently queried columns
- Batch inserts with transactions
- Prune old data regularly (LRU, time-based)

**4. Network Operations**:
- Lower timeout for faster scans (but may miss slow devices)
- Use asynchronous operations (QNetworkAccessManager, QDnsLookup)
- Implement connection pooling for repeated connections

**5. UI Performance**:
- Use Qt Model/View for large datasets (lazy loading)
- Limit chart data points (default: 100)
- Disable animations for large tables

---

**End of API Documentation**

*For user documentation, see [USER_MANUAL.md](USER_MANUAL.md)*
*For installation instructions, see [INSTALLATION_GUIDE.md](INSTALLATION_GUIDE.md)*

**LanScan © 2025** - Network Scanner and Diagnostic Tool
Licensed under MIT License
https://github.com/paolosereno/LanScan
