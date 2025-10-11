#include "utils/TooltipHelper.h"

QString TooltipHelper::createDeviceTooltip(const Device& device) {
    QString tooltip = QString(
        "<table cellspacing='3'>"
        "%1"
        "%2"
        "%3"
        "%4"
        "%5"
        "%6"
        "</table>"
    )
    .arg(formatTableRow("<b>IP Address</b>", device.getIp()))
    .arg(formatTableRow("<b>Hostname</b>", device.getHostname().isEmpty() ? "<i>Unknown</i>" : device.getHostname()))
    .arg(formatTableRow("<b>MAC Address</b>", device.getMacAddress().isEmpty() ? "<i>Unknown</i>" : device.getMacAddress()))
    .arg(formatTableRow("<b>Vendor</b>", device.getVendor().isEmpty() ? "<i>Unknown</i>" : device.getVendor()))
    .arg(formatTableRow("<b>Status</b>", device.isOnline() ? "<span style='color:#4CAF50'>Online</span>" : "<span style='color:#F44336'>Offline</span>"))
    .arg(formatTableRow("<b>Last Seen</b>", device.getLastSeen().toString("yyyy-MM-dd hh:mm:ss")));

    return tooltip;
}

QString TooltipHelper::createMetricsTooltip(const NetworkMetrics& metrics) {
    QString qualityText = metrics.getQualityScoreString();
    QString qualityColor;

    switch (metrics.getQualityScore()) {
        case NetworkMetrics::Excellent: qualityColor = "#4CAF50"; break;  // Green
        case NetworkMetrics::Good: qualityColor = "#8BC34A"; break;        // Light green
        case NetworkMetrics::Fair: qualityColor = "#FF9800"; break;        // Orange
        case NetworkMetrics::Poor: qualityColor = "#F44336"; break;        // Red
        case NetworkMetrics::Critical: qualityColor = "#D32F2F"; break;    // Dark red
        default: qualityColor = "#9E9E9E"; break;                          // Gray
    }

    QString tooltip = QString(
        "<table cellspacing='3'>"
        "%1"
        "%2"
        "%3"
        "%4"
        "%5"
        "</table>"
    )
    .arg(formatTableRow("<b>Latency</b>", QString("%1 ms (min: %2, max: %3)")
        .arg(metrics.getLatencyAvg(), 0, 'f', 2)
        .arg(metrics.getLatencyMin(), 0, 'f', 2)
        .arg(metrics.getLatencyMax(), 0, 'f', 2)))
    .arg(formatTableRow("<b>Jitter</b>", QString("%1 ms").arg(metrics.getJitter(), 0, 'f', 2)))
    .arg(formatTableRow("<b>Packet Loss</b>", QString("%1%").arg(metrics.getPacketLoss(), 0, 'f', 1)))
    .arg(formatTableRow("<b>Quality Score</b>", QString("<span style='color:%1'>%2</span>")
        .arg(qualityColor).arg(qualityText)))
    .arg(formatTableRow("<b>Timestamp</b>", metrics.timestamp().toString("yyyy-MM-dd hh:mm:ss")));

    return tooltip;
}

QString TooltipHelper::createScanTooltip(const QString& scanType, const QString& description) {
    return QString(
        "<b>%1 Scan</b><br/>"
        "<span style='color:#666'>%2</span>"
    ).arg(scanType).arg(description);
}

QString TooltipHelper::createExportTooltip(const QString& format, const QString& description) {
    return QString(
        "<b>Export as %1</b><br/>"
        "<span style='color:#666'>%2</span>"
    ).arg(format).arg(description);
}

QString TooltipHelper::createRichTooltip(const QString& title, const QString& body) {
    return QString(
        "<b>%1</b><br/>"
        "<span style='color:#666'>%2</span>"
    ).arg(title).arg(body);
}

QString TooltipHelper::createListTooltip(const QString& title, const QStringList& items) {
    QString itemsList;
    for (const QString& item : items) {
        itemsList += QString("<li>%1</li>").arg(item);
    }

    return QString(
        "<b>%1</b><br/>"
        "<ul style='margin-top:5px; margin-bottom:0px;'>"
        "%2"
        "</ul>"
    ).arg(title).arg(itemsList);
}

QString TooltipHelper::formatTableRow(const QString& label, const QString& value) {
    return QString(
        "<tr>"
        "<td style='padding-right:10px;'>%1:</td>"
        "<td>%2</td>"
        "</tr>"
    ).arg(label).arg(value);
}

QString TooltipHelper::getQualityColor(double qualityScore) {
    if (qualityScore >= 0.9) return "#4CAF50";  // Green
    if (qualityScore >= 0.7) return "#8BC34A";  // Light green
    if (qualityScore >= 0.5) return "#FF9800";  // Orange
    return "#F44336";                            // Red
}
