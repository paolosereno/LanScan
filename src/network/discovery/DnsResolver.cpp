#include "DnsResolver.h"
#include "utils/Logger.h"
#include <QEventLoop>
#include <QTimer>
#include <QThread>
#include <QRegularExpression>

DnsResolver::DnsResolver(QObject *parent)
    : QObject(parent)
    , m_destroyed(false)
    , m_dnsCache(1000)  // Cache up to 1000 entries
    , m_cacheHits(0)
    , m_cacheMisses(0)
    , m_retryCount(0)
{
    Logger::debug("DnsResolver initialized with cache (max 1000 entries)");
}

DnsResolver::~DnsResolver()
{
    m_destroyed = true;
    cancel();

    // Give pending callbacks a moment to finish
    QThread::msleep(100);
}

void DnsResolver::resolveHostname(const QString& ip)
{
    m_currentIp = ip;

    Logger::debug(QString("Resolving hostname for: %1").arg(ip));

    // Use QHostInfo for reverse DNS lookup (IP -> hostname)
    m_lookupId = QHostInfo::lookupHost(ip, this, &DnsResolver::onLookupFinished);

    Logger::debug(QString("QHostInfo::lookupHost started with ID: %1").arg(m_lookupId));
}

QString DnsResolver::resolveSync(const QString& ip, int timeout, int maxRetries)
{
    // Check cache first
    {
        QMutexLocker locker(&m_cacheMutex);
        QString* cached = m_dnsCache.object(ip);
        if (cached) {
            m_cacheHits++;
            Logger::debug(QString("DNS Cache HIT for %1 -> %2 (hits: %3, misses: %4)")
                         .arg(ip).arg(*cached).arg(m_cacheHits).arg(m_cacheMisses));
            return *cached;
        }
        m_cacheMisses++;
    }

    Logger::debug(QString("DNS Cache MISS for %1 - performing lookup (timeout: %2ms, retries: %3)")
                 .arg(ip).arg(timeout).arg(maxRetries));

    // Perform resolution with retry
    QString result = resolveWithRetry(ip, timeout, maxRetries);

    // Cache successful results (but not failures)
    if (!result.isEmpty()) {
        QMutexLocker locker(&m_cacheMutex);
        m_dnsCache.insert(ip, new QString(result));
        Logger::debug(QString("Cached DNS result: %1 -> %2").arg(ip).arg(result));
    }

    return result;
}

QString DnsResolver::resolveWithRetry(const QString& ip, int timeout, int retriesLeft)
{
    QEventLoop loop;
    QString result;
    bool signalReceived = false;
    int attempt = 0;

    while (retriesLeft >= 0) {
        attempt++;
        signalReceived = false;

        Logger::debug(QString("DNS lookup attempt %1/%2 for %1")
                     .arg(attempt)
                     .arg(attempt + retriesLeft)
                     .arg(ip));

        // Connect signals with explicit disconnection to avoid dangling references
        QMetaObject::Connection conn1 = connect(this, &DnsResolver::hostnameResolved,
            [&](const QString& resolvedIp, const QString& hostname) {
                if (resolvedIp == ip) {
                    Logger::debug(QString("resolveSync: Signal received for %1 -> %2").arg(resolvedIp).arg(hostname));
                    result = hostname;
                    signalReceived = true;
                    loop.quit();
                }
            });

        QMetaObject::Connection conn2 = connect(this, &DnsResolver::resolveFailed,
            [&](const QString& resolvedIp) {
                if (resolvedIp == ip) {
                    Logger::debug(QString("resolveSync: Failed signal for %1").arg(resolvedIp));
                    result = QString();
                    signalReceived = true;
                    loop.quit();
                }
            });

        // Calculate timeout with exponential backoff
        int currentTimeout = timeout * (1 + (attempt - 1) * 0.5);  // 1x, 1.5x, 2x timeout

        // Set timeout
        QTimer::singleShot(currentTimeout, &loop, [&]() {
            Logger::debug(QString("resolveSync: Timeout for %1 after %2ms (signal received: %3)")
                         .arg(ip).arg(currentTimeout).arg(signalReceived));
            loop.quit();
        });

        // Start async resolution
        resolveHostname(ip);

        Logger::debug(QString("resolveSync: Waiting for %1... (timeout: %2ms)").arg(ip).arg(currentTimeout));

        // Wait for completion or timeout
        loop.exec();

        // CRITICAL: Disconnect immediately to prevent late callbacks accessing stack variables
        disconnect(conn1);
        disconnect(conn2);

        // If we got a result, return it
        if (!result.isEmpty()) {
            Logger::info(QString("DNS resolved %1 -> %2 (attempt %3/%4)")
                        .arg(ip).arg(result).arg(attempt).arg(attempt + retriesLeft));
            return result;
        }

        // If we got a signal but no result, it's a definitive failure (not timeout)
        if (signalReceived && result.isEmpty()) {
            Logger::debug(QString("DNS resolution failed for %1 (no hostname found)").arg(ip));
            return QString();
        }

        // Otherwise it was a timeout - retry if we have retries left
        retriesLeft--;
        if (retriesLeft >= 0) {
            m_retryCount++;
            Logger::warn(QString("DNS timeout for %1, retrying... (%2 retries left)")
                        .arg(ip).arg(retriesLeft));
            // Small delay before retry (100ms)
            QThread::msleep(100);
        }
    }

    Logger::warn(QString("DNS resolution failed for %1 after %2 attempts (total retries: %3)")
                .arg(ip).arg(attempt).arg(m_retryCount));

    return QString();
}

void DnsResolver::cancel()
{
    // QHostInfo doesn't provide a way to cancel lookups
    // Just disconnect to ignore results
    disconnect(this);
}

void DnsResolver::clearCache()
{
    QMutexLocker locker(&m_cacheMutex);
    m_dnsCache.clear();
    Logger::info(QString("DNS cache cleared (hits: %1, misses: %2, retries: %3)")
                .arg(m_cacheHits).arg(m_cacheMisses).arg(m_retryCount));
    m_cacheHits = 0;
    m_cacheMisses = 0;
    m_retryCount = 0;
}

void DnsResolver::onLookupFinished(const QHostInfo& info)
{
    // Ignore callbacks if object is being destroyed
    if (m_destroyed) {
        return;
    }

    Logger::debug(QString("onLookupFinished called for: %1").arg(m_currentIp));
    Logger::debug(QString("  Error code: %1").arg(info.error()));
    Logger::debug(QString("  Error string: %1").arg(info.errorString()));
    Logger::debug(QString("  Hostname: '%1'").arg(info.hostName()));
    Logger::debug(QString("  Addresses count: %1").arg(info.addresses().size()));

    if (info.error() == QHostInfo::NoError && !info.hostName().isEmpty()) {
        QString hostname = info.hostName();

        // Validate hostname - should not be an IP address
        // If reverse DNS returns an IP instead of hostname, treat as failure
        QRegularExpression ipRegex("^\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}$");
        if (ipRegex.match(hostname).hasMatch()) {
            Logger::warn(QString("DNS returned IP address instead of hostname for %1: %2").arg(m_currentIp).arg(hostname));
            emit resolveFailed(m_currentIp);
            return;
        }

        Logger::debug(QString("Resolved %1 to %2").arg(m_currentIp).arg(hostname));
        emit hostnameResolved(m_currentIp, hostname);
    } else {
        Logger::debug(QString("Failed to resolve %1: %2").arg(m_currentIp).arg(info.errorString()));
        emit resolveFailed(m_currentIp);
    }
}
