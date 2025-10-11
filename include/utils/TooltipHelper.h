#ifndef TOOLTIPHELPER_H
#define TOOLTIPHELPER_H

#include <QString>
#include "../models/Device.h"
#include "../models/NetworkMetrics.h"

/**
 * @brief Utility class for creating rich HTML tooltips
 *
 * TooltipHelper provides static methods for generating formatted
 * HTML tooltips for various UI elements.
 */
class TooltipHelper {
public:
    /**
     * @brief Creates a rich tooltip for a device
     * @param device Device to create tooltip for
     * @return HTML-formatted tooltip string
     */
    static QString createDeviceTooltip(const Device& device);

    /**
     * @brief Creates a tooltip for network metrics
     * @param metrics Network metrics to display
     * @return HTML-formatted tooltip string
     */
    static QString createMetricsTooltip(const NetworkMetrics& metrics);

    /**
     * @brief Creates a tooltip for a scan action
     * @param scanType Type of scan (e.g., "Quick", "Deep")
     * @param description Scan description
     * @return HTML-formatted tooltip string
     */
    static QString createScanTooltip(const QString& scanType, const QString& description);

    /**
     * @brief Creates a tooltip for an export action
     * @param format Export format (e.g., "CSV", "JSON")
     * @param description Format description
     * @return HTML-formatted tooltip string
     */
    static QString createExportTooltip(const QString& format, const QString& description);

    /**
     * @brief Creates a generic rich tooltip with title and body
     * @param title Tooltip title
     * @param body Tooltip body text
     * @return HTML-formatted tooltip string
     */
    static QString createRichTooltip(const QString& title, const QString& body);

    /**
     * @brief Creates a tooltip with a list of items
     * @param title Tooltip title
     * @param items List of items to display
     * @return HTML-formatted tooltip string
     */
    static QString createListTooltip(const QString& title, const QStringList& items);

private:
    TooltipHelper() = delete; // Utility class, no instantiation

    /**
     * @brief Formats a table row for tooltips
     * @param label Row label
     * @param value Row value
     * @return HTML table row
     */
    static QString formatTableRow(const QString& label, const QString& value);

    /**
     * @brief Gets quality color based on score
     * @param qualityScore Quality score (0.0-1.0)
     * @return HTML color code
     */
    static QString getQualityColor(double qualityScore);
};

#endif // TOOLTIPHELPER_H
