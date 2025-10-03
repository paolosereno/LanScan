#ifndef TIMEFORMATTER_H
#define TIMEFORMATTER_H

#include <QString>
#include <QDateTime>

class TimeFormatter
{
public:
    static QString formatDuration(qint64 milliseconds);
    static QString formatTimestamp(const QDateTime& dt);
    static QString formatRelativeTime(const QDateTime& dt);
    static QString formatElapsedTime(const QDateTime& start, const QDateTime& end);

private:
    TimeFormatter() = delete;
};

#endif // TIMEFORMATTER_H
