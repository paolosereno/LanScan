#include "views/DeviceTableWidget.h"
#include "ui_devicetablewidget.h"
#include "viewmodels/DeviceTableViewModel.h"
#include "delegates/StatusDelegate.h"
#include "delegates/QualityScoreDelegate.h"
#include "services/WakeOnLanService.h"
#include "../utils/Logger.h"
#include <QClipboard>
#include <QApplication>
#include <QMessageBox>

DeviceTableWidget::DeviceTableWidget(DeviceTableViewModel* viewModel, QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::DeviceTableWidget)
    , viewModel(viewModel)
    , proxyModel(new QSortFilterProxyModel(this))
    , contextMenu(nullptr)
    , wolService(nullptr)
{
    ui->setupUi(this);

    setupTableView();
    setupContextMenu();
    setupConnections();

    Logger::info("DeviceTableWidget initialized");
}

DeviceTableWidget::~DeviceTableWidget() {
    delete ui;
}

void DeviceTableWidget::setViewModel(DeviceTableViewModel* viewModel) {
    this->viewModel = viewModel;
    proxyModel->setSourceModel(viewModel);
}

void DeviceTableWidget::setWakeOnLanService(WakeOnLanService* service) {
    this->wolService = service;
    Logger::debug("WakeOnLanService set for DeviceTableWidget");
}

Device DeviceTableWidget::getSelectedDevice() const {
    QModelIndexList selection = ui->tableView->selectionModel()->selectedRows();
    if (selection.isEmpty()) {
        return Device();
    }

    // Map proxy index to source index
    QModelIndex proxyIndex = selection.first();
    QModelIndex sourceIndex = proxyModel->mapToSource(proxyIndex);

    return viewModel->getDeviceAt(sourceIndex.row());
}

QList<Device> DeviceTableWidget::getSelectedDevices() const {
    QList<Device> devices;
    QModelIndexList selection = ui->tableView->selectionModel()->selectedRows();

    for (const QModelIndex& proxyIndex : selection) {
        QModelIndex sourceIndex = proxyModel->mapToSource(proxyIndex);
        devices.append(viewModel->getDeviceAt(sourceIndex.row()));
    }

    return devices;
}

void DeviceTableWidget::setupTableView() {
    // Setup proxy model for sorting/filtering
    proxyModel->setSourceModel(viewModel);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    proxyModel->setFilterKeyColumn(-1); // Search all columns

    ui->tableView->setModel(proxyModel);

    // Set custom delegates
    ui->tableView->setItemDelegateForColumn(DeviceTableViewModel::Status,
                                           new StatusDelegate(this));
    ui->tableView->setItemDelegateForColumn(DeviceTableViewModel::QualityScore,
                                           new QualityScoreDelegate(this));

    // Configure view properties (already set in .ui file, but ensure they're correct)
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->setSortingEnabled(true);
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);

    // Column sizing
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->tableView->resizeColumnsToContents();

    // Row height
    ui->tableView->verticalHeader()->setDefaultSectionSize(30);
    ui->tableView->verticalHeader()->setVisible(false);
}

void DeviceTableWidget::setupContextMenu() {
    contextMenu = new QMenu(this);

    contextMenu->addAction(tr("Ping Device"), this, &DeviceTableWidget::onPingDevice);
    contextMenu->addAction(tr("Show Details"), this, &DeviceTableWidget::onShowDetails);
    contextMenu->addSeparator();
    contextMenu->addAction(tr("Wake on LAN"), this, &DeviceTableWidget::onWakeOnLan);
    contextMenu->addSeparator();
    contextMenu->addAction(tr("Add to Favorites"), this, &DeviceTableWidget::onAddToFavorites);
    contextMenu->addSeparator();
    contextMenu->addAction(tr("Copy IP Address"), this, &DeviceTableWidget::onCopyIpAddress);
    contextMenu->addSeparator();
    contextMenu->addAction(tr("Remove Device"), this, &DeviceTableWidget::onRemoveDevice);
}

void DeviceTableWidget::setupConnections() {
    // Search box filtering
    connect(ui->searchLineEdit, &QLineEdit::textChanged,
            this, &DeviceTableWidget::onSearchTextChanged);

    // Table view signals
    connect(ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &DeviceTableWidget::onSelectionChanged);
    connect(ui->tableView, &QTableView::doubleClicked,
            this, &DeviceTableWidget::onDoubleClicked);
    connect(ui->tableView, &QTableView::customContextMenuRequested,
            this, &DeviceTableWidget::onCustomContextMenuRequested);
}

