#ifndef STRINGFORMATTER_H
#define STRINGFORMATTER_H

#include <QString>

class StringFormatter
{
public:
    static QString formatMacAddress(const QString& mac);
    static QString formatBytes(qint64 bytes);
    static QString formatSpeed(double mbps);
    static QString formatLatency(double milliseconds);
    static QString formatPercentage(double value);
    static QString truncateString(const QString& str, int maxLength);

private:
    StringFormatter() = delete;
};

#endif // STRINGFORMATTER_H
