#include "services/AlertService.h"
#include "utils/Logger.h"
#include <QUuid>
#include <algorithm>

AlertService::AlertService(QObject* parent)
    : QObject(parent)
    , m_maxAlerts(1000)
{
    Logger::info("AlertService initialized with max alerts: " + QString::number(m_maxAlerts));
}

AlertService::~AlertService()
{
    Logger::info("AlertService destroyed");
}

Alert AlertService::createAlert(AlertType type, AlertSeverity severity,
                                const QString& deviceId, const QString& message)
{
    Alert alert(type, severity, deviceId, message);

    // Generate unique ID if not set
    if (alert.id().isEmpty()) {
        alert.setId(generateAlertId());
    }

    m_alerts.append(alert);

    // Update device alert count
    updateDeviceAlertCount(deviceId);

    // Enforce max alerts limit
    enforceMaxAlerts();

    Logger::info(QString("Alert created: [%1] %2 - Device: %3 - %4")
                 .arg(alert.severityToString())
                 .arg(alert.typeToString())
                 .arg(deviceId)
                 .arg(message));

    emit alertCreated(alert);
    emitAlertCountChanged();

    return alert;
}

QList<Alert> AlertService::getAlerts() const
{
    return m_alerts;
}

QList<Alert> AlertService::getUnacknowledgedAlerts() const
{
    QList<Alert> unacknowledged;
    for (const Alert& alert : m_alerts) {
        if (!alert.isAcknowledged()) {
            unacknowledged.append(alert);
        }
    }
    return unacknowledged;
}

QList<Alert> AlertService::getAlertsForDevice(const QString& deviceId) const
{
    QList<Alert> deviceAlerts;
    for (const Alert& alert : m_alerts) {
        if (alert.deviceId() == deviceId) {
            deviceAlerts.append(alert);
        }
    }
    return deviceAlerts;
}

QList<Alert> AlertService::getAlertsBySeverity(AlertSeverity severity) const
{
    QList<Alert> filtered;
    for (const Alert& alert : m_alerts) {
        if (alert.severity() == severity) {
            filtered.append(alert);
        }
    }
    return filtered;
}

QList<Alert> AlertService::getAlertsByType(AlertType type) const
{
    QList<Alert> filtered;
    for (const Alert& alert : m_alerts) {
        if (alert.type() == type) {
            filtered.append(alert);
        }
    }
    return filtered;
}

int AlertService::getAlertCount() const
{
    return m_alerts.size();
}

int AlertService::getUnacknowledgedCount() const
{
    int count = 0;
    for (const Alert& alert : m_alerts) {
        if (!alert.isAcknowledged()) {
            count++;
        }
    }
    return count;
}

bool AlertService::acknowledgeAlert(const QString& alertId)
{
    for (int i = 0; i < m_alerts.size(); ++i) {
        if (m_alerts[i].id() == alertId) {
            if (!m_alerts[i].isAcknowledged()) {
                m_alerts[i].setAcknowledged(true);

                Logger::debug(QString("Alert acknowledged: %1").arg(alertId));

                emit alertAcknowledged(alertId);
                emitAlertCountChanged();

                return true;
            }
            return false; // Already acknowledged
        }
    }

    Logger::warn(QString("Alert not found for acknowledgment: %1").arg(alertId));
    return false;
}

void AlertService::acknowledgeAllAlerts()
{
    int acknowledgedCount = 0;

    for (int i = 0; i < m_alerts.size(); ++i) {
        if (!m_alerts[i].isAcknowledged()) {
            m_alerts[i].setAcknowledged(true);
            acknowledgedCount++;
        }
    }

    if (acknowledgedCount > 0) {
        Logger::info(QString("All alerts acknowledged: %1 alerts").arg(acknowledgedCount));
        emit allAlertsAcknowledged();
        emitAlertCountChanged();
    }
}

void AlertService::clearAlerts()
{
    int count = m_alerts.size();
    m_alerts.clear();
    m_deviceAlertCount.clear();

    Logger::info(QString("All alerts cleared: %1 alerts removed").arg(count));

    emit alertsCleared();
    emitAlertCountChanged();
}

void AlertService::clearAlertsForDevice(const QString& deviceId)
{
    int originalSize = m_alerts.size();

    m_alerts.erase(
        std::remove_if(m_alerts.begin(), m_alerts.end(),
                      [&deviceId](const Alert& alert) {
                          return alert.deviceId() == deviceId;
                      }),
        m_alerts.end()
    );

    int removed = originalSize - m_alerts.size();

    if (removed > 0) {
        m_deviceAlertCount.remove(deviceId);

        Logger::info(QString("Alerts cleared for device %1: %2 alerts removed")
                     .arg(deviceId).arg(removed));

        emitAlertCountChanged();
    }
}

void AlertService::setMaxAlerts(int maxAlerts)
{
    if (maxAlerts > 0) {
        m_maxAlerts = maxAlerts;
        Logger::info(QString("Max alerts set to: %1").arg(maxAlerts));
        enforceMaxAlerts();
    } else {
        Logger::warn("Invalid max alerts value, must be positive");
    }
}

int AlertService::getMaxAlerts() const
{
    return m_maxAlerts;
}

void AlertService::pruneOldAlerts()
{
    enforceMaxAlerts();
}

QString AlertService::generateAlertId()
{
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

void AlertService::enforceMaxAlerts()
{
    if (m_alerts.size() > m_maxAlerts) {
        int toRemove = m_alerts.size() - m_maxAlerts;

        // Sort by timestamp (oldest first)
        std::sort(m_alerts.begin(), m_alerts.end(),
                 [](const Alert& a, const Alert& b) {
                     return a.timestamp() < b.timestamp();
                 });

        // Remove oldest alerts
        m_alerts.erase(m_alerts.begin(), m_alerts.begin() + toRemove);

        Logger::debug(QString("Pruned %1 old alerts").arg(toRemove));
        emitAlertCountChanged();
    }
}

void AlertService::updateDeviceAlertCount(const QString& deviceId)
{
    if (m_deviceAlertCount.contains(deviceId)) {
        m_deviceAlertCount[deviceId]++;
    } else {
        m_deviceAlertCount[deviceId] = 1;
    }
}

void AlertService::emitAlertCountChanged()
{
    emit alertCountChanged(getAlertCount(), getUnacknowledgedCount());
}
