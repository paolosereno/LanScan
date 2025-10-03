#ifndef NETWORKINTERFACEDETECTOR_H
#define NETWORKINTERFACEDETECTOR_H

#include <QList>
#include "models/NetworkInterface.h"

class NetworkInterfaceDetector
{
public:
    static QList<NetworkInterface> detectInterfaces();
    static NetworkInterface getDefaultInterface();
    static QString getLocalIp();
    static QString getGateway();

private:
    NetworkInterfaceDetector() = delete;
    static QString parseGatewayFromRoute();
};

#endif // NETWORKINTERFACEDETECTOR_H
