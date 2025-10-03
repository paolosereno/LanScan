#ifndef NETWORKINTERFACE_H
#define NETWORKINTERFACE_H

#include <QString>

class NetworkInterface
{
public:
    NetworkInterface();
    NetworkInterface(const QString& name, const QString& ipAddress);

    // Getters
    QString name() const;
    QString ipAddress() const;
    QString subnetMask() const;
    QString gateway() const;
    QString macAddress() const;
    bool isActive() const;

    // Setters
    void setName(const QString& name);
    void setIpAddress(const QString& ipAddress);
    void setSubnetMask(const QString& subnetMask);
    void setGateway(const QString& gateway);
    void setMacAddress(const QString& macAddress);
    void setActive(bool active);

    // Utility methods
    bool isValid() const;
    QString getCidrNotation() const;

private:
    QString m_name;
    QString m_ipAddress;
    QString m_subnetMask;
    QString m_gateway;
    QString m_macAddress;
    bool m_isActive;
};

#endif // NETWORKINTERFACE_H
