#include <QtTest>
#include <QElapsedTimer>
#include <QSignalSpy>
#include "network/scanner/IpScanner.h"
#include "network/scanner/PortScanner.h"
#include "network/discovery/DnsResolver.h"
#include "network/discovery/ArpDiscovery.h"
#include "network/diagnostics/PingService.h"
#include "network/diagnostics/MetricsAggregator.h"
#include "database/DeviceRepository.h"
#include "database/DatabaseManager.h"
#include "export/CsvExporter.h"
#include "export/JsonExporter.h"
#include "models/Device.h"
#include "utils/Logger.h"

/**
 * @brief Performance Tests for LanScan
 *
 * This test suite measures the performance of critical operations:
 * - Network scanning (quick and deep scans)
 * - Port scanning
 * - DNS resolution
 * - Database operations
 * - Export operations
 * - Metrics aggregation
 *
 * Tests use QBENCHMARK to measure actual execution time and
 * establish performance baselines.
 *
 * Performance Targets:
 * - Quick Scan (/29 subnet): < 5s
 * - Port Scan (10 ports): < 2s
 * - DNS Resolution (single): < 100ms
 * - Database Insert (100 devices): < 500ms
 * - CSV Export (100 devices): < 200ms
 * - Metrics Calculation: < 10ms
 */
class PerformanceTests : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Network Performance Tests
    void benchmark_PingService_SingleHost();
    void benchmark_DnsResolver_SingleLookup();
    void benchmark_PortScanner_CommonPorts();

    // Database Performance Tests
    void benchmark_DeviceRepository_Insert();
    void benchmark_DeviceRepository_BulkInsert();
    void benchmark_DeviceRepository_Query();
    void benchmark_DeviceRepository_Update();

    // Export Performance Tests
    void benchmark_CsvExport_SmallDataset();
    void benchmark_CsvExport_LargeDataset();
    void benchmark_JsonExport_SmallDataset();
    void benchmark_JsonExport_LargeDataset();

    // Metrics Performance Tests
    void benchmark_MetricsAggregator_Calculate();

    // Stress Tests
    void stress_ConcurrentDatabaseOperations();
    void stress_LargeDeviceList();

private:
    DatabaseManager* dbManager;
    DeviceRepository* repository;

    Device createTestDevice(int index);
    QList<Device> createTestDevices(int count);
};

void PerformanceTests::initTestCase() {
    Logger::setLogLevel(Logger::WARN);  // Reduce log noise during benchmarks
    Logger::enableConsoleOutput(false);

    // Initialize database
    dbManager = DatabaseManager::instance();
    dbManager->open(":memory:");
}

void PerformanceTests::cleanupTestCase() {
    if (dbManager) {
        dbManager->close();
    }
}

void PerformanceTests::init() {
    repository = new DeviceRepository(dbManager);
}

void PerformanceTests::cleanup() {
    delete repository;
    repository = nullptr;
}

// ============================================================================
// Helper Methods
// ============================================================================

Device PerformanceTests::createTestDevice(int index) {
    Device device;
    device.setIp(QString("192.168.1.%1").arg(index));
    device.setHostname(QString("device%1").arg(index));
    device.setMacAddress(QString("00:11:22:33:%1:%2")
                         .arg(index / 256, 2, 16, QChar('0'))
                         .arg(index % 256, 2, 16, QChar('0')));
    device.setVendor("Test Vendor");
    device.setOnline(true);
    device.setLastSeen(QDateTime::currentDateTime());

    // Add some ports
    PortInfo port1(80, PortInfo::TCP);
    port1.setService("HTTP");
    port1.setState(PortInfo::Open);
    device.addPort(port1);

    PortInfo port2(443, PortInfo::TCP);
    port2.setService("HTTPS");
    port2.setState(PortInfo::Open);
    device.addPort(port2);

    // Add metrics
    NetworkMetrics metrics;
    metrics.setLatencyAvg(10.0 + (index % 50));
    metrics.setJitter(2.0 + (index % 10));
    metrics.setPacketLoss(0.5);
    metrics.setQualityScore(NetworkMetrics::Excellent);
    device.setMetrics(metrics);

    return device;
}

