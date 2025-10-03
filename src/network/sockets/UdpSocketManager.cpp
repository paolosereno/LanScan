#include "UdpSocketManager.h"
#include "utils/Logger.h"

UdpSocketManager::UdpSocketManager(QObject *parent)
    : QObject(parent)
    , m_socket(new QUdpSocket(this))
    , m_isBound(false)
{
    connect(m_socket, &QUdpSocket::readyRead, this, &UdpSocketManager::onReadyRead);
    connect(m_socket, &QUdpSocket::errorOccurred, this, &UdpSocketManager::onError);
}

UdpSocketManager::~UdpSocketManager()
{
    if (m_isBound) {
        close();
    }
}

bool UdpSocketManager::sendDatagram(const QByteArray& data, const QString& host, int port)
{
    qint64 bytesWritten = m_socket->writeDatagram(data, QHostAddress(host), port);

    if (bytesWritten < 0) {
        Logger::debug(QString("Failed to send UDP datagram: %1").arg(m_socket->errorString()));
        return false;
    }

    Logger::debug(QString("Sent UDP datagram to %1:%2 (%3 bytes)").arg(host).arg(port).arg(bytesWritten));
    return true;
}

bool UdpSocketManager::bind(int port)
{
    if (m_isBound) {
        Logger::warn("UDP socket already bound, closing first");
        close();
    }

    bool success = m_socket->bind(QHostAddress::Any, port);

    if (success) {
        m_isBound = true;
        Logger::debug(QString("UDP socket bound to port %1").arg(port));
    } else {
        Logger::error(QString("Failed to bind UDP socket: %1").arg(m_socket->errorString()));
    }

    return success;
}

void UdpSocketManager::close()
{
    if (m_isBound) {
        m_socket->close();
        m_isBound = false;
        Logger::debug("UDP socket closed");
    }
}

bool UdpSocketManager::isBound() const
{
    return m_isBound;
}

QUdpSocket* UdpSocketManager::socket() const
{
    return m_socket;
}

void UdpSocketManager::onReadyRead()
{
    while (m_socket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(m_socket->pendingDatagramSize());

        QHostAddress sender;
        quint16 senderPort;

        m_socket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

        Logger::debug(QString("Received UDP datagram from %1:%2 (%3 bytes)")
                      .arg(sender.toString()).arg(senderPort).arg(datagram.size()));

        emit datagramReceived(datagram, sender.toString(), senderPort);
    }
}

void UdpSocketManager::onError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
    QString errorMsg = m_socket->errorString();
    Logger::debug(QString("UDP socket error: %1").arg(errorMsg));
    emit error(errorMsg);
}
