#include "views/BandwidthTestDialog.h"
#include "ui_bandwidthtestdialog.h"

BandwidthTestDialog::BandwidthTestDialog(const QString& targetIp, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::BandwidthTestDialog)
{
    ui->setupUi(this);

    // Set target IP (read-only)
    ui->targetLineEdit->setText(targetIp);

    // Set window properties
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setModal(true);
}

BandwidthTestDialog::~BandwidthTestDialog()
{
    delete ui;
}

BandwidthTestDialog::TestConfig BandwidthTestDialog::getConfig() const
{
    TestConfig config;

    config.targetIp = ui->targetLineEdit->text();
    config.port = static_cast<quint16>(ui->portSpinBox->value());
    config.durationSeconds = ui->durationSpinBox->value();

    // Map protocol combo box to enum
    if (ui->protocolComboBox->currentText() == "TCP") {
        config.protocol = BandwidthTester::TCP;
    } else {
        config.protocol = BandwidthTester::UDP;
    }

    // Map direction combo box to enum
    if (ui->directionComboBox->currentText() == "Download") {
        config.direction = BandwidthTester::Download;
    } else {
        config.direction = BandwidthTester::Upload;
    }

    // Convert KB to bytes
    config.packetSizeKB = ui->packetSizeSpinBox->value();

    return config;
}

void BandwidthTestDialog::setPort(int port)
{
    if (port >= 1 && port <= 65535) {
        ui->portSpinBox->setValue(port);
    }
}

void BandwidthTestDialog::setDuration(int seconds)
{
    if (seconds >= 1 && seconds <= 60) {
        ui->durationSpinBox->setValue(seconds);
    }
}

void BandwidthTestDialog::setProtocol(BandwidthTester::Protocol protocol)
{
    if (protocol == BandwidthTester::TCP) {
        ui->protocolComboBox->setCurrentIndex(0);
    } else {
        ui->protocolComboBox->setCurrentIndex(1);
    }
}

void BandwidthTestDialog::setDirection(BandwidthTester::Direction direction)
{
    if (direction == BandwidthTester::Download) {
        ui->directionComboBox->setCurrentIndex(0);
    } else {
        ui->directionComboBox->setCurrentIndex(1);
    }
}

void BandwidthTestDialog::setPacketSize(int sizeKB)
{
    if (sizeKB >= 1 && sizeKB <= 1024) {
        ui->packetSizeSpinBox->setValue(sizeKB);
    }
}
