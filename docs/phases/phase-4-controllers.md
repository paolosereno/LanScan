# Phase 4: Application Layer & Controllers

**Timeline**: Week 8-9
**Objective**: Orchestrate business logic, coordinate operations, implement multi-threading

---

## 4.1 Core Coordinators

### ScanCoordinator.h/cpp
Coordinate multiple scan operations

```cpp
class ScanCoordinator : public QObject {
    Q_OBJECT

public:
    struct ScanConfig {
        QString subnet;
        bool resolveDns;
        bool resolveArp;
        bool scanPorts;
        QList<int> portsToScan;
        int timeout;
        int maxThreads;
    };

    ScanCoordinator(
        IpScanner* ipScanner,
        PortScanner* portScanner,
        MetricsAggregator* metricsAggregator
    );

    void startScan(const ScanConfig& config);
    void stopScan();
    void pauseScan();
    void resumeScan();

signals:
    void scanStarted(int totalHosts);
    void deviceDiscovered(const Device& device);
    void scanProgress(int current, int total, const QString& currentIp);
    void scanCompleted(int devicesFound, qint64 durationMs);
    void scanError(const QString& error);

private:
    IpScanner* ipScanner;
    PortScanner* portScanner;
    MetricsAggregator* metricsAggregator;

    QThreadPool* threadPool;
    QFuture<void> scanFuture;
    bool isPaused;

    void coordinateScan(const ScanConfig& config);
    void processDiscoveredDevice(Device& device);
};
```

### Multi-threading with QThread
```cpp
class ScanWorker : public QObject {
    Q_OBJECT

public:
    ScanWorker(const QString& ip, const ScanCoordinator::ScanConfig& config);

public slots:
    void process();

signals:
    void deviceFound(const Device& device);
    void finished();
    void error(const QString& err);

private:
    QString ip;
    ScanCoordinator::ScanConfig config;
};

// Usage in ScanCoordinator
void ScanCoordinator::startScan(const ScanConfig& config) {
    QStringList ips = SubnetCalculator::getIpRange(config.subnet);

    for (const QString& ip : ips) {
        QThread* thread = new QThread;
        ScanWorker* worker = new ScanWorker(ip, config);
        worker->moveToThread(thread);

        connect(thread, &QThread::started, worker, &ScanWorker::process);
        connect(worker, &ScanWorker::finished, thread, &QThread::quit);
        connect(worker, &ScanWorker::deviceFound, this, &ScanCoordinator::deviceDiscovered);

        thread->start();
    }
}
```

### Progress Tracking
```cpp
class ProgressTracker {
private:
    std::atomic<int> current{0};
    std::atomic<int> total{0};

public:
    void setTotal(int t) { total = t; }
    void increment() { current++; }
    int getCurrent() const { return current; }
    int getTotal() const { return total; }
    double getPercentage() const {
        return total > 0 ? (current * 100.0) / total : 0;
    }
};
```

### Tests
- [ ] ScanCoordinatorTest

---

## 4.2 Controllers

### ScanController.h/cpp
Control scan workflow

```cpp
class ScanController : public QObject {
    Q_OBJECT

public:
    ScanController(
        ScanCoordinator* coordinator,
        DeviceRepository* repository,
        DeviceCache* cache
    );

    void executeQuickScan(const QString& subnet);
    void executeDeepScan(const QString& subnet);
    void executeCustomScan(const ScanCoordinator::ScanConfig& config);
    void stopCurrentScan();

signals:
    void scanStatusChanged(const QString& status);
    void devicesUpdated(const QList<Device>& devices);

private slots:
    void onDeviceDiscovered(const Device& device);
    void onScanCompleted(int count, qint64 duration);

private:
    ScanCoordinator* coordinator;
    DeviceRepository* repository;
    DeviceCache* cache;

    ScanCoordinator::ScanConfig createQuickScanConfig(const QString& subnet);
    ScanCoordinator::ScanConfig createDeepScanConfig(const QString& subnet);
};
```

