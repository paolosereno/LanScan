#include "export/XmlExporter.h"
#include "utils/Logger.h"
#include <QFile>
#include <QDateTime>

bool XmlExporter::exportData(const QList<Device>& devices, const QString& filepath) {
    QFile file(filepath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        Logger::error("XmlExporter: Failed to open file for writing: " + filepath);
        return false;
    }

    QXmlStreamWriter writer(&file);
    writer.setAutoFormatting(true);
    writer.setAutoFormattingIndent(2);

    // Start document
    writer.writeStartDocument();

    // Root element
    writer.writeStartElement("LanScanExport");
    writer.writeAttribute("version", "1.0");
    writer.writeAttribute("exportDate", QDateTime::currentDateTime().toString(Qt::ISODate));
    writer.writeAttribute("totalDevices", QString::number(devices.size()));

    // Devices container
    writer.writeStartElement("Devices");

    for (const Device& device : devices) {
        writeDevice(writer, device);
    }

    writer.writeEndElement(); // Devices
    writer.writeEndElement(); // LanScanExport
    writer.writeEndDocument();

    file.close();
    Logger::info("XmlExporter: " + QString("Exported %1 devices to %2").arg(devices.size()).arg(filepath));

    return true;
}

void XmlExporter::writeDevice(QXmlStreamWriter& writer, const Device& device) {
    writer.writeStartElement("Device");
    writer.writeAttribute("id", device.getId());

    // Basic information
    writer.writeTextElement("IP", device.getIp());
    writer.writeTextElement("Hostname", device.getHostname());
    writer.writeTextElement("MacAddress", device.getMacAddress());
    writer.writeTextElement("Vendor", device.getVendor());
    writer.writeTextElement("Status", device.isOnline() ? "Online" : "Offline");
    writer.writeTextElement("LastSeen", device.getLastSeen().toString(Qt::ISODate));

    // Network metrics
    writeMetrics(writer, device.getMetrics());

    // Open ports
    writePorts(writer, device.getOpenPorts());

    writer.writeEndElement(); // Device
}

void XmlExporter::writeMetrics(QXmlStreamWriter& writer, const NetworkMetrics& metrics) {
    writer.writeStartElement("Metrics");

    // Latency
    writer.writeStartElement("Latency");
    writer.writeAttribute("unit", "ms");
    writer.writeTextElement("Min", QString::number(metrics.getLatencyMin(), 'f', 2));
    writer.writeTextElement("Avg", QString::number(metrics.getLatencyAvg(), 'f', 2));
    writer.writeTextElement("Max", QString::number(metrics.getLatencyMax(), 'f', 2));
    writer.writeTextElement("Median", QString::number(metrics.getLatencyMedian(), 'f', 2));
    writer.writeEndElement(); // Latency

    // Jitter
    writer.writeStartElement("Jitter");
    writer.writeAttribute("unit", "ms");
    writer.writeTextElement("Value", QString::number(metrics.getJitter(), 'f', 2));
    writer.writeEndElement(); // Jitter

    // Packet Loss
    writer.writeStartElement("PacketLoss");
    writer.writeAttribute("unit", "%");
    writer.writeTextElement("Value", QString::number(metrics.getPacketLoss(), 'f', 2));
    writer.writeEndElement(); // PacketLoss

    // Quality Score
    writer.writeStartElement("Quality");
    writer.writeTextElement("Score", QString::number(metrics.getQualityScore()));
    writer.writeTextElement("Rating", metrics.getQualityScoreString());
    writer.writeEndElement(); // Quality

    writer.writeEndElement(); // Metrics
}

void XmlExporter::writePorts(QXmlStreamWriter& writer, const QList<PortInfo>& ports) {
    writer.writeStartElement("OpenPorts");
    writer.writeAttribute("count", QString::number(ports.size()));

    for (const PortInfo& port : ports) {
        writePort(writer, port);
    }

    writer.writeEndElement(); // OpenPorts
}

void XmlExporter::writePort(QXmlStreamWriter& writer, const PortInfo& port) {
    writer.writeStartElement("Port");
    writer.writeAttribute("number", QString::number(port.getPort()));
    writer.writeTextElement("Protocol", port.protocolString());
    writer.writeTextElement("Service", port.getService());
    writer.writeTextElement("State", port.stateString());
    writer.writeEndElement(); // Port
}
