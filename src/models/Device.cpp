#include "Device.h"

Device::Device()
    : m_isOnline(false)
{
}

Device::Device(const QString& ip, const QString& hostname)
    : m_ip(ip)
    , m_hostname(hostname)
    , m_isOnline(false)
{
}

// Getters
QString Device::ip() const
{
    return m_ip;
}

QString Device::hostname() const
{
    return m_hostname;
}

QString Device::macAddress() const
{
    return m_macAddress;
}

QString Device::vendor() const
{
    return m_vendor;
}

bool Device::isOnline() const
{
    return m_isOnline;
}

QDateTime Device::lastSeen() const
{
    return m_lastSeen;
}

QList<PortInfo> Device::openPorts() const
{
    return m_openPorts;
}

NetworkMetrics Device::metrics() const
{
    return m_metrics;
}

// Setters
void Device::setIp(const QString& ip)
{
    m_ip = ip;
}

void Device::setHostname(const QString& hostname)
{
    m_hostname = hostname;
}

void Device::setMacAddress(const QString& macAddress)
{
    m_macAddress = macAddress;
}

void Device::setVendor(const QString& vendor)
{
    m_vendor = vendor;
}

void Device::setOnline(bool online)
{
    m_isOnline = online;
}

void Device::setLastSeen(const QDateTime& lastSeen)
{
    m_lastSeen = lastSeen;
}

void Device::setOpenPorts(const QList<PortInfo>& ports)
{
    m_openPorts = ports;
}

void Device::setMetrics(const NetworkMetrics& metrics)
{
    m_metrics = metrics;
}

// Utility methods
void Device::addPort(const PortInfo& port)
{
    if (!hasPort(port.portNumber())) {
        m_openPorts.append(port);
    }
}

void Device::removePort(int portNumber)
{
    for (int i = 0; i < m_openPorts.size(); ++i) {
        if (m_openPorts[i].portNumber() == portNumber) {
            m_openPorts.removeAt(i);
            break;
        }
    }
}

bool Device::hasPort(int portNumber) const
{
    for (const PortInfo& port : m_openPorts) {
        if (port.portNumber() == portNumber) {
            return true;
        }
    }
    return false;
}
