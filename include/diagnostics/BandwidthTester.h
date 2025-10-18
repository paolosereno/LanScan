#ifndef BANDWIDTHTESTER_H
#define BANDWIDTHTESTER_H

#include <QObject>
#include <QString>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QElapsedTimer>
#include <QTimer>

/**
 * @brief Service for testing network bandwidth using TCP/UDP data transfer
 *
 * This class provides bandwidth testing capabilities by measuring data transfer
 * rates over TCP or UDP connections. It supports both download and upload testing
 * with configurable duration and packet sizes.
 *
 * Features:
 * - TCP and UDP bandwidth testing
 * - Download and upload speed measurement
 * - Configurable test duration (1-60 seconds)
 * - Configurable packet size (1KB - 1MB)
 * - Real-time progress updates
 * - Results in Mbps (Megabits per second)
 *
 * Test Process:
 * 1. Connect to target server on specified port
 * 2. Send/receive data for specified duration
 * 3. Calculate throughput based on bytes transferred
 * 4. Report bandwidth in Mbps
 *
 * Example usage:
 * @code
 * BandwidthTester* tester = new BandwidthTester(this);
 * connect(tester, &BandwidthTester::testCompleted, [](double bandwidth) {
 *     qDebug() << "Bandwidth:" << bandwidth << "Mbps";
 * });
 * connect(tester, &BandwidthTester::progressUpdated, [](int percent, double currentBandwidth) {
 *     qDebug() << percent << "% -" << currentBandwidth << "Mbps";
 * });
 * tester->testDownloadSpeed("192.168.1.1", 8080, 10); // 10 second test
 * @endcode
 */
class BandwidthTester : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Protocol type for bandwidth testing
     */
    enum Protocol {
        TCP,    ///< Use TCP for reliable connection-oriented testing
        UDP     ///< Use UDP for connectionless testing
    };

    /**
     * @brief Test direction
     */
    enum Direction {
        Download,   ///< Test download speed (receive data)
        Upload      ///< Test upload speed (send data)
    };

    /**
     * @brief Protocol state for LanScan bandwidth test protocol
     */
    enum ProtocolState {
        Idle,                       ///< No test running
        Connecting,                 ///< Connecting to server
        WaitingHandshakeResponse,   ///< Waiting for server OK/ERROR response
        DataTransfer,               ///< Transferring data
        WaitingResults,             ///< Waiting for server results
        Completed,                  ///< Test completed successfully
        Error                       ///< Error state
    };

    /**
     * @brief Constructs a BandwidthTester service
     * @param parent The parent QObject
     */
    explicit BandwidthTester(QObject* parent = nullptr);

    /**
     * @brief Destructor
     */
    ~BandwidthTester();

    /**
     * @brief Tests download speed from a target server
     * @param target IP address or hostname of test server
     * @param port Port number for connection
     * @param durationSeconds Test duration in seconds (1-60)
     * @param protocol Protocol to use (TCP or UDP)
     * @return true if test started successfully
     */
    bool testDownloadSpeed(const QString& target, quint16 port, int durationSeconds = 10, Protocol protocol = TCP);

    /**
     * @brief Tests upload speed to a target server
     * @param target IP address or hostname of test server
     * @param port Port number for connection
     * @param durationSeconds Test duration in seconds (1-60)
     * @param protocol Protocol to use (TCP or UDP)
     * @return true if test started successfully
     */
    bool testUploadSpeed(const QString& target, quint16 port, int durationSeconds = 10, Protocol protocol = TCP);

    /**
     * @brief Cancels the currently running bandwidth test
     */
    void cancel();

    /**
     * @brief Checks if a bandwidth test is currently running
     * @return true if test is in progress
     */
    bool isRunning() const { return m_isRunning; }

    /**
     * @brief Returns the measured bandwidth (valid after test completes)
     * @return Bandwidth in Mbps, or 0.0 if not measured yet
     */
    double measuredBandwidth() const { return m_measuredBandwidth; }

    /**
     * @brief Sets the packet size for testing
     * @param size Packet size in bytes (1024 - 1048576)
     */
    void setPacketSize(int size);

    /**
     * @brief Returns the current packet size
     * @return Packet size in bytes
     */
    int packetSize() const { return m_packetSize; }