void DeviceTableWidget::onSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {
    Q_UNUSED(deselected)

    if (selected.isEmpty()) {
        return;
    }

    Device device = getSelectedDevice();
    if (!device.getIp().isEmpty()) {
        emit deviceSelected(device);
    }
}

void DeviceTableWidget::onDoubleClicked(const QModelIndex& index) {
    QModelIndex sourceIndex = proxyModel->mapToSource(index);
    Device device = viewModel->getDeviceAt(sourceIndex.row());

    if (!device.getIp().isEmpty()) {
        emit deviceDoubleClicked(device);
    }
}

void DeviceTableWidget::onCustomContextMenuRequested(const QPoint& pos) {
    QModelIndex index = ui->tableView->indexAt(pos);
    if (!index.isValid()) {
        return;
    }

    QModelIndex sourceIndex = proxyModel->mapToSource(index);
    Device device = viewModel->getDeviceAt(sourceIndex.row());

    if (!device.getIp().isEmpty()) {
        emit contextMenuRequested(device, ui->tableView->viewport()->mapToGlobal(pos));
        contextMenu->exec(ui->tableView->viewport()->mapToGlobal(pos));
    }
}

void DeviceTableWidget::onSearchTextChanged(const QString& text) {
    proxyModel->setFilterFixedString(text);
}

void DeviceTableWidget::onPingDevice() {
    Device device = getSelectedDevice();
    if (device.getIp().isEmpty()) {
        return;
    }

    // Emit signal to MainWindow to show MetricsWidget
    emit pingDeviceRequested(device);
}

void DeviceTableWidget::onShowDetails() {
    Device device = getSelectedDevice();
    if (device.getIp().isEmpty()) {
        return;
    }

    emit deviceDoubleClicked(device);
}

void DeviceTableWidget::onWakeOnLan() {
    Device device = getSelectedDevice();
    if (device.getIp().isEmpty()) {
        return;
    }

    // Check if MAC address is available
    if (device.macAddress().isEmpty()) {
        QMessageBox::warning(this, tr("Wake on LAN"),
            tr("Cannot send Wake-on-LAN packet:\nMAC address is unknown for device %1")
            .arg(device.getIp()));
        return;
    }

    // Check if WoL service is available
    if (!wolService) {
        QMessageBox::critical(this, tr("Wake on LAN"),
            tr("Wake-on-LAN service is not available."));
        Logger::error("WakeOnLanService not set for DeviceTableWidget");
        return;
    }

    // Confirm action
    QString deviceName = device.hostname().isEmpty() ? device.getIp() : device.hostname();
    int result = QMessageBox::question(this, tr("Wake on LAN"),
        tr("Send Wake-on-LAN packet to:\n\n"
           "Device: %1\n"
           "IP: %2\n"
           "MAC: %3\n\n"
           "This will attempt to wake up the device if it supports Wake-on-LAN.")
           .arg(deviceName)
           .arg(device.getIp())
           .arg(device.macAddress()),
        QMessageBox::Yes | QMessageBox::No);

    if (result == QMessageBox::Yes) {
        // Send WoL packet
        wolService->sendWakeOnLanToDevice(device);

        // Show confirmation
        QMessageBox::information(this, tr("Wake on LAN"),
            tr("Wake-on-LAN packet sent to %1 (%2)")
            .arg(deviceName)
            .arg(device.macAddress()));

        Logger::info(QString("WoL packet sent from UI to %1 (%2)")
                    .arg(device.getIp())
                    .arg(device.macAddress()));
    }
}

void DeviceTableWidget::onAddToFavorites() {
    Device device = getSelectedDevice();
    if (device.getIp().isEmpty()) {
        return;
    }

    // Placeholder - will be implemented with FavoritesManager
    QMessageBox::information(this, tr("Add to Favorites"),
        tr("Device %1 added to favorites.\n\n(FavoritesManager integration pending)")
        .arg(device.getIp()));
}

void DeviceTableWidget::onRemoveDevice() {
    Device device = getSelectedDevice();
    if (device.getIp().isEmpty()) {
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        tr("Remove Device"),
        tr("Remove device %1 from the list?").arg(device.getIp()),
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        viewModel->removeDevice(device.getIp());
        Logger::info("Device removed: " + device.getIp());
    }
}

void DeviceTableWidget::onCopyIpAddress() {
    Device device = getSelectedDevice();
    if (device.getIp().isEmpty()) {
        return;
    }

    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(device.getIp());

    Logger::debug("IP address copied to clipboard: " + device.getIp());
}
