#include "NetworkInterface.h"
#include <QHostAddress>

NetworkInterface::NetworkInterface()
    : m_isActive(false)
{
}

NetworkInterface::NetworkInterface(const QString& name, const QString& ipAddress)
    : m_name(name)
    , m_ipAddress(ipAddress)
    , m_isActive(true)
{
}

// Getters
QString NetworkInterface::name() const
{
    return m_name;
}

QString NetworkInterface::ipAddress() const
{
    return m_ipAddress;
}

QString NetworkInterface::subnetMask() const
{
    return m_subnetMask;
}

QString NetworkInterface::gateway() const
{
    return m_gateway;
}

QString NetworkInterface::macAddress() const
{
    return m_macAddress;
}

bool NetworkInterface::isActive() const
{
    return m_isActive;
}

// Setters
void NetworkInterface::setName(const QString& name)
{
    m_name = name;
}

void NetworkInterface::setIpAddress(const QString& ipAddress)
{
    m_ipAddress = ipAddress;
}

void NetworkInterface::setSubnetMask(const QString& subnetMask)
{
    m_subnetMask = subnetMask;
}

void NetworkInterface::setGateway(const QString& gateway)
{
    m_gateway = gateway;
}

void NetworkInterface::setMacAddress(const QString& macAddress)
{
    m_macAddress = macAddress;
}

void NetworkInterface::setActive(bool active)
{
    m_isActive = active;
}

// Utility methods
bool NetworkInterface::isValid() const
{
    QHostAddress addr(m_ipAddress);
    return !m_ipAddress.isEmpty() && !addr.isNull();
}

QString NetworkInterface::getCidrNotation() const
{
    if (m_ipAddress.isEmpty() || m_subnetMask.isEmpty()) {
        return QString();
    }

    // Convert subnet mask to CIDR prefix
    QHostAddress mask(m_subnetMask);
    if (mask.isNull()) {
        return QString();
    }

    quint32 maskValue = mask.toIPv4Address();
    int prefixLength = 0;

    while (maskValue) {
        prefixLength += (maskValue & 1);
        maskValue >>= 1;
    }

    return QString("%1/%2").arg(m_ipAddress).arg(prefixLength);
}
