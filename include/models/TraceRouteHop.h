#ifndef TRACEROUTEHOP_H
#define TRACEROUTEHOP_H

#include <QString>
#include <QList>
#include <QMetaType>

/**
 * @brief Represents a single hop in a traceroute path
 *
 * This class models one hop (router/gateway) discovered during traceroute execution.
 * Each hop contains information about the IP address, hostname, and RTT (Round-Trip Time)
 * measurements for multiple probe packets sent to that hop.
 *
 * Features:
 * - Multiple RTT measurements per hop (typically 3 probes)
 * - Hostname resolution (when available)
 * - Timeout detection for unreachable hops
 * - Statistical calculations (min/max/average RTT)
 *
 * Example usage:
 * @code
 * TraceRouteHop hop(1, "192.168.1.1", "gateway.local");
 * hop.addRtt(1.5);
 * hop.addRtt(2.1);
 * hop.addRtt(1.8);
 * qDebug() << "Average RTT:" << hop.averageRtt() << "ms";
 * @endcode
 */
class TraceRouteHop
{
public:
    /**
     * @brief Default constructor
     */
    TraceRouteHop();

    /**
     * @brief Constructs a hop with hop number, IP address, and optional hostname
     * @param hopNumber The hop sequence number (1-based)
     * @param ipAddress The IP address of this hop
     * @param hostname The resolved hostname (empty if unavailable)
     */
    TraceRouteHop(int hopNumber, const QString& ipAddress, const QString& hostname = QString());

    /**
     * @brief Copy constructor
     */
    TraceRouteHop(const TraceRouteHop& other);

    /**
     * @brief Assignment operator
     */
    TraceRouteHop& operator=(const TraceRouteHop& other);

    /**
     * @brief Equality operator
     */
    bool operator==(const TraceRouteHop& other) const;

    // Getters
    int hopNumber() const { return m_hopNumber; }
    QString ipAddress() const { return m_ipAddress; }
    QString hostname() const { return m_hostname; }
    QList<double> rttList() const { return m_rttList; }
    bool isTimeout() const { return m_timeout; }

    // Setters
    void setHopNumber(int hopNumber) { m_hopNumber = hopNumber; }
    void setIpAddress(const QString& ipAddress) { m_ipAddress = ipAddress; }
    void setHostname(const QString& hostname) { m_hostname = hostname; }
    void setTimeout(bool timeout) { m_timeout = timeout; }

    /**
     * @brief Adds an RTT measurement to this hop
     * @param rtt The round-trip time in milliseconds
     */
    void addRtt(double rtt);

    /**
     * @brief Clears all RTT measurements
     */
    void clearRtt();

    /**
     * @brief Calculates the minimum RTT from all measurements
     * @return Minimum RTT in milliseconds, or 0.0 if no measurements
     */
    double minRtt() const;

    /**
     * @brief Calculates the maximum RTT from all measurements
     * @return Maximum RTT in milliseconds, or 0.0 if no measurements
     */
    double maxRtt() const;

    /**
     * @brief Calculates the average RTT from all measurements
     * @return Average RTT in milliseconds, or 0.0 if no measurements
     */
    double averageRtt() const;

    /**
     * @brief Returns a string representation suitable for display
     * @return Formatted string like "1  gateway.local (192.168.1.1)  1.5ms  2.1ms  1.8ms"
     */
    QString toString() const;

    /**
     * @brief Checks if this hop has valid RTT measurements
     * @return true if at least one RTT measurement exists
     */
    bool hasValidRtt() const { return !m_rttList.isEmpty() && !m_timeout; }

private:
    int m_hopNumber;           ///< Hop sequence number (1-based)
    QString m_ipAddress;       ///< IP address of this hop
    QString m_hostname;        ///< Resolved hostname (may be empty)
    QList<double> m_rttList;   ///< List of RTT measurements in milliseconds
    bool m_timeout;            ///< True if this hop timed out (*)
};

Q_DECLARE_METATYPE(TraceRouteHop)

#endif // TRACEROUTEHOP_H
