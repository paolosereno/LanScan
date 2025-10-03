#ifndef HOSTDISCOVERY_H
#define HOSTDISCOVERY_H

#include <QObject>
#include <QProcess>

class HostDiscovery : public QObject
{
    Q_OBJECT

public:
    explicit HostDiscovery(QObject *parent = nullptr);
    ~HostDiscovery();

    void pingHost(const QString& ip);
    bool isHostAlive(const QString& ip, int timeout = 1000);

    void stopPing();

signals:
    void hostDiscovered(const QString& ip, double latency);
    void hostNotFound(const QString& ip);

private slots:
    void onPingFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    QProcess* m_pingProcess;
    QString m_currentIp;

    QString parsePingOutput(const QString& output);
    double extractLatency(const QString& output);
};

#endif // HOSTDISCOVERY_H
