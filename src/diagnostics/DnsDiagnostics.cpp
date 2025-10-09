#include "diagnostics/DnsDiagnostics.h"
#include "utils/Logger.h"
#include <QHostAddress>

QString DnsDiagnostics::DnsRecord::toString() const
{
    QString result = QString("%1: %2 = %3 (TTL: %4s)")
                         .arg(DnsDiagnostics::recordTypeToString(type))
                         .arg(name)
                         .arg(value)
                         .arg(ttl);

    if (type == MX || type == SRV) {
        result += QString(" [Priority: %1]").arg(priority);
    }

    return result;
}

DnsDiagnostics::DnsDiagnostics(QObject* parent)
    : QObject(parent)
    , m_dnsLookup(nullptr)
    , m_hostname("")
    , m_nameserver("")
    , m_recordType(A)
    , m_queryTime(0)
    , m_isRunning(false)
{
}

DnsDiagnostics::~DnsDiagnostics()
{
    cancel();
}

bool DnsDiagnostics::queryRecords(const QString& hostname, RecordType type, const QString& nameserver)
{
    if (hostname.isEmpty()) {
        Logger::error("DnsDiagnostics: Cannot query: hostname is empty");
        return false;
    }

    return startLookup(hostname, type, nameserver);
}

bool DnsDiagnostics::forwardLookup(const QString& hostname, const QString& nameserver)
{
    return queryRecords(hostname, A, nameserver);
}

bool DnsDiagnostics::reverseLookup(const QString& ipAddress, const QString& nameserver)
{
    if (ipAddress.isEmpty()) {
        Logger::error("DnsDiagnostics: Cannot perform reverse lookup: IP address is empty");
        return false;
    }

    // Validate IP address
    QHostAddress addr(ipAddress);
    if (addr.isNull()) {
        Logger::error(QString("DnsDiagnostics: Invalid IP address: %1").arg(ipAddress));
        return false;
    }

    // Create reverse DNS hostname (e.g., 1.0.168.192.in-addr.arpa for 192.168.0.1)
    QString reverseHostname;
    if (addr.protocol() == QAbstractSocket::IPv4Protocol) {
        QStringList parts = ipAddress.split('.');
        std::reverse(parts.begin(), parts.end());
        reverseHostname = parts.join('.') + ".in-addr.arpa";
    } else {
        // IPv6 reverse DNS (not implemented for simplicity)
        Logger::warn("DnsDiagnostics: IPv6 reverse DNS not implemented");
        return false;
    }

    return startLookup(reverseHostname, PTR, nameserver);
}

void DnsDiagnostics::cancel()
{
    if (!m_isRunning) {
        return;
    }

    Logger::info("DnsDiagnostics: Cancelling lookup");

    if (m_dnsLookup) {
        m_dnsLookup->abort();
        m_dnsLookup->deleteLater();
        m_dnsLookup = nullptr;
    }

    m_isRunning = false;
}

bool DnsDiagnostics::startLookup(const QString& hostname, RecordType type, const QString& nameserver)
{
    if (m_isRunning) {
        Logger::warn("DnsDiagnostics: Cannot start lookup: already running");
        return false;
    }

    m_hostname = hostname;
    m_nameserver = nameserver;
    m_recordType = type;
    m_queryTime = 0;
    m_isRunning = true;

    Logger::info(QString("DnsDiagnostics: Starting %1 query for %2%3")
                 .arg(recordTypeToString(type))
                 .arg(hostname)
                 .arg(nameserver.isEmpty() ? "" : QString(" (using %1)").arg(nameserver)));

    emit progressUpdated(QString("Querying %1 records for %2...").arg(recordTypeToString(type)).arg(hostname));

    // Create DNS lookup
    m_dnsLookup = new QDnsLookup(this);
    m_dnsLookup->setType(toQDnsLookupType(type));
    m_dnsLookup->setName(hostname);

    // Set custom nameserver if provided
    if (!nameserver.isEmpty()) {
        QHostAddress nsAddr(nameserver);
        if (!nsAddr.isNull()) {
            m_dnsLookup->setNameserver(nsAddr);
        } else {
            Logger::warn(QString("DnsDiagnostics: Invalid nameserver address: %1, using system default").arg(nameserver));
        }
    }

    // Connect signal
    connect(m_dnsLookup, &QDnsLookup::finished, this, &DnsDiagnostics::onLookupFinished);

    // Start timer and lookup
    m_timer.start();
    m_dnsLookup->lookup();

    return true;
}

void DnsDiagnostics::onLookupFinished()
{
    if (!m_dnsLookup) {
        return;
    }

    m_queryTime = m_timer.elapsed();

    Logger::debug(QString("DnsDiagnostics: Lookup finished in %1 ms").arg(m_queryTime));

    // Check for errors
    if (m_dnsLookup->error() != QDnsLookup::NoError) {
        QString errorMsg;
        switch (m_dnsLookup->error()) {
            case QDnsLookup::NotFoundError:
                errorMsg = "Domain not found";
                break;
            case QDnsLookup::ServerFailureError:
                errorMsg = "DNS server failure";
                break;
            case QDnsLookup::ServerRefusedError:
                errorMsg = "DNS server refused query";
                break;
            case QDnsLookup::TimeoutError:
                errorMsg = "DNS query timeout";
                break;
            case QDnsLookup::InvalidRequestError:
                errorMsg = "Invalid DNS request";
                break;
            case QDnsLookup::InvalidReplyError:
                errorMsg = "Invalid DNS reply";
                break;
            default:
                errorMsg = m_dnsLookup->errorString();
                break;
        }

        Logger::error(QString("DnsDiagnostics: Lookup failed: %1").arg(errorMsg));
        m_isRunning = false;
        emit lookupError(errorMsg);

        m_dnsLookup->deleteLater();
        m_dnsLookup = nullptr;
        return;
    }

    // Process results
    processResults();
}

