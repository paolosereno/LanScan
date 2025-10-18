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
    , m_protocolState(Idle)
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
    m_protocolState = Connecting;
    m_receiveBuffer.clear();

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
    Logger::debug("BandwidthTester: TCP connected, sending handshake");

    // Send protocol handshake
    QByteArray handshake = generateHandshake();
    m_tcpSocket->write(handshake);
    m_tcpSocket->flush();

    m_protocolState = WaitingHandshakeResponse;
}

void BandwidthTester::onTcpError(QAbstractSocket::SocketError error)
{
    Logger::debug(QString("BandwidthTester: onTcpError called - error=%1, state=%2, socketState=%3")
                 .arg(error)
                 .arg(m_protocolState)
                 .arg(m_tcpSocket ? m_tcpSocket->state() : -1));

    // If we're waiting for results and get RemoteHostClosedError, check if there's data to read first
    if (error == QAbstractSocket::RemoteHostClosedError &&
        (m_protocolState == WaitingResults || m_protocolState == DataTransfer)) {
        Logger::debug("BandwidthTester: Remote host closed, checking for pending data");

        // Check if there's still data available to read
        if (m_tcpSocket && m_tcpSocket->bytesAvailable() > 0) {
            Logger::debug(QString("BandwidthTester: %1 bytes still available, processing them")
                         .arg(m_tcpSocket->bytesAvailable()));
            // Let the readyRead handler process the remaining data
            onTcpReadyRead();

            // If we completed successfully, don't treat this as an error
            if (m_protocolState == Completed) {
                Logger::debug("BandwidthTester: Successfully processed remaining data after remote close");
                return;
            }
        }

        // If we're in WaitingResults state, don't treat remote close as an error yet
        // Let the timeout handle it if results don't arrive
        if (m_protocolState == WaitingResults) {
            Logger::debug("BandwidthTester: Remote closed while waiting for results, relying on timeout");
            return;
        }
    }

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
    m_protocolState = Error;
    emit testError(errorMsg);
}

void BandwidthTester::onTcpReadyRead()
{
    if (!m_tcpSocket) {
        return;
    }

    if (m_protocolState == WaitingHandshakeResponse) {
        // Accumulate data in receive buffer
        m_receiveBuffer.append(m_tcpSocket->readAll());

        // Check if we have complete response (ends with "READY\n" for OK or contains "ERROR:" for error)
        if (m_receiveBuffer.contains("READY\n") || m_receiveBuffer.contains("LANSCAN_BW_ERROR")) {
            if (parseHandshakeResponse(m_receiveBuffer)) {
                Logger::debug("BandwidthTester: Handshake successful, starting data transfer");
                m_protocolState = DataTransfer;
                m_receiveBuffer.clear();

                // Reset timer for data transfer phase
                m_timer.restart();

                // Start data transfer
                if (m_direction == Upload) {
                    sendData();
                }
                // For download, just wait for server to send data
            } else {
                // Error in handshake, cancel test
                cancel();
            }
        }
    }
    else if (m_protocolState == DataTransfer) {
        if (m_direction == Download) {
            // Read all available data
            QByteArray data = m_tcpSocket->readAll();

            // Check if data contains results marker
            if (data.contains("LANSCAN_BW_RESULTS")) {
                // Split data and results
                int resultsStart = data.indexOf("LANSCAN_BW_RESULTS");
                QByteArray testData = data.left(resultsStart);
                QByteArray resultsData = data.mid(resultsStart);

                // Count only test data
                m_totalBytes += testData.size();

                Logger::debug(QString("BandwidthTester: Received %1 bytes + results message (total: %2)")
                              .arg(testData.size()).arg(m_totalBytes));

                // Process results immediately
                m_receiveBuffer = resultsData;
                m_protocolState = WaitingResults;

                // Process the results
                if (m_receiveBuffer.contains("END\n")) {
                    if (parseResults(m_receiveBuffer)) {
                        Logger::debug("BandwidthTester: Results parsed successfully");
                    }
                    m_protocolState = Completed;
                    completeTest();
                }
            } else {
                m_totalBytes += data.size();
                Logger::debug(QString("BandwidthTester: Received %1 bytes (total: %2)")
                              .arg(data.size()).arg(m_totalBytes));
            }
        }
    }
    else if (m_protocolState == WaitingResults) {
        // Read new data
        QByteArray newData = m_tcpSocket->readAll();
        Logger::debug(QString("BandwidthTester: WaitingResults - received %1 bytes").arg(newData.size()));
        m_receiveBuffer.append(newData);

        // Look for the results marker
        if (m_receiveBuffer.contains("LANSCAN_BW_RESULTS")) {
            // Found results marker, extract results message
            int resultsStart = m_receiveBuffer.indexOf("LANSCAN_BW_RESULTS");
            QByteArray resultsMessage = m_receiveBuffer.mid(resultsStart);

            Logger::debug(QString("BandwidthTester: Found results marker at position %1 (discarded %2 bytes of test data)")
                         .arg(resultsStart).arg(resultsStart));

            // Check if we have complete results (ends with "END\n")
            if (resultsMessage.contains("END\n")) {
                Logger::debug(QString("BandwidthTester: Received complete results message (%1 bytes)")
                             .arg(resultsMessage.size()));
                if (parseResults(resultsMessage)) {
                    Logger::info("BandwidthTester: Results parsed successfully");
                }
                m_protocolState = Completed;
                completeTest();
            } else {
                Logger::debug(QString("BandwidthTester: Partial results message, waiting for END (results size: %1)")
                             .arg(resultsMessage.size()));
                // Keep only the results message
                m_receiveBuffer = resultsMessage;
            }
        } else {
            // No results marker yet, keep accumulating (but log if buffer gets too large)
            if (m_receiveBuffer.size() > 1048576) { // > 1MB
                Logger::warn(QString("BandwidthTester: Still no results marker after receiving %1 bytes")
                            .arg(m_receiveBuffer.size()));
            }
        }
    }
}

