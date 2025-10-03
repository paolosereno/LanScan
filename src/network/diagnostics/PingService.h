#ifndef PINGSERVICE_H
#define PINGSERVICE_H

#include <QObject>
#include <QProcess>
#include <QString>
#include <QVector>
#include <QTimer>

/**
 * @brief Service for executing ping operations
 *
 * Provides both synchronous and asynchronous ping functionality
 * with cross-platform support for Windows, Linux, and macOS.
 */
class PingService : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Result of a single ping operation
     */
    struct PingResult {
        QString host;           ///< Target host (IP or hostname)
        double latency;         ///< Round-trip time in milliseconds
        int ttl;               ///< Time To Live value
        int bytes;             ///< Bytes in the ICMP packet
        bool success;          ///< Whether ping succeeded
        QString errorMessage;  ///< Error message if failed

        PingResult()
            : latency(0.0), ttl(0), bytes(0), success(false) {}
    };

    explicit PingService(QObject* parent = nullptr);
    ~PingService();

    /**
     * @brief Execute asynchronous ping with multiple packets
     * @param host Target IP address or hostname
     * @param count Number of ping packets to send (default: 4)
     */
    void ping(const QString& host, int count = 4);

    /**
     * @brief Execute synchronous ping (blocking)
     * @param host Target IP address or hostname
     * @param timeout Timeout in milliseconds (default: 1000)
     * @return PingResult containing the ping outcome
     */
    PingResult pingSync(const QString& host, int timeout = 1000);

    /**
     * @brief Start continuous ping at regular intervals
     * @param host Target IP address or hostname
     * @param interval Interval between pings in milliseconds (default: 1000)
     */
    void continuousPing(const QString& host, int interval = 1000);

    /**
     * @brief Stop continuous ping operation
     */
    void stopContinuousPing();

    /**
     * @brief Check if continuous ping is running
     * @return True if continuous ping is active
     */
    bool isContinuousPingActive() const;

signals:
    /**
     * @brief Emitted when a single ping result is available
     * @param result The ping result
     */
    void pingResult(const PingResult& result);

    /**
     * @brief Emitted when a ping operation completes (all packets sent)
     * @param results Vector of all ping results
     */
    void pingCompleted(const QVector<PingResult>& results);

    /**
     * @brief Emitted when an error occurs
     * @param error Error message
     */
    void errorOccurred(const QString& error);

private slots:
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessError(QProcess::ProcessError error);
    void onContinuousPingTimeout();

private:
    QProcess* pingProcess;
    QTimer* continuousTimer;
    QString currentHost;
    int currentCount;
    QVector<PingResult> currentResults;
    bool isContinuous;

    /**
     * @brief Build platform-specific ping command
     * @param host Target host
     * @param count Number of pings
     * @return Command string with arguments
     */
    QStringList buildPingCommand(const QString& host, int count);

    /**
     * @brief Parse ping output based on platform
     * @param output Ping command output
     * @return Vector of parsed ping results
     */
    QVector<PingResult> parsePingOutput(const QString& output);

    /**
     * @brief Parse Windows ping output
     * @param line Single line of ping output
     * @return Parsed ping result
     */
    PingResult parseWindowsPing(const QString& line);

    /**
     * @brief Parse Unix (Linux/macOS) ping output
     * @param line Single line of ping output
     * @return Parsed ping result
     */
    PingResult parseUnixPing(const QString& line);

    /**
     * @brief Detect current platform
     * @return Platform identifier: "windows", "linux", or "macos"
     */
    QString detectPlatform() const;
};

#endif // PINGSERVICE_H
