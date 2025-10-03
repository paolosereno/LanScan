#include "IpAddressValidator.h"
#include <QHostAddress>
#include <QRegularExpression>

bool IpAddressValidator::isValidIpv4(const QString& ip)
{
    QHostAddress addr(ip);
    return addr.protocol() == QAbstractSocket::IPv4Protocol;
}

bool IpAddressValidator::isValidCidr(const QString& cidr)
{
    QRegularExpression cidrRegex("^(\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3})/(\\d{1,2})$");
    QRegularExpressionMatch match = cidrRegex.match(cidr);

    if (!match.hasMatch()) {
        return false;
    }

    QString ip = match.captured(1);
    int prefix = match.captured(2).toInt();

    return isValidIpv4(ip) && prefix >= 0 && prefix <= 32;
}

bool IpAddressValidator::isValidIpRange(const QString& start, const QString& end)
{
    if (!isValidIpv4(start) || !isValidIpv4(end)) {
        return false;
    }

    quint32 startValue = ipToUInt32(start);
    quint32 endValue = ipToUInt32(end);

    return startValue <= endValue;
}

bool IpAddressValidator::isValidPort(int port)
{
    return port >= 1 && port <= 65535;
}

bool IpAddressValidator::isValidMacAddress(const QString& mac)
{
    // Accepts formats: AA:BB:CC:DD:EE:FF or AA-BB-CC-DD-EE-FF
    QRegularExpression macRegex("^([0-9A-Fa-f]{2}[:-]){5}([0-9A-Fa-f]{2})$");
    return macRegex.match(mac).hasMatch();
}

QPair<QString, int> IpAddressValidator::parseCidr(const QString& cidr)
{
    QRegularExpression cidrRegex("^(\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3})/(\\d{1,2})$");
    QRegularExpressionMatch match = cidrRegex.match(cidr);

    if (!match.hasMatch()) {
        return QPair<QString, int>(QString(), 0);
    }

    return QPair<QString, int>(match.captured(1), match.captured(2).toInt());
}

quint32 IpAddressValidator::ipToUInt32(const QString& ip)
{
    QHostAddress addr(ip);
    return addr.toIPv4Address();
}

QString IpAddressValidator::uInt32ToIp(quint32 value)
{
    QHostAddress addr(value);
    return addr.toString();
}

int IpAddressValidator::calculateHostCount(int prefixLength)
{
    if (prefixLength < 0 || prefixLength > 32) {
        return 0;
    }

    int hostBits = 32 - prefixLength;
    return (1 << hostBits) - 2; // Exclude network and broadcast addresses
}

bool IpAddressValidator::isValidOctet(int value)
{
    return value >= 0 && value <= 255;
}
