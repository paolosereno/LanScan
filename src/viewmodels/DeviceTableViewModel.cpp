#include "viewmodels/DeviceTableViewModel.h"
#include "database/DeviceRepository.h"
#include "../models/PortInfo.h"
#include "../models/NetworkMetrics.h"
#include "../utils/Logger.h"

DeviceTableViewModel::DeviceTableViewModel(DeviceRepository* repository, QObject* parent)
    : QAbstractTableModel(parent)
    , repository(repository)
{
    Logger::info("DeviceTableViewModel initialized");
}

int DeviceTableViewModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid())
        return 0;
    return devices.count();
}

int DeviceTableViewModel::columnCount(const QModelIndex& parent) const {
    if (parent.isValid())
        return 0;
    return ColumnCount;
}

QVariant DeviceTableViewModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= devices.count())
        return QVariant();

    const Device& device = devices.at(index.row());

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case Status:
                return getStatusIcon(device.isOnline());
            case IpAddress:
                return device.getIp();
            case Hostname:
                return device.getHostname().isEmpty() ? tr("Unknown") : device.getHostname();
            case MacAddress:
                return device.getMacAddress().isEmpty() ? tr("N/A") : device.getMacAddress();
            case Vendor:
                return device.getVendor().isEmpty() ? tr("Unknown") : device.getVendor();
            case OpenPorts:
                return formatOpenPorts(device.getOpenPorts());
            case Latency:
                return formatLatency(device.getMetrics());
            case QualityScore:
                return device.getMetrics().getQualityScoreString();
            default:
                return QVariant();
        }
    }
    else if (role == Qt::TextAlignmentRole) {
        switch (index.column()) {
            case Status:
            case Latency:
            case QualityScore:
                return static_cast<int>(Qt::AlignCenter);
            default:
                return static_cast<int>(Qt::AlignLeft | Qt::AlignVCenter);
        }
    }
    else if (role == Qt::ForegroundRole) {
        // Quality Score column always uses its specific color (even when offline)
        if (index.column() == QualityScore) {
            return QColor(getQualityColor(device.getMetrics().getQualityScore()));
        }
        // Other columns become gray when device is offline
        if (!device.isOnline()) {
            return QColor(Qt::gray);
        }
    }
    else if (role == Qt::UserRole) {
        // Store the full device object for easy retrieval
        return QVariant::fromValue(device);
    }

    return QVariant();
}

QVariant DeviceTableViewModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
            case Status:        return tr("Status");
            case IpAddress:     return tr("IP Address");
            case Hostname:      return tr("Hostname");
            case MacAddress:    return tr("MAC Address");
            case Vendor:        return tr("Vendor");
            case OpenPorts:     return tr("Open Ports");
            case Latency:       return tr("Latency");
            case QualityScore:  return tr("Quality");
            default:            return QVariant();
        }
    }

    return QVariant();
}

void DeviceTableViewModel::loadDevices() {
    beginResetModel();

    if (repository) {
        devices = repository->findAll();
        Logger::info("Loaded " + QString::number(devices.count()) + " devices from repository");
    } else {
        devices.clear();
        Logger::warn("Repository is null, cannot load devices");
    }

    endResetModel();
    emit deviceCountChanged(devices.count());
}

void DeviceTableViewModel::addDevice(const Device& device) {
    // Check if device already exists
    int existingRow = findDeviceRow(device.getIp());

    if (existingRow >= 0) {
        // Update existing device
        updateDevice(device);
        return;
    }

    // Add new device
    int row = devices.count();
    beginInsertRows(QModelIndex(), row, row);
    devices.append(device);
    endInsertRows();

    Logger::debug("Device added to table: " + device.getIp());
    emit deviceCountChanged(devices.count());
}

void DeviceTableViewModel::updateDevice(const Device& device) {
    int row = findDeviceRow(device.getIp());

    if (row < 0) {
        Logger::warn("Device not found for update: " + device.getIp());
        return;
    }

    devices[row] = device;

    // Notify view that this row has changed
    QModelIndex topLeft = index(row, 0);
    QModelIndex bottomRight = index(row, ColumnCount - 1);
    emit dataChanged(topLeft, bottomRight);

    Logger::debug("Device updated in table: " + device.getIp());
}

void DeviceTableViewModel::removeDevice(const QString& ip) {
    int row = findDeviceRow(ip);

    if (row < 0) {
        Logger::warn("Device not found for removal: " + ip);
        return;
    }

    beginRemoveRows(QModelIndex(), row, row);
    devices.removeAt(row);
    endRemoveRows();

    Logger::debug("Device removed from table: " + ip);
    emit deviceCountChanged(devices.count());
}

void DeviceTableViewModel::clear() {
    beginResetModel();
    devices.clear();
    endResetModel();

    Logger::info("Device table cleared");
    emit deviceCountChanged(0);
}

void DeviceTableViewModel::markAllDevicesOffline() {
    for (int i = 0; i < devices.count(); ++i) {
        devices[i].setOnline(false);
    }

    // Notify view that all rows have changed
    if (devices.count() > 0) {
        QModelIndex topLeft = index(0, 0);
        QModelIndex bottomRight = index(devices.count() - 1, ColumnCount - 1);
        emit dataChanged(topLeft, bottomRight);
    }

    Logger::info("All devices marked as offline");
}

Device DeviceTableViewModel::getDeviceAt(int row) const {
    if (row < 0 || row >= devices.count()) {
        Logger::warn("Invalid row index: " + QString::number(row));
        return Device();
    }
    return devices.at(row);
}

int DeviceTableViewModel::findDeviceRow(const QString& ip) const {
    for (int i = 0; i < devices.count(); ++i) {
        if (devices.at(i).getIp() == ip) {
            return i;
        }
    }
    return -1;
}

QString DeviceTableViewModel::getStatusIcon(bool isOnline) const {
    return isOnline ? "●" : "○";  // Filled/empty circle
}

QColor DeviceTableViewModel::getQualityColor(int score) const {
    // Map enum values to colors
    switch (score) {
        case NetworkMetrics::Excellent:  // 0
            return QColor(0, 200, 0);         // Green
        case NetworkMetrics::Good:           // 1
            return QColor(150, 200, 0);       // Yellow-Green
        case NetworkMetrics::Fair:           // 2
            return QColor(255, 165, 0);       // Orange
        case NetworkMetrics::Poor:           // 3
            return QColor(255, 100, 0);       // Dark Orange
        case NetworkMetrics::Critical:       // 4
        default:
            return QColor(255, 0, 0);         // Red
    }
}

QString DeviceTableViewModel::formatOpenPorts(const QList<PortInfo>& ports) const {
    if (ports.isEmpty()) {
        return tr("None");
    }

    if (ports.count() <= 3) {
        // Show first 3 ports directly
        QStringList portStrings;
        for (const PortInfo& port : ports) {
            portStrings.append(QString::number(port.getPort()));
        }
        return portStrings.join(", ");
    }

    // Show count if more than 3
    return tr("%1 ports").arg(ports.count());
}

QString DeviceTableViewModel::formatLatency(const NetworkMetrics& metrics) const {
    double avgLatency = metrics.getLatencyAvg();

    if (avgLatency <= 0) {
        return tr("N/A");
    }

    return QString::number(avgLatency, 'f', 1) + " ms";
}
