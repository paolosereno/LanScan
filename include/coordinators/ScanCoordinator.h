#ifndef SCANCOORDINATOR_H
#define SCANCOORDINATOR_H

#include <QObject>
#include <QString>
#include <QList>
#include <QThreadPool>
#include <QFuture>
#include <QMap>
#include <QMutex>
#include <atomic>

class Device;
class IpScanner;
class PortScanner;
class MetricsAggregator;
class IScanStrategy;

/**
 * @brief Coordinates multiple scan operations with multi-threading support
 */
class ScanCoordinator : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Configuration for scan operations
     */
    struct ScanConfig {
        QString subnet;              ///< Target subnet (e.g., "192.168.1.0/24")
        bool resolveDns;             ///< Enable DNS resolution
        bool resolveArp;             ///< Enable ARP resolution
        bool scanPorts;              ///< Enable port scanning
        QList<int> portsToScan;      ///< List of ports to scan (empty for default)
        int timeout;                 ///< Timeout in milliseconds
        int maxThreads;              ///< Maximum concurrent threads

        ScanConfig()
            : resolveDns(true)
            , resolveArp(true)
            , scanPorts(false)
            , timeout(3000)
            , maxThreads(0)  // 0 means auto-detect
        {}
    };

    /**
     * @brief Constructor
     * @param ipScanner IP scanner instance
     * @param portScanner Port scanner instance
     * @param metricsAggregator Metrics aggregator instance
     * @param parent Parent QObject
     */
    explicit ScanCoordinator(
        IpScanner* ipScanner,
        PortScanner* portScanner,
        MetricsAggregator* metricsAggregator,
        QObject* parent = nullptr
    );

    /**
     * @brief Destructor
     */
    ~ScanCoordinator();

    /**
     * @brief Start a new scan operation
     * @param config Scan configuration
     */
    void startScan(const ScanConfig& config);

    /**
     * @brief Stop the current scan operation
     */
    void stopScan();

    /**
     * @brief Pause the current scan operation
     */
    void pauseScan();

    /**
     * @brief Resume a paused scan operation
     */
    void resumeScan();

    /**
     * @brief Check if a scan is currently running
     * @return True if scan is running
     */
    bool isScanning() const { return scanning; }

    /**
     * @brief Check if scan is paused
     * @return True if scan is paused
     */
    bool isPaused() const { return paused; }

signals:
    /**
     * @brief Emitted when scan starts
     * @param totalHosts Total number of hosts to scan
     */
    void scanStarted(int totalHosts);

    /**
     * @brief Emitted when a device is discovered
     * @param device Discovered device
     */
    void deviceDiscovered(const Device& device);

    /**
     * @brief Emitted to report scan progress
     * @param current Current progress count
     * @param total Total items to scan
     * @param currentIp Currently scanning IP address
     */
    void scanProgress(int current, int total, const QString& currentIp);

    /**
     * @brief Emitted when scan completes
     * @param devicesFound Number of devices found
     * @param durationMs Scan duration in milliseconds
     */
    void scanCompleted(int devicesFound, qint64 durationMs);

    /**
     * @brief Emitted when scan encounters an error
     * @param error Error message
     */
    void scanError(const QString& error);

    /**
     * @brief Emitted when scan is paused
     */
    void scanPaused();

    /**
     * @brief Emitted when scan is resumed
     */
    void scanResumed();

private slots:
    void onDeviceFound(const Device& device);
    void onScanFinished();

private:
    IpScanner* ipScanner;
    PortScanner* portScanner;
    MetricsAggregator* metricsAggregator;

    QThreadPool* threadPool;
    QFuture<void> scanFuture;

    std::atomic<bool> scanning;
    std::atomic<bool> paused;
    std::atomic<bool> stopRequested;

    std::atomic<int> currentProgress;
    std::atomic<int> totalProgress;
    std::atomic<int> devicesFoundCount;

    qint64 scanStartTime;
    ScanConfig currentConfig;

    mutable QMutex mutex;

    void coordinateScan(const ScanConfig& config);
    void processDiscoveredDevice(Device& device);
    void updateProgress(const QString& currentIp);
    void cleanup();
    IScanStrategy* createScanStrategy(const ScanConfig& config);
};

#endif // SCANCOORDINATOR_H
