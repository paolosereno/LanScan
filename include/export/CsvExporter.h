#ifndef CSVEXPORTER_H
#define CSVEXPORTER_H

#include "interfaces/IExporter.h"
#include <QTextStream>

class CsvExporter : public IExporter {
public:
    CsvExporter() = default;
    ~CsvExporter() override = default;

    bool exportData(const QList<Device>& devices, const QString& filepath) override;
    QString getFormatName() const override { return "CSV"; }
    QString getFileExtension() const override { return ".csv"; }

private:
    QString escapeField(const QString& field);
    QString buildCsvRow(const Device& device);
    QString buildHeader();
    QString formatPortsList(const QList<PortInfo>& ports);
};

#endif // CSVEXPORTER_H
