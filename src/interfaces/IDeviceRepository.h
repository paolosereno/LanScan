#ifndef IDEVICEREPOSITORY_H
#define IDEVICEREPOSITORY_H

#include <QString>
#include <QList>
#include "models/Device.h"

class IDeviceRepository
{
public:
    virtual ~IDeviceRepository() = default;

    virtual void save(const Device& device) = 0;
    virtual Device findById(const QString& id) = 0;
    virtual QList<Device> findAll() = 0;
    virtual void remove(const QString& id) = 0;
    virtual void clear() = 0;
    virtual int count() const = 0;
};

#endif // IDEVICEREPOSITORY_H
