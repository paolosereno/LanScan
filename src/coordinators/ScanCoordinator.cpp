#include "coordinators/ScanCoordinator.h"
#include "../models/Device.h"
#include "../network/scanner/IpScanner.h"
#include "../network/scanner/QuickScanStrategy.h"
#include "../network/scanner/DeepScanStrategy.h"
#include "../network/diagnostics/PortScanner.h"
#include "../network/diagnostics/MetricsAggregator.h"
#include "../network/services/SubnetCalculator.h"
#include "../utils/Logger.h"

#include <QtConcurrent>
#include <QThread>
#include <QElapsedTimer>

ScanCoordinator::ScanCoordinator(
    IpScanner* ipScanner,
    PortScanner* portScanner,
    MetricsAggregator* metricsAggregator,
    QObject* parent
)
    : QObject(parent)
    , ipScanner(ipScanner)
    , portScanner(portScanner)
    , metricsAggregator(metricsAggregator)
    , threadPool(new QThreadPool(this))
    , scanning(false)
    , paused(false)
    , stopRequested(false)
    , currentProgress(0)
    , totalProgress(0)
    , devicesFoundCount(0)
    , scanStartTime(0)
{
    // Auto-detect optimal thread count if not specified
    int optimalThreads = QThread::idealThreadCount();
    if (optimalThreads > 0) {
        threadPool->setMaxThreadCount(optimalThreads * 2);
    } else {
        threadPool->setMaxThreadCount(8); // Fallback
    }

    // Connect IpScanner signals
    if (ipScanner) {
        connect(ipScanner, &IpScanner::deviceDiscovered,
                this, &ScanCoordinator::onDeviceFound);
        connect(ipScanner, &IpScanner::scanFinished,
                this, &ScanCoordinator::onScanFinished);
        connect(ipScanner, &IpScanner::scanProgress,
                this, [this](int current, int total) {
                    currentProgress = current;
                    totalProgress = total;
                    emit scanProgress(current, total, QString());
                });
    }

    Logger::info("ScanCoordinator initialized with " +
                 QString::number(threadPool->maxThreadCount()) + " threads");
}

ScanCoordinator::~ScanCoordinator() {
    stopScan();
    threadPool->waitForDone();
}

void ScanCoordinator::startScan(const ScanConfig& config) {
    if (scanning) {
        Logger::warn("Scan already in progress");
        emit scanError("Scan already in progress");
        return;
    }

    // Validate subnet (basic check - CIDR format should have a slash)
    if (!config.subnet.contains('/')) {
        Logger::error("Invalid subnet: " + config.subnet);
        emit scanError("Invalid subnet: " + config.subnet);
        return;
    }

    // Reset state
    scanning = true;
    paused = false;
    stopRequested = false;
    currentProgress = 0;
    devicesFoundCount = 0;
    currentConfig = config;

    // Calculate total IPs to scan
    QStringList ipList = SubnetCalculator::getIpRange(config.subnet);
    totalProgress = ipList.size();

    // Configure thread pool
    if (config.maxThreads > 0) {
        threadPool->setMaxThreadCount(config.maxThreads);
    }

    scanStartTime = QDateTime::currentMSecsSinceEpoch();

    Logger::info("Starting scan of " + config.subnet +
                 " (" + QString::number(totalProgress) + " hosts)");

    emit scanStarted(totalProgress);

    // Create and set the appropriate scan strategy based on config
    IScanStrategy* strategy = createScanStrategy(config);
    if (ipScanner && strategy) {
        ipScanner->setScanStrategy(strategy);
        ipScanner->startScan(config.subnet);
    } else {
        Logger::error("Failed to create scan strategy");
        emit scanError("Failed to create scan strategy");
        cleanup();
    }
}

void ScanCoordinator::stopScan() {
    if (!scanning) {
        return;
    }

    Logger::info("Stopping scan...");
    stopRequested = true;

    // Stop IpScanner
    if (ipScanner) {
        ipScanner->stopScan();
    }

    // Stop PortScanner if running
    if (portScanner && portScanner->isScanning()) {
        portScanner->cancelScan();
    }

    cleanup();
}

void ScanCoordinator::pauseScan() {
    if (!scanning || paused) {
        return;
    }

    Logger::info("Pausing scan...");
    paused = true;
    emit scanPaused();
}

void ScanCoordinator::resumeScan() {
    if (!scanning || !paused) {
        return;
    }

    Logger::info("Resuming scan...");
    paused = false;
    emit scanResumed();
}

void ScanCoordinator::coordinateScan(const ScanConfig& config) {
    // This method is now handled by IpScanner callbacks
    // Kept for potential future use
}

void ScanCoordinator::processDiscoveredDevice(Device& device) {
    // Additional processing can be done here
    // For example, collect metrics if aggregator is available
    if (metricsAggregator && currentConfig.timeout > 0) {
        // Start collecting metrics for the device
        // This is optional and can be done asynchronously
        metricsAggregator->startContinuousCollection(device.getIp(), currentConfig.timeout);
    }

    // Scan ports if requested
    if (currentConfig.scanPorts && portScanner && !device.getIp().isEmpty()) {
        if (currentConfig.portsToScan.isEmpty()) {
            // Use quick scan with common ports
            portScanner->scanPorts(device.getIp(), PortScanner::QUICK_SCAN);
        } else {
            // Use custom port list
            portScanner->scanPorts(device.getIp(), currentConfig.portsToScan);
        }
    }
}

void ScanCoordinator::updateProgress(const QString& currentIp) {
    emit scanProgress(currentProgress, totalProgress, currentIp);
}

void ScanCoordinator::cleanup() {
    scanning = false;
    paused = false;
    stopRequested = false;

    // Stop metrics collection
    if (metricsAggregator && metricsAggregator->isCollecting()) {
        metricsAggregator->stopContinuousCollection();
    }
}

void ScanCoordinator::onDeviceFound(const Device& device) {
    if (stopRequested || paused) {
        return;
    }

    Device deviceCopy = device;
    processDiscoveredDevice(deviceCopy);

    emit deviceDiscovered(deviceCopy);
    devicesFoundCount++;
}

void ScanCoordinator::onScanFinished() {
    qint64 duration = QDateTime::currentMSecsSinceEpoch() - scanStartTime;

    if (!stopRequested) {
        Logger::info("Scan completed: " + QString::number(devicesFoundCount) +
                     " devices found in " + QString::number(duration) + " ms");
        emit scanCompleted(devicesFoundCount, duration);
    }

    cleanup();
}

IScanStrategy* ScanCoordinator::createScanStrategy(const ScanConfig& config) {
    // Determine scan strategy based on configuration
    // Use DeepScanStrategy if ANY advanced feature is requested (DNS, ARP, or Ports)
    if (config.resolveDns || config.resolveArp || config.scanPorts) {
        Logger::debug("Creating DeepScanStrategy (DNS/ARP/Ports requested)");
        return new DeepScanStrategy();
    } else {
        // QuickScanStrategy: basic ping-only scan
        Logger::debug("Creating QuickScanStrategy (ping-only)");
        return new QuickScanStrategy();
    }
}
