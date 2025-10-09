#ifndef DNSDIAGNOSTICS_H
#define DNSDIAGNOSTICS_H

#include <QObject>
#include <QString>
#include <QList>
#include <QHostInfo>
#include <QDnsLookup>
#include <QElapsedTimer>

/**
 * @brief Service for advanced DNS diagnostics and queries
 *
 * This class provides comprehensive DNS diagnostic capabilities including
 * forward lookups, reverse lookups, DNS record queries (A, AAAA, MX, TXT, etc.),
 * and DNS server resolution time measurements.
 *
 * Features:
 * - Multiple DNS record type queries (A, AAAA, MX, NS, TXT, CNAME, PTR, SOA, SRV)
 * - Forward and reverse DNS lookups
 * - Custom DNS server support
 * - Query time measurement
 * - Multiple result handling
 * - DNSSEC-aware queries
 *
 * DNS Query Types:
 * - A: IPv4 address records
 * - AAAA: IPv6 address records
 * - MX: Mail exchange records
 * - NS: Name server records
 * - TXT: Text records
 * - CNAME: Canonical name records
 * - PTR: Pointer records (reverse DNS)
 * - SOA: Start of authority records
 * - SRV: Service records
 *
 * Example usage:
 * @code
 * DnsDiagnostics* dns = new DnsDiagnostics(this);
 * connect(dns, &DnsDiagnostics::lookupCompleted, [](const QList<QString>& results) {
 *     for (const QString& result : results) {
 *         qDebug() << "DNS result:" << result;
 *     }
 * });
 * dns->queryRecords("google.com", DnsDiagnostics::A);
 * @endcode
 */
class DnsDiagnostics : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief DNS record types
     */
    enum RecordType {
        A,          ///< IPv4 address record
        AAAA,       ///< IPv6 address record
        MX,         ///< Mail exchange record
        NS,         ///< Name server record
        TXT,        ///< Text record
        CNAME,      ///< Canonical name record
        PTR,        ///< Pointer record (reverse DNS)
        SOA,        ///< Start of authority record
        SRV,        ///< Service record
        ANY         ///< All available records
    };

    /**
     * @brief DNS query result
     */
    struct DnsRecord {
        RecordType type;        ///< Record type
        QString name;           ///< Record name
        QString value;          ///< Record value
        quint32 ttl;            ///< Time to live in seconds
        quint16 priority;       ///< Priority (for MX, SRV records)

        /**
         * @brief Returns formatted string representation
         * @return Formatted record string
         */
        QString toString() const;
    };

    /**
     * @brief Constructs a DnsDiagnostics service
     * @param parent The parent QObject
     */
    explicit DnsDiagnostics(QObject* parent = nullptr);

    /**
     * @brief Destructor
     */
    ~DnsDiagnostics();

    /**
     * @brief Performs DNS query for specified record type
     * @param hostname Hostname to query
     * @param type DNS record type to query
     * @param nameserver Optional custom DNS server (e.g., "8.8.8.8")
     * @return true if query started successfully
     */
    bool queryRecords(const QString& hostname, RecordType type, const QString& nameserver = QString());

    /**
     * @brief Performs forward DNS lookup (hostname to IP)
     * @param hostname Hostname to resolve
     * @param nameserver Optional custom DNS server
     * @return true if lookup started successfully
     */
    bool forwardLookup(const QString& hostname, const QString& nameserver = QString());

    /**
     * @brief Performs reverse DNS lookup (IP to hostname)
     * @param ipAddress IP address to resolve
     * @param nameserver Optional custom DNS server
     * @return true if lookup started successfully
     */
    bool reverseLookup(const QString& ipAddress, const QString& nameserver = QString());

    /**
     * @brief Cancels the currently running DNS query
     */
    void cancel();

    /**
     * @brief Checks if a DNS query is currently running
     * @return true if query is in progress
     */
    bool isRunning() const { return m_isRunning; }

    /**
     * @brief Returns the last query resolution time
     * @return Query time in milliseconds, or 0 if not measured
     */
    qint64 queryTime() const { return m_queryTime; }

    /**
     * @brief Returns the hostname being queried
     * @return Hostname or empty if no query running
     */
    QString hostname() const { return m_hostname; }

    /**
     * @brief Returns the DNS server being used
     * @return DNS server address or empty for system default
     */
    QString nameserver() const { return m_nameserver; }

    /**
     * @brief Converts RecordType to QDnsLookup::Type
     * @param type RecordType to convert
     * @return Corresponding QDnsLookup::Type
     */
    static QDnsLookup::Type toQDnsLookupType(RecordType type);

    /**
     * @brief Converts RecordType to string
     * @param type RecordType to convert
     * @return String representation (e.g., "A", "AAAA", "MX")
     */
    static QString recordTypeToString(RecordType type);

signals:
    /**
     * @brief Emitted when DNS lookup completes successfully
     * @param records List of DNS records found
     */
    void lookupCompleted(const QList<DnsDiagnostics::DnsRecord>& records);

    /**
     * @brief Emitted to report query progress
     * @param message Progress message
     */
    void progressUpdated(const QString& message);

    /**
     * @brief Emitted when an error occurs
     * @param error Error message
     */
    void lookupError(const QString& error);

private slots:
    /**
     * @brief Handles DNS lookup completion
     */
    void onLookupFinished();

private:
    /**
     * @brief Starts a DNS lookup
     * @param hostname Hostname to query
     * @param type Record type
     * @param nameserver Custom DNS server
     * @return true if started successfully
     */
    bool startLookup(const QString& hostname, RecordType type, const QString& nameserver);

    /**
     * @brief Processes DNS lookup results
     */
    void processResults();

    /**
     * @brief Creates DNS record from QDnsLookup results
     * @return List of DNS records
     */
    QList<DnsRecord> extractRecords();

    QDnsLookup* m_dnsLookup;          ///< DNS lookup object
    QElapsedTimer m_timer;             ///< Timer for measuring query time

    QString m_hostname;                ///< Hostname being queried
    QString m_nameserver;              ///< Custom DNS server (if any)
    RecordType m_recordType;           ///< Record type being queried
    qint64 m_queryTime;                ///< Last query time in milliseconds
    bool m_isRunning;                  ///< Query in progress flag
};

#endif // DNSDIAGNOSTICS_H
