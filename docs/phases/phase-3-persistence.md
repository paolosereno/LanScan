# Phase 3: Persistence & Data Management

**Timeline**: Week 7
**Objective**: Implement data persistence, caching, and export functionality

---

## 3.1 Repository Pattern

### DeviceRepository.h/cpp
Persist devices to SQLite database

```cpp
class DeviceRepository : public IDeviceRepository {
public:
    DeviceRepository(DatabaseManager* dbManager);

    void save(const Device& device) override;
    Device findById(const QString& id) override;
    Device findByIp(const QString& ip);
    QList<Device> findAll() override;
    QList<Device> findBySubnet(const QString& cidr);
    void remove(const QString& id) override;
    void update(const Device& device);

private:
    DatabaseManager* db;
    Device mapFromQuery(const QSqlQuery& query);
};
```

### DeviceCache.h/cpp
In-memory cache for fast access

```cpp
class DeviceCache {
public:
    void put(const QString& key, const Device& device);
    Device get(const QString& key);
    bool contains(const QString& key) const;
    void remove(const QString& key);
    void clear();
    QList<Device> getAll() const;

    // Cache eviction policy
    void setMaxSize(int size);
    void evictLRU(); // Least Recently Used

private:
    QMap<QString, Device> cache;
    QQueue<QString> accessOrder; // For LRU
    int maxCacheSize = 1000;
};
```

### Database Schema
```sql
CREATE TABLE devices (
    id TEXT PRIMARY KEY,
    ip TEXT NOT NULL UNIQUE,
    hostname TEXT,
    mac_address TEXT,
    vendor TEXT,
    is_online INTEGER,
    last_seen DATETIME,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE ports (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    device_id TEXT,
    port_number INTEGER,
    protocol TEXT,
    service TEXT,
    state TEXT,
    FOREIGN KEY (device_id) REFERENCES devices(id)
);

CREATE TABLE metrics (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    device_id TEXT,
    latency_min REAL,
    latency_avg REAL,
    latency_max REAL,
    latency_median REAL,
    jitter REAL,
    packet_loss REAL,
    quality_score TEXT,
    timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (device_id) REFERENCES devices(id)
);

CREATE INDEX idx_devices_ip ON devices(ip);
CREATE INDEX idx_devices_last_seen ON devices(last_seen);
CREATE INDEX idx_metrics_device_timestamp ON metrics(device_id, timestamp);
```

### DatabaseManager.h/cpp
```cpp
class DatabaseManager {
public:
    static DatabaseManager* instance();
    bool open(const QString& dbPath);
    void close();
    bool executeQuery(const QString& query);
    QSqlQuery prepareQuery(const QString& query);
    bool createSchema();

private:
    DatabaseManager();
    QSqlDatabase db;
    static DatabaseManager* _instance;
};
```

### Tests
- [ ] DeviceRepositoryTest (with in-memory SQLite)

---

## 3.2 Export Functionality

### ExportStrategy.h (Interface)
```cpp
class IExporter {
public:
    virtual ~IExporter() = default;
    virtual bool exportData(const QList<Device>& devices, const QString& filepath) = 0;
    virtual QString getFormatName() const = 0;
    virtual QString getFileExtension() const = 0;
};
```

### CsvExporter.h/cpp
Export to CSV format

```cpp
class CsvExporter : public IExporter {
public:
    bool exportData(const QList<Device>& devices, const QString& filepath) override;
    QString getFormatName() const override { return "CSV"; }
    QString getFileExtension() const override { return ".csv"; }

private:
    QString escapeField(const QString& field);
    QString buildCsvRow(const Device& device);
};
```

**CSV Format:**
```csv
IP,Hostname,MAC Address,Vendor,Status,Last Seen,Open Ports,Latency (ms),Packet Loss (%),Quality
192.168.1.1,router.local,AA:BB:CC:DD:EE:FF,Cisco,Online,2024-01-15 10:30:00,"80,443",5.2,0,Excellent
192.168.1.10,nas.local,11:22:33:44:55:66,Synology,Online,2024-01-15 10:30:00,"22,80,443,5000",12.5,0.5,Good
```

### JsonExporter.h/cpp
Export to JSON format

```cpp
class JsonExporter : public IExporter {
public:
    bool exportData(const QList<Device>& devices, const QString& filepath) override;
    QString getFormatName() const override { return "JSON"; }
    QString getFileExtension() const override { return ".json"; }

private:
    QJsonObject deviceToJson(const Device& device);
    QJsonArray devicesToJsonArray(const QList<Device>& devices);
};
```

