#ifndef PORTSERVICEMAPPER_H
#define PORTSERVICEMAPPER_H

#include <QString>
#include <QMap>
#include <QList>

class PortServiceMapper
{
public:
    PortServiceMapper();

    QString getServiceName(int port, const QString& protocol = "tcp");
    QList<int> getCommonPorts();
    bool isCommonPort(int port);

private:
    QMap<int, QString> m_tcpPorts;
    QMap<int, QString> m_udpPorts;

    void loadCommonPorts();
};

#endif // PORTSERVICEMAPPER_H
