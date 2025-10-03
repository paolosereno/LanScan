# Phase 2: Metrics & Diagnostics Core

**Timeline**: Week 5-6
**Objective**: Implement network metrics (latency, jitter, packet loss) and port scanning

---

## 2.1 Ping Service

### PingService.h/cpp
Execute ping operations using QProcess wrapper

```cpp
class PingService : public QObject {
    Q_OBJECT

public:
    struct PingResult {
        QString host;
        double latency;        // ms
        int ttl;
        int bytes;
        bool success;
        QString errorMessage;
    };

    void ping(const QString& host, int count = 4);
    PingResult pingSync(const QString& host, int timeout = 1000);
    void continuousPing(const QString& host, int interval = 1000);
    void stopContinuousPing();

signals:
    void pingResult(const PingResult& result);
    void pingCompleted(const QVector<PingResult>& results);

private:
    QProcess* pingProcess;
    PingResult parsePingOutput(const QString& output);
    QString buildPingCommand(const QString& host, int count);
};
```

### Platform-specific Parsing

#### Windows
```cpp
// Parse: "Reply from 192.168.1.1: bytes=32 time=5ms TTL=64"
PingResult PingService::parseWindowsPing(const QString& line) {
    QRegularExpression regex(R"(time[=<](\d+)ms)");
    QRegularExpressionMatch match = regex.match(line);
    // ...
}
```

#### Linux/macOS
```cpp
// Parse: "64 bytes from 192.168.1.1: icmp_seq=1 ttl=64 time=5.0 ms"
PingResult PingService::parseUnixPing(const QString& line) {
    QRegularExpression regex(R"(time=(\d+\.?\d*)\s*ms)");
    QRegularExpressionMatch match = regex.match(line);
    // ...
}
```

### Tests
- [ ] PingServiceTest with mock data

---

## 2.2 Metrics Calculators

### LatencyCalculator.h/cpp
Calculate latency statistics

```cpp
class LatencyCalculator : public IMetricsCalculator {
public:
    struct LatencyStats {
        double min;
        double max;
        double avg;
        double median;
        double stdDev;
    };

    double calculate(const QVector<double>& rttValues) override;
    LatencyStats calculateStats(const QVector<double>& rttValues);

private:
    double calculateMedian(const QVector<double>& sorted);
};
```

### JitterCalculator.h/cpp
Calculate jitter (latency variation)

```cpp
class JitterCalculator : public IMetricsCalculator {
public:
    // Jitter = standard deviation of latency
    double calculate(const QVector<double>& rttValues) override;

    // Jitter between consecutive measurements
    double calculateConsecutiveJitter(const QVector<double>& rttValues);

private:
    double calculateStdDev(const QVector<double>& values, double mean);
};
```

### PacketLossCalculator.h/cpp
Calculate packet loss percentage

```cpp
class PacketLossCalculator : public IMetricsCalculator {
public:
    struct PacketLossStats {
        int sent;
        int received;
        int lost;
        double lossPercentage;
        bool isBurstLoss;      // consecutive losses
        bool isRandomLoss;     // scattered losses
    };

    double calculate(const QVector<double>& data) override;
    PacketLossStats calculateStats(int sent, int received);
    bool detectBurstPattern(const QVector<bool>& successVector);
};
```

### QualityScoreCalculator.h/cpp
Calculate connection quality score

```cpp
class QualityScoreCalculator {
public:
    enum QualityRating {
        EXCELLENT,  // 90-100
        GOOD,       // 70-89
        FAIR,       // 50-69
        POOR,       // 30-49
        CRITICAL    // 0-29
    };

    struct QualityScore {
        double score;          // 0-100
        QualityRating rating;
        QString description;
    };

    QualityScore calculate(const NetworkMetrics& metrics);

private:
    double calculateLatencyScore(double latency);
    double calculatePacketLossScore(double loss);
    double calculateJitterScore(double jitter);

    // Weights
    const double LATENCY_WEIGHT = 0.30;
    const double PACKET_LOSS_WEIGHT = 0.40;
    const double JITTER_WEIGHT = 0.20;
    const double AVAILABILITY_WEIGHT = 0.10;
};
```

### Tests
- [ ] LatencyCalculatorTest
- [ ] JitterCalculatorTest
- [ ] PacketLossCalculatorTest
- [ ] QualityScoreCalculatorTest

---

## 2.3 Metrics Aggregation

### MetricsAggregator.h/cpp
Aggregate all metrics for a device

```cpp
class MetricsAggregator : public QObject {
    Q_OBJECT

public:
    MetricsAggregator(
        IMetricsCalculator* latencyCalc,
        IMetricsCalculator* jitterCalc,
        IMetricsCalculator* packetLossCalc,
        QualityScoreCalculator* qualityCalc
    );

    NetworkMetrics aggregate(const QVector<PingService::PingResult>& results);
    void startContinuousCollection(const QString& host, int interval = 1000);
    void stopContinuousCollection();

signals:
    void metricsUpdated(const NetworkMetrics& metrics);
    void metricsHistoryUpdated(const QVector<NetworkMetrics>& history);

private:
    IMetricsCalculator* latencyCalculator;
    IMetricsCalculator* jitterCalculator;
    IMetricsCalculator* packetLossCalculator;
    QualityScoreCalculator* qualityCalculator;

    PingService* pingService;
    QVector<PingService::PingResult> resultHistory;
    QTimer* collectionTimer;
};
```

