#ifndef SCANCONTROLLER_H
#define SCANCONTROLLER_H

#include <QObject>
#include <QString>
#include <QList>
#include "coordinators/ScanCoordinator.h"

class Device;
class DeviceRepository;
class DeviceCache;

/**
 * @brief Controls scan workflow and device management
 */
class ScanController : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param coordinator Scan coordinator instance
     * @param repository Device repository instance
     * @param cache Device cache instance
     * @param parent Parent QObject
     */
    explicit ScanController(
        ScanCoordinator* coordinator,
        DeviceRepository* repository,
        DeviceCache* cache,
        QObject* parent = nullptr
    );

    /**
     * @brief Execute a quick scan (ping + DNS only)
     * @param subnet Target subnet in CIDR notation
     */
    void executeQuickScan(const QString& subnet);

    /**
     * @brief Execute a deep scan (ping + DNS + ARP + common ports)
     * @param subnet Target subnet in CIDR notation
     */
    void executeDeepScan(const QString& subnet);

    /**
     * @brief Execute a custom scan with specific configuration
     * @param config Custom scan configuration
     */
    void executeCustomScan(const ScanCoordinator::ScanConfig& config);

    /**
     * @brief Stop the current scan operation
     */
    void stopCurrentScan();

    /**
     * @brief Pause the current scan operation
     */
    void pauseCurrentScan();

    /**
     * @brief Resume a paused scan operation
     */
    void resumeCurrentScan();

    /**
     * @brief Get all devices from cache and repository
     * @return List of all devices
     */
    QList<Device> getAllDevices();

    /**
     * @brief Clear all devices from cache and repository
     */
    void clearAllDevices();

signals:
    /**
     * @brief Emitted when a new scan is about to start
     * @param totalHosts Total number of hosts to scan
     */
    void scanStarted(int totalHosts);

    /**
     * @brief Emitted when scan status changes
     * @param status Status message
     */
    void scanStatusChanged(const QString& status);

    /**
     * @brief Emitted when devices list is updated
     */
    void devicesUpdated();

    /**
     * @brief Emitted when a specific device is discovered or updated
     * @param device The discovered/updated device
     */
    void deviceDiscovered(const Device& device);

    /**
     * @brief Emitted for scan progress updates
     * @param current Current progress
     * @param total Total items
     * @param percentage Progress percentage
     */
    void scanProgressUpdated(int current, int total, double percentage);

private slots:
    void onScanStarted(int totalHosts);
    void onDeviceDiscovered(const Device& device);
    void onScanProgress(int current, int total, const QString& currentIp);
    void onScanCompleted(int count, qint64 duration);
    void onScanError(const QString& error);
    void onScanPaused();
    void onScanResumed();

private:
    ScanCoordinator* coordinator;
    DeviceRepository* repository;
    DeviceCache* cache;

    ScanCoordinator::ScanConfig createQuickScanConfig(const QString& subnet);
    ScanCoordinator::ScanConfig createDeepScanConfig(const QString& subnet);

    void saveDevice(const Device& device);
    void connectSignals();
};

#endif // SCANCONTROLLER_H
