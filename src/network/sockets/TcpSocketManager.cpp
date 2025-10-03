#include "TcpSocketManager.h"
#include "utils/Logger.h"
#include <QTimer>

TcpSocketManager::TcpSocketManager(QObject *parent)
    : QObject(parent)
    , m_socket(new QTcpSocket(this))
    , m_isConnected(false)
{
    connect(m_socket, &QTcpSocket::connected, this, &TcpSocketManager::onConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &TcpSocketManager::onDisconnected);
    connect(m_socket, &QTcpSocket::errorOccurred, this, &TcpSocketManager::onError);
}

TcpSocketManager::~TcpSocketManager()
{
    if (m_isConnected) {
        disconnect();
    }
}

bool TcpSocketManager::connectToHost(const QString& host, int port, int timeout)
{
    if (m_isConnected) {
        Logger::warn("Already connected, disconnecting first");
        disconnect();
    }

    Logger::debug(QString("Attempting TCP connection to %1:%2").arg(host).arg(port));

    m_socket->connectToHost(host, port);

    if (!m_socket->waitForConnected(timeout)) {
        Logger::debug(QString("TCP connection failed: %1").arg(m_socket->errorString()));
        return false;
    }

    return m_isConnected;
}

void TcpSocketManager::disconnect()
{
    if (m_socket->state() != QAbstractSocket::UnconnectedState) {
        m_socket->disconnectFromHost();

        if (m_socket->state() != QAbstractSocket::UnconnectedState) {
            m_socket->waitForDisconnected(1000);
        }
    }

    m_isConnected = false;
}

bool TcpSocketManager::isConnected() const
{
    return m_isConnected;
}

QTcpSocket* TcpSocketManager::socket() const
{
    return m_socket;
}

void TcpSocketManager::onConnected()
{
    m_isConnected = true;
    Logger::debug("TCP socket connected");
    emit connected();
}

void TcpSocketManager::onDisconnected()
{
    m_isConnected = false;
    Logger::debug("TCP socket disconnected");
    emit disconnected();
}

void TcpSocketManager::onError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
    QString errorMsg = m_socket->errorString();
    Logger::debug(QString("TCP socket error: %1").arg(errorMsg));
    emit error(errorMsg);
}
