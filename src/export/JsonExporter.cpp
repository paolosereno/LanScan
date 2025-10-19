#include "export/JsonExporter.h"
#include "utils/Logger.h"
#include <QFile>
#include <QJsonDocument>
#include <QDateTime>

bool JsonExporter::exportData(const QList<Device>& devices, const QString& filepath) {
    QFile file(filepath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        Logger::error("JsonExporter: Failed to open file for writing: " + filepath);
        return false;
    }

    QJsonObject root;
    root["export_date"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    root["total_devices"] = devices.size();
    root["devices"] = devicesToJsonArray(devices);

    QJsonDocument doc(root);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    Logger::info("JsonExporter: " + QString("Exported %1 devices to %2").arg(devices.size()).arg(filepath));

    return true;
}

QJsonArray JsonExporter::devicesToJsonArray(const QList<Device>& devices) {
    QJsonArray array;
    for (const Device& device : devices) {
        array.append(deviceToJson(device));
    }
    return array;
}

QJsonObject JsonExporter::deviceToJson(const Device& device) {
    QJsonObject obj;
    obj["id"] = device.getId();
    obj["ip"] = device.getIp();
    obj["hostname"] = device.getHostname();
    obj["mac_address"] = device.getMacAddress();
    obj["vendor"] = device.getVendor();
    obj["is_online"] = device.isOnline();
    obj["last_seen"] = device.getLastSeen().toString(Qt::ISODate);
    obj["open_ports"] = portsToJsonArray(device.getOpenPorts());
    obj["metrics"] = metricsToJson(device.getMetrics());
    obj["comments"] = device.getComments();

    return obj;
}

QJsonObject JsonExporter::metricsToJson(const NetworkMetrics& metrics) {
    QJsonObject obj;
    obj["latency_min"] = metrics.getLatencyMin();
    obj["latency_avg"] = metrics.getLatencyAvg();
    obj["latency_max"] = metrics.getLatencyMax();
    obj["latency_median"] = metrics.getLatencyMedian();
    obj["jitter"] = metrics.getJitter();
    obj["packet_loss"] = metrics.getPacketLoss();
    obj["quality_score"] = metrics.getQualityScore();

    return obj;
}

QJsonArray JsonExporter::portsToJsonArray(const QList<PortInfo>& ports) {
    QJsonArray array;
    for (const PortInfo& port : ports) {
        array.append(portToJson(port));
    }
    return array;
}

QJsonObject JsonExporter::portToJson(const PortInfo& port) {
    QJsonObject obj;
    obj["port"] = port.getPort();
    obj["protocol"] = port.getProtocol();
    obj["service"] = port.getService();
    obj["state"] = port.getState();

    return obj;
}
