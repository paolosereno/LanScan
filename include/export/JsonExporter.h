#ifndef JSONEXPORTER_H
#define JSONEXPORTER_H

#include "interfaces/IExporter.h"
#include <QJsonObject>
#include <QJsonArray>

class JsonExporter : public IExporter {
public:
    JsonExporter() = default;
    ~JsonExporter() override = default;

    bool exportData(const QList<Device>& devices, const QString& filepath) override;
    QString getFormatName() const override { return "JSON"; }
    QString getFileExtension() const override { return ".json"; }

private:
    QJsonObject deviceToJson(const Device& device);
    QJsonArray devicesToJsonArray(const QList<Device>& devices);
    QJsonObject metricsToJson(const NetworkMetrics& metrics);
    QJsonArray portsToJsonArray(const QList<PortInfo>& ports);
    QJsonObject portToJson(const PortInfo& port);
};

#endif // JSONEXPORTER_H
