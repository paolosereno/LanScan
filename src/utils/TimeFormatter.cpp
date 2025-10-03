#include "TimeFormatter.h"

QString TimeFormatter::formatDuration(qint64 milliseconds)
{
    qint64 seconds = milliseconds / 1000;
    qint64 minutes = seconds / 60;
    qint64 hours = minutes / 60;
    qint64 days = hours / 24;

    if (days > 0) {
        return QString("%1d %2h").arg(days).arg(hours % 24);
    } else if (hours > 0) {
        return QString("%1h %2m").arg(hours).arg(minutes % 60);
    } else if (minutes > 0) {
        return QString("%1m %2s").arg(minutes).arg(seconds % 60);
    } else if (seconds > 0) {
        return QString("%1s").arg(seconds);
    } else {
        return QString("%1ms").arg(milliseconds);
    }
}

QString TimeFormatter::formatTimestamp(const QDateTime& dt)
{
    return dt.toString("yyyy-MM-dd hh:mm:ss");
}

QString TimeFormatter::formatRelativeTime(const QDateTime& dt)
{
    QDateTime now = QDateTime::currentDateTime();
    qint64 secondsAgo = dt.secsTo(now);

    if (secondsAgo < 0) {
        return "in the future";
    } else if (secondsAgo < 60) {
        return "just now";
    } else if (secondsAgo < 3600) {
        int minutes = secondsAgo / 60;
        return QString("%1 minute%2 ago").arg(minutes).arg(minutes > 1 ? "s" : "");
    } else if (secondsAgo < 86400) {
        int hours = secondsAgo / 3600;
        return QString("%1 hour%2 ago").arg(hours).arg(hours > 1 ? "s" : "");
    } else if (secondsAgo < 604800) {
        int days = secondsAgo / 86400;
        return QString("%1 day%2 ago").arg(days).arg(days > 1 ? "s" : "");
    } else {
        return dt.toString("yyyy-MM-dd");
    }
}

QString TimeFormatter::formatElapsedTime(const QDateTime& start, const QDateTime& end)
{
    qint64 milliseconds = start.msecsTo(end);
    return formatDuration(milliseconds);
}
