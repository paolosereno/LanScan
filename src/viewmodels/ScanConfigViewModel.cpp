#include "viewmodels/ScanConfigViewModel.h"
#include "../network/services/NetworkInterfaceDetector.h"
#include "../utils/IpAddressValidator.h"
#include "../utils/Logger.h"
#include <QThread>

ScanConfigViewModel::ScanConfigViewModel(QObject* parent)
    : QObject(parent)
    , scanType(Quick)
    , threadCount(QThread::idealThreadCount())
    , timeout(1000)
    , resolveDns(true)
    , resolveArp(false)
    , scanPorts(false)
    , interfaceDetector(nullptr)
{
    // Initialize with Quick scan preset
    loadQuickScanPreset();
    Logger::info("ScanConfigViewModel initialized");
}

QString ScanConfigViewModel::getSubnet() const {
    return subnet;
}

ScanConfigViewModel::ScanType ScanConfigViewModel::getScanType() const {
    return scanType;
}

int ScanConfigViewModel::getThreadCount() const {
    return threadCount;
}

int ScanConfigViewModel::getTimeout() const {
    return timeout;
}

bool ScanConfigViewModel::isResolveDns() const {
    return resolveDns;
}

bool ScanConfigViewModel::isResolveArp() const {
    return resolveArp;
}

bool ScanConfigViewModel::isScanPorts() const {
    return scanPorts;
}

QList<int> ScanConfigViewModel::getPortsToScan() const {
    return portsToScan;
}

void ScanConfigViewModel::setSubnet(const QString& subnet) {
    if (this->subnet != subnet) {
        this->subnet = subnet;
        emit subnetChanged(subnet);
        updateValidation();
    }
}

void ScanConfigViewModel::setScanType(ScanType type) {
    if (this->scanType != type) {
        this->scanType = type;
        emit scanTypeChanged(type);
        loadPreset(type);
    }
}

void ScanConfigViewModel::setThreadCount(int count) {
    this->threadCount = count;
}

void ScanConfigViewModel::setTimeout(int timeout) {
    this->timeout = timeout;
}

void ScanConfigViewModel::setResolveDns(bool resolve) {
    this->resolveDns = resolve;
}

void ScanConfigViewModel::setResolveArp(bool resolve) {
    this->resolveArp = resolve;
}

void ScanConfigViewModel::setScanPorts(bool scan) {
    this->scanPorts = scan;
}

void ScanConfigViewModel::setPortsToScan(const QList<int>& ports) {
    this->portsToScan = ports;
}

bool ScanConfigViewModel::isSubnetValid() const {
    return IpAddressValidator::isValidCidr(subnet);
}

QString ScanConfigViewModel::getSubnetError() const {
    if (subnet.isEmpty()) {
        return tr("Subnet is required");
    }
    if (!isSubnetValid()) {
        return tr("Invalid CIDR notation (e.g., 192.168.1.0/24)");
    }
    return QString();
}

bool ScanConfigViewModel::isValid() const {
    return isSubnetValid() && threadCount > 0 && timeout > 0;
}

void ScanConfigViewModel::loadQuickScanPreset() {
    scanType = Quick;
    resolveDns = true;
    resolveArp = false;
    scanPorts = false;
    timeout = 1000;  // 1 second
    threadCount = QThread::idealThreadCount();
    portsToScan.clear();

    Logger::debug("Loaded Quick scan preset");
    updateValidation();
}

void ScanConfigViewModel::loadDeepScanPreset() {
    scanType = Deep;
    resolveDns = true;
    resolveArp = true;
    scanPorts = true;
    timeout = 3000;  // 3 seconds
    threadCount = QThread::idealThreadCount();
    portsToScan = {21, 22, 23, 25, 53, 80, 110, 135, 139, 143, 443, 445, 3389, 8080};

    Logger::debug("Loaded Deep scan preset");
    updateValidation();
}

void ScanConfigViewModel::loadCustomScanPreset() {
    scanType = Custom;
    // Keep current settings
    Logger::debug("Loaded Custom scan preset");
    updateValidation();
}

void ScanConfigViewModel::loadPreset(ScanType type) {
    switch (type) {
        case Quick:
            loadQuickScanPreset();
            break;
        case Deep:
            loadDeepScanPreset();
            break;
        case Custom:
            loadCustomScanPreset();
            break;
    }
}

QStringList ScanConfigViewModel::detectLocalNetworks() {
    QStringList networks;

    // Simple fallback - return common network as placeholder
    // Real implementation would use QNetworkInterface
    networks.append("192.168.1.0/24");
    networks.append("192.168.0.0/24");
    networks.append("10.0.0.0/24");

    Logger::info("Detected " + QString::number(networks.count()) + " local networks (placeholder)");
    return networks;
}

ScanCoordinator::ScanConfig ScanConfigViewModel::toScanConfig() const {
    ScanCoordinator::ScanConfig config;
    config.subnet = subnet;
    config.resolveDns = resolveDns;
    config.resolveArp = resolveArp;
    config.scanPorts = scanPorts;
    config.portsToScan = portsToScan;
    config.timeout = timeout;
    config.maxThreads = threadCount;
    return config;
}

void ScanConfigViewModel::updateValidation() {
    emit validationChanged(isValid());
}
