#ifndef ISCANSTRATEGY_H
#define ISCANSTRATEGY_H

#include <QString>
#include <QObject>

class IScanStrategy
{
public:
    virtual ~IScanStrategy() = default;

    virtual void scan(const QString& target) = 0;
    virtual void stop() = 0;
    virtual QString getStrategyName() const = 0;
    virtual bool isRunning() const = 0;
};

#endif // ISCANSTRATEGY_H
