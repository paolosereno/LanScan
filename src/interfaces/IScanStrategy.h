#ifndef ISCANSTRATEGY_H
#define ISCANSTRATEGY_H

#include <QString>
#include "models/Device.h"

/**
 * Interface for scan strategies
 * Strategies define how to scan a single host (e.g., quick ping vs deep port scan)
 */
class IScanStrategy
{
public:
    virtual ~IScanStrategy() = default;

    // Scan a single IP address and return device information
    virtual Device scan(const QString& ip) = 0;

    // Get strategy name
    virtual QString getName() const = 0;

    // Get strategy description
    virtual QString getDescription() const = 0;
};

#endif // ISCANSTRATEGY_H
