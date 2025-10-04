#ifndef SCANCONFIGDIALOG_H
#define SCANCONFIGDIALOG_H

#include <QDialog>
#include "coordinators/ScanCoordinator.h"

class ScanConfigViewModel;

QT_BEGIN_NAMESPACE
namespace Ui { class ScanConfigDialog; }
QT_END_NAMESPACE

/**
 * @brief Scan configuration dialog (View)
 *
 * Allows user to configure scan parameters before starting a scan.
 */
class ScanConfigDialog : public QDialog {
    Q_OBJECT

public:
    explicit ScanConfigDialog(ScanConfigViewModel* viewModel, QWidget* parent = nullptr);
    ~ScanConfigDialog();

    ScanCoordinator::ScanConfig getConfiguration() const;

private slots:
    void onScanTypeChanged(int index);
    void onSubnetChanged(const QString& subnet);
    void onDetectNetwork();
    void onAccepted();
    void onValidationChanged(bool isValid);

private:
    Ui::ScanConfigDialog* ui;
    ScanConfigViewModel* viewModel;

    void setupValidators();
    void setupConnections();
    void updateUiForScanType(int scanType);
    void loadConfigFromViewModel();
    void saveConfigToViewModel();
};

#endif // SCANCONFIGDIALOG_H
