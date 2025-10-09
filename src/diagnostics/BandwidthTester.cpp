#include "diagnostics/BandwidthTester.h"
#include "utils/Logger.h"
#include <QHostAddress>

BandwidthTester::BandwidthTester(QObject* parent)
    : QObject(parent)
    , m_tcpSocket(nullptr)
    , m_udpSocket(nullptr)
    , m_progressTimer(new QTimer(this))
    , m_testTimer(new QTimer(this))
    , m_target("")
    , m_port(0)
    , m_durationMs(0)
    , m_direction(Download)
    , m_protocol(TCP)
    , m_totalBytes(0)
    , m_packetSize(65536)  // 64 KB default
    , m_measuredBandwidth(0.0)
    , m_isRunning(false)
{
    // Configure progress timer (update every 500ms)
    m_progressTimer->setInterval(500);
    connect(m_progressTimer, &QTimer::timeout, this, &BandwidthTester::onProgressTimerTick);

    // Configure test timer (single shot)
    m_testTimer->setSingleShot(true);
    connect(m_testTimer, &QTimer::timeout, this, &BandwidthTester::onTestTimeout);
}

BandwidthTester::~BandwidthTester()
{
    cancel();
}

bool BandwidthTester::testDownloadSpeed(const QString& target, quint16 port, int durationSeconds, Protocol protocol)
{
    if (durationSeconds < 1 || durationSeconds > 60) {
        Logger::error(QString("BandwidthTester: Invalid duration %1 (must be 1-60 seconds)").arg(durationSeconds));
        return false;
    }

    return startTest(target, port, durationSeconds, Download, protocol);
}

bool BandwidthTester::testUploadSpeed(const QString& target, quint16 port, int durationSeconds, Protocol protocol)
{
    if (durationSeconds < 1 || durationSeconds > 60) {
        Logger::error(QString("BandwidthTester: Invalid duration %1 (must be 1-60 seconds)").arg(durationSeconds));
        return false;
    }

    return startTest(target, port, durationSeconds, Upload, protocol);
}

void BandwidthTester::cancel()
{
    if (!m_isRunning) {
        return;
    }

    Logger::info("BandwidthTester: Cancelling test");

    m_progressTimer->stop();
    m_testTimer->stop();

    if (m_tcpSocket) {
        m_tcpSocket->abort();
        m_tcpSocket->deleteLater();
        m_tcpSocket = nullptr;
    }

    if (m_udpSocket) {
        m_udpSocket->abort();
        m_udpSocket->deleteLater();
        m_udpSocket = nullptr;
    }

    m_isRunning = false;
}

void BandwidthTester::setPacketSize(int size)
{
    if (size < 1024 || size > 1048576) {
        Logger::warn(QString("BandwidthTester: Invalid packet size %1 (must be 1024-1048576)").arg(size));
        return;
    }

    m_packetSize = size;
    Logger::debug(QString("BandwidthTester: Packet size set to %1 bytes").arg(size));
}

bool BandwidthTester::startTest(const QString& target, quint16 port, int duration, Direction direction, Protocol protocol)
{
    if (m_isRunning) {
        Logger::warn("BandwidthTester: Cannot start test: already running");
        return false;
    }

    if (target.isEmpty()) {
        Logger::error("BandwidthTester: Cannot start test: target is empty");
        return false;
    }

    if (port == 0) {
        Logger::error("BandwidthTester: Cannot start test: invalid port");
        return false;
    }

    m_target = target;
    m_port = port;
    m_durationMs = duration * 1000;
    m_direction = direction;
    m_protocol = protocol;
    m_totalBytes = 0;
    m_measuredBandwidth = 0.0;
    m_isRunning = true;

    Logger::info(QString("BandwidthTester: Starting %1 %2 test to %3:%4 for %5 seconds")
                 .arg(protocol == TCP ? "TCP" : "UDP")
                 .arg(direction == Download ? "download" : "upload")
                 .arg(target)
                 .arg(port)
                 .arg(duration));

    // Prepare send buffer for upload tests
    if (m_direction == Upload) {
        m_sendBuffer.resize(m_packetSize);
        m_sendBuffer.fill('X');  // Fill with dummy data
    }

    // Start timer
    m_timer.start();

    // Start test timer
    m_testTimer->start(m_durationMs);

    // Start progress timer
    m_progressTimer->start();

    if (m_protocol == TCP) {
        // Create TCP socket
        m_tcpSocket = new QTcpSocket(this);
        connect(m_tcpSocket, &QTcpSocket::connected, this, &BandwidthTester::onTcpConnected);
        connect(m_tcpSocket, &QTcpSocket::errorOccurred, this, &BandwidthTester::onTcpError);
        connect(m_tcpSocket, &QTcpSocket::readyRead, this, &BandwidthTester::onTcpReadyRead);
        connect(m_tcpSocket, &QTcpSocket::bytesWritten, this, &BandwidthTester::onTcpBytesWritten);

        // Connect to target
        m_tcpSocket->connectToHost(target, port);

        // Wait for connection (5 second timeout)
        if (!m_tcpSocket->waitForConnected(5000)) {
            Logger::error(QString("BandwidthTester: Failed to connect to %1:%2").arg(target).arg(port));
            cancel();
            emit testError("Connection failed");
            return false;
        }
    } else {
        // Create UDP socket
        m_udpSocket = new QUdpSocket(this);
        connect(m_udpSocket, &QUdpSocket::readyRead, this, &BandwidthTester::onUdpReadyRead);

        if (!m_udpSocket->bind()) {
            Logger::error("BandwidthTester: Failed to bind UDP socket");
            cancel();
            emit testError("Failed to bind UDP socket");
            return false;
        }

        // For upload, start sending immediately
        if (m_direction == Upload) {
            sendData();
        }
    }

    return true;
}

