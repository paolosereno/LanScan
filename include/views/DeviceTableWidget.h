#ifndef DEVICETABLEWIDGET_H
#define DEVICETABLEWIDGET_H

#include <QWidget>
#include <QTableView>
#include <QMenu>
#include <QSortFilterProxyModel>
#include "../models/Device.h"

class DeviceTableViewModel;

QT_BEGIN_NAMESPACE
namespace Ui { class DeviceTableWidget; }
QT_END_NAMESPACE

/**
 * @brief Device table widget (View)
 *
 * Displays devices in a table with sorting, filtering, and context menu.
 */
class DeviceTableWidget : public QWidget {
    Q_OBJECT

public:
    explicit DeviceTableWidget(DeviceTableViewModel* viewModel, QWidget* parent = nullptr);
    ~DeviceTableWidget();

    void setViewModel(DeviceTableViewModel* viewModel);
    Device getSelectedDevice() const;
    QList<Device> getSelectedDevices() const;

signals:
    void deviceSelected(const Device& device);
    void deviceDoubleClicked(const Device& device);
    void contextMenuRequested(const Device& device, const QPoint& pos);

private slots:
    void onSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
    void onDoubleClicked(const QModelIndex& index);
    void onCustomContextMenuRequested(const QPoint& pos);
    void onSearchTextChanged(const QString& text);

    // Context menu actions
    void onPingDevice();
    void onShowDetails();
    void onAddToFavorites();
    void onRemoveDevice();
    void onCopyIpAddress();

private:
    Ui::DeviceTableWidget* ui;
    DeviceTableViewModel* viewModel;
    QSortFilterProxyModel* proxyModel;
    QMenu* contextMenu;

    void setupTableView();
    void setupContextMenu();
    void setupConnections();
};

#endif // DEVICETABLEWIDGET_H
