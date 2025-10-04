#ifndef DEVICE_H
#define DEVICE_H

#include <QString>
#include <QDateTime>
#include <QList>
#include <QMetaType>
#include "PortInfo.h"
#include "NetworkMetrics.h"

class Device
{
public:
    Device();
    Device(const QString& ip, const QString& hostname = QString());

    // Getters
    QString getId() const;
    QString getIp() const;
    QString ip() const;
    QString getHostname() const;
    QString hostname() const;
    QString getMacAddress() const;
    QString macAddress() const;
    QString getVendor() const;
    QString vendor() const;
    bool isOnline() const;
    QDateTime getLastSeen() const;
    QDateTime lastSeen() const;
    QList<PortInfo> getOpenPorts() const;
    QList<PortInfo> openPorts() const;
    NetworkMetrics getMetrics() const;
    NetworkMetrics metrics() const;

    // Setters
    void setId(const QString& id);
    void setIp(const QString& ip);
    void setHostname(const QString& hostname);
    void setMacAddress(const QString& macAddress);
    void setVendor(const QString& vendor);
    void setOnline(bool online);
    void setLastSeen(const QDateTime& lastSeen);
    void setOpenPorts(const QList<PortInfo>& ports);
    void setMetrics(const NetworkMetrics& metrics);

    // Utility methods
    void addPort(const PortInfo& port);
    void removePort(int portNumber);
    bool hasPort(int portNumber) const;

private:
    QString m_id;
    QString m_ip;
    QString m_hostname;
    QString m_macAddress;
    QString m_vendor;
    bool m_isOnline;
    QDateTime m_lastSeen;
    QList<PortInfo> m_openPorts;
    NetworkMetrics m_metrics;
};

#endif // DEVICE_H