### MetricsController.h/cpp
Control metrics collection

```cpp
class MetricsController : public QObject {
    Q_OBJECT

public:
    MetricsController(
        MetricsAggregator* aggregator,
        DeviceRepository* repository
    );

    void startContinuousMonitoring(const QString& deviceId);
    void stopContinuousMonitoring(const QString& deviceId);
    void collectMetricsOnce(const QString& deviceId);

    void startMonitoringAll();
    void stopMonitoringAll();

signals:
    void metricsCollected(const QString& deviceId, const NetworkMetrics& metrics);
    void metricsError(const QString& deviceId, const QString& error);

private slots:
    void onMetricsUpdated(const NetworkMetrics& metrics);

private:
    MetricsAggregator* aggregator;
    DeviceRepository* repository;

    QMap<QString, QTimer*> monitoringTimers;
    int defaultInterval = 1000; // ms
};
```

### ExportController.h/cpp
Control export operations

```cpp
class ExportController : public QObject {
    Q_OBJECT

public:
    enum ExportFormat {
        CSV,
        JSON,
        XML,
        HTML
    };

    ExportController(DeviceRepository* repository);

    void exportDevices(ExportFormat format, const QString& filepath);
    void exportDevice(const Device& device, ExportFormat format, const QString& filepath);
    void exportFiltered(const QList<Device>& devices, ExportFormat format, const QString& filepath);

signals:
    void exportStarted();
    void exportProgress(int percentage);
    void exportCompleted(const QString& filepath);
    void exportError(const QString& error);

private:
    DeviceRepository* repository;
    QMap<ExportFormat, IExporter*> exporters;

    IExporter* getExporter(ExportFormat format);
    void executeExport(const QList<Device>& devices, IExporter* exporter, const QString& filepath);
};
```

### Tests
- [ ] Integration tests for controllers

---

## 4.3 Management Services

### ProfileManager.h/cpp
Manage scan profiles

```cpp
class ScanProfile {
public:
    QString id;
    QString name;
    QString description;
    QString subnet;
    bool resolveDns;
    bool resolveArp;
    bool scanPorts;
    QList<int> portsToScan;
    int timeout;
    QDateTime createdAt;
};

class ProfileManager {
public:
    ProfileManager(const QString& storageFile);

    void saveProfile(const ScanProfile& profile);
    ScanProfile loadProfile(const QString& id);
    QList<ScanProfile> getAllProfiles();
    void deleteProfile(const QString& id);

    ScanProfile createQuickProfile(const QString& subnet);
    ScanProfile createDeepProfile(const QString& subnet);

private:
    QString storageFile;
    QMap<QString, ScanProfile> profiles;

    void loadFromFile();
    void saveToFile();
    QJsonObject profileToJson(const ScanProfile& profile);
    ScanProfile profileFromJson(const QJsonObject& json);
};
```

### FavoritesManager.h/cpp
Manage favorite devices

```cpp
class FavoriteDevice {
public:
    QString id;
    QString ip;
    QString name;        // User-defined name
    QString description; // User notes
    QStringList tags;    // User tags (e.g., "router", "server")
    bool wakeOnLanEnabled;
    QDateTime addedAt;
};

class FavoritesManager {
public:
    FavoritesManager(const QString& storageFile);

    void addFavorite(const FavoriteDevice& favorite);
    void removeFavorite(const QString& id);
    void updateFavorite(const FavoriteDevice& favorite);
    FavoriteDevice getFavorite(const QString& id);
    QList<FavoriteDevice> getAllFavorites();
    QList<FavoriteDevice> getFavoritesByTag(const QString& tag);

    bool isFavorite(const QString& ip);

private:
    QString storageFile;
    QMap<QString, FavoriteDevice> favorites;

    void loadFromFile();
    void saveToFile();
};
```

### Tests
- [ ] ProfileManagerTest

---

## Implementation Details