signals:
    /**
     * @brief Emitted when bandwidth test completes successfully
     * @param bandwidth Measured bandwidth in Mbps
     */
    void testCompleted(double bandwidth);

    /**
     * @brief Emitted to report progress during test
     * @param percentComplete Percentage of test completed (0-100)
     * @param currentBandwidth Current measured bandwidth in Mbps
     */
    void progressUpdated(int percentComplete, double currentBandwidth);

    /**
     * @brief Emitted when an error occurs
     * @param error Error message
     */
    void testError(const QString& error);

private slots:
    /**
     * @brief Handles TCP connection established
     */
    void onTcpConnected();

    /**
     * @brief Handles TCP socket errors
     * @param error Socket error type
     */
    void onTcpError(QAbstractSocket::SocketError error);

    /**
     * @brief Handles incoming data on TCP socket
     */
    void onTcpReadyRead();

    /**
     * @brief Handles data written to TCP socket
     * @param bytes Number of bytes written
     */
    void onTcpBytesWritten(qint64 bytes);

    /**
     * @brief Handles incoming data on UDP socket
     */
    void onUdpReadyRead();

    /**
     * @brief Handles progress timer tick
     */
    void onProgressTimerTick();

    /**
     * @brief Handles test duration timeout
     */
    void onTestTimeout();

private:
    /**
     * @brief Starts a bandwidth test
     * @param target Target address
     * @param port Target port
     * @param duration Duration in seconds
     * @param direction Download or Upload
     * @param protocol TCP or UDP
     * @return true if test started successfully
     */
    bool startTest(const QString& target, quint16 port, int duration, Direction direction, Protocol protocol);

    /**
     * @brief Sends data for upload testing
     */
    void sendData();

    /**
     * @brief Calculates current bandwidth
     * @return Bandwidth in Mbps
     */
    double calculateBandwidth() const;

    /**
     * @brief Completes the test and emits results
     */
    void completeTest();

    /**
     * @brief Generates the LanScan protocol handshake message
     * @return Handshake message in protocol format
     */
    QByteArray generateHandshake() const;

    /**
     * @brief Parses the server's handshake response
     * @param data Response data from server
     * @return true if server responded OK, false on error
     */
    bool parseHandshakeResponse(const QByteArray& data);

    /**
     * @brief Parses the server's results message
     * @param data Results data from server
     * @return true if parsing successful
     */
    bool parseResults(const QByteArray& data);

    QTcpSocket* m_tcpSocket;          ///< TCP socket for testing
    QUdpSocket* m_udpSocket;          ///< UDP socket for testing
    QElapsedTimer m_timer;            ///< Timer for measuring duration
    QTimer* m_progressTimer;          ///< Timer for progress updates
    QTimer* m_testTimer;              ///< Timer for test duration

    QString m_target;                 ///< Target being tested
    quint16 m_port;                   ///< Target port
    int m_durationMs;                 ///< Test duration in milliseconds
    Direction m_direction;            ///< Test direction (download/upload)
    Protocol m_protocol;              ///< Protocol being used

    qint64 m_totalBytes;              ///< Total bytes transferred
    int m_packetSize;                 ///< Size of packets in bytes
    QByteArray m_sendBuffer;          ///< Buffer for sending data

    double m_measuredBandwidth;       ///< Final measured bandwidth
    bool m_isRunning;                 ///< Test in progress flag

    // Protocol state management
    ProtocolState m_protocolState;    ///< Current protocol state
    QByteArray m_receiveBuffer;       ///< Buffer for receiving protocol messages
};

#endif // BANDWIDTHTESTER_H
