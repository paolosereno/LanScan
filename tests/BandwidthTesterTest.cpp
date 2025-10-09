#include <QtTest>
#include <QSignalSpy>
#include "diagnostics/BandwidthTester.h"
#include "utils/Logger.h"

class BandwidthTesterTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
        Logger::setLogLevel(Logger::DEBUG);
        Logger::enableConsoleOutput(true);
    }

    void cleanupTestCase()
    {
        // No cleanup needed
    }

    // Test 1: Basic construction
    void testConstruction()
    {
        BandwidthTester tester;
        QVERIFY(!tester.isRunning());
        QCOMPARE(tester.measuredBandwidth(), 0.0);
        QCOMPARE(tester.packetSize(), 65536);  // Default 64KB
    }

    // Test 2: Set packet size
    void testSetPacketSize()
    {
        BandwidthTester tester;

        // Valid packet sizes
        tester.setPacketSize(1024);
        QCOMPARE(tester.packetSize(), 1024);

        tester.setPacketSize(1048576);  // 1MB
        QCOMPARE(tester.packetSize(), 1048576);

        tester.setPacketSize(32768);
        QCOMPARE(tester.packetSize(), 32768);

        // Invalid packet sizes (too small)
        int before = tester.packetSize();
        tester.setPacketSize(512);
        QCOMPARE(tester.packetSize(), before);  // Should not change

        // Invalid packet sizes (too large)
        tester.setPacketSize(2000000);
        QCOMPARE(tester.packetSize(), before);  // Should not change
    }

    // Test 3: Invalid duration
    void testInvalidDuration()
    {
        BandwidthTester tester;

        // Duration too short
        QVERIFY(!tester.testDownloadSpeed("127.0.0.1", 8080, 0));

        // Duration too long
        QVERIFY(!tester.testUploadSpeed("127.0.0.1", 8080, 61));

        // Negative duration
        QVERIFY(!tester.testDownloadSpeed("127.0.0.1", 8080, -5));
    }

    // Test 4: Invalid target
    void testInvalidTarget()
    {
        BandwidthTester tester;

        QSignalSpy errorSpy(&tester, &BandwidthTester::testError);

        // Empty target
        QVERIFY(!tester.testDownloadSpeed("", 8080, 5));

        // Invalid port
        QVERIFY(!tester.testDownloadSpeed("127.0.0.1", 0, 5));
    }

    // Test 5: Connection failure
    void testConnectionFailure()
    {
        BandwidthTester tester;

        QSignalSpy errorSpy(&tester, &BandwidthTester::testError);
        QSignalSpy completedSpy(&tester, &BandwidthTester::testCompleted);

        // Try to connect to a port that's not listening
        QVERIFY(!tester.testDownloadSpeed("127.0.0.1", 9999, 5, BandwidthTester::TCP));

        // Should get error signal (connection failure)
        // Note: The test returns false immediately on connection failure during waitForConnected
    }

    // Test 6: Cancel test
    void testCancelTest()
    {
        BandwidthTester tester;

        QSignalSpy completedSpy(&tester, &BandwidthTester::testCompleted);

        // Start a test (will fail to connect, but we cancel immediately)
        // Use UDP to avoid connection failure
        QVERIFY(tester.testUploadSpeed("127.0.0.1", 8080, 10, BandwidthTester::UDP));
        QVERIFY(tester.isRunning());

        // Cancel immediately
        tester.cancel();

        QVERIFY(!tester.isRunning());

        // Wait to ensure no completion signal
        QTest::qWait(1000);
        QCOMPARE(completedSpy.count(), 0);
    }

    // Test 7: Cannot start multiple tests
    void testMultipleTests()
    {
        BandwidthTester tester;

        QVERIFY(tester.testUploadSpeed("127.0.0.1", 8080, 5, BandwidthTester::UDP));
        QVERIFY(tester.isRunning());

        // Try to start another test
        QVERIFY(!tester.testDownloadSpeed("127.0.0.1", 8080, 5, BandwidthTester::UDP));

        tester.cancel();
    }

    // Test 8: Progress updates (UDP upload test)
    void testProgressUpdates()
    {
        BandwidthTester tester;

        QSignalSpy progressSpy(&tester, &BandwidthTester::progressUpdated);
        QSignalSpy completedSpy(&tester, &BandwidthTester::testCompleted);

        // Short UDP upload test (no server needed)
        QVERIFY(tester.testUploadSpeed("127.0.0.1", 8080, 2, BandwidthTester::UDP));

        // Wait for completion
        QVERIFY(completedSpy.wait(5000));

        // Should have received progress updates
        QVERIFY(progressSpy.count() > 0);

        // Verify progress signal structure: (percent, currentBandwidth)
        for (const auto& arguments : progressSpy) {
            QCOMPARE(arguments.count(), 2);
            int percent = arguments.at(0).toInt();
            double bandwidth = arguments.at(1).toDouble();

            QVERIFY(percent >= 0 && percent <= 100);
            QVERIFY(bandwidth >= 0.0);
        }

        // Should have completed
        QVERIFY(!tester.isRunning());
        QVERIFY(tester.measuredBandwidth() >= 0.0);

        qDebug() << "UDP upload test completed:" << tester.measuredBandwidth() << "Mbps";
    }

    // Test 9: Short duration test
    void testShortDuration()
    {
        BandwidthTester tester;

        QSignalSpy completedSpy(&tester, &BandwidthTester::testCompleted);

        // 1 second UDP upload test
        QVERIFY(tester.testUploadSpeed("127.0.0.1", 8080, 1, BandwidthTester::UDP));

        // Should complete in ~1 second (plus overhead)
        QVERIFY(completedSpy.wait(3000));

        QVERIFY(!tester.isRunning());
        QVERIFY(tester.measuredBandwidth() >= 0.0);

        qDebug() << "1-second test bandwidth:" << tester.measuredBandwidth() << "Mbps";
    }

    // Test 10: Destructor cancels running test
    void testDestructorCancels()
    {
        {
            BandwidthTester tester;
            QVERIFY(tester.testUploadSpeed("127.0.0.1", 8080, 10, BandwidthTester::UDP));
            QVERIFY(tester.isRunning());
            // tester goes out of scope and destructor is called
        }
        // If we reach here without crash, destructor properly cleaned up
        QVERIFY(true);
    }

    // Test 11: Different packet sizes
    void testDifferentPacketSizes()
    {
        BandwidthTester tester;

        QSignalSpy completedSpy(&tester, &BandwidthTester::testCompleted);

        // Test with small packets (1KB)
        tester.setPacketSize(1024);
        QVERIFY(tester.testUploadSpeed("127.0.0.1", 8080, 1, BandwidthTester::UDP));
        QVERIFY(completedSpy.wait(3000));

        double smallPacketBandwidth = tester.measuredBandwidth();
        QVERIFY(smallPacketBandwidth >= 0.0);

        completedSpy.clear();

        // Test with large packets (256KB)
        tester.setPacketSize(262144);
        QVERIFY(tester.testUploadSpeed("127.0.0.1", 8080, 1, BandwidthTester::UDP));
        QVERIFY(completedSpy.wait(3000));

        double largePacketBandwidth = tester.measuredBandwidth();
        QVERIFY(largePacketBandwidth >= 0.0);

        qDebug() << "Small packets (1KB):" << smallPacketBandwidth << "Mbps";
        qDebug() << "Large packets (256KB):" << largePacketBandwidth << "Mbps";

        // Both should produce some bandwidth
        QVERIFY(smallPacketBandwidth > 0.0);
        QVERIFY(largePacketBandwidth > 0.0);
    }
};

QTEST_MAIN(BandwidthTesterTest)
#include "BandwidthTesterTest.moc"
