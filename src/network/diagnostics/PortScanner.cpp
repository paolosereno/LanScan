#include "PortScanner.h"
#include "../sockets/TcpSocketManager.h"
#include "../services/PortServiceMapper.h"
#include "../../utils/Logger.h"
#include <QElapsedTimer>
#include <QtConcurrent>

PortScanner::PortScanner(QObject* parent)
    : QObject(parent)
    , socketManager(new TcpSocketManager(this))
    , serviceMapper(new PortServiceMapper())
    , totalPorts(0)
    , scannedPorts(0)
    , scanning(false)
    , scanWatcher(new QFutureWatcher<void>(this))
{
    // Connect watcher to slot
    connect(scanWatcher, &QFutureWatcher<void>::finished,
            this, &PortScanner::onScanFinished);
}

PortScanner::~PortScanner() {
    cancelScan();
    delete serviceMapper;
}

void PortScanner::scanPorts(const QString& host, ScanType type) {
    QList<int> ports;

    switch (type) {
        case QUICK_SCAN:
            ports = getCommonPorts();
            break;
        case FULL_SCAN:
            for (int i = 1; i <= 65535; ++i) {
                ports.append(i);
            }
            break;
        case CUSTOM_SCAN:
            Logger::warn("PortScanner: CUSTOM_SCAN requires explicit port list");
            return;
    }

    scanPorts(host, ports);
}

void PortScanner::scanPorts(const QString& host, const QList<int>& ports) {
    if (scanning) {
        Logger::warn("PortScanner: Scan already in progress");
        return;
    }

    if (ports.isEmpty()) {
        Logger::warn("PortScanner: No ports to scan");
        return;
    }

    executeScan(host, ports);
}

void PortScanner::scanPortRange(const QString& host, int startPort, int endPort) {
    if (startPort < 1 || startPort > 65535 || endPort < 1 || endPort > 65535) {
        QString error = QString("PortScanner: Invalid port range: %1-%2").arg(startPort).arg(endPort);
        Logger::error(error);
        emit errorOccurred(error);
        return;
    }

    if (startPort > endPort) {
        QString error = QString("PortScanner: Start port (%1) greater than end port (%2)").arg(startPort).arg(endPort);
        Logger::error(error);
        emit errorOccurred(error);
        return;
    }

    QList<int> ports;
    for (int port = startPort; port <= endPort; ++port) {
        ports.append(port);
    }

    scanPorts(host, ports);
}

void PortScanner::cancelScan() {
    if (scanning) {
        scanning = false;
        Logger::info("PortScanner: Cancelling scan...");

        // Wait for the scan to finish (it will stop at the next port check)
        if (scanWatcher && scanWatcher->isRunning()) {
            scanWatcher->waitForFinished();
        }

        Logger::info("PortScanner: Scan cancelled");
    }
}

bool PortScanner::isScanning() const {
    return scanning;
}

QList<int> PortScanner::getCommonPorts() {
    return {
        21,    // FTP
        22,    // SSH
        23,    // Telnet
        25,    // SMTP
        53,    // DNS
        80,    // HTTP
        110,   // POP3
        143,   // IMAP
        443,   // HTTPS
        445,   // SMB
        3306,  // MySQL
        3389,  // RDP
        5432,  // PostgreSQL
        5900,  // VNC
        8080,  // HTTP-Alt
        8443   // HTTPS-Alt
    };
}

PortScanner::PortScanResult PortScanner::scanSinglePort(const QString& host, int port, int timeout) {
    PortScanResult result;
    result.host = host;
    result.port = port;

    QElapsedTimer timer;
    timer.start();

    bool connected = socketManager->connectToHost(host, port, timeout);
    result.responseTime = timer.elapsed();

    if (connected) {
        result.state = "open";
        result.service = serviceMapper->getServiceName(port);
        socketManager->disconnect();
    } else {
        result.state = "closed";
        result.service = "";
    }

    return result;
}

void PortScanner::executeScan(const QString& host, const QList<int>& ports) {
    currentHost = host;
    scanResults.clear();
    totalPorts = ports.size();
    scannedPorts = 0;
    scanning = true;

    Logger::info(QString("PortScanner: Starting async scan of %1 ports on %2")
                 .arg(totalPorts).arg(host));

    // Run scan asynchronously using QtConcurrent
    QFuture<void> future = QtConcurrent::run([this, host, ports]() {
        // Scan ports sequentially in background thread
        for (int port : ports) {
            if (!scanning) {
                Logger::info("PortScanner: Scan cancelled by user");
                break;
            }

            PortScanResult result = scanSinglePort(host, port, 1000);

            if (result.state == "open") {
                scanResults.append(result);
                emit portFound(result);
                Logger::debug(QString("PortScanner: Port %1 is open (%2)")
                             .arg(port).arg(result.service));
            }

            scannedPorts++;
            updateProgress();
        }
    });

    scanWatcher->setFuture(future);
}

void PortScanner::updateProgress() {
    if (totalPorts > 0) {
        emit scanProgress(scannedPorts, totalPorts);

        // Log progress at 25%, 50%, 75%, 100%
        double percentage = (scannedPorts * 100.0) / totalPorts;
        if (scannedPorts == totalPorts ||
            (static_cast<int>(percentage) % 25 == 0 && scannedPorts % (totalPorts / 4) == 0)) {
            Logger::debug(QString("PortScanner: Scan progress: %1%").arg(percentage, 0, 'f', 1));
        }
    }
}

void PortScanner::onScanFinished() {
    scanning = false;

    // If no ports found, create a dummy result with the host so the coordinator knows which host finished
    QList<PortScanResult> resultsToEmit = scanResults;
    if (resultsToEmit.isEmpty() && !currentHost.isEmpty()) {
        PortScanResult dummyResult;
        dummyResult.host = currentHost;
        dummyResult.port = 0;
        dummyResult.state = "none";
        resultsToEmit.append(dummyResult);
    }

    emit scanCompleted(resultsToEmit);

    Logger::info(QString("PortScanner: Async scan completed: %1 open ports found on %2")
                 .arg(scanResults.size())
                 .arg(currentHost));
}
