#include "ArpDiscovery.h"
#include "utils/Logger.h"
#include <QProcess>
#include <QRegularExpression>
#include <QNetworkInterface>
#include <QList>

QMap<QString, QString> ArpDiscovery::getArpTable()
{
    QProcess process;

#ifdef Q_OS_WIN
    process.start("arp", QStringList() << "-a");
#else
    process.start("arp", QStringList() << "-n");
#endif

    if (!process.waitForFinished(5000)) {
        Logger::error("ARP command timeout");
        return QMap<QString, QString>();
    }

    QString output = QString::fromLocal8Bit(process.readAllStandardOutput());

#ifdef Q_OS_WIN
    return parseArpTableWindows(output);
#else
    return parseArpTableLinux(output);
#endif
}

QString ArpDiscovery::getMacAddress(const QString& ip)
{
    // First check if this IP belongs to a local interface
    QString localMac = getLocalMacAddress(ip);
    if (!localMac.isEmpty()) {
        return localMac;
    }

    // Otherwise check ARP table
    QMap<QString, QString> arpTable = getArpTable();
    return arpTable.value(ip, QString());
}

QString ArpDiscovery::getLocalMacAddress(const QString& ip)
{
    // Check all network interfaces to see if the IP matches a local interface
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();

    for (const QNetworkInterface& iface : interfaces) {
        // Skip loopback
        if (iface.flags() & QNetworkInterface::IsLoopBack) {
            continue;
        }

        // Check all addresses on this interface
        QList<QNetworkAddressEntry> entries = iface.addressEntries();
        for (const QNetworkAddressEntry& entry : entries) {
            if (entry.ip().toString() == ip) {
                // Found matching IP - return MAC address
                QString mac = iface.hardwareAddress();
                if (!mac.isEmpty()) {
                    Logger::debug(QString("Found local MAC for %1: %2").arg(ip).arg(mac));
                    return mac;
                }
            }
        }
    }

    return QString();
}

QString ArpDiscovery::parseArpOutput(const QString& output)
{
    // Generic parse - not used, platform-specific parsers are used instead
    return output;
}

QMap<QString, QString> ArpDiscovery::parseArpTableWindows(const QString& output)
{
    QMap<QString, QString> arpTable;

    // Windows format:
    // Internet Address      Physical Address      Type
    // 192.168.1.1           aa-bb-cc-dd-ee-ff     dynamic

    QStringList lines = output.split('\n');

    for (const QString& line : lines) {
        // Match IP and MAC address
        QRegularExpression re("(\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3})\\s+([0-9a-fA-F]{2}-[0-9a-fA-F]{2}-[0-9a-fA-F]{2}-[0-9a-fA-F]{2}-[0-9a-fA-F]{2}-[0-9a-fA-F]{2})");
        QRegularExpressionMatch match = re.match(line);

        if (match.hasMatch()) {
            QString ip = match.captured(1);
            QString mac = match.captured(2);

            // Convert MAC format from aa-bb-cc-dd-ee-ff to AA:BB:CC:DD:EE:FF
            mac = mac.replace('-', ':').toUpper();

            arpTable[ip] = mac;
        }
    }

    Logger::debug(QString("Parsed %1 ARP entries (Windows)").arg(arpTable.size()));
    return arpTable;
}

QMap<QString, QString> ArpDiscovery::parseArpTableLinux(const QString& output)
{
    QMap<QString, QString> arpTable;

    // Linux format:
    // Address                  HWtype  HWaddress           Flags Mask            Iface
    // 192.168.1.1              ether   aa:bb:cc:dd:ee:ff   C                     eth0

    QStringList lines = output.split('\n');

    for (const QString& line : lines) {
        // Skip header and incomplete entries
        if (line.contains("Address") || line.contains("incomplete")) {
            continue;
        }

        // Match IP and MAC address
        QRegularExpression re("(\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}).*?([0-9a-fA-F]{2}:[0-9a-fA-F]{2}:[0-9a-fA-F]{2}:[0-9a-fA-F]{2}:[0-9a-fA-F]{2}:[0-9a-fA-F]{2})");
        QRegularExpressionMatch match = re.match(line);

        if (match.hasMatch()) {
            QString ip = match.captured(1);
            QString mac = match.captured(2).toUpper();

            arpTable[ip] = mac;
        }
    }

    Logger::debug(QString("Parsed %1 ARP entries (Linux)").arg(arpTable.size()));
    return arpTable;
}
