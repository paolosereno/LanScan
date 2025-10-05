#ifndef DNSRESOLVER_H
#define DNSRESOLVER_H

#include <QObject>
#include <QDnsLookup>
#include <QHostInfo>

class DnsResolver : public QObject
{
    Q_OBJECT

public:
    explicit DnsResolver(QObject *parent = nullptr);
    ~DnsResolver();

    void resolveHostname(const QString& ip);
    QString resolveSync(const QString& ip, int timeout = 2000);

    void cancel();

signals:
    void hostnameResolved(const QString& ip, const QString& hostname);
    void resolveFailed(const QString& ip);

private slots:
    void onLookupFinished(const QHostInfo& info);

private:
    int m_lookupId;
    QString m_currentIp;
    bool m_destroyed;
};

#endif // DNSRESOLVER_H
