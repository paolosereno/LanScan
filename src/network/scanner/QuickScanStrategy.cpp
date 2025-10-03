#include "QuickScanStrategy.h"
#include "network/services/MacVendorLookup.h"
#include "utils/Logger.h"

QuickScanStrategy::QuickScanStrategy()
    : m_hostDiscovery(new HostDiscovery())
{
}

QuickScanStrategy::~QuickScanStrategy()
{
    delete m_hostDiscovery;
}

Device QuickScanStrategy::scan(const QString& ip)
{
    Device device;
    device.setIp(ip);
    device.setOnline(false);

    // Check if host is alive
    bool alive = m_hostDiscovery->isHostAlive(ip, 1000);

    if (alive) {
        device.setOnline(true);
        device.setLastSeen(QDateTime::currentDateTime());

        // Try to get MAC address from ARP cache
        QString mac = ArpDiscovery::getMacAddress(ip);
        if (!mac.isEmpty()) {
            device.setMacAddress(mac);

            // Lookup vendor from MAC
            MacVendorLookup vendorLookup;
            QString vendor = vendorLookup.lookupVendor(mac);
            if (!vendor.isEmpty()) {
                device.setVendor(vendor);
            }
        }

        Logger::debug(QString("Quick scan: %1 is online").arg(ip));
    }

    return device;
}

QString QuickScanStrategy::getName() const
{
    return "Quick Scan";
}

QString QuickScanStrategy::getDescription() const
{
    return "Fast host discovery using ping. No port scanning.";
}
