#include "NetworkInterfaceDetector.h"
#include <QNetworkInterface>
#include <QProcess>
#include <QRegularExpression>
#include "utils/Logger.h"

QList<NetworkInterface> NetworkInterfaceDetector::detectInterfaces()
{
    QList<NetworkInterface> interfaces;

    QList<QNetworkInterface> allInterfaces = QNetworkInterface::allInterfaces();

    for (const QNetworkInterface& iface : allInterfaces) {
        // Skip loopback and inactive interfaces
        if (iface.flags() & QNetworkInterface::IsLoopBack) {
            continue;
        }

        if (!(iface.flags() & QNetworkInterface::IsUp)) {
            continue;
        }

        QList<QNetworkAddressEntry> entries = iface.addressEntries();

        for (const QNetworkAddressEntry& entry : entries) {
            if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol) {
                NetworkInterface netIface;
                netIface.setName(iface.humanReadableName());
                netIface.setIpAddress(entry.ip().toString());
                netIface.setSubnetMask(entry.netmask().toString());
                netIface.setMacAddress(iface.hardwareAddress());
                netIface.setActive(true);

                interfaces.append(netIface);
            }
        }
    }

    return interfaces;
}

NetworkInterface NetworkInterfaceDetector::getDefaultInterface()
{
    QList<NetworkInterface> interfaces = detectInterfaces();

    if (interfaces.isEmpty()) {
        return NetworkInterface();
    }

    // Try to find the default interface by checking gateway
    QString gateway = getGateway();

    if (!gateway.isEmpty()) {
        for (const NetworkInterface& iface : interfaces) {
            // Check if interface is in the same subnet as gateway
            // Simplified: return first interface
            return iface;
        }
    }

    // Fallback: return first active interface
    return interfaces.first();
}

QString NetworkInterfaceDetector::getLocalIp()
{
    NetworkInterface defaultIface = getDefaultInterface();
    return defaultIface.ipAddress();
}

QString NetworkInterfaceDetector::getGateway()
{
    return parseGatewayFromRoute();
}

QString NetworkInterfaceDetector::parseGatewayFromRoute()
{
#ifdef Q_OS_WIN
    // Windows: route print
    QProcess process;
    process.start("route", QStringList() << "print");
    process.waitForFinished(3000);

    QString output = QString::fromLocal8Bit(process.readAllStandardOutput());

    // Parse default gateway from "0.0.0.0" line
    QStringList lines = output.split('\n');
    for (const QString& line : lines) {
        if (line.contains("0.0.0.0") && line.contains("0.0.0.0")) {
            QStringList parts = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
            if (parts.size() >= 3) {
                return parts[2]; // Gateway IP
            }
        }
    }
#else
    // Linux/macOS: ip route or route -n
    QProcess process;
    process.start("ip", QStringList() << "route" << "show" << "default");
    process.waitForFinished(3000);

    if (process.error() != QProcess::UnknownError) {
        // Fallback to route command
        process.start("route", QStringList() << "-n");
        process.waitForFinished(3000);
    }

    QString output = QString::fromLocal8Bit(process.readAllStandardOutput());

    // Parse: "default via 192.168.1.1 ..."
    QStringList lines = output.split('\n');
    for (const QString& line : lines) {
        if (line.contains("default")) {
            QStringList parts = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
            for (int i = 0; i < parts.size() - 1; ++i) {
                if (parts[i] == "via" || parts[i] == "gateway") {
                    return parts[i + 1];
                }
            }
        }
    }
#endif

    Logger::warn("Could not detect default gateway");
    return QString();
}
