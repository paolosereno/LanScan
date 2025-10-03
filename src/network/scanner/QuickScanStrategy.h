#ifndef QUICKSCANSTRATEGY_H
#define QUICKSCANSTRATEGY_H

#include "interfaces/IScanStrategy.h"
#include "network/discovery/HostDiscovery.h"
#include "network/discovery/ArpDiscovery.h"

/**
 * Quick scan strategy - fast host discovery
 * Only checks if host is alive via ping and gets MAC from ARP
 * No port scanning or deep inspection
 */
class QuickScanStrategy : public IScanStrategy
{
public:
    QuickScanStrategy();
    ~QuickScanStrategy() override;

    Device scan(const QString& ip) override;
    QString getName() const override;
    QString getDescription() const override;

private:
    HostDiscovery* m_hostDiscovery;
};

#endif // QUICKSCANSTRATEGY_H
