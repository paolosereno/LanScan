#include "DnsResolver.h"
#include "utils/Logger.h"
#include <QEventLoop>
#include <QTimer>

DnsResolver::DnsResolver(QObject *parent)
    : QObject(parent)
{
}

DnsResolver::~DnsResolver()
{
    cancel();
}

void DnsResolver::resolveHostname(const QString& ip)
{
    m_currentIp = ip;

    Logger::debug(QString("Resolving hostname for: %1").arg(ip));

    // Use QHostInfo for reverse DNS lookup (IP -> hostname)
    int lookupId = QHostInfo::lookupHost(ip, this, SLOT(onLookupFinished()));

    Q_UNUSED(lookupId);
}

QString DnsResolver::resolveSync(const QString& ip, int timeout)
{
    QEventLoop loop;
    QString result;

    // Connect signals
    connect(this, &DnsResolver::hostnameResolved, [&](const QString& resolvedIp, const QString& hostname) {
        if (resolvedIp == ip) {
            result = hostname;
            loop.quit();
        }
    });

    connect(this, &DnsResolver::resolveFailed, [&](const QString& resolvedIp) {
        if (resolvedIp == ip) {
            result = QString();
            loop.quit();
        }
    });

    // Set timeout
    QTimer::singleShot(timeout, &loop, &QEventLoop::quit);

    // Start async resolution
    resolveHostname(ip);

    // Wait for completion or timeout
    loop.exec();

    return result;
}

void DnsResolver::cancel()
{
    // QHostInfo doesn't provide a way to cancel lookups
    // Just disconnect to ignore results
    disconnect(this);
}

void DnsResolver::onLookupFinished()
{
    // This slot is called via QHostInfo::lookupHost
    // The QHostInfo object is passed as parameter but we need to handle it differently

    // For now, we'll do synchronous resolution
    QHostInfo info = QHostInfo::fromName(m_currentIp);

    if (info.error() == QHostInfo::NoError && !info.hostName().isEmpty()) {
        QString hostname = info.hostName();
        Logger::debug(QString("Resolved %1 to %2").arg(m_currentIp).arg(hostname));
        emit hostnameResolved(m_currentIp, hostname);
    } else {
        Logger::debug(QString("Failed to resolve %1: %2").arg(m_currentIp).arg(info.errorString()));
        emit resolveFailed(m_currentIp);
    }
}
