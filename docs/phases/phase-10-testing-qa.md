# Phase 10: Testing & Quality Assurance

**Timeline**: Week 18-19
**Objective**: Comprehensive testing, bug fixing, performance optimization, quality assurance

---

## 10.1 Complete Unit Tests

### Test Coverage Goals

- **Calculators**: 100% coverage (critical business logic)
- **Network Services**: >80% coverage
- **Repositories**: 100% coverage (data integrity)
- **Utils**: >90% coverage
- **ViewModels**: >80% coverage

### Calculator Tests (100% Coverage)

All metric calculator classes should have comprehensive unit tests:

```cpp
// LatencyCalculatorTest.cpp - Complete coverage
class LatencyCalculatorTest : public QObject {
    Q_OBJECT

private slots:
    void testCalculateMin();
    void testCalculateMax();
    void testCalculateAverage();
    void testCalculateMedian_OddCount();
    void testCalculateMedian_EvenCount();
    void testEmptyInput();
    void testSingleValue();
    void testNegativeValues();
    void testLargeDataset();
    void testDuplicateValues();
};

// JitterCalculatorTest.cpp
class JitterCalculatorTest : public QObject {
    Q_OBJECT

private slots:
    void testCalculateStandardDeviation();
    void testZeroJitter();
    void testHighVariability();
    void testConsistentValues();
    void testEmptyInput();
};

// PacketLossCalculatorTest.cpp
class PacketLossCalculatorTest : public QObject {
    Q_OBJECT

private slots:
    void testNoPacketLoss();
    void testPartialLoss();
    void testCompleteLoss();
    void testInvalidInput_ZeroSent();
    void testInvalidInput_NegativeValues();
    void testRoundingPrecision();
};

// QualityScoreCalculatorTest.cpp
class QualityScoreCalculatorTest : public QObject {
    Q_OBJECT

private slots:
    void testExcellentQuality();
    void testGoodQuality();
    void testFairQuality();
    void testPoorQuality();
    void testCriticalQuality();
    void testWeightedScore();
    void testBoundaryValues();
};
```

### Network Service Tests (>80% Coverage)

```cpp
// SubnetCalculatorTest.cpp
class SubnetCalculatorTest : public QObject {
    Q_OBJECT

private slots:
    void testCidrToRange_ClassC();
    void testCidrToRange_ClassB();
    void testCidrToRange_ClassA();
    void testCidrToRange_SmallSubnet();
    void testIsInSubnet_Valid();
    void testIsInSubnet_Invalid();
    void testGetNetworkAddress();
    void testGetBroadcastAddress();
    void testGetHostCount();
    void testInvalidCidr();
};

// IpScannerTest.cpp
class IpScannerTest : public QObject {
    Q_OBJECT

private slots:
    void testScanSingleIp_Reachable();
    void testScanSingleIp_Unreachable();
    void testScanSubnet_SmallRange();
    void testConcurrentScanning();
    void testTimeout();
    void testStopScan();
    void testProgressReporting();
};
```

### Repository Tests (100% Coverage)

```cpp
// DeviceRepositoryTest.cpp
class DeviceRepositoryTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase(); // Setup in-memory database
    void cleanupTestCase(); // Cleanup

    void testSave_NewDevice();
    void testSave_UpdateExisting();
    void testFindById_Existing();
    void testFindById_NotFound();
    void testFindAll();
    void testDelete();
    void testExists();
    void testClear();
    void testBatchInsert();
    void testTransaction();
    void testConcurrentAccess();

private:
    DeviceRepository* repository;
};
```

### Mock Objects for Dependencies

