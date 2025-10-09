#include "controllers/ExportController.h"
#include "../models/Device.h"
#include "database/DeviceRepository.h"
#include "../interfaces/IExporter.h"
#include "../export/CsvExporter.h"
#include "../export/JsonExporter.h"
#include "../export/XmlExporter.h"
#include "../export/HtmlReportGenerator.h"
#include "../utils/Logger.h"

#include <QFile>
#include <QFileInfo>

ExportController::ExportController(
    DeviceRepository* repository,
    QObject* parent
)
    : QObject(parent)
    , repository(repository)
{
    initializeExporters();
    Logger::info("ExportController initialized");
}

ExportController::~ExportController() {
    cleanupExporters();
}

void ExportController::exportDevices(ExportFormat format, const QString& filepath) {
    Logger::info("Exporting all devices to " + filepath +
                 " (format: " + formatToString(format) + ")");

    emit exportStarted();

    try {
        // Get all devices from repository
        QList<Device> devices = repository->findAll();

        if (devices.isEmpty()) {
            Logger::warn("No devices to export");
            emit exportError("No devices to export");
            return;
        }

        // Get appropriate exporter
        IExporter* exporter = getExporter(format);
        if (!exporter) {
            QString error = "Unsupported export format: " + formatToString(format);
            Logger::error(error);
            emit exportError(error);
            return;
        }

        // Execute export
        executeExport(devices, exporter, filepath);

        Logger::info("Export completed: " + QString::number(devices.size()) +
                     " devices exported to " + filepath);
        emit exportCompleted(filepath);

    } catch (const std::exception& e) {
        QString error = "Export failed: " + QString(e.what());
        Logger::error(error);
        emit exportError(error);
    }
}

void ExportController::exportDevice(const Device& device, ExportFormat format, const QString& filepath) {
    Logger::info("Exporting single device to " + filepath);

    QList<Device> devices;
    devices.append(device);

    exportFiltered(devices, format, filepath);
}

void ExportController::exportFiltered(const QList<Device>& devices, ExportFormat format, const QString& filepath) {
    Logger::info("Exporting " + QString::number(devices.size()) +
                 " filtered devices to " + filepath +
                 " (format: " + formatToString(format) + ")");

    emit exportStarted();

    try {
        if (devices.isEmpty()) {
            Logger::warn("No devices to export");
            emit exportError("No devices to export");
            return;
        }

        // Get appropriate exporter
        IExporter* exporter = getExporter(format);
        if (!exporter) {
            QString error = "Unsupported export format: " + formatToString(format);
            Logger::error(error);
            emit exportError(error);
            return;
        }

        // Execute export
        executeExport(devices, exporter, filepath);

        Logger::info("Filtered export completed: " + QString::number(devices.size()) +
                     " devices exported to " + filepath);
        emit exportCompleted(filepath);

    } catch (const std::exception& e) {
        QString error = "Export failed: " + QString(e.what());
        Logger::error(error);
        emit exportError(error);
    }
}

QStringList ExportController::getSupportedFormats() {
    return {"CSV", "JSON", "XML", "HTML"};
}

QString ExportController::formatToString(ExportFormat format) {
    switch (format) {
        case CSV:  return "CSV";
        case JSON: return "JSON";
        case XML:  return "XML";
        case HTML: return "HTML";
        default:   return "Unknown";
    }
}

ExportController::ExportFormat ExportController::stringToFormat(const QString& formatStr) {
    QString upper = formatStr.toUpper();
    if (upper == "CSV") return CSV;
    if (upper == "JSON") return JSON;
    if (upper == "XML") return XML;
    if (upper == "HTML") return HTML;
    return CSV; // Default
}

IExporter* ExportController::getExporter(ExportFormat format) {
    if (exporters.contains(format)) {
        return exporters[format];
    }
    return nullptr;
}

void ExportController::executeExport(const QList<Device>& devices, IExporter* exporter, const QString& filepath) {
    emit exportProgress(10);

    // Export devices using IExporter interface
    bool success = exporter->exportData(devices, filepath);

    if (!success) {
        throw std::runtime_error("Export failed");
    }

    emit exportProgress(100);

    Logger::info("Data exported to file: " + filepath);
}

void ExportController::initializeExporters() {
    // Initialize CSV exporter
    exporters[CSV] = new CsvExporter();

    // Initialize JSON exporter
    exporters[JSON] = new JsonExporter();

    // Initialize XML exporter (Phase 8.2)
    exporters[XML] = new XmlExporter();

    // Initialize HTML report generator (Phase 8.2)
    exporters[HTML] = new HtmlReportGenerator();

    Logger::debug("Exporters initialized: CSV, JSON, XML, HTML");
}

void ExportController::cleanupExporters() {
    for (IExporter* exporter : exporters.values()) {
        delete exporter;
    }
    exporters.clear();
}
