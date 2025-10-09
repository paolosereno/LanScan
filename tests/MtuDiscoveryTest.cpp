#include <QtTest>
#include <QSignalSpy>
#include "diagnostics/MtuDiscovery.h"
#include "utils/Logger.h"

class MtuDiscoveryTest : public QObject
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
        MtuDiscovery discovery;
        QVERIFY(!discovery.isRunning());
        QCOMPARE(discovery.discoveredMtu(), 0);
        QVERIFY(discovery.target().isEmpty());
    }

    // Test 2: Invalid inputs
    void testInvalidInputs()
    {
        MtuDiscovery discovery;

        // Empty target
        QVERIFY(!discovery.discoverMtu("", 576, 1500));

        // Invalid MTU range (min too small)
        QVERIFY(!discovery.discoverMtu("8.8.8.8", 67, 1500));

        // Invalid MTU range (min > max)
        QVERIFY(!discovery.discoverMtu("8.8.8.8", 1500, 576));
    }

    // Test 3: Start discovery with valid inputs
    void testStartDiscovery()
    {
        MtuDiscovery discovery;

        QSignalSpy progressSpy(&discovery, &MtuDiscovery::progressUpdated);
        QSignalSpy discoveredSpy(&discovery, &MtuDiscovery::mtuDiscovered);

        QVERIFY(discovery.discoverMtu("127.0.0.1", 576, 1500));
        QVERIFY(discovery.isRunning());
        QCOMPARE(discovery.target(), QString("127.0.0.1"));

        // Wait for completion (timeout 30 seconds for binary search)
        QVERIFY(discoveredSpy.wait(30000));

        QVERIFY(!discovery.isRunning());
        QVERIFY(discovery.discoveredMtu() > 0);
        QVERIFY(progressSpy.count() > 0);

        qDebug() << "Discovered MTU for localhost:" << discovery.discoveredMtu() << "bytes";
    }

    // Test 4: Cancel discovery
    void testCancelDiscovery()
    {
        MtuDiscovery discovery;

        QSignalSpy discoveredSpy(&discovery, &MtuDiscovery::mtuDiscovered);

        QVERIFY(discovery.discoverMtu("8.8.8.8", 576, 9000));
        QVERIFY(discovery.isRunning());

        // Cancel immediately
        discovery.cancel();

        QVERIFY(!discovery.isRunning());

        // Wait a bit to ensure no signal is emitted
        QTest::qWait(1000);
        QCOMPARE(discoveredSpy.count(), 0);
    }

    // Test 5: Cannot start multiple discoveries simultaneously
    void testMultipleDiscoveries()
    {
        MtuDiscovery discovery;

        QVERIFY(discovery.discoverMtu("127.0.0.1", 576, 1500));
        QVERIFY(discovery.isRunning());

        // Try to start another discovery
        QVERIFY(!discovery.discoverMtu("8.8.8.8", 576, 1500));

        discovery.cancel();
    }

    // Test 6: Progress updates
    void testProgressUpdates()
    {
        MtuDiscovery discovery;

        QSignalSpy progressSpy(&discovery, &MtuDiscovery::progressUpdated);
        QSignalSpy discoveredSpy(&discovery, &MtuDiscovery::mtuDiscovered);

        QVERIFY(discovery.discoverMtu("127.0.0.1", 1000, 1500));

        // Wait for completion
        QVERIFY(discoveredSpy.wait(30000));

        // Should have received multiple progress updates (binary search steps)
        QVERIFY(progressSpy.count() > 0);

        // Verify progress signal structure: (currentMtu, minMtu, maxMtu)
        for (const auto& arguments : progressSpy) {
            QCOMPARE(arguments.count(), 3);
            int currentMtu = arguments.at(0).toInt();
            int minMtu = arguments.at(1).toInt();
            int maxMtu = arguments.at(2).toInt();

            QVERIFY(currentMtu >= minMtu);
            QVERIFY(currentMtu <= maxMtu);
        }
    }

    // Test 7: Small MTU range
    void testSmallMtuRange()
    {
        MtuDiscovery discovery;

        QSignalSpy discoveredSpy(&discovery, &MtuDiscovery::mtuDiscovered);

        // Very small range should converge quickly
        QVERIFY(discovery.discoverMtu("127.0.0.1", 1400, 1500));

        QVERIFY(discoveredSpy.wait(10000));

        int mtu = discovery.discoveredMtu();
        QVERIFY(mtu >= 1400 && mtu <= 1500);

        qDebug() << "Discovered MTU in range 1400-1500:" << mtu << "bytes";
    }

    // Test 8: Standard Ethernet MTU range
    void testStandardEthernetMtu()
    {
        MtuDiscovery discovery;

        QSignalSpy discoveredSpy(&discovery, &MtuDiscovery::mtuDiscovered);

        // Test standard Ethernet MTU (1500 bytes)
        QVERIFY(discovery.discoverMtu("127.0.0.1", 576, 1500));

        QVERIFY(discoveredSpy.wait(30000));

        int mtu = discovery.discoveredMtu();
        QVERIFY(mtu > 0);
        QVERIFY(mtu >= 576 && mtu <= 1500);

        qDebug() << "Discovered MTU for standard Ethernet:" << mtu << "bytes";
    }

    // Test 9: Error handling for unreachable host
    void testUnreachableHost()
    {
        MtuDiscovery discovery;

        QSignalSpy errorSpy(&discovery, &MtuDiscovery::discoveryError);
        QSignalSpy discoveredSpy(&discovery, &MtuDiscovery::mtuDiscovered);

        // Use an unreachable private IP
        QVERIFY(discovery.discoverMtu("192.168.255.254", 576, 1500));

        // Wait for either completion or error (timeout 30 seconds)
        // For unreachable hosts, the discovery may still complete but with incorrect results
        // or timeout on each ping
        bool finished = discoveredSpy.wait(30000) || errorSpy.wait(30000);

        if (errorSpy.count() > 0) {
            qDebug() << "Error detected (expected for unreachable host):" << errorSpy.first().first().toString();
        } else if (discoveredSpy.count() > 0) {
            qDebug() << "Discovery completed (may have incorrect MTU for unreachable host):" << discovery.discoveredMtu();
        }

        // Either error or completion is acceptable
        QVERIFY(finished || !discovery.isRunning());
    }

    // Test 10: Destructor cancels running discovery
    void testDestructorCancels()
    {
        {
            MtuDiscovery discovery;
            QVERIFY(discovery.discoverMtu("8.8.8.8", 576, 9000));
            QVERIFY(discovery.isRunning());
            // discovery goes out of scope and destructor is called
        }
        // If we reach here without crash, destructor properly cleaned up
        QVERIFY(true);
    }
};

QTEST_MAIN(MtuDiscoveryTest)
#include "MtuDiscoveryTest.moc"
