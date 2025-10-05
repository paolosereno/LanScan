#include "DnsResolver.h"
#include "utils/Logger.h"
#include <QEventLoop>
#include <QTimer>
#include <QThread>

DnsResolver::DnsResolver(QObject *parent)
    : QObject(parent)
    , m_destroyed(false)
{
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

QString DnsResolver::resolveSync(const QString& ip, int timeout)
{
    QEventLoop loop;
    QString result;
    bool signalReceived = false;

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

    // Set timeout
    QTimer::singleShot(timeout, &loop, [&]() {
        Logger::debug(QString("resolveSync: Timeout for %1 (signal received: %2)").arg(ip).arg(signalReceived));
        loop.quit();
    });

    // Start async resolution
    resolveHostname(ip);

    Logger::debug(QString("resolveSync: Waiting for %1...").arg(ip));

    // Wait for completion or timeout
    loop.exec();

    // CRITICAL: Disconnect immediately to prevent late callbacks accessing stack variables
    disconnect(conn1);
    disconnect(conn2);

    Logger::debug(QString("resolveSync: Done for %1, result: '%2'").arg(ip).arg(result));

    return result;
}

void DnsResolver::cancel()
{
    // QHostInfo doesn't provide a way to cancel lookups
    // Just disconnect to ignore results
    disconnect(this);
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
    Logger::debug(QString("  Hostname: %1").arg(info.hostName()));
    Logger::debug(QString("  Addresses count: %1").arg(info.addresses().size()));

    if (info.error() == QHostInfo::NoError && !info.hostName().isEmpty()) {
        QString hostname = info.hostName();
        Logger::debug(QString("Resolved %1 to %2").arg(m_currentIp).arg(hostname));
        emit hostnameResolved(m_currentIp, hostname);
    } else {
        Logger::debug(QString("Failed to resolve %1: %2").arg(m_currentIp).arg(info.errorString()));
        emit resolveFailed(m_currentIp);
    }
}
