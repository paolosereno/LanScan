#ifndef MTUDISCOVERY_H
#define MTUDISCOVERY_H

#include <QObject>
#include <QString>
#include <QProcess>

/**
 * @brief Service for discovering the Path MTU (Maximum Transmission Unit)
 *
 * This class implements Path MTU Discovery using ICMP echo requests with
 * the Don't Fragment (DF) flag set. It uses binary search to efficiently
 * find the maximum packet size that can traverse the path without fragmentation.
 *
 * Features:
 * - Binary search algorithm (576 - 9000 bytes range)
 * - Cross-platform support (Windows ping -f / Linux ping -M do)
 * - Progress tracking with signals
 * - Asynchronous operation with QProcess
 * - Detection of "packet needs to be fragmented" errors
 *
 * MTU Discovery Process:
 * 1. Start with range [minMtu, maxMtu] (default: 576 to 9000)
 * 2. Test midpoint with ping + DF flag
 * 3. If successful: MTU >= midpoint, search upper half
 * 4. If fragmentation needed: MTU < midpoint, search lower half
 * 5. Repeat until range converges
 *
 * Example usage:
 * @code
 * MtuDiscovery* discovery = new MtuDiscovery(this);
 * connect(discovery, &MtuDiscovery::mtuDiscovered, [](int mtu) {
 *     qDebug() << "Path MTU:" << mtu << "bytes";
 * });
 * connect(discovery, &MtuDiscovery::progressUpdated, [](int current, int min, int max) {
 *     qDebug() << QString("Testing MTU %1 (range: %2-%3)").arg(current).arg(min).arg(max);
 * });
 * discovery->discoverMtu("8.8.8.8");
 * @endcode
 */
class MtuDiscovery : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructs an MtuDiscovery service
     * @param parent The parent QObject
     */
    explicit MtuDiscovery(QObject* parent = nullptr);

    /**
     * @brief Destructor
     */
    ~MtuDiscovery();

    /**
     * @brief Starts MTU discovery for the specified target
     * @param target IP address or hostname to test
     * @param minMtu Minimum MTU to test (default: 576 bytes, IPv4 minimum)
     * @param maxMtu Maximum MTU to test (default: 9000 bytes, jumbo frames)
     * @return true if discovery started successfully
     */
    bool discoverMtu(const QString& target, int minMtu = 576, int maxMtu = 9000);

    /**
     * @brief Cancels the currently running MTU discovery
     */
    void cancel();

    /**
     * @brief Checks if MTU discovery is currently running
     * @return true if discovery is in progress
     */
    bool isRunning() const { return m_isRunning; }

    /**
     * @brief Returns the discovered MTU (valid after discovery completes)
     * @return Discovered MTU in bytes, or 0 if not discovered yet
     */
    int discoveredMtu() const { return m_discoveredMtu; }

    /**
     * @brief Returns the target being tested
     * @return Target IP or hostname
     */
    QString target() const { return m_target; }

signals:
    /**
     * @brief Emitted when MTU discovery completes successfully
     * @param mtu The discovered path MTU in bytes
     */
    void mtuDiscovered(int mtu);

    /**
     * @brief Emitted to report progress during discovery
     * @param currentMtu Current MTU being tested
     * @param minMtu Current minimum MTU in range
     * @param maxMtu Current maximum MTU in range
     */
    void progressUpdated(int currentMtu, int minMtu, int maxMtu);

    /**
     * @brief Emitted when an error occurs
     * @param error Error message
     */
    void discoveryError(const QString& error);

private slots:
    /**
     * @brief Handles process completion
     * @param exitCode Process exit code
     * @param exitStatus Process exit status
     */
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

    /**
     * @brief Handles process errors
     * @param error Process error type
     */
    void onProcessError(QProcess::ProcessError error);

private:
    /**
     * @brief Performs binary search step for MTU discovery
     */
    void performBinarySearchStep();

    /**
     * @brief Tests a specific MTU size with ping + DF flag
     * @param mtuSize MTU size to test in bytes
     * @return true if test initiated successfully
     */
    bool testMtuSize(int mtuSize);

    /**
     * @brief Builds platform-specific ping command with DF flag
     * @param target Target IP or hostname
     * @param packetSize ICMP data size (MTU - IP header - ICMP header)
     * @return Command and arguments list
     */
    QStringList buildPingCommand(const QString& target, int packetSize);

    /**
     * @brief Analyzes ping output to determine if MTU test succeeded
     * @param output Ping command output
     * @param exitCode Process exit code
     * @return true if MTU size is acceptable (no fragmentation needed)
     */
    bool analyzePingResult(const QString& output, int exitCode);

    QProcess* m_process;              ///< Process for ping command
    QString m_target;                 ///< Target being tested
    int m_minMtu;                     ///< Current minimum MTU in binary search
    int m_maxMtu;                     ///< Current maximum MTU in binary search
    int m_currentMtu;                 ///< Current MTU being tested
    int m_discoveredMtu;              ///< Final discovered MTU
    bool m_isRunning;                 ///< Discovery in progress flag
    QString m_outputBuffer;           ///< Buffer for ping output
};

#endif // MTUDISCOVERY_H
