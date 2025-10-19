#include "Device.h"

Device::Device()
    : m_isOnline(false)
    , m_comments("")
{
}

Device::Device(const QString& ip, const QString& hostname)
    : m_ip(ip)
    , m_hostname(hostname)
    , m_isOnline(false)
    , m_comments("")
{
}

// Getters
QString Device::getId() const { return m_id; }
QString Device::getIp() const { return m_ip; }
QString Device::ip() const { return m_ip; }

QString Device::getHostname() const { return m_hostname; }
QString Device::hostname() const { return m_hostname; }

QString Device::getMacAddress() const { return m_macAddress; }
QString Device::macAddress() const { return m_macAddress; }

QString Device::getVendor() const { return m_vendor; }
QString Device::vendor() const { return m_vendor; }

bool Device::isOnline() const { return m_isOnline; }

QDateTime Device::getLastSeen() const { return m_lastSeen; }
QDateTime Device::lastSeen() const { return m_lastSeen; }

QList<PortInfo> Device::getOpenPorts() const { return m_openPorts; }
QList<PortInfo> Device::openPorts() const { return m_openPorts; }

NetworkMetrics Device::getMetrics() const { return m_metrics; }
NetworkMetrics Device::metrics() const { return m_metrics; }

QString Device::getComments() const { return m_comments; }
QString Device::comments() const { return m_comments; }

// Setters
void Device::setId(const QString& id) { m_id = id; }

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

void Device::setComments(const QString& comments)
{
    m_comments = comments;
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
