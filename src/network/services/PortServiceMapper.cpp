#include "PortServiceMapper.h"

PortServiceMapper::PortServiceMapper()
{
    loadCommonPorts();
}

QString PortServiceMapper::getServiceName(int port, const QString& protocol)
{
    if (protocol.toLower() == "tcp") {
        return m_tcpPorts.value(port, "Unknown");
    } else if (protocol.toLower() == "udp") {
        return m_udpPorts.value(port, "Unknown");
    }

    return "Unknown";
}

QList<int> PortServiceMapper::getCommonPorts()
{
    return m_tcpPorts.keys();
}

bool PortServiceMapper::isCommonPort(int port)
{
    return m_tcpPorts.contains(port) || m_udpPorts.contains(port);
}

void PortServiceMapper::loadCommonPorts()
{
    // Common TCP ports
    m_tcpPorts[20] = "FTP-DATA";
    m_tcpPorts[21] = "FTP";
    m_tcpPorts[22] = "SSH";
    m_tcpPorts[23] = "Telnet";
    m_tcpPorts[25] = "SMTP";
    m_tcpPorts[53] = "DNS";
    m_tcpPorts[80] = "HTTP";
    m_tcpPorts[110] = "POP3";
    m_tcpPorts[143] = "IMAP";
    m_tcpPorts[443] = "HTTPS";
    m_tcpPorts[445] = "SMB";
    m_tcpPorts[465] = "SMTPS";
    m_tcpPorts[587] = "SMTP-Submission";
    m_tcpPorts[993] = "IMAPS";
    m_tcpPorts[995] = "POP3S";
    m_tcpPorts[1433] = "MSSQL";
    m_tcpPorts[1521] = "Oracle";
    m_tcpPorts[3306] = "MySQL";
    m_tcpPorts[3389] = "RDP";
    m_tcpPorts[5432] = "PostgreSQL";
    m_tcpPorts[5900] = "VNC";
    m_tcpPorts[6379] = "Redis";
    m_tcpPorts[8080] = "HTTP-Alt";
    m_tcpPorts[8443] = "HTTPS-Alt";
    m_tcpPorts[27017] = "MongoDB";

    // Common UDP ports
    m_udpPorts[53] = "DNS";
    m_udpPorts[67] = "DHCP-Server";
    m_udpPorts[68] = "DHCP-Client";
    m_udpPorts[69] = "TFTP";
    m_udpPorts[123] = "NTP";
    m_udpPorts[137] = "NetBIOS-NS";
    m_udpPorts[138] = "NetBIOS-DGM";
    m_udpPorts[161] = "SNMP";
    m_udpPorts[162] = "SNMP-Trap";
    m_udpPorts[500] = "IKE";
    m_udpPorts[514] = "Syslog";
    m_udpPorts[1194] = "OpenVPN";
    m_udpPorts[1701] = "L2TP";
    m_udpPorts[4500] = "NAT-T";
}
