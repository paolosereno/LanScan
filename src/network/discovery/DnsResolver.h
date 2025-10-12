#ifndef DNSRESOLVER_H
#define DNSRESOLVER_H

#include <QObject>
#include <QDnsLookup>
#include <QHostInfo>
#include <QCache>
#include <QMutex>
#include <QMap>

class DnsResolver : public QObject
{
    Q_OBJECT

public:
    explicit DnsResolver(QObject *parent = nullptr);
    ~DnsResolver();

    void resolveHostname(const QString& ip);
    QString resolveSync(const QString& ip, int timeout = 2000, int maxRetries = 2);

    void cancel();
    void clearCache();

    // Statistics
    int getCacheHits() const { return m_cacheHits; }
    int getCacheMisses() const { return m_cacheMisses; }
    int getRetryCount() const { return m_retryCount; }

signals:
    void hostnameResolved(const QString& ip, const QString& hostname);
    void resolveFailed(const QString& ip);

private slots:
    void onLookupFinished(const QHostInfo& info);

private:
    QString resolveWithRetry(const QString& ip, int timeout, int retriesLeft);

    int m_lookupId;
    QString m_currentIp;
    bool m_destroyed;

    // DNS cache: IP -> hostname mapping
    QCache<QString, QString> m_dnsCache;
    mutable QMutex m_cacheMutex;

    // Map lookupId -> IP address to avoid race conditions
    QMap<int, QString> m_lookupIdToIp;
    QMutex m_lookupMutex;

    // Statistics
    int m_cacheHits;
    int m_cacheMisses;
    int m_retryCount;
};

#endif // DNSRESOLVER_H