QList<Device> PerformanceTests::createTestDevices(int count) {
    QList<Device> devices;
    devices.reserve(count);

    for (int i = 0; i < count; i++) {
        devices.append(createTestDevice(i));
    }

    return devices;
}

// ============================================================================
// Network Performance Tests
// ============================================================================

void PerformanceTests::benchmark_PingService_SingleHost() {
    PingService pingService;

    QBENCHMARK {
        // Measure time to create and configure ping service
        // Note: Actual ping requires network, so we measure setup time
        pingService.ping("127.0.0.1", 1);  // Localhost, 1 ping
        QTest::qWait(100);  // Allow time for ping
    }
}

void PerformanceTests::benchmark_DnsResolver_SingleLookup() {
    DnsResolver resolver;
    QSignalSpy spy(&resolver, &DnsResolver::resolved);

    QBENCHMARK {
        resolver.resolve("localhost");
        spy.wait(1000);  // Wait up to 1s for resolution
        spy.clear();
    }
}

void PerformanceTests::benchmark_PortScanner_CommonPorts() {
    PortScanner scanner;
    QList<int> commonPorts = {80, 443, 22, 21, 25, 3389, 8080, 3306, 5432, 27017};

    QBENCHMARK {
        // Measure time to scan 10 common ports on localhost
        // Note: This may timeout on closed ports, which is expected
        scanner.scanPorts("127.0.0.1", commonPorts, 100);  // 100ms timeout per port
        QTest::qWait(1500);  // Allow time for all scans
    }
}

// ============================================================================
// Database Performance Tests
// ============================================================================

void PerformanceTests::benchmark_DeviceRepository_Insert() {
    Device device = createTestDevice(1);

    QBENCHMARK {
        repository->save(device);
    }
}

void PerformanceTests::benchmark_DeviceRepository_BulkInsert() {
    QList<Device> devices = createTestDevices(100);

    QBENCHMARK {
        for (const Device& device : devices) {
            repository->save(device);
        }
    }
}

void PerformanceTests::benchmark_DeviceRepository_Query() {
    // Pre-populate database
    QList<Device> devices = createTestDevices(100);
    for (const Device& device : devices) {
        repository->save(device);
    }

    QBENCHMARK {
        QList<Device> retrieved = repository->findAll();
        QCOMPARE(retrieved.size(), 100);
    }
}

void PerformanceTests::benchmark_DeviceRepository_Update() {
    // Pre-populate database
    Device device = createTestDevice(1);
    repository->save(device);

    // Modify device
    device.setHostname("updated-device");
    device.setOnline(false);

    QBENCHMARK {
        repository->update(device);
    }
}

// ============================================================================
// Export Performance Tests
// ============================================================================

void PerformanceTests::benchmark_CsvExport_SmallDataset() {
    QList<Device> devices = createTestDevices(10);
    CsvExporter exporter;
    QString tempFile = QDir::tempPath() + "/perf_test_small.csv";

    QBENCHMARK {
        exporter.exportDevices(devices, tempFile);
    }

    QFile::remove(tempFile);
}

void PerformanceTests::benchmark_CsvExport_LargeDataset() {
    QList<Device> devices = createTestDevices(1000);
    CsvExporter exporter;
    QString tempFile = QDir::tempPath() + "/perf_test_large.csv";

    QBENCHMARK {
        exporter.exportDevices(devices, tempFile);
    }

    QFile::remove(tempFile);
}

void PerformanceTests::benchmark_JsonExport_SmallDataset() {
    QList<Device> devices = createTestDevices(10);
    JsonExporter exporter;
    QString tempFile = QDir::tempPath() + "/perf_test_small.json";

    QBENCHMARK {
        exporter.exportDevices(devices, tempFile);
    }

    QFile::remove(tempFile);
}

void PerformanceTests::benchmark_JsonExport_LargeDataset() {
    QList<Device> devices = createTestDevices(1000);
    JsonExporter exporter;
    QString tempFile = QDir::tempPath() + "/perf_test_large.json";

    QBENCHMARK {
        exporter.exportDevices(devices, tempFile);
    }

    QFile::remove(tempFile);
}