void BandwidthTester::onTcpBytesWritten(qint64 bytes)
{
    if (m_direction != Upload || m_protocolState != DataTransfer) {
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
    if (m_protocolState == DataTransfer && m_protocol == TCP) {
        // Test duration completed, now wait for server results asynchronously
        Logger::info("BandwidthTester: Test duration completed, waiting for server results");
        m_protocolState = WaitingResults;
        m_receiveBuffer.clear();

        if (m_tcpSocket && m_tcpSocket->state() == QAbstractSocket::ConnectedState) {
            // Check if results are already available in the buffer
            qint64 available = m_tcpSocket->bytesAvailable();
            Logger::debug(QString("BandwidthTester: Socket state=%1, bytesAvailable=%2")
                         .arg(m_tcpSocket->state()).arg(available));

            if (available > 0) {
                Logger::debug(QString("BandwidthTester: %1 bytes already available, processing now")
                             .arg(available));
                // Trigger readyRead handler to process available data
                onTcpReadyRead();

                // If we successfully completed, return
                if (m_protocolState == Completed) {
                    return;
                }
            }

            // Start a long timeout timer (30 seconds) to wait for results
            // The onTcpReadyRead() signal will process results when they arrive
            Logger::debug("BandwidthTester: Waiting asynchronously for server results (30 second timeout)");
            m_testTimer->start(30000);
        } else {
            Logger::warn(QString("BandwidthTester: Socket not connected (state=%1), completing without results")
                        .arg(m_tcpSocket ? m_tcpSocket->state() : -1));
            completeTest();
        }
        return;
    }
    else if (m_protocolState == WaitingResults) {
        // Timeout while waiting for results
        Logger::error("BandwidthTester: Timeout waiting for server results after 30 seconds");
        completeTest();
        return;
    }

    // For UDP or other cases, complete immediately
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

    // Use measured bandwidth if not already set by server results
    if (m_measuredBandwidth == 0.0) {
        m_measuredBandwidth = calculateBandwidth();
    }

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
    m_protocolState = Idle;
    emit testCompleted(m_measuredBandwidth);
}

QByteArray BandwidthTester::generateHandshake() const
{
    QString handshake = QString("LANSCAN_BW_TEST\n"
                               "VERSION:1.0\n"
                               "PROTOCOL:%1\n"
                               "DIRECTION:%2\n"
                               "DURATION:%3\n"
                               "PACKET_SIZE:%4\n"
                               "END\n")
                        .arg(m_protocol == TCP ? "TCP" : "UDP")
                        .arg(m_direction == Download ? "DOWNLOAD" : "UPLOAD")
                        .arg(m_durationMs / 1000)
                        .arg(m_packetSize);

    return handshake.toUtf8();
}

bool BandwidthTester::parseHandshakeResponse(const QByteArray& data)
{
    QString response = QString::fromUtf8(data);
    Logger::debug(QString("BandwidthTester: Received handshake response (%1 bytes): %2")
                  .arg(data.size()).arg(response.left(100)));

    QStringList lines = response.split('\n', Qt::SkipEmptyParts);

    if (lines.isEmpty()) {
        Logger::error("BandwidthTester: Empty handshake response");
        return false;
    }

    Logger::debug(QString("BandwidthTester: First line: '%1'").arg(lines.first()));

    // Check for OK or ERROR response
    if (lines.first() == "LANSCAN_BW_OK") {
        Logger::debug("BandwidthTester: Handshake OK received");
        return true;
    }
    else if (lines.first() == "LANSCAN_BW_ERROR") {
        // Parse error message
        QString errorMsg = "Unknown error";
        for (const QString& line : lines) {
            if (line.startsWith("ERROR:")) {
                errorMsg = line.mid(6).trimmed();
                break;
            }
        }
        Logger::error(QString("BandwidthTester: Server error: %1").arg(errorMsg));
        emit testError(QString("Server error: %1").arg(errorMsg));
        return false;
    }
    else {
        Logger::error(QString("BandwidthTester: Invalid handshake response: %1").arg(lines.first()));
        return false;
    }
}

bool BandwidthTester::parseResults(const QByteArray& data)
{
    QString response = QString::fromUtf8(data);
    Logger::debug(QString("BandwidthTester: Received results (%1 bytes): %2")
                  .arg(data.size()).arg(response.left(200)));

    QStringList lines = response.split('\n', Qt::SkipEmptyParts);

    if (lines.isEmpty() || lines.first() != "LANSCAN_BW_RESULTS") {
        Logger::error(QString("BandwidthTester: Invalid results message. First line: %1")
                      .arg(lines.isEmpty() ? "(empty)" : lines.first()));
        return false;
    }

    // Parse result fields
    qint64 bytes = 0;
    double throughputMbps = 0.0;
    qint64 durationMs = 0;

    for (const QString& line : lines) {
        if (line.startsWith("BYTES:")) {
            bytes = line.mid(6).trimmed().toLongLong();
        }
        else if (line.startsWith("THROUGHPUT_MBPS:")) {
            throughputMbps = line.mid(16).trimmed().toDouble();
        }
        else if (line.startsWith("DURATION_MS:")) {
            durationMs = line.mid(12).trimmed().toLongLong();
        }
    }

    Logger::info(QString("BandwidthTester: Server results - %1 bytes, %2 Mbps, %3 ms")
                 .arg(bytes).arg(throughputMbps, 0, 'f', 2).arg(durationMs));

    // Use server's calculated throughput
    m_measuredBandwidth = throughputMbps;

    return true;
}
