#ifndef IEXPORTER_H
#define IEXPORTER_H

#include <QString>
#include <QList>
#include "models/Device.h"

class IExporter
{
public:
    virtual ~IExporter() = default;

    virtual bool exportData(const QList<Device>& devices, const QString& filepath) = 0;
    virtual QString getFormatName() const = 0;
    virtual QString getFileExtension() const = 0;
};

#endif // IEXPORTER_H
