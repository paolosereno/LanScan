#include "models/Alert.h"
#include <QUuid>

Alert::Alert()
    : m_id(QUuid::createUuid().toString(QUuid::WithoutBraces))
    , m_type(AlertType::ThresholdExceeded)
    , m_severity(AlertSeverity::Info)
    , m_deviceId("")
    , m_message("")
    , m_timestamp(QDateTime::currentDateTime())
    , m_acknowledged(false)
{
}

Alert::Alert(AlertType type, AlertSeverity severity,
             const QString& deviceId, const QString& message)
    : m_id(QUuid::createUuid().toString(QUuid::WithoutBraces))
    , m_type(type)
    , m_severity(severity)
    , m_deviceId(deviceId)
    , m_message(message)
    , m_timestamp(QDateTime::currentDateTime())
    , m_acknowledged(false)
{
}

Alert::Alert(const Alert& other)
    : m_id(other.m_id)
    , m_type(other.m_type)
    , m_severity(other.m_severity)
    , m_deviceId(other.m_deviceId)
    , m_message(other.m_message)
    , m_timestamp(other.m_timestamp)
    , m_acknowledged(other.m_acknowledged)
{
}

Alert& Alert::operator=(const Alert& other)
{
    if (this != &other) {
        m_id = other.m_id;
        m_type = other.m_type;
        m_severity = other.m_severity;
        m_deviceId = other.m_deviceId;
        m_message = other.m_message;
        m_timestamp = other.m_timestamp;
        m_acknowledged = other.m_acknowledged;
    }
    return *this;
}

bool Alert::operator==(const Alert& other) const
{
    return m_id == other.m_id &&
           m_type == other.m_type &&
           m_severity == other.m_severity &&
           m_deviceId == other.m_deviceId &&
           m_message == other.m_message &&
           m_timestamp == other.m_timestamp &&
           m_acknowledged == other.m_acknowledged;
}

bool Alert::operator!=(const Alert& other) const
{
    return !(*this == other);
}

QColor Alert::getSeverityColor() const
{
    return severityColor(m_severity);
}

QString Alert::typeToString() const
{
    return alertTypeToString(m_type);
}

QString Alert::severityToString() const
{
    return alertSeverityToString(m_severity);
}

QString Alert::toString() const
{
    return QString("[%1] [%2] %3 - Device: %4 - %5 - %6")
        .arg(m_timestamp.toString("yyyy-MM-dd hh:mm:ss"))
        .arg(severityToString())
        .arg(typeToString())
        .arg(m_deviceId)
        .arg(m_message)
        .arg(m_acknowledged ? "Acknowledged" : "Unacknowledged");
}

QString Alert::alertTypeToString(AlertType type)
{
    switch (type) {
        case AlertType::HighLatency:
            return "High Latency";
        case AlertType::PacketLoss:
            return "Packet Loss";
        case AlertType::HighJitter:
            return "High Jitter";
        case AlertType::DeviceOffline:
            return "Device Offline";
        case AlertType::DeviceOnline:
            return "Device Online";
        case AlertType::ThresholdExceeded:
            return "Threshold Exceeded";
        default:
            return "Unknown";
    }
}

QString Alert::alertSeverityToString(AlertSeverity severity)
{
    switch (severity) {
        case AlertSeverity::Info:
            return "Info";
        case AlertSeverity::Warning:
            return "Warning";
        case AlertSeverity::Critical:
            return "Critical";
        default:
            return "Unknown";
    }
}

QColor Alert::severityColor(AlertSeverity severity)
{
    switch (severity) {
        case AlertSeverity::Info:
            return QColor(0, 122, 204);    // Blue
        case AlertSeverity::Warning:
            return QColor(255, 165, 0);    // Orange
        case AlertSeverity::Critical:
            return QColor(220, 20, 60);    // Crimson Red
        default:
            return QColor(128, 128, 128);  // Gray
    }
}
