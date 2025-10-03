#include "StringFormatter.h"
#include <QRegularExpression>

QString StringFormatter::formatMacAddress(const QString& mac)
{
    // Remove all separators
    QString cleaned = mac;
    cleaned.remove(QRegularExpression("[:-]"));

    // Validate length
    if (cleaned.length() != 12) {
        return mac; // Return original if invalid
    }

    // Format as AA:BB:CC:DD:EE:FF
    QString formatted;
    for (int i = 0; i < 12; i += 2) {
        if (i > 0) {
            formatted += ":";
        }
        formatted += cleaned.mid(i, 2).toUpper();
    }

    return formatted;
}

QString StringFormatter::formatBytes(qint64 bytes)
{
    const qint64 KB = 1024;
    const qint64 MB = KB * 1024;
    const qint64 GB = MB * 1024;
    const qint64 TB = GB * 1024;

    if (bytes >= TB) {
        return QString("%1 TB").arg(bytes / (double)TB, 0, 'f', 2);
    } else if (bytes >= GB) {
        return QString("%1 GB").arg(bytes / (double)GB, 0, 'f', 2);
    } else if (bytes >= MB) {
        return QString("%1 MB").arg(bytes / (double)MB, 0, 'f', 2);
    } else if (bytes >= KB) {
        return QString("%1 KB").arg(bytes / (double)KB, 0, 'f', 2);
    } else {
        return QString("%1 B").arg(bytes);
    }
}

QString StringFormatter::formatSpeed(double mbps)
{
    if (mbps >= 1000.0) {
        return QString("%1 Gbps").arg(mbps / 1000.0, 0, 'f', 2);
    } else if (mbps >= 1.0) {
        return QString("%1 Mbps").arg(mbps, 0, 'f', 2);
    } else {
        return QString("%1 Kbps").arg(mbps * 1000.0, 0, 'f', 2);
    }
}

QString StringFormatter::formatLatency(double milliseconds)
{
    if (milliseconds >= 1000.0) {
        return QString("%1 s").arg(milliseconds / 1000.0, 0, 'f', 2);
    } else {
        return QString("%1 ms").arg(milliseconds, 0, 'f', 2);
    }
}

QString StringFormatter::formatPercentage(double value)
{
    return QString("%1%").arg(value, 0, 'f', 1);
}

QString StringFormatter::truncateString(const QString& str, int maxLength)
{
    if (str.length() <= maxLength) {
        return str;
    }

    return str.left(maxLength - 3) + "...";
}
