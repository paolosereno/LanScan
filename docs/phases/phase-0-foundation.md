# Phase 0: Foundation & Infrastructure

**Timeline**: Week 1-2
**Objective**: Setup project, base infrastructure, foundational layers

---

## 0.1 Project Setup

### Tasks
- [ ] Initialize Git repository
- [ ] Setup base CMakeLists.txt with Qt6
- [ ] Create directory structure (src/, ui/, resources/, tests/)
- [ ] Configure Qt Designer workflow
- [ ] Setup CI/CD (optional: GitHub Actions)

### Directory Structure
```
LanScan/
├── src/
│   ├── main.cpp
│   ├── core/
│   ├── network/
│   ├── models/
│   ├── viewmodels/
│   ├── ui/
│   ├── controllers/
│   ├── services/
│   ├── persistence/
│   ├── utils/
│   └── interfaces/
├── ui/
├── resources/
├── tests/
├── CMakeLists.txt
└── README.md
```

---

## 0.2 Models Layer (Data Entities)

### Classes to Implement

#### Device.h/cpp
```cpp
class Device {
    QString ip;
    QString hostname;
    QString macAddress;
    QString vendor;
    bool isOnline;
    QDateTime lastSeen;
    QList<PortInfo> openPorts;
    NetworkMetrics metrics;
};
```

#### NetworkMetrics.h/cpp
```cpp
class NetworkMetrics {
    double latencyMin;
    double latencyAvg;
    double latencyMax;
    double latencyMedian;
    double jitter;
    double packetLoss;
    QString qualityScore; // Excellent/Good/Fair/Poor/Critical
    QDateTime timestamp;
};
```

#### PortInfo.h/cpp
```cpp
class PortInfo {
    int portNumber;
    QString protocol; // TCP/UDP
    QString service;
    QString state; // open/closed/filtered
};
```

#### NetworkInterface.h/cpp
```cpp
class NetworkInterface {
    QString name;
    QString ipAddress;
    QString subnetMask;
    QString gateway;
    QString macAddress;
    bool isActive;
};
```

### Tests
- [ ] DeviceTest
- [ ] NetworkMetricsTest

---

## 0.3 Utils Layer

### Classes to Implement

#### Logger.h/cpp
```cpp
class Logger {
public:
    enum Level { DEBUG, INFO, WARN, ERROR };
    static void log(Level level, const QString& message);
    static void debug(const QString& message);
    static void info(const QString& message);
    static void warn(const QString& message);
    static void error(const QString& message);
};
```

#### IpAddressValidator.h/cpp
```cpp
class IpAddressValidator {
public:
    static bool isValidIpv4(const QString& ip);
    static bool isValidCidr(const QString& cidr);
    static bool isValidIpRange(const QString& start, const QString& end);
};
```

#### StringFormatter.h/cpp
```cpp
class StringFormatter {
public:
    static QString formatMacAddress(const QString& mac);
    static QString formatBytes(qint64 bytes);
    static QString formatSpeed(double mbps);
};
```

#### TimeFormatter.h/cpp
```cpp
class TimeFormatter {
public:
    static QString formatDuration(qint64 milliseconds);
    static QString formatTimestamp(const QDateTime& dt);
    static QString formatRelativeTime(const QDateTime& dt);
};
```

#### StatisticsCalculator.h/cpp
```cpp
class StatisticsCalculator {
public:
    static double calculateMean(const QVector<double>& values);
    static double calculateMedian(const QVector<double>& values);
    static double calculateStdDev(const QVector<double>& values);
    static double calculateMin(const QVector<double>& values);
    static double calculateMax(const QVector<double>& values);
};
```

### Tests
- [ ] IpAddressValidatorTest
- [ ] StatisticsCalculatorTest

---

## 0.4 Interfaces Layer

### Interfaces to Define

#### IScanStrategy.h
```cpp
class IScanStrategy {
public:
    virtual ~IScanStrategy() = default;
    virtual void scan(const QString& target) = 0;
    virtual QString getStrategyName() const = 0;
};
```

#### IMetricsCalculator.h
```cpp
class IMetricsCalculator {
public:
    virtual ~IMetricsCalculator() = default;
    virtual double calculate(const QVector<double>& data) = 0;
};
```

#### IExporter.h
```cpp
class IExporter {
public:
    virtual ~IExporter() = default;
    virtual bool exportData(const QList<Device>& devices, const QString& filepath) = 0;
    virtual QString getFormatName() const = 0;
};
```

#### IDeviceRepository.h
```cpp
class IDeviceRepository {
public:
    virtual ~IDeviceRepository() = default;
    virtual void save(const Device& device) = 0;
    virtual Device findById(const QString& id) = 0;
    virtual QList<Device> findAll() = 0;
    virtual void remove(const QString& id) = 0;
};
```

---

## Deliverable

**Phase 0 Completion Criteria**:
- ✅ Base infrastructure working (CMake + Qt6)
- ✅ All data models implemented and tested
- ✅ All utility classes implemented and tested
- ✅ All core interfaces defined
- ✅ Unit tests passing with >90% coverage
- ✅ Clean architecture with proper dependency injection

---

## Dependencies

### Required Tools
- Qt 6.2+
- CMake 3.16+
- C++17 compiler (MSVC/GCC/Clang)
- Qt Test framework

### Qt Modules
- Qt Core
- Qt Network
- Qt Test (for unit tests)

---

## Best Practices

### Single Responsibility Principle (SRP)
Each class has ONE responsibility:
- **Models**: Data structure only
- **Utils**: Single-purpose utility functions
- **Interfaces**: Contract definition only

### Naming Conventions
- Classes: PascalCase (e.g., `Device`, `IpAddressValidator`)
- Methods: camelCase (e.g., `isValidIpv4`, `calculateMean`)
- Constants: UPPER_SNAKE_CASE (e.g., `MAX_DEVICES`)
- Interfaces: Prefix with `I` (e.g., `IScanStrategy`)

### Testing Strategy
- Unit test for each utility class
- Mock objects for external dependencies
- Test coverage target: >90% for utils and models
- Use Qt Test framework

---

## Next Phase

After completing Phase 0, proceed to:
→ **Phase 1: Network Layer & Core Discovery** (Week 3-4)
