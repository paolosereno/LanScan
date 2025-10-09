#ifndef XMLEXPORTER_H
#define XMLEXPORTER_H

#include "interfaces/IExporter.h"
#include <QXmlStreamWriter>
#include <QIODevice>

/**
 * @brief XML exporter for device data
 *
 * Exports device information to structured XML format with proper hierarchy.
 * Includes all device properties, metrics, and port information.
 */
class XmlExporter : public IExporter {
public:
    XmlExporter() = default;
    ~XmlExporter() override = default;

    bool exportData(const QList<Device>& devices, const QString& filepath) override;
    QString getFormatName() const override { return "XML"; }
    QString getFileExtension() const override { return ".xml"; }

private:
    void writeDevice(QXmlStreamWriter& writer, const Device& device);
    void writeMetrics(QXmlStreamWriter& writer, const NetworkMetrics& metrics);
    void writePorts(QXmlStreamWriter& writer, const QList<PortInfo>& ports);
    void writePort(QXmlStreamWriter& writer, const PortInfo& port);
};

#endif // XMLEXPORTER_H
