#ifndef DEEPSCANSTRATEGY_H
#define DEEPSCANSTRATEGY_H

#include "interfaces/IScanStrategy.h"
#include "network/discovery/HostDiscovery.h"
#include "network/discovery/DnsResolver.h"
#include "network/discovery/ArpDiscovery.h"
#include "network/sockets/TcpSocketManager.h"

/**
 * Deep scan strategy - comprehensive host analysis
 * - Ping for host discovery
 * - DNS reverse lookup for hostname
 * - MAC address from ARP
 * - Common port scanning
 * - Service identification
 */
class DeepScanStrategy : public IScanStrategy
{
public:
    DeepScanStrategy();
    ~DeepScanStrategy() override;

    Device scan(const QString& ip) override;
    QString getName() const override;
    QString getDescription() const override;

    // Enable/disable port scanning
    void setPortScanningEnabled(bool enabled);

    // Configure DNS timeout and retries
    void setDnsTimeout(int timeoutMs);
    void setDnsRetries(int maxRetries);

private:
    HostDiscovery* m_hostDiscovery;
    DnsResolver* m_dnsResolver;
    bool m_portScanningEnabled;
    int m_dnsTimeout;       // DNS timeout in milliseconds
    int m_dnsMaxRetries;    // Max DNS retry attempts

    // Common ports to scan
    static const QList<int> COMMON_PORTS;

    bool scanPort(const QString& ip, int port);
};

#endif // DEEPSCANSTRATEGY_H