```cpp
// MockDeviceRepository.h
class MockDeviceRepository : public IDeviceRepository {
public:
    MOCK_METHOD(void, save, (const Device& device), (override));
    MOCK_METHOD(Device, findById, (const QString& id), (override));
    MOCK_METHOD(QList<Device>, findAll, (), (override));
    MOCK_METHOD(void, remove, (const QString& id), (override));

    // Helper methods for testing
    void addMockDevice(const Device& device);
    void clearMockData();
};

// MockPingService.h
class MockPingService : public IPingService {
public:
    MOCK_METHOD(PingResult, ping, (const QString& ip, int timeout), (override));
    MOCK_METHOD(void, pingAsync, (const QString& ip), (override));

    void setMockResult(const PingResult& result);
};

// MockMetricsCalculator.h
class MockMetricsCalculator : public IMetricsCalculator {
public:
    MOCK_METHOD(double, calculate, (const QVector<double>& data), (override));

    void setMockValue(double value);
};
```

---

## 10.2 Integration Tests

### Workflow Tests

```cpp
// ScanWorkflowTest.cpp
class ScanWorkflowTest : public QObject {
    Q_OBJECT

private slots:
    void testFullScanWorkflow();
    void testQuickScanWorkflow();
    void testDeepScanWorkflow();
    void testScanWithDnsResolution();
    void testScanWithPortScanning();
    void testStopScan();
    void testPauseScan();
    void testResumeScan();

private:
    void testFullScanWorkflow() {
        // Setup
        ScanController controller;
        QSignalSpy scanStartedSpy(&controller, &ScanController::scanStarted);
        QSignalSpy deviceDiscoveredSpy(&controller, &ScanController::deviceDiscovered);
        QSignalSpy scanCompletedSpy(&controller, &ScanController::scanCompleted);

        // Execute
        controller.executeQuickScan("192.168.1.0/29"); // Small subnet

        // Wait for completion
        QVERIFY(scanCompletedSpy.wait(30000)); // 30 seconds timeout

        // Verify
        QCOMPARE(scanStartedSpy.count(), 1);
        QVERIFY(deviceDiscoveredSpy.count() > 0);
        QCOMPARE(scanCompletedSpy.count(), 1);

        // Check devices
        QList<Device> devices = controller.getDiscoveredDevices();
        QVERIFY(devices.size() > 0);

        for (const Device& device : devices) {
            QVERIFY(!device.ipAddress.isEmpty());
            QVERIFY(IpAddressValidator::isValidIpv4(device.ipAddress));
        }
    }
};

// MetricsCollectionTest.cpp
class MetricsCollectionTest : public QObject {
    Q_OBJECT

private slots:
    void testCollectMetricsOnce();
    void testContinuousMonitoring();
    void testMultipleDeviceMonitoring();
    void testAlertTriggering();
    void testMetricsHistory();

private:
    void testCollectMetricsOnce() {
        MetricsController controller;
        Device device;
        device.id = "test-device";
        device.ipAddress = "8.8.8.8"; // Google DNS

        QSignalSpy metricsSpy(&controller, &MetricsController::metricsCollected);

        controller.collectMetricsOnce(device.id);

        QVERIFY(metricsSpy.wait(5000));

        QList<QVariant> arguments = metricsSpy.takeFirst();
        QString deviceId = arguments.at(0).toString();
        NetworkMetrics metrics = arguments.at(1).value<NetworkMetrics>();

        QCOMPARE(deviceId, device.id);
        QVERIFY(metrics.latencyAvg > 0);
        QVERIFY(metrics.timestamp.isValid());
    }
};

// ExportWorkflowTest.cpp
class ExportWorkflowTest : public QObject {
    Q_OBJECT

private slots:
    void testCsvExportWorkflow();
    void testJsonExportWorkflow();
    void testXmlExportWorkflow();
    void testHtmlExportWorkflow();
    void testEmptyExport();
    void testLargeDatasetExport();

private:
    void testCsvExportWorkflow() {
        // Prepare test data
        QList<Device> devices = createTestDevices(10);

        ExportController controller;
        QTemporaryFile tempFile;
        tempFile.open();
        QString filepath = tempFile.fileName();

        QSignalSpy completedSpy(&controller, &ExportController::exportCompleted);

        // Execute export
        controller.exportDevices(ExportController::CSV, filepath);

        // Wait for completion
        QVERIFY(completedSpy.wait(5000));

        // Verify file exists and has content
        QVERIFY(QFile::exists(filepath));
        QFile file(filepath);
        file.open(QIODevice::ReadOnly);
        QByteArray content = file.readAll();
        QVERIFY(content.size() > 0);

        // Verify CSV structure
        QString csvData = QString::fromUtf8(content);
        QStringList lines = csvData.split('\n', Qt::SkipEmptyParts);
        QVERIFY(lines.size() > 10); // Header + 10 devices
    }
};

// DatabaseIntegrationTest.cpp
class DatabaseIntegrationTest : public QObject {
    Q_OBJECT

private slots:
    void testDatabaseConnection();
    void testTableCreation();
    void testDataPersistence();
    void testTransactions();
    void testConcurrentAccess();
    void testDataMigration();

private:
    void testDataPersistence() {
        DatabaseManager* db = DatabaseManager::instance();
        db->open(":memory:");

        DeviceRepository repository(db);

        // Save devices
        Device device1;
        device1.id = "device-1";
        device1.ipAddress = "192.168.1.100";
        device1.hostname = "test-device";
        repository.save(device1);

        // Close and reopen database
        db->close();
        db->open(":memory:");

        // Verify data persists
        Device found = repository.findById("device-1");
        QCOMPARE(found.id, device1.id);
        QCOMPARE(found.ipAddress, device1.ipAddress);
    }
};
```

