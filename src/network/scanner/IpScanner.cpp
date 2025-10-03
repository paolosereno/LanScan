#include "IpScanner.h"
#include "utils/Logger.h"
#include "network/services/SubnetCalculator.h"
#include <QRunnable>

// Worker class for scanning individual IPs
class ScanWorker : public QObject, public QRunnable
{
    Q_OBJECT

public:
    ScanWorker(const QString& ip, IScanStrategy* strategy)
        : m_ip(ip)
        , m_strategy(strategy)
    {
        setAutoDelete(true);
    }

    void run() override
    {
        if (m_strategy) {
            Device device = m_strategy->scan(m_ip);
            if (device.isOnline()) {
                emit deviceScanned(device);
            } else {
                // Still emit for progress tracking, even if offline
                emit deviceScanned(device);
            }
        }
        emit scanComplete();
    }

signals:
    void deviceScanned(const Device& device);
    void scanComplete();

private:
    QString m_ip;
    IScanStrategy* m_strategy;
};

IpScanner::IpScanner(QObject *parent)
    : QObject(parent)
    , m_strategy(nullptr)
    , m_threadPool(new QThreadPool(this))
    , m_isScanning(0)
    , m_scannedCount(0)
    , m_totalHosts(0)
    , m_devicesFound(0)
{
    // Set optimal thread count (CPU cores)
    m_threadPool->setMaxThreadCount(QThread::idealThreadCount());

    Logger::debug(QString("IpScanner initialized with %1 threads").arg(m_threadPool->maxThreadCount()));
}

IpScanner::~IpScanner()
{
    stopScan();
}

void IpScanner::setScanStrategy(IScanStrategy* strategy)
{
    m_strategy = strategy;
    Logger::debug("Scan strategy set");
}

void IpScanner::startScan(const QString& cidr)
{
    if (m_isScanning.loadAcquire()) {
        Logger::warn("Scan already in progress");
        emit scanError("Scan already in progress");
        return;
    }

    if (!m_strategy) {
        Logger::error("No scan strategy set");
        emit scanError("No scan strategy set");
        return;
    }

    // Get IP range from CIDR
    QStringList ipRange = SubnetCalculator::getIpRange(cidr);

    if (ipRange.isEmpty()) {
        Logger::error(QString("Invalid CIDR notation: %1").arg(cidr));
        emit scanError("Invalid CIDR notation");
        return;
    }

    resetCounters();
    m_totalHosts = ipRange.size();
    m_isScanning.storeRelease(1);

    Logger::info(QString("Starting scan of %1 (%2 hosts)").arg(cidr).arg(m_totalHosts));
    emit scanStarted(m_totalHosts);

    // Create and queue scan workers
    for (const QString& ip : ipRange) {
        ScanWorker* worker = new ScanWorker(ip, m_strategy);

        connect(worker, &ScanWorker::deviceScanned, this, &IpScanner::onHostScanned, Qt::QueuedConnection);
        connect(worker, &ScanWorker::scanComplete, this, &IpScanner::onScanComplete, Qt::QueuedConnection);

        m_threadPool->start(worker);
    }
}

void IpScanner::stopScan()
{
    if (m_isScanning.loadAcquire()) {
        Logger::info("Stopping scan...");
        m_isScanning.storeRelease(0);
        m_threadPool->clear();
        m_threadPool->waitForDone(5000);

        emit scanFinished(m_devicesFound);
    }
}

bool IpScanner::isScanning() const
{
    return m_isScanning.loadAcquire() != 0;
}

int IpScanner::getProgress() const
{
    return m_scannedCount.loadAcquire();
}

int IpScanner::getTotalHosts() const
{
    return m_totalHosts;
}

void IpScanner::onHostScanned(const Device& device)
{
    if (device.isOnline()) {
        m_devicesFound++;
        Logger::debug(QString("Device found: %1 (%2)").arg(device.ip()).arg(device.hostname()));
        emit deviceDiscovered(device);
    }

    int current = m_scannedCount.fetchAndAddAcquire(1) + 1;
    emit scanProgress(current, m_totalHosts);
}

void IpScanner::onScanComplete()
{
    int scanned = m_scannedCount.loadAcquire();

    if (scanned >= m_totalHosts) {
        m_isScanning.storeRelease(0);
        Logger::info(QString("Scan completed. Found %1 devices out of %2 hosts")
                     .arg(m_devicesFound).arg(m_totalHosts));
        emit scanFinished(m_devicesFound);
    }
}

void IpScanner::resetCounters()
{
    m_scannedCount.storeRelease(0);
    m_totalHosts = 0;
    m_devicesFound = 0;
}

#include "IpScanner.moc"