**JSON Format:**
```json
{
  "export_date": "2024-01-15T10:30:00",
  "total_devices": 2,
  "devices": [
    {
      "ip": "192.168.1.1",
      "hostname": "router.local",
      "mac_address": "AA:BB:CC:DD:EE:FF",
      "vendor": "Cisco",
      "is_online": true,
      "last_seen": "2024-01-15T10:30:00",
      "open_ports": [80, 443],
      "metrics": {
        "latency_avg": 5.2,
        "packet_loss": 0,
        "jitter": 1.2,
        "quality_score": "Excellent"
      }
    }
  ]
}
```

### Tests
- [ ] CsvExporterTest
- [ ] JsonExporterTest

---

## 3.3 Settings Management

### SettingsManager.h/cpp
Wrapper around QSettings

```cpp
class SettingsManager {
public:
    static SettingsManager* instance();

    // General Settings
    QString getDefaultSubnet() const;
    void setDefaultSubnet(const QString& subnet);

    // Scan Settings
    int getScanTimeout() const;
    void setScanTimeout(int ms);
    int getMaxThreads() const;
    void setMaxThreads(int threads);

    // UI Settings
    QString getTheme() const;
    void setTheme(const QString& theme);
    QString getLanguage() const;
    void setLanguage(const QString& lang);

    // Export Settings
    QString getDefaultExportPath() const;
    void setDefaultExportPath(const QString& path);

    // Persistence
    void save();
    void load();
    void reset();

private:
    SettingsManager();
    QSettings* settings;
    static SettingsManager* _instance;
};
```

### ConfigValidator.h/cpp
Validate configuration values

```cpp
class ConfigValidator {
public:
    struct ValidationResult {
        bool isValid;
        QString errorMessage;
    };

    static ValidationResult validateSubnet(const QString& subnet);
    static ValidationResult validateTimeout(int timeout);
    static ValidationResult validateThreadCount(int threads);
    static ValidationResult validateExportPath(const QString& path);

    static bool isValidIpRange(const QString& start, const QString& end);
    static bool isValidPort(int port);
};
```

### Settings Storage Locations

- **Windows**: `HKEY_CURRENT_USER\Software\LanScan`
- **Linux**: `~/.config/LanScan/config.ini`
- **macOS**: `~/Library/Preferences/com.lanscan.plist`

### Tests
- [ ] SettingsManagerTest

---

## Implementation Details

### Repository Pattern Flow

```
Controller → DeviceRepository → Database
                ↓
            DeviceCache (read-through cache)
```

1. **Write Operation**:
   ```
   Controller → Repository.save() → DB Write → Cache Update
   ```

2. **Read Operation**:
   ```
   Controller → Repository.find()
   ↓
   Check Cache → If hit: return cached
   ↓
   If miss: Query DB → Update Cache → Return result
   ```

### Cache Strategy

- **Cache Key**: Device IP address (unique)
- **Eviction Policy**: LRU (Least Recently Used)
- **Max Size**: 1000 devices (configurable)
- **TTL**: No expiration (manual invalidation)

### Database Optimization

- **Indices**: Created on frequently queried columns (ip, last_seen)
- **Batch Inserts**: Use transactions for multiple inserts
- **Connection Pooling**: Single connection reused
- **Query Preparation**: Use prepared statements

### Export Performance

- **Streaming**: Write data as it's processed (don't load all in memory)
- **Async Export**: Run export in background thread
- **Progress Reporting**: Emit progress signals for UI updates
- **Large Datasets**: Handle 10,000+ devices efficiently

---

## Deliverable

**Phase 3 Completion Criteria**:
- ✅ SQLite database schema created
- ✅ DeviceRepository fully functional (CRUD operations)
- ✅ In-memory cache working with LRU eviction
- ✅ CSV export functional
- ✅ JSON export functional
- ✅ Settings persistence working
- ✅ Configuration validation implemented
- ✅ Unit tests passing with >90% coverage
- ✅ Performance: Handle 1000+ devices efficiently

---

## Integration Points

### With Phase 0
- Uses `Device` and `NetworkMetrics` models
- Uses `IDeviceRepository` interface
- Uses `IExporter` interface

### With Phase 1 & 2
- Persists discovered devices
- Stores metrics history
- Caches scan results

### For Phase 4
- Controllers will use Repository for data access
- Export will be triggered by ExportController
- Settings will configure scan behavior

---

## Data Migration Strategy

### Version 1.0 Schema
```sql
-- migrations/v1.0.sql
CREATE TABLE schema_version (
    version TEXT PRIMARY KEY,
    applied_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

INSERT INTO schema_version (version) VALUES ('1.0');
```

### Future Migrations
```cpp
class DatabaseMigrator {
public:
    bool migrate(const QString& fromVersion, const QString& toVersion);
    QString getCurrentVersion();
    QStringList getPendingMigrations();
};
```

---

## Next Phase

After completing Phase 3, proceed to:
→ **Phase 4: Application Layer & Controllers** (Week 8-9)
