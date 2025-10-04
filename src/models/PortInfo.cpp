#include "PortInfo.h"
#include <QMap>

PortInfo::PortInfo()
    : m_portNumber(0)
    , m_protocol(TCP)
    , m_state(Closed)
{
}

PortInfo::PortInfo(int portNumber, Protocol protocol)
    : m_portNumber(portNumber)
    , m_protocol(protocol)
    , m_state(Open)
    , m_service(getDefaultService(portNumber, protocol))
{
}

// Getters
int PortInfo::portNumber() const
{
    return m_portNumber;
}

PortInfo::Protocol PortInfo::protocol() const
{
    return m_protocol;
}

QString PortInfo::protocolString() const
{
    return (m_protocol == TCP) ? "TCP" : "UDP";
}

QString PortInfo::service() const
{
    return m_service;
}

PortInfo::State PortInfo::state() const
{
    return m_state;
}

QString PortInfo::stateString() const
{
    switch (m_state) {
        case Open: return "Open";
        case Closed: return "Closed";
        case Filtered: return "Filtered";
        default: return "Unknown";
    }
}

// Alternative getters with "get" prefix
int PortInfo::getPort() const
{
    return portNumber();
}

PortInfo::Protocol PortInfo::getProtocol() const
{
    return protocol();
}

QString PortInfo::getService() const
{
    return service();
}

PortInfo::State PortInfo::getState() const
{
    return state();
}

// Setters
void PortInfo::setPortNumber(int portNumber)
{
    m_portNumber = portNumber;
}

void PortInfo::setProtocol(Protocol protocol)
{
    m_protocol = protocol;
}

void PortInfo::setService(const QString& service)
{
    m_service = service;
}

void PortInfo::setState(State state)
{
    m_state = state;
}

// Utility methods
QString PortInfo::getDefaultService(int portNumber, Protocol protocol)
{
    static QMap<int, QString> tcpServices = {
        {20, "FTP-DATA"},
        {21, "FTP"},
        {22, "SSH"},
        {23, "Telnet"},
        {25, "SMTP"},
        {53, "DNS"},
        {80, "HTTP"},
        {110, "POP3"},
        {143, "IMAP"},
        {443, "HTTPS"},
        {445, "SMB"},
        {3306, "MySQL"},
        {3389, "RDP"},
        {5432, "PostgreSQL"},
        {5900, "VNC"},
        {8080, "HTTP-ALT"}
    };

    static QMap<int, QString> udpServices = {
        {53, "DNS"},
        {67, "DHCP-Server"},
        {68, "DHCP-Client"},
        {123, "NTP"},
        {161, "SNMP"},
        {500, "IKE"},
        {514, "Syslog"}
    };

    if (protocol == TCP && tcpServices.contains(portNumber)) {
        return tcpServices[portNumber];
    } else if (protocol == UDP && udpServices.contains(portNumber)) {
        return udpServices[portNumber];
    }

    return "Unknown";
}
