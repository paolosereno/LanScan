#ifndef SCANCONFIGVIEWMODEL_H
#define SCANCONFIGVIEWMODEL_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QList>
#include "coordinators/ScanCoordinator.h"

class IpAddressValidator;
class NetworkInterfaceDetector;

/**
 * @brief ViewModel for scan configuration dialog
 *
 * Manages scan configuration state and validation.
 * Provides preset configurations (Quick/Deep/Custom).
 */
class ScanConfigViewModel : public QObject {
    Q_OBJECT

public:
    enum ScanType {
        Quick,
        Deep,
        Custom
    };
    Q_ENUM(ScanType)

    explicit ScanConfigViewModel(QObject* parent = nullptr);

    // Getters
    QString getSubnet() const;
    ScanType getScanType() const;
    int getThreadCount() const;
    int getTimeout() const;
    bool isResolveDns() const;
    bool isResolveArp() const;
    bool isScanPorts() const;
    QList<int> getPortsToScan() const;

    // Setters
    void setSubnet(const QString& subnet);
    void setScanType(ScanType type);
    void setThreadCount(int count);
    void setTimeout(int timeout);
    void setResolveDns(bool resolve);
    void setResolveArp(bool resolve);
    void setScanPorts(bool scan);
    void setPortsToScan(const QList<int>& ports);

    // Validation
    bool isSubnetValid() const;
    QString getSubnetError() const;
    bool isValid() const;

    // Presets
    void loadQuickScanPreset();
    void loadDeepScanPreset();
    void loadCustomScanPreset();
    void loadPreset(ScanType type);

    // Network detection
    QStringList detectLocalNetworks();

    // Convert to ScanCoordinator config
    ScanCoordinator::ScanConfig toScanConfig() const;

signals:
    void subnetChanged(const QString& subnet);
    void scanTypeChanged(ScanType type);
    void validationChanged(bool isValid);

private:
    QString subnet;
    ScanType scanType;
    int threadCount;
    int timeout;
    bool resolveDns;
    bool resolveArp;
    bool scanPorts;
    QList<int> portsToScan;

    NetworkInterfaceDetector* interfaceDetector;

    void updateValidation();
};

#endif // SCANCONFIGVIEWMODEL_H
