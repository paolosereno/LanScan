#ifndef TCPSOCKETMANAGER_H
#define TCPSOCKETMANAGER_H

#include <QObject>
#include <QTcpSocket>

class TcpSocketManager : public QObject
{
    Q_OBJECT

public:
    explicit TcpSocketManager(QObject *parent = nullptr);
    ~TcpSocketManager();

    bool connectToHost(const QString& host, int port, int timeout = 1000);
    void disconnect();
    bool isConnected() const;

    QTcpSocket* socket() const;

signals:
    void connected();
    void disconnected();
    void error(const QString& errorString);

private slots:
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError socketError);

private:
    QTcpSocket* m_socket;
    bool m_isConnected;
};

#endif // TCPSOCKETMANAGER_H
