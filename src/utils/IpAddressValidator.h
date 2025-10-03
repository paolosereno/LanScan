#ifndef IPADDRESSVALIDATOR_H
#define IPADDRESSVALIDATOR_H

#include <QString>
#include <QPair>

class IpAddressValidator
{
public:
    static bool isValidIpv4(const QString& ip);
    static bool isValidCidr(const QString& cidr);
    static bool isValidIpRange(const QString& start, const QString& end);
    static bool isValidPort(int port);
    static bool isValidMacAddress(const QString& mac);

    // Utility methods
    static QPair<QString, int> parseCidr(const QString& cidr);
    static quint32 ipToUInt32(const QString& ip);
    static QString uInt32ToIp(quint32 value);
    static int calculateHostCount(int prefixLength);

private:
    IpAddressValidator() = delete;
    static bool isValidOctet(int value);
};

#endif // IPADDRESSVALIDATOR_H
