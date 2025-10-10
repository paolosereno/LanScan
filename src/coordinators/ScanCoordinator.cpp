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

    // Connect PortScanner signals
    if (portScanner) {
        connect(portScanner, &PortScanner::portFound,
                this, [this](const PortScanner::PortScanResult& result) {
                    onPortFound(result.host, result.port, result.service);
                });
        connect(portScanner, &PortScanner::scanCompleted,
                this, [this](const QList<PortScanner::PortScanResult>& results) {
                    // Get host from results OR from currentPortScanHost as fallback
                    QString host;

                    if (!results.isEmpty()) {
                        // Use host from first result (all results are for same host)
                        host = results.first().host;
                    } else {
                        // Fallback: use currentPortScanHost (but this might have race condition)
                        QMutexLocker locker(&mutex);
                        host = currentPortScanHost;
                    }

                    if (!host.isEmpty()) {
                        Logger::debug(QString("scanCompleted signal received for host: %1").arg(host));
                        onPortScanCompleted(host);
                    } else {
                        Logger::warn("scanCompleted signal received but host is empty!");
                    }
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
    Logger::info(QString("processDiscoveredDevice called for %1 - scanPorts=%2")
                .arg(device.getIp())
                .arg(currentConfig.scanPorts ? "true" : "false"));

    // Additional processing can be done here
    // For example, collect metrics if aggregator is available
    if (metricsAggregator && currentConfig.timeout > 0) {
        // Start collecting metrics for the device
        // This is optional and can be done asynchronously
        metricsAggregator->startContinuousCollection(device.getIp(), currentConfig.timeout);
    }

    // Scan ports if requested AND device doesn't already have ports from DeepScanStrategy
    if (currentConfig.scanPorts && portScanner && !device.getIp().isEmpty()) {
        // Check if device already has ports (from DeepScanStrategy)
        int existingPorts = device.getOpenPorts().size();

        if (existingPorts > 0) {
            Logger::info(QString("Device %1 already has %2 ports from deep scan - skipping PortScanner")
                        .arg(device.getIp())
                        .arg(existingPorts));
            return;  // Skip PortScanner if ports already found
        }

        Logger::info(QString("Port scanning enabled for %1").arg(device.getIp()));
        QMutexLocker locker(&mutex);

        // Add device to pending map and queue
        pendingDevices[device.getIp()] = device;
        portScanResults[device.getIp()].clear();  // Initialize empty port list

        // Add to queue if not already there
        if (!portScanQueue.contains(device.getIp())) {
            portScanQueue.append(device.getIp());
            Logger::debug(QString("Added %1 to port scan queue (position %2)")
                         .arg(device.getIp())
                         .arg(portScanQueue.size()));
        }

        locker.unlock();

        // Try to process next in queue (will only start if scanner is free)
        processNextPortScan();
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

    // Clear port scanning data
    QMutexLocker locker(&mutex);
    pendingDevices.clear();
    portScanResults.clear();
    portScanQueue.clear();
    currentPortScanHost.clear();
}

void ScanCoordinator::onDeviceFound(const Device& device) {
    if (stopRequested || paused) {
        return;
    }

    Device deviceCopy = device;

    // Always emit device immediately (even if we'll scan ports later)
    // This ensures users see discovered devices right away
    emit deviceDiscovered(deviceCopy);
    devicesFoundCount++;

    // Process device (including port scanning if enabled)
    // If port scanning is enabled, we'll emit another update when ports are found
    processDiscoveredDevice(deviceCopy);
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
        Logger::debug(QString("Creating DeepScanStrategy (DNS=%1, ARP=%2, Ports=%3)")
                     .arg(config.resolveDns ? "true" : "false")
                     .arg(config.resolveArp ? "true" : "false")
                     .arg(config.scanPorts ? "true" : "false"));

        DeepScanStrategy* strategy = new DeepScanStrategy();
        // Only enable port scanning if explicitly requested
        strategy->setPortScanningEnabled(config.scanPorts);
        return strategy;
    } else {
        // QuickScanStrategy: basic ping-only scan
        Logger::debug("Creating QuickScanStrategy (ping-only)");
        return new QuickScanStrategy();
    }
}

void ScanCoordinator::onPortFound(const QString& host, int port, const QString& service) {
    QMutexLocker locker(&mutex);

    // Store port result for this host
    if (portScanResults.contains(host)) {
        portScanResults[host].append(qMakePair(port, service));
        Logger::debug(QString("Port found on %1: %2 (%3)").arg(host).arg(port).arg(service));
    }
}

void ScanCoordinator::onPortScanCompleted(const QString& host) {
    Logger::info(QString("onPortScanCompleted called for host: %1").arg(host));

    QMutexLocker locker(&mutex);
    int portsFound = portScanResults.value(host).size();
    locker.unlock();

    Logger::info(QString("Port scan completed for %1 - found %2 ports").arg(host).arg(portsFound));

    emitDeviceWithPorts(host);

    // Process next device in queue
    Logger::debug("Calling processNextPortScan from onPortScanCompleted");
    processNextPortScan();
}

void ScanCoordinator::emitDeviceWithPorts(const QString& ip) {
    QMutexLocker locker(&mutex);

    // Check if we have this device in pending list
    if (!pendingDevices.contains(ip)) {
        Logger::warn(QString("Device %1 not found in pending devices").arg(ip));
        return;
    }

    // Get the device and port results
    Device device = pendingDevices[ip];
    QList<QPair<int, QString>> ports = portScanResults.value(ip);

    // Add all found ports to the device
    for (const auto& portPair : ports) {
        PortInfo portInfo(portPair.first, PortInfo::TCP);
        portInfo.setService(portPair.second);
        portInfo.setState(PortInfo::Open);
        device.addPort(portInfo);
    }

    // Clean up
    pendingDevices.remove(ip);
    portScanResults.remove(ip);
    if (currentPortScanHost == ip) {
        currentPortScanHost.clear();
    }

    locker.unlock();

    // Emit the device with ports
    emit deviceDiscovered(device);
    devicesFoundCount++;

    Logger::info(QString("Device %1 discovered with %2 open ports").arg(ip).arg(ports.size()));

    // Debug: Log each port
    for (const PortInfo& port : device.getOpenPorts()) {
        Logger::debug(QString("  - Port %1/%2 (%3) - %4")
                     .arg(port.getPort())
                     .arg(port.protocolString())
                     .arg(port.getService())
                     .arg(port.stateString()));
    }
}

void ScanCoordinator::processNextPortScan() {
    QMutexLocker locker(&mutex);

    Logger::debug(QString("processNextPortScan called - queue size: %1").arg(portScanQueue.size()));

    // Check if there's anything in the queue
    if (portScanQueue.isEmpty()) {
        Logger::debug("Port scan queue is empty, nothing to process");
        return;
    }

    // Check if scanner is already busy
    if (portScanner && portScanner->isScanning()) {
        Logger::debug("PortScanner is busy, waiting...");
        return;
    }

    if (!portScanner) {
        Logger::error("PortScanner is null!");
        return;
    }

    // Get next IP from queue
    QString nextIp = portScanQueue.takeFirst();
    currentPortScanHost = nextIp;

    locker.unlock();

    Logger::info(QString("Processing port scan for %1 (%2 remaining in queue)")
                .arg(nextIp)
                .arg(portScanQueue.size()));

    // Log the ports we're going to scan
    if (currentConfig.portsToScan.isEmpty()) {
        Logger::debug("Using QUICK_SCAN (common ports)");
        portScanner->scanPorts(nextIp, PortScanner::QUICK_SCAN);
    } else {
        Logger::debug(QString("Scanning custom ports: %1").arg(currentConfig.portsToScan.size()));
        portScanner->scanPorts(nextIp, currentConfig.portsToScan);
    }
}
