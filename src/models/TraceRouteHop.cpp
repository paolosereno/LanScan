#include "models/TraceRouteHop.h"
#include <algorithm>
#include <numeric>

TraceRouteHop::TraceRouteHop()
    : m_hopNumber(0)
    , m_ipAddress("")
    , m_hostname("")
    , m_timeout(false)
{
}

TraceRouteHop::TraceRouteHop(int hopNumber, const QString& ipAddress, const QString& hostname)
    : m_hopNumber(hopNumber)
    , m_ipAddress(ipAddress)
    , m_hostname(hostname)
    , m_timeout(false)
{
}

TraceRouteHop::TraceRouteHop(const TraceRouteHop& other)
    : m_hopNumber(other.m_hopNumber)
    , m_ipAddress(other.m_ipAddress)
    , m_hostname(other.m_hostname)
    , m_rttList(other.m_rttList)
    , m_timeout(other.m_timeout)
{
}

TraceRouteHop& TraceRouteHop::operator=(const TraceRouteHop& other)
{
    if (this != &other) {
        m_hopNumber = other.m_hopNumber;
        m_ipAddress = other.m_ipAddress;
        m_hostname = other.m_hostname;
        m_rttList = other.m_rttList;
        m_timeout = other.m_timeout;
    }
    return *this;
}

bool TraceRouteHop::operator==(const TraceRouteHop& other) const
{
    return m_hopNumber == other.m_hopNumber &&
           m_ipAddress == other.m_ipAddress &&
           m_hostname == other.m_hostname &&
           m_rttList == other.m_rttList &&
           m_timeout == other.m_timeout;
}

void TraceRouteHop::addRtt(double rtt)
{
    if (rtt >= 0.0) {
        m_rttList.append(rtt);
    }
}

void TraceRouteHop::clearRtt()
{
    m_rttList.clear();
}

double TraceRouteHop::minRtt() const
{
    if (m_rttList.isEmpty()) {
        return 0.0;
    }
    return *std::min_element(m_rttList.begin(), m_rttList.end());
}

double TraceRouteHop::maxRtt() const
{
    if (m_rttList.isEmpty()) {
        return 0.0;
    }
    return *std::max_element(m_rttList.begin(), m_rttList.end());
}

double TraceRouteHop::averageRtt() const
{
    if (m_rttList.isEmpty()) {
        return 0.0;
    }
    double sum = std::accumulate(m_rttList.begin(), m_rttList.end(), 0.0);
    return sum / m_rttList.size();
}

QString TraceRouteHop::toString() const
{
    QString result = QString("%1  ").arg(m_hopNumber, 2);

    if (m_timeout) {
        result += "* * *";
        return result;
    }

    // Add hostname and IP address
    if (!m_hostname.isEmpty()) {
        result += QString("%1 (%2)  ").arg(m_hostname, m_ipAddress);
    } else {
        result += QString("%1  ").arg(m_ipAddress);
    }

    // Add RTT measurements
    if (m_rttList.isEmpty()) {
        result += "* * *";
    } else {
        QStringList rttStrings;
        for (double rtt : m_rttList) {
            rttStrings.append(QString("%1 ms").arg(rtt, 0, 'f', 1));
        }
        result += rttStrings.join("  ");
    }

    return result;
}
