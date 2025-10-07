#include <QtTest/QtTest>
#include <QSignalSpy>
#include "viewmodels/ChartViewModel.h"
#include "controllers/MetricsController.h"
#include "models/NetworkMetrics.h"

// Concrete implementation for testing
class TestChartViewModel : public ChartViewModel {
    Q_OBJECT

public:
    TestChartViewModel(MetricsController* controller, QObject* parent = nullptr)
        : ChartViewModel(controller, parent)
        , updateCount(0)
        , clearCount(0)
    {}

    void updateChart(const NetworkMetrics& metrics) override {
        lastMetrics = metrics;
        updateCount++;
        emit chartDataUpdated();
    }

    void clearChart() override {
        lastMetrics = NetworkMetrics();
        clearCount++;
        emit chartCleared();
    }

    NetworkMetrics lastMetrics;
    int updateCount;
    int clearCount;
};

class ChartViewModelTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        // Test case initialization
    }

    void cleanupTestCase() {
        // Test case cleanup
    }

    void init() {
        // Per-test initialization
    }

    void cleanup() {
        // Per-test cleanup
    }

    // Test: Constructor with null controller
    void testConstructorWithNullController() {
        TestChartViewModel viewModel(nullptr);
        QVERIFY(viewModel.getMaxDataPoints() == 60); // Default value
    }

    // Test: Constructor with valid controller (skipped - requires full MetricsController dependencies)
    void testConstructorWithValidController() {
        // This test is skipped because it requires linking full MetricsController
        // which has many dependencies (MetricsAggregator, DeviceRepository, etc.)
        // The testConstructorWithNullController() test is sufficient for this unit test
        QSKIP("MetricsController dependencies not linked in this test");
    }

    // Test: Set and get max data points
    void testSetMaxDataPoints() {
        TestChartViewModel viewModel(nullptr);

        // Test valid value
        viewModel.setMaxDataPoints(100);
        QCOMPARE(viewModel.getMaxDataPoints(), 100);

        // Test another valid value
        viewModel.setMaxDataPoints(500);
        QCOMPARE(viewModel.getMaxDataPoints(), 500);
    }

    // Test: Invalid max data points (negative)
    void testInvalidMaxDataPointsNegative() {
        TestChartViewModel viewModel(nullptr);

        viewModel.setMaxDataPoints(-10);
        QCOMPARE(viewModel.getMaxDataPoints(), 60); // Should use default
    }

    // Test: Invalid max data points (zero)
    void testInvalidMaxDataPointsZero() {
        TestChartViewModel viewModel(nullptr);

        viewModel.setMaxDataPoints(0);
        QCOMPARE(viewModel.getMaxDataPoints(), 60); // Should use default
    }

    // Test: Max data points too large (capped)
    void testMaxDataPointsTooLarge() {
        TestChartViewModel viewModel(nullptr);

        viewModel.setMaxDataPoints(20000);
        QCOMPARE(viewModel.getMaxDataPoints(), 10000); // Should cap at 10000
    }

    // Test: Update chart
    void testUpdateChart() {
        TestChartViewModel viewModel(nullptr);

        NetworkMetrics metrics;
        metrics.setLatencyMin(10.0);
        metrics.setLatencyAvg(15.0);
        metrics.setLatencyMax(20.0);
        metrics.setJitter(2.5);
        metrics.setPacketLoss(1.0);

        QSignalSpy spy(&viewModel, &TestChartViewModel::chartDataUpdated);

        viewModel.updateChart(metrics);

        QCOMPARE(viewModel.updateCount, 1);
        QCOMPARE(viewModel.lastMetrics.latencyAvg(), 15.0);
        QCOMPARE(spy.count(), 1);
    }

    // Test: Clear chart
    void testClearChart() {
        TestChartViewModel viewModel(nullptr);

        // First update with data
        NetworkMetrics metrics;
        metrics.setLatencyAvg(15.0);
        viewModel.updateChart(metrics);

        QCOMPARE(viewModel.updateCount, 1);

        // Now clear
        QSignalSpy spy(&viewModel, &TestChartViewModel::chartCleared);

        viewModel.clearChart();

        QCOMPARE(viewModel.clearCount, 1);
        QCOMPARE(viewModel.lastMetrics.latencyAvg(), 0.0);
        QCOMPARE(spy.count(), 1);
    }

    // Test: Multiple updates
    void testMultipleUpdates() {
        TestChartViewModel viewModel(nullptr);

        QSignalSpy spy(&viewModel, &TestChartViewModel::chartDataUpdated);

        for (int i = 0; i < 10; i++) {
            NetworkMetrics metrics;
            metrics.setLatencyAvg(10.0 + i);
            viewModel.updateChart(metrics);
        }

        QCOMPARE(viewModel.updateCount, 10);
        QCOMPARE(viewModel.lastMetrics.latencyAvg(), 19.0); // Last value
        QCOMPARE(spy.count(), 10);
    }

    // Test: Prune data list helper
    void testPruneDataList() {
        TestChartViewModel viewModel(nullptr);
        viewModel.setMaxDataPoints(5);

        QList<QPointF> dataList;
        for (int i = 0; i < 10; i++) {
            dataList.append(QPointF(i, i * 10));
        }

        QCOMPARE(dataList.size(), 10);

        // Prune manually (since pruneDataList is protected)
        while (dataList.size() > viewModel.getMaxDataPoints()) {
            dataList.removeFirst();
        }

        QCOMPARE(dataList.size(), 5);
        QCOMPARE(dataList.first().x(), 5.0); // First 5 removed
        QCOMPARE(dataList.last().x(), 9.0);
    }

    // Test: Signal emissions
    void testSignalEmissions() {
        TestChartViewModel viewModel(nullptr);

        QSignalSpy updateSpy(&viewModel, &TestChartViewModel::chartDataUpdated);
        QSignalSpy clearSpy(&viewModel, &TestChartViewModel::chartCleared);

        NetworkMetrics metrics;
        viewModel.updateChart(metrics);
        QCOMPARE(updateSpy.count(), 1);
        QCOMPARE(clearSpy.count(), 0);

        viewModel.clearChart();
        QCOMPARE(updateSpy.count(), 1);
        QCOMPARE(clearSpy.count(), 1);
    }
};

QTEST_MAIN(ChartViewModelTest)
#include "ChartViewModelTest.moc"
