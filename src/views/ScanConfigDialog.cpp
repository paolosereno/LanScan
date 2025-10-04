#include "views/ScanConfigDialog.h"
#include "ui_scanconfigdialog.h"
#include "viewmodels/ScanConfigViewModel.h"
#include "../utils/Logger.h"
#include <QMessageBox>
#include <QInputDialog>

ScanConfigDialog::ScanConfigDialog(ScanConfigViewModel* viewModel, QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::ScanConfigDialog)
    , viewModel(viewModel)
{
    ui->setupUi(this);

    // Disconnect the automatic accept/reject connections from .ui file
    disconnect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    disconnect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    setupValidators();
    setupConnections();
    loadConfigFromViewModel();

    Logger::info("ScanConfigDialog initialized");
}

ScanConfigDialog::~ScanConfigDialog() {
    delete ui;
}

ScanCoordinator::ScanConfig ScanConfigDialog::getConfiguration() const {
    return viewModel->toScanConfig();
}

void ScanConfigDialog::setupValidators() {
    // Thread count and timeout are already configured in .ui file
    // We just need to set initial values
    ui->threadSpinBox->setValue(viewModel->getThreadCount());
    ui->timeoutSpinBox->setValue(viewModel->getTimeout());
}

void ScanConfigDialog::setupConnections() {
    // Scan type combo box
    connect(ui->scanTypeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ScanConfigDialog::onScanTypeChanged);

    // Subnet input
    connect(ui->subnetLineEdit, &QLineEdit::textChanged,
            this, &ScanConfigDialog::onSubnetChanged);

    // Detect network button
    connect(ui->detectNetworkButton, &QPushButton::clicked,
            this, &ScanConfigDialog::onDetectNetwork);

    // Dialog buttons - connect to our custom handlers
    connect(ui->buttonBox, &QDialogButtonBox::accepted,
            this, &ScanConfigDialog::onAccepted);
    connect(ui->buttonBox, &QDialogButtonBox::rejected,
            this, &QDialog::reject);

    // ViewModel validation
    connect(viewModel, &ScanConfigViewModel::validationChanged,
            this, &ScanConfigDialog::onValidationChanged);
}

void ScanConfigDialog::onScanTypeChanged(int index) {
    ScanConfigViewModel::ScanType scanType = static_cast<ScanConfigViewModel::ScanType>(index);
    viewModel->setScanType(scanType);
    updateUiForScanType(index);
    loadConfigFromViewModel();
}

void ScanConfigDialog::onSubnetChanged(const QString& subnet) {
    viewModel->setSubnet(subnet);
}

void ScanConfigDialog::onDetectNetwork() {
    QStringList networks = viewModel->detectLocalNetworks();

    if (networks.isEmpty()) {
        QMessageBox::warning(this, tr("Network Detection"),
                            tr("No active network interfaces found."));
        return;
    }

    // Show selection dialog if multiple networks
    if (networks.size() == 1) {
        ui->subnetLineEdit->setText(networks.first());
        viewModel->setSubnet(networks.first());
    } else {
        bool ok;
        QString selected = QInputDialog::getItem(this,
            tr("Select Network"),
            tr("Multiple networks detected. Select one:"),
            networks, 0, false, &ok);

        if (ok && !selected.isEmpty()) {
            ui->subnetLineEdit->setText(selected);
            viewModel->setSubnet(selected);
        }
    }
}

void ScanConfigDialog::onAccepted() {
    // Save UI values to ViewModel before accepting
    saveConfigToViewModel();

    // Validate
    if (!viewModel->isValid()) {
        QMessageBox::warning(this, tr("Invalid Configuration"),
            viewModel->getSubnetError());
        return;
    }

    accept();
}

void ScanConfigDialog::onValidationChanged(bool isValid) {
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(isValid);
}

void ScanConfigDialog::updateUiForScanType(int scanType) {
    // Enable/disable options based on scan type
    bool isCustom = (scanType == ScanConfigViewModel::Custom);

    ui->threadSpinBox->setEnabled(isCustom);
    ui->timeoutSpinBox->setEnabled(isCustom);
    ui->resolveDnsCheckBox->setEnabled(isCustom);
    ui->resolveArpCheckBox->setEnabled(isCustom);
    ui->scanPortsCheckBox->setEnabled(isCustom);
}

void ScanConfigDialog::loadConfigFromViewModel() {
    // Load values from ViewModel to UI
    ui->subnetLineEdit->setText(viewModel->getSubnet());
    ui->scanTypeComboBox->setCurrentIndex(static_cast<int>(viewModel->getScanType()));
    ui->threadSpinBox->setValue(viewModel->getThreadCount());
    ui->timeoutSpinBox->setValue(viewModel->getTimeout());
    ui->resolveDnsCheckBox->setChecked(viewModel->isResolveDns());
    ui->resolveArpCheckBox->setChecked(viewModel->isResolveArp());
    ui->scanPortsCheckBox->setChecked(viewModel->isScanPorts());
}

void ScanConfigDialog::saveConfigToViewModel() {
    // Save UI values to ViewModel
    viewModel->setSubnet(ui->subnetLineEdit->text());
    viewModel->setScanType(static_cast<ScanConfigViewModel::ScanType>(
        ui->scanTypeComboBox->currentIndex()));
    viewModel->setThreadCount(ui->threadSpinBox->value());
    viewModel->setTimeout(ui->timeoutSpinBox->value());
    viewModel->setResolveDns(ui->resolveDnsCheckBox->isChecked());
    viewModel->setResolveArp(ui->resolveArpCheckBox->isChecked());
    viewModel->setScanPorts(ui->scanPortsCheckBox->isChecked());
}