void DnsDiagnostics::processResults()
{
    QList<DnsRecord> records = extractRecords();

    if (records.isEmpty()) {
        Logger::warn(QString("DnsDiagnostics: No %1 records found for %2")
                     .arg(recordTypeToString(m_recordType))
                     .arg(m_hostname));
        emit lookupError("No records found");
    } else {
        Logger::info(QString("DnsDiagnostics: Found %1 %2 record(s) in %3 ms")
                     .arg(records.size())
                     .arg(recordTypeToString(m_recordType))
                     .arg(m_queryTime));

        for (const auto& record : records) {
            Logger::debug(QString("  - %1").arg(record.toString()));
        }

        emit lookupCompleted(records);
    }

    // Clean up
    m_dnsLookup->deleteLater();
    m_dnsLookup = nullptr;
    m_isRunning = false;
}

QList<DnsDiagnostics::DnsRecord> DnsDiagnostics::extractRecords()
{
    QList<DnsRecord> records;

    if (!m_dnsLookup) {
        return records;
    }

    // Extract A records
    for (const auto& record : m_dnsLookup->hostAddressRecords()) {
        DnsRecord dnsRecord;
        dnsRecord.type = A;
        dnsRecord.name = record.name();
        dnsRecord.value = record.value().toString();
        dnsRecord.ttl = record.timeToLive();
        dnsRecord.priority = 0;
        records.append(dnsRecord);
    }

    // Extract MX records
    for (const auto& record : m_dnsLookup->mailExchangeRecords()) {
        DnsRecord dnsRecord;
        dnsRecord.type = MX;
        dnsRecord.name = record.name();
        dnsRecord.value = record.exchange();
        dnsRecord.ttl = record.timeToLive();
        dnsRecord.priority = record.preference();
        records.append(dnsRecord);
    }

    // Extract NS records
    for (const auto& record : m_dnsLookup->nameServerRecords()) {
        DnsRecord dnsRecord;
        dnsRecord.type = NS;
        dnsRecord.name = record.name();
        dnsRecord.value = record.value();
        dnsRecord.ttl = record.timeToLive();
        dnsRecord.priority = 0;
        records.append(dnsRecord);
    }

    // Extract TXT records
    for (const auto& record : m_dnsLookup->textRecords()) {
        DnsRecord dnsRecord;
        dnsRecord.type = TXT;
        dnsRecord.name = record.name();
        dnsRecord.value = record.values().join("; ");
        dnsRecord.ttl = record.timeToLive();
        dnsRecord.priority = 0;
        records.append(dnsRecord);
    }

    // Extract CNAME records
    for (const auto& record : m_dnsLookup->canonicalNameRecords()) {
        DnsRecord dnsRecord;
        dnsRecord.type = CNAME;
        dnsRecord.name = record.name();
        dnsRecord.value = record.value();
        dnsRecord.ttl = record.timeToLive();
        dnsRecord.priority = 0;
        records.append(dnsRecord);
    }

    // Extract PTR records
    for (const auto& record : m_dnsLookup->pointerRecords()) {
        DnsRecord dnsRecord;
        dnsRecord.type = PTR;
        dnsRecord.name = record.name();
        dnsRecord.value = record.value();
        dnsRecord.ttl = record.timeToLive();
        dnsRecord.priority = 0;
        records.append(dnsRecord);
    }

    // Extract SRV records
    for (const auto& record : m_dnsLookup->serviceRecords()) {
        DnsRecord dnsRecord;
        dnsRecord.type = SRV;
        dnsRecord.name = record.name();
        dnsRecord.value = QString("%1:%2 (weight: %3)")
                             .arg(record.target())
                             .arg(record.port())
                             .arg(record.weight());
        dnsRecord.ttl = record.timeToLive();
        dnsRecord.priority = record.priority();
        records.append(dnsRecord);
    }

    return records;
}

QDnsLookup::Type DnsDiagnostics::toQDnsLookupType(RecordType type)
{
    switch (type) {
        case A:
            return QDnsLookup::A;
        case AAAA:
            return QDnsLookup::AAAA;
        case MX:
            return QDnsLookup::MX;
        case NS:
            return QDnsLookup::NS;
        case TXT:
            return QDnsLookup::TXT;
        case CNAME:
            return QDnsLookup::CNAME;
        case PTR:
            return QDnsLookup::PTR;
        case SOA:
            // SOA not available in all Qt versions, fall through to ANY
            return QDnsLookup::ANY;
        case SRV:
            return QDnsLookup::SRV;
        case ANY:
            return QDnsLookup::ANY;
        default:
            return QDnsLookup::A;
    }
}

QString DnsDiagnostics::recordTypeToString(RecordType type)
{
    switch (type) {
        case A:
            return "A";
        case AAAA:
            return "AAAA";
        case MX:
            return "MX";
        case NS:
            return "NS";
        case TXT:
            return "TXT";
        case CNAME:
            return "CNAME";
        case PTR:
            return "PTR";
        case SOA:
            return "SOA";
        case SRV:
            return "SRV";
        case ANY:
            return "ANY";
        default:
            return "UNKNOWN";
    }
}
