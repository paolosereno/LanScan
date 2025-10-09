#ifndef TRACEROUTESERVICE_H
#define TRACEROUTESERVICE_H

#include <QObject>
#include <QString>
#include <QProcess>
#include <QList>
#include "models/TraceRouteHop.h"

/**
 * @brief Service for executing traceroute operations across different platforms
 *
 * This class provides traceroute functionality with cross-platform support:
 * - Windows: Uses 'tracert' command
 * - Linux/macOS: Uses 'traceroute' command
 *
 * Features:
 * - Asynchronous traceroute execution with real-time hop discovery
 * - Platform-specific command parsing (Windows/Unix formats)
 * - Configurable maximum hops and timeout
 * - Qt signals for progress updates
 * - Support for both IP addresses and hostnames
 *
 * Signals emitted during execution:
 * - hopDiscovered(TraceRouteHop): Emitted for each hop discovered
 * - traceCompleted(QList<TraceRouteHop>): Emitted when trace finishes
 * - traceError(QString): Emitted on errors
 *
 * Example usage:
 * @code
 * TraceRouteService service;
 * connect(&service, &TraceRouteService::hopDiscovered, [](const TraceRouteHop& hop) {
 *     qDebug() << hop.toString();
 * });
 * service.traceRoute("8.8.8.8", 30);
 * @endcode
 */
class TraceRouteService : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a TraceRouteService
     * @param parent The parent QObject
     */
    explicit TraceRouteService(QObject* parent = nullptr);

    /**
     * @brief Destructor
     */
    ~TraceRouteService();

    /**
     * @brief Starts a traceroute to the specified target
     * @param target IP address or hostname to trace
     * @param maxHops Maximum number of hops (default: 30)
     * @param timeout Timeout per hop in milliseconds (default: 5000ms)
     * @return true if traceroute started successfully
     */
    bool traceRoute(const QString& target, int maxHops = 30, int timeout = 5000);

    /**
     * @brief Cancels the currently running traceroute
     */
    void cancel();

    /**
     * @brief Checks if a traceroute is currently running
     * @return true if traceroute is in progress
     */
    bool isRunning() const { return m_process && m_process->state() == QProcess::Running; }

    /**
     * @brief Returns the list of discovered hops (so far or from completed trace)
     * @return List of TraceRouteHop objects
     */
    QList<TraceRouteHop> hops() const { return m_hops; }

    /**
     * @brief Returns the target being traced
     * @return Target IP or hostname
     */
    QString target() const { return m_target; }

signals:
    /**
     * @brief Emitted when a new hop is discovered
     * @param hop The discovered hop information
     */
    void hopDiscovered(const TraceRouteHop& hop);

    /**
     * @brief Emitted when the traceroute completes successfully
     * @param hops Complete list of all discovered hops
     */
    void traceCompleted(const QList<TraceRouteHop>& hops);

    /**
     * @brief Emitted when an error occurs
     * @param error Error message
     */
    void traceError(const QString& error);

    /**
     * @brief Emitted to report progress (hop number out of max hops)
     * @param currentHop Current hop number
     * @param maxHops Maximum hops configured
     */
    void progressUpdated(int currentHop, int maxHops);

private slots:
    /**
     * @brief Handles process output (stdout)
     */
    void onReadyReadStandardOutput();

    /**
     * @brief Handles process errors (stderr)
     */
    void onReadyReadStandardError();

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
     * @brief Parses a line of traceroute output (Windows format)
     * @param line Output line to parse
     * @return Parsed hop or invalid hop if parsing fails
     */
    TraceRouteHop parseWindowsLine(const QString& line);

    /**
     * @brief Parses a line of traceroute output (Unix format)
     * @param line Output line to parse
     * @return Parsed hop or invalid hop if parsing fails
     */
    TraceRouteHop parseUnixLine(const QString& line);

    /**
     * @brief Detects the platform and returns the appropriate command
     * @param target Target IP or hostname
     * @param maxHops Maximum hops
     * @param timeout Timeout in milliseconds
     * @return Command and arguments list
     */
    QStringList buildTraceCommand(const QString& target, int maxHops, int timeout);

    /**
     * @brief Extracts IP address from a string (handles formats like "host (ip)" or "ip")
     * @param text Input text
     * @return Extracted IP address or empty string
     */
    QString extractIpAddress(const QString& text) const;

    /**
     * @brief Extracts hostname from a string (handles formats like "host (ip)")
     * @param text Input text
     * @return Extracted hostname or empty string
     */
    QString extractHostname(const QString& text) const;

    QProcess* m_process;              ///< Process for executing traceroute command
    QList<TraceRouteHop> m_hops;      ///< Discovered hops
    QString m_target;                 ///< Target being traced
    int m_maxHops;                    ///< Maximum hops configured
    QString m_outputBuffer;           ///< Buffer for incomplete output lines
    int m_currentHop;                 ///< Current hop being processed
};

#endif // TRACEROUTESERVICE_H
