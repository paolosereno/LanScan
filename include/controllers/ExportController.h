#ifndef EXPORTCONTROLLER_H
#define EXPORTCONTROLLER_H

#include <QObject>
#include <QString>
#include <QList>
#include <QMap>

class Device;
class DeviceRepository;
class IExporter;

/**
 * @brief Controls export operations for devices and metrics
 */
class ExportController : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Export format enumeration
     */
    enum ExportFormat {
        CSV,   ///< Comma-separated values
        JSON,  ///< JavaScript Object Notation
        XML,   ///< Extensible Markup Language
        HTML   ///< HyperText Markup Language
    };
    Q_ENUM(ExportFormat)

    /**
     * @brief Constructor
     * @param repository Device repository instance
     * @param parent Parent QObject
     */
    explicit ExportController(
        DeviceRepository* repository,
        QObject* parent = nullptr
    );

    /**
     * @brief Destructor
     */
    ~ExportController();

    /**
     * @brief Export all devices from repository
     * @param format Export format
     * @param filepath Output file path
     */
    void exportDevices(ExportFormat format, const QString& filepath);

    /**
     * @brief Export a single device
     * @param device Device to export
     * @param format Export format
     * @param filepath Output file path
     */
    void exportDevice(const Device& device, ExportFormat format, const QString& filepath);

    /**
     * @brief Export a filtered list of devices
     * @param devices List of devices to export
     * @param format Export format
     * @param filepath Output file path
     */
    void exportFiltered(const QList<Device>& devices, ExportFormat format, const QString& filepath);

    /**
     * @brief Get supported export formats
     * @return List of supported format names
     */
    static QStringList getSupportedFormats();

    /**
     * @brief Convert format enum to string
     * @param format Export format
     * @return Format name as string
     */
    static QString formatToString(ExportFormat format);

    /**
     * @brief Convert string to format enum
     * @param formatStr Format name as string
     * @return Export format enum
     */
    static ExportFormat stringToFormat(const QString& formatStr);

signals:
    /**
     * @brief Emitted when export operation starts
     */
    void exportStarted();

    /**
     * @brief Emitted to report export progress
     * @param percentage Progress percentage (0-100)
     */
    void exportProgress(int percentage);

    /**
     * @brief Emitted when export operation completes successfully
     * @param filepath Path to exported file
     */
    void exportCompleted(const QString& filepath);

    /**
     * @brief Emitted when export operation fails
     * @param error Error message
     */
    void exportError(const QString& error);

private:
    DeviceRepository* repository;
    QMap<ExportFormat, IExporter*> exporters;

    IExporter* getExporter(ExportFormat format);
    void executeExport(const QList<Device>& devices, IExporter* exporter, const QString& filepath);
    void initializeExporters();
    void cleanupExporters();
};

#endif // EXPORTCONTROLLER_H
