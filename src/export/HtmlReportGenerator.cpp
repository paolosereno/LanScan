#include "export/HtmlReportGenerator.h"
#include "utils/Logger.h"
#include <QFile>
#include <QTextStream>
#include <QDateTime>

bool HtmlReportGenerator::exportData(const QList<Device>& devices, const QString& filepath) {
    QFile file(filepath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        Logger::error("HtmlReportGenerator: Failed to open file for writing: " + filepath);
        return false;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << generateHtml(devices);

    file.close();
    Logger::info("HtmlReportGenerator: " + QString("Exported %1 devices to %2").arg(devices.size()).arg(filepath));

    return true;
}

QString HtmlReportGenerator::generateHtml(const QList<Device>& devices) {
    QString html;
    html += "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n";
    html += "    <meta charset=\"UTF-8\">\n";
    html += "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    html += "    <title>LanScan Network Report</title>\n";
    html += "    <style>\n" + generateCss() + "    </style>\n";
    html += "</head>\n<body>\n";
    html += generateHeader();
    html += generateSummary(devices);
    html += generateDeviceTable(devices);
    html += generateFooter();
    html += "</body>\n</html>";

    return html;
}

QString HtmlReportGenerator::generateCss() {
    return R"(
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        body {
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, sans-serif;
            line-height: 1.6;
            color: #333;
            background: #f5f5f5;
            padding: 20px;
        }
        .container {
            max-width: 1400px;
            margin: 0 auto;
            background: white;
            padding: 30px;
            border-radius: 8px;
            box-shadow: 0 2px 10px rgba(0,0,0,0.1);
        }
        h1 {
            color: #2c3e50;
            margin-bottom: 10px;
            font-size: 2em;
        }
        .subtitle {
            color: #7f8c8d;
            margin-bottom: 30px;
            font-size: 0.9em;
        }
        .summary {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
            gap: 20px;
            margin-bottom: 30px;
        }
        .summary-card {
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            padding: 20px;
            border-radius: 6px;
            box-shadow: 0 2px 6px rgba(0,0,0,0.15);
        }
        .summary-card h3 {
            font-size: 0.9em;
            opacity: 0.9;
            margin-bottom: 8px;
            font-weight: 500;
        }
        .summary-card .value {
            font-size: 2em;
            font-weight: bold;
        }
        .summary-card.online {
            background: linear-gradient(135deg, #56ab2f 0%, #a8e063 100%);
        }
        .summary-card.offline {
            background: linear-gradient(135deg, #bdc3c7 0%, #95a5a6 100%);
        }
        table {
            width: 100%;
            border-collapse: collapse;
            margin-top: 20px;
        }
        th {
            background: #34495e;
            color: white;
            padding: 12px;
            text-align: left;
            font-weight: 600;
            font-size: 0.9em;
            position: sticky;
            top: 0;
        }
        td {
            padding: 12px;
            border-bottom: 1px solid #ecf0f1;
        }
        tr:hover {
            background: #f8f9fa;
        }
        .status-badge {
            display: inline-block;
            padding: 4px 12px;
            border-radius: 12px;
            font-size: 0.85em;
            font-weight: 600;
        }
        .status-online {
            background: #d4edda;
            color: #155724;
        }
        .status-offline {
            background: #f8d7da;
            color: #721c24;
        }
        .quality-excellent {
            color: #27ae60;
            font-weight: 600;
        }
        .quality-good {
            color: #2ecc71;
            font-weight: 600;
        }
        .quality-fair {
            color: #f39c12;
            font-weight: 600;
        }
        .quality-poor {
            color: #e67e22;
            font-weight: 600;
        }
        .quality-bad {
            color: #c0392b;
            font-weight: 600;
        }
        .footer {
            margin-top: 40px;
            padding-top: 20px;
            border-top: 1px solid #ecf0f1;
            text-align: center;
            color: #7f8c8d;
            font-size: 0.85em;
        }
        .metric {
            font-size: 0.9em;
        }
        .ports-list {
            font-size: 0.85em;
            color: #555;
        }
    )";
}

QString HtmlReportGenerator::generateHeader() {
    QString html;
    html += "<div class=\"container\">\n";
    html += "    <h1>🔍 LanScan Network Report</h1>\n";
    html += "    <p class=\"subtitle\">Generated on " + QDateTime::currentDateTime().toString("MMMM dd, yyyy 'at' hh:mm AP") + "</p>\n";
    return html;
}

QString HtmlReportGenerator::generateSummary(const QList<Device>& devices) {
    int totalDevices = devices.size();
    int onlineDevices = countOnlineDevices(devices);
    int offlineDevices = totalDevices - onlineDevices;

    QString html;
    html += "    <div class=\"summary\">\n";
    html += "        <div class=\"summary-card\">\n";
    html += "            <h3>Total Devices</h3>\n";
    html += "            <div class=\"value\">" + QString::number(totalDevices) + "</div>\n";
    html += "        </div>\n";
    html += "        <div class=\"summary-card online\">\n";
    html += "            <h3>Online Devices</h3>\n";
    html += "            <div class=\"value\">" + QString::number(onlineDevices) + "</div>\n";
    html += "        </div>\n";
    html += "        <div class=\"summary-card offline\">\n";
    html += "            <h3>Offline Devices</h3>\n";
    html += "            <div class=\"value\">" + QString::number(offlineDevices) + "</div>\n";
    html += "        </div>\n";
    html += "    </div>\n";

    return html;
}

QString HtmlReportGenerator::generateDeviceTable(const QList<Device>& devices) {
    QString html;
    html += "    <table>\n";
    html += "        <thead>\n";
    html += "            <tr>\n";
    html += "                <th>IP Address</th>\n";
    html += "                <th>Hostname</th>\n";
    html += "                <th>MAC Address</th>\n";
    html += "                <th>Vendor</th>\n";
    html += "                <th>Status</th>\n";
    html += "                <th>Latency (ms)</th>\n";
    html += "                <th>Packet Loss</th>\n";
    html += "                <th>Jitter (ms)</th>\n";
    html += "                <th>Quality</th>\n";
    html += "                <th>Open Ports</th>\n";
    html += "                <th>Comments</th>\n";
    html += "            </tr>\n";
    html += "        </thead>\n";
    html += "        <tbody>\n";

    for (const Device& device : devices) {
        html += generateDeviceRow(device);
    }

    html += "        </tbody>\n";
    html += "    </table>\n";

    return html;
}

QString HtmlReportGenerator::generateDeviceRow(const Device& device) {
    const NetworkMetrics& metrics = device.getMetrics();
    QString qualityString = metrics.getQualityScoreString();
    QString qualityClass = getQualityColorClass(qualityString);

    QString html;
    html += "            <tr>\n";
    html += "                <td><strong>" + device.getIp() + "</strong></td>\n";
    html += "                <td>" + (device.getHostname().isEmpty() ? "-" : device.getHostname()) + "</td>\n";
    html += "                <td class=\"metric\">" + (device.getMacAddress().isEmpty() ? "-" : device.getMacAddress()) + "</td>\n";
    html += "                <td>" + (device.getVendor().isEmpty() ? "-" : device.getVendor()) + "</td>\n";

    // Status badge
    if (device.isOnline()) {
        html += "                <td><span class=\"status-badge status-online\">Online</span></td>\n";
    } else {
        html += "                <td><span class=\"status-badge status-offline\">Offline</span></td>\n";
    }

    // Metrics
    html += "                <td class=\"metric\">" + QString::number(metrics.getLatencyAvg(), 'f', 2) + "</td>\n";
    html += "                <td class=\"metric\">" + QString::number(metrics.getPacketLoss(), 'f', 2) + "%</td>\n";
    html += "                <td class=\"metric\">" + QString::number(metrics.getJitter(), 'f', 2) + "</td>\n";
    html += "                <td class=\"" + qualityClass + "\">" + qualityString + "</td>\n";
    html += "                <td class=\"ports-list\">" + formatPortsList(device.getOpenPorts()) + "</td>\n";
    html += "                <td>" + (device.getComments().isEmpty() ? "-" : device.getComments()) + "</td>\n";
    html += "            </tr>\n";

    return html;
}

QString HtmlReportGenerator::generateFooter() {
    QString html;
    html += "    <div class=\"footer\">\n";
    html += "        <p>Report generated by LanScan v1.0</p>\n";
    html += "        <p>Network scanning and monitoring tool</p>\n";
    html += "    </div>\n";
    html += "</div>\n";
    return html;
}

QString HtmlReportGenerator::getQualityColorClass(const QString& qualityString) {
    if (qualityString == "Excellent") return "quality-excellent";
    if (qualityString == "Good") return "quality-good";
    if (qualityString == "Fair") return "quality-fair";
    if (qualityString == "Poor") return "quality-poor";
    if (qualityString == "Bad") return "quality-bad";
    return "";
}

QString HtmlReportGenerator::formatPortsList(const QList<PortInfo>& ports) {
    if (ports.isEmpty()) {
        return "-";
    }

    QStringList portStrings;
    for (const PortInfo& port : ports) {
        portStrings << QString::number(port.getPort());
    }

    if (portStrings.size() > 5) {
        return portStrings.mid(0, 5).join(", ") + QString(" (+%1 more)").arg(portStrings.size() - 5);
    }

    return portStrings.join(", ");
}

int HtmlReportGenerator::countOnlineDevices(const QList<Device>& devices) {
    int count = 0;
    for (const Device& device : devices) {
        if (device.isOnline()) {
            count++;
        }
    }
    return count;
}