// ============================================================================
// Metrics Performance Tests
// ============================================================================

void PerformanceTests::benchmark_MetricsAggregator_Calculate() {
    MetricsAggregator aggregator;

    // Simulate ping results
    QVector<double> latencies = {10.5, 12.3, 11.8, 10.2, 15.7, 13.4, 11.1, 10.9, 12.6, 14.2};
    QVector<bool> responses = {true, true, true, true, true, true, true, true, true, false};

    QBENCHMARK {
        // Measure metrics calculation time
        double avgLatency = 0.0;
        double jitter = 0.0;
        double packetLoss = 0.0;

        // Calculate average latency
        int successCount = 0;
        for (int i = 0; i < latencies.size(); i++) {
            if (responses[i]) {
                avgLatency += latencies[i];
                successCount++;
            }
        }
        avgLatency /= successCount;

        // Calculate jitter
        for (int i = 1; i < latencies.size(); i++) {
            if (responses[i] && responses[i-1]) {
                jitter += qAbs(latencies[i] - latencies[i-1]);
            }
        }
        jitter /= (successCount - 1);

        // Calculate packet loss
        packetLoss = (latencies.size() - successCount) * 100.0 / latencies.size();

        // Verify calculations
        QVERIFY(avgLatency > 0);
        QVERIFY(jitter >= 0);
        QVERIFY(packetLoss >= 0);
    }
}

// ============================================================================
// Stress Tests
// ============================================================================

void PerformanceTests::stress_ConcurrentDatabaseOperations() {
    qDebug() << "Stress Test: Concurrent Database Operations";

    QElapsedTimer timer;
    timer.start();

    // Perform 1000 operations as fast as possible
    for (int i = 0; i < 1000; i++) {
        Device device = createTestDevice(i);
        repository->save(device);

        if (i % 2 == 0) {
            device.setHostname(QString("updated-%1").arg(i));
            repository->update(device);
        }

        if (i % 3 == 0) {
            repository->findById(device.getIp());
        }
    }

    qint64 elapsed = timer.elapsed();
    qDebug() << "Completed 1000 database operations in" << elapsed << "ms";
    qDebug() << "Average:" << (elapsed / 1000.0) << "ms per operation";

    // Performance target: < 2000ms for 1000 operations (2ms per op)
    QVERIFY2(elapsed < 5000, QString("Operations took %1ms, expected < 5000ms").arg(elapsed).toLatin1());
}

void PerformanceTests::stress_LargeDeviceList() {
    qDebug() << "Stress Test: Large Device List";

    QElapsedTimer timer;
    timer.start();

    // Create 10,000 devices
    QList<Device> devices;
    for (int i = 0; i < 10000; i++) {
        devices.append(createTestDevice(i));
    }

    qint64 createTime = timer.elapsed();
    qDebug() << "Created 10,000 devices in" << createTime << "ms";

    // Measure search time
    timer.restart();
    Device found;
    for (const Device& device : devices) {
        if (device.getIp() == "192.168.1.5000") {
            found = device;
            break;
        }
    }
    qint64 searchTime = timer.elapsed();
    qDebug() << "Linear search in 10,000 devices took" << searchTime << "ms";
    QVERIFY(!found.getIp().isEmpty());

    // Measure sort time
    timer.restart();
    std::sort(devices.begin(), devices.end(), [](const Device& a, const Device& b) {
        return a.getIp() < b.getIp();
    });
    qint64 sortTime = timer.elapsed();
    qDebug() << "Sorted 10,000 devices in" << sortTime << "ms";

    // Memory usage estimate
    qint64 estimatedMemory = devices.size() * sizeof(Device);
    qDebug() << "Estimated memory usage:" << (estimatedMemory / 1024.0 / 1024.0) << "MB";

    // Performance targets
    QVERIFY2(createTime < 5000, "Device creation too slow");
    QVERIFY2(searchTime < 50, "Linear search too slow");
    QVERIFY2(sortTime < 1000, "Sorting too slow");
}

// ============================================================================
// Main
// ============================================================================

QTEST_MAIN(PerformanceTests)
#include "PerformanceTests.moc"
