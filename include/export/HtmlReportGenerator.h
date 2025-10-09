#ifndef HTMLREPORTGENERATOR_H
#define HTMLREPORTGENERATOR_H

#include "interfaces/IExporter.h"
#include <QString>

/**
 * @brief HTML report generator for device data
 *
 * Generates professional HTML reports with embedded CSS styling.
 * Includes summary statistics, device table, and visual quality indicators.
 */
class HtmlReportGenerator : public IExporter {
public:
    HtmlReportGenerator() = default;
    ~HtmlReportGenerator() override = default;

    bool exportData(const QList<Device>& devices, const QString& filepath) override;
    QString getFormatName() const override { return "HTML"; }
    QString getFileExtension() const override { return ".html"; }

private:
    QString generateHtml(const QList<Device>& devices);
    QString generateCss();
    QString generateHeader();
    QString generateSummary(const QList<Device>& devices);
    QString generateDeviceTable(const QList<Device>& devices);
    QString generateDeviceRow(const Device& device);
    QString generateFooter();
    QString getQualityColorClass(const QString& qualityString);
    QString formatPortsList(const QList<PortInfo>& ports);
    int countOnlineDevices(const QList<Device>& devices);
};

#endif // HTMLREPORTGENERATOR_H
