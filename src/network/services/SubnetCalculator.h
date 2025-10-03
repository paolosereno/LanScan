#ifndef SUBNETCALCULATOR_H
#define SUBNETCALCULATOR_H

#include <QString>
#include <QStringList>

class SubnetCalculator
{
public:
    static QString getNetworkAddress(const QString& ip, const QString& mask);
    static QString getBroadcastAddress(const QString& ip, const QString& mask);
    static QStringList getIpRange(const QString& cidr);
    static int getHostCount(const QString& cidr);
    static bool isIpInSubnet(const QString& ip, const QString& cidr);

    // Utility methods
    static QString cidrToSubnetMask(int prefixLength);
    static int subnetMaskToCidr(const QString& mask);
    static quint32 ipToUInt32(const QString& ip);
    static QString uInt32ToIp(quint32 value);

private:
    SubnetCalculator() = delete;
};

#endif // SUBNETCALCULATOR_H
