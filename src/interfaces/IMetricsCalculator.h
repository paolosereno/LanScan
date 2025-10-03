#ifndef IMETRICSCALCULATOR_H
#define IMETRICSCALCULATOR_H

#include <QVector>

class IMetricsCalculator
{
public:
    virtual ~IMetricsCalculator() = default;

    virtual double calculate(const QVector<double>& data) = 0;
    virtual QString getCalculatorName() const = 0;
};

#endif // IMETRICSCALCULATOR_H