### Controller Architecture

```
UI Layer
   ↓
Controllers (Orchestration)
   ↓
Services/Coordinators (Business Logic)
   ↓
Repositories (Data Access)
   ↓
Database/Cache
```

### Dependency Injection Example

```cpp
// main.cpp - Setup DI container
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Infrastructure
    DatabaseManager* db = DatabaseManager::instance();
    db->open("lanscan.db");

    // Repositories
    DeviceRepository* deviceRepo = new DeviceRepository(db);
    DeviceCache* cache = new DeviceCache();

    // Network Services
    IpScanner* ipScanner = new IpScanner();
    PortScanner* portScanner = new PortScanner();
    MetricsAggregator* metricsAgg = new MetricsAggregator(/*...*/);

    // Coordinators
    ScanCoordinator* scanCoord = new ScanCoordinator(ipScanner, portScanner, metricsAgg);

    // Controllers
    ScanController* scanCtrl = new ScanController(scanCoord, deviceRepo, cache);
    MetricsController* metricsCtrl = new MetricsController(metricsAgg, deviceRepo);
    ExportController* exportCtrl = new ExportController(deviceRepo);

    // UI
    MainWindow* mainWindow = new MainWindow(scanCtrl, metricsCtrl, exportCtrl);
    mainWindow->show();

    return app.exec();
}
```

### Thread Safety

- **DeviceRepository**: Use mutex for database access
- **DeviceCache**: Thread-safe QMap access with QReadWriteLock
- **Signals/Slots**: Use Qt::QueuedConnection for cross-thread communication

```cpp
// Thread-safe cache access
class DeviceCache {
private:
    mutable QReadWriteLock lock;
    QMap<QString, Device> cache;

public:
    void put(const QString& key, const Device& device) {
        QWriteLocker locker(&lock);
        cache[key] = device;
    }

    Device get(const QString& key) {
        QReadLocker locker(&lock);
        return cache.value(key);
    }
};
```

### Error Handling Strategy

```cpp
class ScanController : public QObject {
    // ...
private:
    void handleScanError(const QString& error) {
        Logger::error("Scan failed: " + error);
        emit scanStatusChanged("Error: " + error);

        // Cleanup
        stopCurrentScan();

        // Notify UI
        QMessageBox::critical(nullptr, "Scan Error", error);
    }
};
```

---

## Deliverable

**Phase 4 Completion Criteria**:
- ✅ ScanCoordinator working with multi-threading
- ✅ All controllers implemented (Scan, Metrics, Export)
- ✅ Progress tracking functional
- ✅ Profile management working
- ✅ Favorites management working
- ✅ Thread safety ensured
- ✅ Dependency injection setup
- ✅ Integration tests passing
- ✅ Performance: Scan 254 IPs in < 10 seconds

---

## Integration Points

### With Phase 0-3
- Uses all models, utils, interfaces
- Uses repositories and cache
- Uses network services and metrics calculators

### For Phase 5
- Controllers will be injected into UI
- Controllers provide signals for UI updates
- UI calls controller methods for actions

---

## Performance Optimization

### Thread Pool Configuration
```cpp
int optimalThreads = QThread::idealThreadCount();
threadPool->setMaxThreadCount(optimalThreads * 2);
```

### Batch Processing
```cpp
// Process devices in batches for DB writes
void ScanCoordinator::processBatch(const QList<Device>& devices) {
    db->transaction();
    for (const Device& device : devices) {
        repository->save(device);
    }
    db->commit();
}
```

### Async Operations
```cpp
// Use QFuture for async operations
QFuture<QList<Device>> future = QtConcurrent::run([this, config]() {
    return executeScan(config);
});

futureWatcher->setFuture(future);
connect(futureWatcher, &QFutureWatcher::finished, this, &ScanController::onScanFinished);
```

---

## Next Phase

After completing Phase 4, proceed to:
→ **Phase 5: UI Foundation & Basic Views** (Week 10-11)
