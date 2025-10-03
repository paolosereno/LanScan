#include "SubnetCalculator.h"
#include "utils/IpAddressValidator.h"
#include <QHostAddress>

QString SubnetCalculator::getNetworkAddress(const QString& ip, const QString& mask)
{
    if (!IpAddressValidator::isValidIpv4(ip) || !IpAddressValidator::isValidIpv4(mask)) {
        return QString();
    }

    quint32 ipValue = ipToUInt32(ip);
    quint32 maskValue = ipToUInt32(mask);
    quint32 networkValue = ipValue & maskValue;

    return uInt32ToIp(networkValue);
}

QString SubnetCalculator::getBroadcastAddress(const QString& ip, const QString& mask)
{
    if (!IpAddressValidator::isValidIpv4(ip) || !IpAddressValidator::isValidIpv4(mask)) {
        return QString();
    }

    quint32 ipValue = ipToUInt32(ip);
    quint32 maskValue = ipToUInt32(mask);
    quint32 networkValue = ipValue & maskValue;
    quint32 broadcastValue = networkValue | ~maskValue;

    return uInt32ToIp(broadcastValue);
}

QStringList SubnetCalculator::getIpRange(const QString& cidr)
{
    if (!IpAddressValidator::isValidCidr(cidr)) {
        return QStringList();
    }

    QPair<QString, int> parsed = IpAddressValidator::parseCidr(cidr);
    QString baseIp = parsed.first;
    int prefixLength = parsed.second;

    QString mask = cidrToSubnetMask(prefixLength);
    QString networkAddr = getNetworkAddress(baseIp, mask);
    QString broadcastAddr = getBroadcastAddress(baseIp, mask);

    quint32 start = ipToUInt32(networkAddr);
    quint32 end = ipToUInt32(broadcastAddr);

    QStringList range;

    // Skip network address and broadcast address for usable IPs
    if (prefixLength < 31) {
        start++; // Skip network address
        end--;   // Skip broadcast address
    }

    // Limit to reasonable range size (e.g., /16 = 65534 hosts)
    if (end - start > 65534) {
        return QStringList(); // Too large
    }

    for (quint32 i = start; i <= end; ++i) {
        range.append(uInt32ToIp(i));
    }

    return range;
}

int SubnetCalculator::getHostCount(const QString& cidr)
{
    if (!IpAddressValidator::isValidCidr(cidr)) {
        return 0;
    }

    QPair<QString, int> parsed = IpAddressValidator::parseCidr(cidr);
    int prefixLength = parsed.second;

    return IpAddressValidator::calculateHostCount(prefixLength);
}

bool SubnetCalculator::isIpInSubnet(const QString& ip, const QString& cidr)
{
    if (!IpAddressValidator::isValidIpv4(ip) || !IpAddressValidator::isValidCidr(cidr)) {
        return false;
    }

    QPair<QString, int> parsed = IpAddressValidator::parseCidr(cidr);
    QString baseIp = parsed.first;
    int prefixLength = parsed.second;

    QString mask = cidrToSubnetMask(prefixLength);
    QString networkAddr = getNetworkAddress(baseIp, mask);
    QString ipNetworkAddr = getNetworkAddress(ip, mask);

    return networkAddr == ipNetworkAddr;
}

QString SubnetCalculator::cidrToSubnetMask(int prefixLength)
{
    if (prefixLength < 0 || prefixLength > 32) {
        return QString();
    }

    quint32 mask = 0xFFFFFFFF << (32 - prefixLength);
    return uInt32ToIp(mask);
}

int SubnetCalculator::subnetMaskToCidr(const QString& mask)
{
    if (!IpAddressValidator::isValidIpv4(mask)) {
        return -1;
    }

    quint32 maskValue = ipToUInt32(mask);
    int cidr = 0;

    while (maskValue) {
        cidr += (maskValue & 1);
        maskValue >>= 1;
    }

    return cidr;
}

quint32 SubnetCalculator::ipToUInt32(const QString& ip)
{
    QHostAddress addr(ip);
    return addr.toIPv4Address();
}

QString SubnetCalculator::uInt32ToIp(quint32 value)
{
    QHostAddress addr(value);
    return addr.toString();
}