---

## 10.3 UI Tests

### ViewModel Tests

```cpp
// DeviceTableViewModelTest.cpp
class DeviceTableViewModelTest : public QObject {
    Q_OBJECT

private slots:
    void testRowCount();
    void testColumnCount();
    void testDataRetrieval();
    void testHeaderData();
    void testAddDevice();
    void testUpdateDevice();
    void testRemoveDevice();
    void testClear();
    void testSorting();
    void testFiltering();

private:
    void testAddDevice() {
        MockDeviceRepository mockRepo;
        DeviceTableViewModel viewModel(&mockRepo);

        QSignalSpy rowsInsertedSpy(&viewModel, &QAbstractItemModel::rowsInserted);

        Device device;
        device.ipAddress = "192.168.1.1";
        device.hostname = "test";

        viewModel.addDevice(device);

        QCOMPARE(rowsInsertedSpy.count(), 1);
        QCOMPARE(viewModel.rowCount(), 1);

        QModelIndex index = viewModel.index(0, DeviceTableViewModel::IpAddress);
        QCOMPARE(viewModel.data(index).toString(), device.ipAddress);
    }
};

// MetricsViewModelTest.cpp
class MetricsViewModelTest : public QObject {
    Q_OBJECT

private slots:
    void testSetDevice();
    void testStartMonitoring();
    void testStopMonitoring();
    void testMetricsUpdate();
    void testHistoryManagement();

private:
    void testMetricsUpdate() {
        MockMetricsController mockController;
        MetricsViewModel viewModel(&mockController);

        QSignalSpy metricsSpy(&viewModel, &MetricsViewModel::metricsUpdated);

        NetworkMetrics metrics;
        metrics.latencyAvg = 10.5;
        metrics.packetLoss = 0.0;
        metrics.jitter = 1.2;

        // Simulate metrics update from controller
        emit mockController.metricsCollected("device-1", metrics);

        QVERIFY(metricsSpy.wait(1000));

        NetworkMetrics latest = viewModel.getLatestMetrics();
        QCOMPARE(latest.latencyAvg, metrics.latencyAvg);
    }
};
```

### Signal/Slot Connection Tests

```cpp
// SignalSlotTest.cpp
class SignalSlotTest : public QObject {
    Q_OBJECT

private slots:
    void testScanControllerSignals();
    void testMetricsControllerSignals();
    void testViewModelSignals();

private:
    void testScanControllerSignals() {
        ScanController controller;

        // Test all signals are emitted correctly
        QSignalSpy scanStartedSpy(&controller, &ScanController::scanStarted);
        QSignalSpy deviceDiscoveredSpy(&controller, &ScanController::deviceDiscovered);
        QSignalSpy scanProgressSpy(&controller, &ScanController::scanProgress);
        QSignalSpy scanCompletedSpy(&controller, &ScanController::scanCompleted);

        controller.executeQuickScan("192.168.1.0/29");

        QVERIFY(scanStartedSpy.wait(1000));
        // Continue verification...
    }
};
```

