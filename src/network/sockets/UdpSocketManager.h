#ifndef UDPSOCKETMANAGER_H
#define UDPSOCKETMANAGER_H

#include <QObject>
#include <QUdpSocket>

class UdpSocketManager : public QObject
{
    Q_OBJECT

public:
    explicit UdpSocketManager(QObject *parent = nullptr);
    ~UdpSocketManager();

    bool sendDatagram(const QByteArray& data, const QString& host, int port);
    bool bind(int port);
    void close();

    bool isBound() const;
    QUdpSocket* socket() const;

signals:
    void datagramReceived(const QByteArray& data, const QString& sender, int port);
    void error(const QString& errorString);

private slots:
    void onReadyRead();
    void onError(QAbstractSocket::SocketError socketError);

private:
    QUdpSocket* m_socket;
    bool m_isBound;
};

#endif // UDPSOCKETMANAGER_H
