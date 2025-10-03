#ifndef ARPDISCOVERY_H
#define ARPDISCOVERY_H

#include <QString>
#include <QMap>

class ArpDiscovery
{
public:
    static QMap<QString, QString> getArpTable();
    static QString getMacAddress(const QString& ip);

private:
    ArpDiscovery() = delete;

    static QString parseArpOutput(const QString& output);
    static QMap<QString, QString> parseArpTableWindows(const QString& output);
    static QMap<QString, QString> parseArpTableLinux(const QString& output);
};

#endif // ARPDISCOVERY_H
