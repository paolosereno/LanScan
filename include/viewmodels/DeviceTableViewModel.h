#ifndef DEVICETABLEVIEWMODEL_H
#define DEVICETABLEVIEWMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include <QColor>
#include "../models/Device.h"

class DeviceRepository;

/**
 * @brief ViewModel for device table (MVVM pattern)
 *
 * Provides a Qt model interface for displaying devices in a QTableView.
 * Integrates with DeviceRepository for data persistence.
 */
class DeviceTableViewModel : public QAbstractTableModel {
    Q_OBJECT

public:
    enum Column {
        Status = 0,
        IpAddress,
        Hostname,
        MacAddress,
        Vendor,
        OpenPorts,
        Latency,
        QualityScore,
        ColumnCount
    };

    explicit DeviceTableViewModel(DeviceRepository* repository, QObject* parent = nullptr);

    // QAbstractTableModel interface
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Custom methods
    void loadDevices();
    void addDevice(const Device& device);
    void updateDevice(const Device& device);
    void removeDevice(const QString& ip);
    void clear();

    Device getDeviceAt(int row) const;
    int findDeviceRow(const QString& ip) const;

signals:
    void deviceCountChanged(int count);

private:
    DeviceRepository* repository;
    QList<Device> devices;

    QString getStatusIcon(bool isOnline) const;
    QColor getQualityColor(int score) const;
    QString formatOpenPorts(const QList<PortInfo>& ports) const;
    QString formatLatency(const NetworkMetrics& metrics) const;
};

#endif // DEVICETABLEVIEWMODEL_H