### User Interaction Flow Tests

```cpp
// UserInteractionTest.cpp
class UserInteractionTest : public QObject {
    Q_OBJECT

private slots:
    void testScanButtonClick();
    void testDeviceSelection();
    void testContextMenuActions();
    void testDialogWorkflow();

private:
    void testScanButtonClick() {
        MainWindow window;
        window.show();

        // Find scan button
        QPushButton* scanButton = window.findChild<QPushButton*>("scanButton");
        QVERIFY(scanButton != nullptr);

        // Simulate click
        QTest::mouseClick(scanButton, Qt::LeftButton);

        // Verify scan started
        // ...
    }
};
```

---

## 10.4 Performance & Optimization

### Profiling

```bash
# Valgrind (Linux)
valgrind --tool=callgrind --dump-instr=yes --collect-jumps=yes ./LanScan

# Analyze with KCachegrind
kcachegrind callgrind.out.<pid>

# Qt Creator Profiler
# Run with profiler enabled in Qt Creator
# Analyze CPU usage, memory allocation, etc.
```

### Performance Tests

```cpp
// PerformanceTest.cpp
class PerformanceTest : public QObject {
    Q_OBJECT

private slots:
    void benchmarkSubnetCalculation();
    void benchmarkDatabaseOperations();
    void benchmarkMetricsCalculation();
    void benchmarkLargeNetworkScan();

private:
    void benchmarkSubnetCalculation() {
        SubnetCalculator calculator;

        QBENCHMARK {
            calculator.cidrToRange("192.168.1.0/24");
        }
    }

    void benchmarkDatabaseOperations() {
        DatabaseManager db;
        db.open(":memory:");
        DeviceRepository repository(&db);

        QList<Device> devices = createTestDevices(1000);

        QBENCHMARK {
            db.transaction();
            for (const Device& device : devices) {
                repository.save(device);
            }
            db.commit();
        }
    }

    void benchmarkLargeNetworkScan() {
        ScanController controller;

        QElapsedTimer timer;
        timer.start();

        controller.executeQuickScan("192.168.1.0/24"); // 254 IPs

        // Wait for completion
        QEventLoop loop;
        connect(&controller, &ScanController::scanCompleted, &loop, &QEventLoop::quit);
        loop.exec();

        qint64 elapsed = timer.elapsed();

        QVERIFY(elapsed < 10000); // Should complete in < 10 seconds
        qDebug() << "Scan completed in" << elapsed << "ms";
    }
};
```

### Memory Leak Detection

```cpp
// MemoryLeakTest.cpp
class MemoryLeakTest : public QObject {
    Q_OBJECT

private slots:
    void testRepeatedScans();
    void testRepeatedMetricsCollection();
    void testWidgetCreationDestruction();

private:
    void testRepeatedScans() {
        ScanController controller;

        // Record initial memory
        size_t initialMemory = getCurrentMemoryUsage();

        // Perform 100 scans
        for (int i = 0; i < 100; i++) {
            controller.executeQuickScan("192.168.1.0/29");

            // Wait for completion
            QEventLoop loop;
            connect(&controller, &ScanController::scanCompleted,
                    &loop, &QEventLoop::quit);
            loop.exec();
        }

        // Check memory growth
        size_t finalMemory = getCurrentMemoryUsage();
        size_t growth = finalMemory - initialMemory;

        // Allow some growth but not excessive (< 10 MB)
        QVERIFY(growth < 10 * 1024 * 1024);
    }
};
```

### Thread Pool Optimization

