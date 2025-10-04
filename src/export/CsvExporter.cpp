#include "export/CsvExporter.h"
#include "utils/Logger.h"
#include <QFile>
#include <QTextStream>

bool CsvExporter::exportData(const QList<Device>& devices, const QString& filepath) {
    QFile file(filepath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        Logger::error("CsvExporter: Failed to open file for writing: " + filepath);
        return false;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);

    // Write header
    out << buildHeader() << "\n";

    // Write data rows
    for (const Device& device : devices) {
        out << buildCsvRow(device) << "\n";
    }

    file.close();
    Logger::info("CsvExporter: " + QString("Exported %1 devices to %2").arg(devices.size()).arg(filepath));

    return true;
}

QString CsvExporter::buildHeader() {
    return "IP,Hostname,MAC Address,Vendor,Status,Last Seen,Open Ports,Latency (ms),Packet Loss (%),Jitter (ms),Quality";
}

QString CsvExporter::buildCsvRow(const Device& device) {
    QStringList fields;

    fields << escapeField(device.getIp());
    fields << escapeField(device.getHostname());
    fields << escapeField(device.getMacAddress());
    fields << escapeField(device.getVendor());
    fields << (device.isOnline() ? "Online" : "Offline");
    fields << escapeField(device.getLastSeen().toString("yyyy-MM-dd HH:mm:ss"));
    fields << escapeField(formatPortsList(device.getOpenPorts()));

    // Metrics
    const NetworkMetrics& metrics = device.getMetrics();
    fields << QString::number(metrics.getLatencyAvg(), 'f', 2);
    fields << QString::number(metrics.getPacketLoss(), 'f', 2);
    fields << QString::number(metrics.getJitter(), 'f', 2);
    fields << escapeField(metrics.getQualityScoreString());

    return fields.join(",");
}

QString CsvExporter::escapeField(const QString& field) {
    // If field contains comma, quotes, or newline, wrap in quotes and escape internal quotes
    if (field.contains(',') || field.contains('"') || field.contains('\n')) {
        QString escaped = field;
        escaped.replace("\"", "\"\""); // Escape quotes by doubling them
        return "\"" + escaped + "\"";
    }
    return field;
}

QString CsvExporter::formatPortsList(const QList<PortInfo>& ports) {
    if (ports.isEmpty()) {
        return "-";
    }

    QStringList portStrings;
    for (const PortInfo& port : ports) {
        portStrings << QString::number(port.getPort());
    }

    return "\"" + portStrings.join(",") + "\"";
}