### Continuous Ping Tracking
```cpp
// Store historical metrics for trending
class MetricsHistory {
private:
    QMap<QString, QVector<NetworkMetrics>> deviceMetrics;
    int maxHistorySize = 1000;

public:
    void addMetrics(const QString& deviceId, const NetworkMetrics& metrics);
    QVector<NetworkMetrics> getHistory(const QString& deviceId);
    QVector<NetworkMetrics> getRecentHistory(const QString& deviceId, int count);
    void clearHistory(const QString& deviceId);
};
```

### Tests
- [ ] MetricsAggregatorTest with dependency injection

---

## 2.4 Port Scanning

### PortScanner.h/cpp
TCP port scanning

```cpp
class PortScanner : public QObject {
    Q_OBJECT

public:
    enum ScanType {
        QUICK_SCAN,    // Common ports only
        FULL_SCAN,     // Ports 1-65535
        CUSTOM_SCAN    // User-defined range
    };

    struct PortScanResult {
        QString host;
        int port;
        QString state;      // open/closed/filtered
        QString service;
        double responseTime; // ms
    };

    void scanPorts(const QString& host, ScanType type);
    void scanPorts(const QString& host, const QList<int>& ports);
    void scanPortRange(const QString& host, int startPort, int endPort);

signals:
    void portFound(const PortScanResult& result);
    void scanProgress(int current, int total);
    void scanCompleted(const QList<PortScanResult>& results);

private:
    TcpSocketManager* socketManager;
    PortServiceMapper* serviceMapper;
    QThreadPool* threadPool;

    QList<int> getCommonPorts();
    PortScanResult scanSinglePort(const QString& host, int port, int timeout);
};
```

### Common Ports Definition
```cpp
QList<int> PortScanner::getCommonPorts() {
    return {
        21,    // FTP
        22,    // SSH
        23,    // Telnet
        25,    // SMTP
        53,    // DNS
        80,    // HTTP
        110,   // POP3
        143,   // IMAP
        443,   // HTTPS
        445,   // SMB
        3306,  // MySQL
        3389,  // RDP
        5432,  // PostgreSQL
        5900,  // VNC
        8080,  // HTTP-Alt
        8443   // HTTPS-Alt
    };
}
```

### Tests
- [ ] PortScannerTest

---

## Implementation Details

### Metrics Collection Flow

1. **Single Host Metrics**
   ```
   PingService → Multiple Pings → Results Vector
   ↓
   MetricsAggregator → Calculate all metrics
   ↓
   NetworkMetrics object → Emit metricsUpdated signal
   ```

2. **Continuous Monitoring**
   ```
   QTimer (1s interval) → PingService → Result
   ↓
   Append to History → Re-calculate metrics
   ↓
   Update UI with new metrics
   ```

### Performance Optimization

- **Port Scanning**: Use thread pool for parallel port checks
- **Ping**: Batch ping operations for multiple hosts
- **Metrics Calculation**: Cache results, recalculate only when new data arrives
- **Memory**: Limit history size (default 1000 entries per device)

### Error Handling

- Network unreachable during ping
- Timeout handling for port scans
- Invalid port ranges
- Permission errors (raw sockets)
- Memory limits for large histories

---

## Deliverable

**Phase 2 Completion Criteria**:
- ✅ Ping service working cross-platform
- ✅ Latency calculation (min/avg/max/median)
- ✅ Jitter calculation working correctly
- ✅ Packet loss calculation accurate
- ✅ Quality score algorithm implemented
- ✅ Metrics aggregation functional
- ✅ Port scanning operational (TCP)
- ✅ Continuous ping tracking working
- ✅ Unit tests passing with >85% coverage

---

## Integration Points

### With Phase 0
- Uses `NetworkMetrics` model
- Uses `PortInfo` model
- Uses `StatisticsCalculator` for calculations
- Uses `Logger` for debugging

### With Phase 1
- Uses `TcpSocketManager` for port scanning
- Uses `PingService` built on `HostDiscovery`
- Enriches `Device` objects with metrics

### For Phase 3
- Metrics will be persisted to database
- Export functionality will include metrics data

---

## Quality Metrics Algorithm

### Score Calculation
```
Total Score = (Latency × 0.30) + (PacketLoss × 0.40) + (Jitter × 0.20) + (Availability × 0.10)

Latency Score:
- < 20ms:  100 points
- < 50ms:  80 points
- < 100ms: 60 points
- < 200ms: 40 points
- >= 200ms: 20 points

Packet Loss Score:
- 0%:      100 points
- < 1%:    80 points
- < 5%:    50 points
- < 10%:   20 points
- >= 10%:  0 points

Jitter Score:
- < 5ms:   100 points
- < 20ms:  70 points
- < 50ms:  40 points
- >= 50ms: 10 points

Availability Score:
- 100% uptime: 100 points
- < 99%:       50 points
- < 95%:       0 points
```

---

## Next Phase

After completing Phase 2, proceed to:
→ **Phase 3: Persistence & Data Management** (Week 7)