```cpp
// ThreadPoolTest.cpp
class ThreadPoolTest : public QObject {
    Q_OBJECT

private slots:
    void testOptimalThreadCount();
    void testThreadPoolPerformance();
    void testConcurrentTasks();

private:
    void testOptimalThreadCount() {
        int idealCount = QThread::idealThreadCount();
        qDebug() << "Ideal thread count:" << idealCount;

        ScanCoordinator coordinator;

        // Test different thread counts
        for (int threads = 1; threads <= idealCount * 2; threads++) {
            QElapsedTimer timer;
            timer.start();

            coordinator.setMaxThreads(threads);
            coordinator.startScan("192.168.1.0/28"); // 16 IPs

            // Wait for completion
            QEventLoop loop;
            connect(&coordinator, &ScanCoordinator::scanCompleted,
                    &loop, &QEventLoop::quit);
            loop.exec();

            qint64 elapsed = timer.elapsed();
            qDebug() << "Threads:" << threads << "Time:" << elapsed << "ms";
        }
    }
};
```

### Database Query Optimization

```cpp
// DatabaseOptimizationTest.cpp
class DatabaseOptimizationTest : public QObject {
    Q_OBJECT

private slots:
    void testIndexedQueries();
    void testBatchInserts();
    void testPreparedStatements();

private:
    void testBatchInserts() {
        DatabaseManager db;
        db.open(":memory:");
        DeviceRepository repository(&db);

        QList<Device> devices = createTestDevices(10000);

        // Single inserts (slow)
        QElapsedTimer singleTimer;
        singleTimer.start();
        for (const Device& device : devices) {
            repository.save(device);
        }
        qint64 singleTime = singleTimer.elapsed();

        // Clear table
        repository.clear();

        // Batch inserts (fast)
        QElapsedTimer batchTimer;
        batchTimer.start();
        db.transaction();
        for (const Device& device : devices) {
            repository.save(device);
        }
        db.commit();
        qint64 batchTime = batchTimer.elapsed();

        qDebug() << "Single inserts:" << singleTime << "ms";
        qDebug() << "Batch inserts:" << batchTime << "ms";
        qDebug() << "Speedup:" << (singleTime / batchTime) << "x";

        QVERIFY(batchTime < singleTime);
    }
};
```

### Large Network Handling (1000+ devices)

```cpp
// LargeNetworkTest.cpp
class LargeNetworkTest : public QObject {
    Q_OBJECT

private slots:
    void testScan1000Devices();
    void testUIResponsiveness();
    void testDatabasePerformance();
    void testMemoryUsage();

private:
    void testScan1000Devices() {
        // Create mock network with 1000 devices
        ScanController controller;

        QElapsedTimer timer;
        timer.start();

        // Scan multiple /24 subnets
        for (int i = 0; i < 4; i++) {
            QString subnet = QString("192.168.%1.0/24").arg(i);
            controller.executeQuickScan(subnet);

            QEventLoop loop;
            connect(&controller, &ScanController::scanCompleted,
                    &loop, &QEventLoop::quit);
            loop.exec();
        }

        qint64 elapsed = timer.elapsed();
        int deviceCount = controller.getDiscoveredDevices().size();

        qDebug() << "Scanned" << deviceCount << "devices in" << elapsed << "ms";
        qDebug() << "Average:" << (elapsed / deviceCount) << "ms per device";

        // Should complete in reasonable time (< 2 minutes)
        QVERIFY(elapsed < 120000);
    }
};
```

---

## 10.5 Cross-Platform Testing

### Platform-Specific Tests

```cpp
// PlatformTest.cpp
class PlatformTest : public QObject {
    Q_OBJECT

private slots:
    void testWindowsSpecific();
    void testLinuxSpecific();
    void testMacOSSpecific();
    void testNetworkCommands();
    void testFileSystem();

private:
#ifdef Q_OS_WIN
    void testWindowsSpecific() {
        // Test Windows-specific functionality
        QVERIFY(testRawSocketPrivileges());
        QVERIFY(testWinPcapAvailability());
    }
#endif

#ifdef Q_OS_LINUX
    void testLinuxSpecific() {
        // Test Linux-specific functionality
        QVERIFY(testRootPrivileges());
        QVERIFY(testNetworkInterfaces());
    }
#endif

    void testNetworkCommands() {
#ifdef Q_OS_WIN
        QString pingCommand = "ping -n 1 8.8.8.8";
#else
        QString pingCommand = "ping -c 1 8.8.8.8";
#endif

        QProcess process;
        process.start(pingCommand);
        process.waitForFinished(5000);

        QCOMPARE(process.exitCode(), 0);
    }
};
```

