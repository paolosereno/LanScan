#include "DeepScanStrategy.h"
#include "network/services/MacVendorLookup.h"
#include "network/services/PortServiceMapper.h"
#include "utils/Logger.h"
#include "models/PortInfo.h"

// Common ports to scan
const QList<int> DeepScanStrategy::COMMON_PORTS = {
    21,   // FTP
    22,   // SSH
    23,   // Telnet
    25,   // SMTP
    53,   // DNS
    80,   // HTTP
    110,  // POP3
    143,  // IMAP
    443,  // HTTPS
    445,  // SMB
    3306, // MySQL
    3389, // RDP
    5432, // PostgreSQL
    5900, // VNC
    8080, // HTTP-Alt
    8443  // HTTPS-Alt
};

DeepScanStrategy::DeepScanStrategy()
    : m_hostDiscovery(new HostDiscovery())
    , m_dnsResolver(new DnsResolver())
{
}

DeepScanStrategy::~DeepScanStrategy()
{
    delete m_hostDiscovery;
    delete m_dnsResolver;
}

Device DeepScanStrategy::scan(const QString& ip)
{
    Device device;
    device.setIp(ip);
    device.setOnline(false);

    // Check if host is alive
    bool alive = m_hostDiscovery->isHostAlive(ip, 1000);

    if (!alive) {
        return device;
    }

    device.setOnline(true);
    device.setLastSeen(QDateTime::currentDateTime());

    Logger::debug(QString("Deep scan: %1 is online").arg(ip));

    // Get MAC address from ARP cache
    QString mac = ArpDiscovery::getMacAddress(ip);
    if (!mac.isEmpty()) {
        device.setMacAddress(mac);

        // Lookup vendor from MAC using singleton
        QString vendor = MacVendorLookup::instance()->lookupVendor(mac);
        if (!vendor.isEmpty() && vendor != "Unknown") {
            device.setVendor(vendor);
        }
    }

    // Reverse DNS lookup for hostname
    QString hostname = m_dnsResolver->resolveSync(ip, 2000);
    if (!hostname.isEmpty()) {
        device.setHostname(hostname);
        Logger::debug(QString("Hostname: %1").arg(hostname));
    }

    // Scan common ports
    PortServiceMapper portMapper;
    int openPortCount = 0;

    for (int port : COMMON_PORTS) {
        if (scanPort(ip, port)) {
            PortInfo portInfo;
            portInfo.setPortNumber(port);
            portInfo.setProtocol(PortInfo::TCP);
            portInfo.setState(PortInfo::Open);

            QString service = portMapper.getServiceName(port, "tcp");
            portInfo.setService(service);

            device.addPort(portInfo);
            openPortCount++;

            Logger::debug(QString("Port %1/%2 open (%3)").arg(port).arg("tcp").arg(service));
        }
    }

    Logger::debug(QString("Deep scan complete: %1 has %2 open ports").arg(ip).arg(openPortCount));

    return device;
}

QString DeepScanStrategy::getName() const
{
    return "Deep Scan";
}

QString DeepScanStrategy::getDescription() const
{
    return "Comprehensive scan: ping, DNS, MAC, and common port scanning.";
}

bool DeepScanStrategy::scanPort(const QString& ip, int port)
{
    TcpSocketManager socket;
    return socket.connectToHost(ip, port, 500);
}