void BandwidthTester::onTcpConnected()
{
    Logger::debug("BandwidthTester: TCP connected");

    // If upload test, start sending data
    if (m_direction == Upload) {
        sendData();
    }
    // For download test, just wait for data (server should start sending)
}

void BandwidthTester::onTcpError(QAbstractSocket::SocketError error)
{
    QString errorMsg;
    switch (error) {
        case QAbstractSocket::ConnectionRefusedError:
            errorMsg = "Connection refused";
            break;
        case QAbstractSocket::RemoteHostClosedError:
            errorMsg = "Remote host closed connection";
            break;
        case QAbstractSocket::HostNotFoundError:
            errorMsg = "Host not found";
            break;
        case QAbstractSocket::SocketTimeoutError:
            errorMsg = "Connection timeout";
            break;
        case QAbstractSocket::NetworkError:
            errorMsg = "Network error";
            break;
        default:
            errorMsg = m_tcpSocket ? m_tcpSocket->errorString() : "Unknown error";
            break;
    }

    Logger::error(QString("BandwidthTester: TCP error: %1").arg(errorMsg));
    m_isRunning = false;
    emit testError(errorMsg);
}

void BandwidthTester::onTcpReadyRead()
{
    if (!m_tcpSocket || m_direction != Download) {
        return;
    }

    // Read all available data
    QByteArray data = m_tcpSocket->readAll();
    m_totalBytes += data.size();

    Logger::debug(QString("BandwidthTester: Received %1 bytes (total: %2)")
                  .arg(data.size()).arg(m_totalBytes));
}

void BandwidthTester::onTcpBytesWritten(qint64 bytes)
{
    if (m_direction != Upload) {
        return;
    }

    // Continue sending if test is still running
    if (m_isRunning && m_timer.elapsed() < m_durationMs) {
        sendData();
    }
}

void BandwidthTester::onUdpReadyRead()
{
    if (!m_udpSocket || m_direction != Download) {
        return;
    }

    while (m_udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(m_udpSocket->pendingDatagramSize());
        m_udpSocket->readDatagram(datagram.data(), datagram.size());
        m_totalBytes += datagram.size();
    }

    Logger::debug(QString("BandwidthTester: Received UDP data (total: %1 bytes)").arg(m_totalBytes));
}

void BandwidthTester::sendData()
{
    if (!m_isRunning || m_timer.elapsed() >= m_durationMs) {
        return;
    }

    if (m_protocol == TCP && m_tcpSocket && m_tcpSocket->state() == QAbstractSocket::ConnectedState) {
        qint64 written = m_tcpSocket->write(m_sendBuffer);
        if (written > 0) {
            m_totalBytes += written;
            Logger::debug(QString("BandwidthTester: Sent %1 bytes (total: %2)")
                          .arg(written).arg(m_totalBytes));
        }
    } else if (m_protocol == UDP && m_udpSocket) {
        qint64 written = m_udpSocket->writeDatagram(m_sendBuffer, QHostAddress(m_target), m_port);
        if (written > 0) {
            m_totalBytes += written;
        }

        // For UDP upload, schedule next send
        if (m_isRunning && m_timer.elapsed() < m_durationMs) {
            QTimer::singleShot(10, this, &BandwidthTester::sendData);  // Send every 10ms
        }
    }
}

void BandwidthTester::onProgressTimerTick()
{
    if (!m_isRunning) {
        return;
    }

    qint64 elapsed = m_timer.elapsed();
    int percent = qMin(100, static_cast<int>((elapsed * 100) / m_durationMs));
    double currentBandwidth = calculateBandwidth();

    emit progressUpdated(percent, currentBandwidth);
}

void BandwidthTester::onTestTimeout()
{
    Logger::info("BandwidthTester: Test duration completed");
    completeTest();
}

double BandwidthTester::calculateBandwidth() const
{
    qint64 elapsed = m_timer.elapsed();
    if (elapsed == 0) {
        return 0.0;
    }

    // Convert to Mbps: (bytes * 8 bits/byte) / (elapsed_ms / 1000) / 1000000
    double bandwidth = (m_totalBytes * 8.0) / (elapsed / 1000.0) / 1000000.0;
    return bandwidth;
}

void BandwidthTester::completeTest()
{
    if (!m_isRunning) {
        return;
    }

    m_progressTimer->stop();
    m_testTimer->stop();

    m_measuredBandwidth = calculateBandwidth();

    Logger::info(QString("BandwidthTester: Test completed - %1 Mbps (%2 bytes in %3 ms)")
                 .arg(m_measuredBandwidth, 0, 'f', 2)
                 .arg(m_totalBytes)
                 .arg(m_timer.elapsed()));

    // Clean up sockets
    if (m_tcpSocket) {
        m_tcpSocket->disconnectFromHost();
        m_tcpSocket->deleteLater();
        m_tcpSocket = nullptr;
    }

    if (m_udpSocket) {
        m_udpSocket->close();
        m_udpSocket->deleteLater();
        m_udpSocket = nullptr;
    }

    m_isRunning = false;
    emit testCompleted(m_measuredBandwidth);
}