### Privilege Handling Tests

```cpp
// PrivilegeTest.cpp
class PrivilegeTest : public QObject {
    Q_OBJECT

private slots:
    void testRawSocketAccess();
    void testElevatedPrivileges();
    void testPrivilegePrompt();

private:
    void testRawSocketAccess() {
#ifdef Q_OS_WIN
        // Windows: requires admin privileges
        bool isAdmin = IsUserAnAdmin();
        if (!isAdmin) {
            QSKIP("Test requires administrator privileges");
        }
#endif

#ifdef Q_OS_LINUX
        // Linux: requires root or CAP_NET_RAW capability
        if (geteuid() != 0) {
            QSKIP("Test requires root privileges");
        }
#endif

        // Test raw socket creation
        int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
        QVERIFY(sockfd >= 0);
        close(sockfd);
    }
};
```

---

## Deliverable

**Phase 10 Completion Criteria**:
- ✅ Unit test coverage >85%
- ✅ All critical bugs fixed
- ✅ Integration tests passing
- ✅ UI tests passing
- ✅ Performance benchmarks met:
  - Scan 254 IPs in < 10 seconds
  - Handle 1000+ devices without lag
  - Database operations optimized
- ✅ No memory leaks detected
- ✅ Cross-platform testing complete (Windows, Linux)
- ✅ Privilege handling working correctly
- ✅ Code quality metrics met:
  - No critical issues
  - No code smells
  - Cyclomatic complexity < 15
- ✅ All tests documented and reproducible

---

## Integration Points

### With All Previous Phases
- Tests cover all implemented functionality
- Performance optimizations applied throughout
- Bug fixes don't introduce regressions

### For Phase 11
- Test reports for documentation
- Known issues documented
- Performance benchmarks documented

---

## Testing Tools

### Required Tools
- Qt Test framework
- Google Test (optional)
- Valgrind (Linux, memory leaks)
- Qt Creator Profiler
- Code coverage tools (gcov, lcov)

### CI/CD Integration

```yaml
# .github/workflows/tests.yml
name: Tests

on: [push, pull_request]

jobs:
  test:
    runs-on: ${{ matrix.os }}
    strategy:
      matrix:
        os: [ubuntu-latest, windows-latest]
        qt-version: [6.5.0]

    steps:
    - uses: actions/checkout@v2

    - name: Install Qt
      uses: jurplel/install-qt-action@v2
      with:
        version: ${{ matrix.qt-version }}

    - name: Build
      run: |
        mkdir build
        cd build
        cmake ..
        cmake --build .

    - name: Run Tests
      run: |
        cd build
        ctest --output-on-failure

    - name: Generate Coverage Report
      if: matrix.os == 'ubuntu-latest'
      run: |
        lcov --capture --directory . --output-file coverage.info
        lcov --remove coverage.info '/usr/*' --output-file coverage.info
        lcov --list coverage.info
```

---

## Quality Metrics

### Code Quality Targets
- **Test Coverage**: >85%
- **Code Duplication**: <3%
- **Cyclomatic Complexity**: <15 per method
- **Documentation**: >70% of public APIs documented
- **Compiler Warnings**: 0 warnings
- **Static Analysis**: 0 critical issues

### Performance Targets
- **Scan Performance**: 254 IPs in <10 seconds
- **UI Responsiveness**: <100ms response time
- **Memory Usage**: <100 MB for 1000 devices
- **Database Queries**: <50ms for common queries
- **Startup Time**: <2 seconds

---

## Next Phase

After completing Phase 10, proceed to:
→ **Phase 11: Documentation & Release** (Week 20)
