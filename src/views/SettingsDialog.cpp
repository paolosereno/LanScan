#include "views/SettingsDialog.h"
#include "ui_settingsdialog.h"
#include "utils/Logger.h"
#include <QMessageBox>

SettingsDialog::SettingsDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::SettingsDialog)
    , settings(new QSettings("LanScan", "LanScan", this))
    , modified(false) {
    ui->setupUi(this);

    setupTabGeneral();
    setupTabNetwork();
    setupTabAppearance();
    setupTabNotifications();
    setupTabAdvanced();

    setupConnections();
    loadSettings();

    setModified(false);
}

SettingsDialog::~SettingsDialog() {
    delete ui;
}

void SettingsDialog::setupConnections() {
    // Dialog buttons
    connect(ui->buttonBox, &QDialogButtonBox::clicked, [this](QAbstractButton* button) {
        if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
            onApplyClicked();
        } else if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::AcceptRole) {
            onOkClicked();
        } else if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::RejectRole) {
            onCancelClicked();
        }
    });

    connect(ui->restoreDefaultsButton, &QPushButton::clicked,
            this, &SettingsDialog::onRestoreDefaultsClicked);

    // General tab
    connect(ui->startWithSystemCheck, &QCheckBox::toggled,
            this, &SettingsDialog::onStartWithSystemChanged);
    connect(ui->minimizeToTrayCheck, &QCheckBox::toggled,
            this, &SettingsDialog::onMinimizeToTrayChanged);
    connect(ui->closeToTrayCheck, &QCheckBox::toggled,
            this, &SettingsDialog::onCloseToTrayChanged);
    connect(ui->languageCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SettingsDialog::onLanguageChanged);

    // Network tab
    connect(ui->timeoutSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &SettingsDialog::onTimeoutChanged);
    connect(ui->threadsSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &SettingsDialog::onThreadsChanged);
    connect(ui->subnetEdit, &QLineEdit::textChanged,
            this, &SettingsDialog::onSubnetChanged);
    connect(ui->pingCountSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &SettingsDialog::onPingCountChanged);
    connect(ui->pingIntervalSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &SettingsDialog::onPingIntervalChanged);

    // Appearance tab
    connect(ui->themeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SettingsDialog::onThemeChanged);
    connect(ui->fontSizeSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &SettingsDialog::onFontSizeChanged);

    // Notifications tab
    connect(ui->enableAlertsCheck, &QCheckBox::toggled,
            this, &SettingsDialog::onEnableAlertsChanged);
    connect(ui->alertSoundCheck, &QCheckBox::toggled,
            this, &SettingsDialog::onAlertSoundChanged);
    connect(ui->systemNotificationsCheck, &QCheckBox::toggled,
            this, &SettingsDialog::onSystemNotificationsChanged);
    connect(ui->latencyThresholdSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &SettingsDialog::onLatencyThresholdChanged);
    connect(ui->packetLossThresholdSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &SettingsDialog::onPacketLossThresholdChanged);
    connect(ui->jitterThresholdSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &SettingsDialog::onJitterThresholdChanged);

    // Advanced tab
    connect(ui->historyRetentionSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &SettingsDialog::onHistoryRetentionChanged);
    connect(ui->metricsRetentionSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &SettingsDialog::onMetricsRetentionChanged);
    connect(ui->logLevelCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SettingsDialog::onLogLevelChanged);
    connect(ui->logToFileCheck, &QCheckBox::toggled,
            this, &SettingsDialog::onEnableFileLoggingChanged);
}

void SettingsDialog::setupTabGeneral() {
    // Language is already set in UI file
    // Just add additional languages if needed
    ui->languageCombo->addItem("Español", "es");
    ui->languageCombo->addItem("Français", "fr");
    ui->languageCombo->addItem("Deutsch", "de");
}

void SettingsDialog::setupTabNetwork() {
    // Timeout: 100-10000 ms
    ui->timeoutSpin->setRange(100, 10000);
    ui->timeoutSpin->setSingleStep(100);
    ui->timeoutSpin->setSuffix(" ms");

    // Threads: 1-16
    ui->threadsSpin->setRange(1, 16);

    // Ping count: 1-10
    ui->pingCountSpin->setRange(1, 10);

    // Ping interval: 100-5000 ms
    ui->pingIntervalSpin->setRange(100, 5000);
    ui->pingIntervalSpin->setSingleStep(100);
    ui->pingIntervalSpin->setSuffix(" ms");
}

void SettingsDialog::setupTabAppearance() {
    // Theme options
    ui->themeCombo->addItem("System Default", "system");
    ui->themeCombo->addItem("Light", "light");
    ui->themeCombo->addItem("Dark", "dark");

    // Font size: 8-24 pt
    ui->fontSizeSpin->setRange(8, 24);
    ui->fontSizeSpin->setSuffix(" pt");
}

void SettingsDialog::setupTabNotifications() {
    // Latency threshold: 0-1000 ms
    ui->latencyThresholdSpin->setRange(0, 1000);
    ui->latencyThresholdSpin->setSuffix(" ms");

    // Packet loss threshold: 0-100% (using int for QSpinBox)
    ui->packetLossThresholdSpin->setRange(0, 100);
    ui->packetLossThresholdSpin->setSuffix(" %");

    // Jitter threshold: 0-100 ms (using int for QSpinBox)
    ui->jitterThresholdSpin->setRange(0, 100);
    ui->jitterThresholdSpin->setSuffix(" ms");
}

void SettingsDialog::setupTabAdvanced() {
    // History retention: 1-365 days
    ui->historyRetentionSpin->setRange(1, 365);
    ui->historyRetentionSpin->setSuffix(" days");

    // Metrics retention: 1-90 days
    ui->metricsRetentionSpin->setRange(1, 90);
    ui->metricsRetentionSpin->setSuffix(" days");

    // Log levels
    ui->logLevelCombo->addItem("Debug", static_cast<int>(Logger::DEBUG));
    ui->logLevelCombo->addItem("Info", static_cast<int>(Logger::INFO));
    ui->logLevelCombo->addItem("Warning", static_cast<int>(Logger::WARN));
    ui->logLevelCombo->addItem("Error", static_cast<int>(Logger::ERROR));
}

void SettingsDialog::loadSettings() {
    // General tab
    ui->startWithSystemCheck->setChecked(
        settings->value("General/StartWithSystem", false).toBool());
    ui->minimizeToTrayCheck->setChecked(
        settings->value("General/MinimizeToTray", false).toBool());
    ui->closeToTrayCheck->setChecked(
        settings->value("General/CloseToTray", false).toBool());

    QString language = settings->value("General/Language", "en").toString();
    int langIndex = ui->languageCombo->findData(language);
    if (langIndex >= 0) {
        ui->languageCombo->setCurrentIndex(langIndex);
    }

    // Network tab
    ui->timeoutSpin->setValue(
        settings->value("Network/Timeout", 1000).toInt());
    ui->threadsSpin->setValue(
        settings->value("Network/Threads", 4).toInt());
    ui->subnetEdit->setText(
        settings->value("Network/DefaultSubnet", "192.168.1.0/24").toString());
    ui->pingCountSpin->setValue(
        settings->value("Network/PingCount", 4).toInt());
    ui->pingIntervalSpin->setValue(
        settings->value("Network/PingInterval", 1000).toInt());

    // Appearance tab
    QString theme = settings->value("Appearance/Theme", "system").toString();
    int themeIndex = ui->themeCombo->findData(theme);
    if (themeIndex >= 0) {
        ui->themeCombo->setCurrentIndex(themeIndex);
    }
    ui->fontSizeSpin->setValue(
        settings->value("Appearance/FontSize", 10).toInt());

    // Notifications tab
    ui->enableAlertsCheck->setChecked(
        settings->value("Notifications/EnableAlerts", true).toBool());
    ui->alertSoundCheck->setChecked(
        settings->value("Notifications/AlertSound", true).toBool());
    ui->systemNotificationsCheck->setChecked(
        settings->value("Notifications/SystemNotifications", false).toBool());
    ui->latencyThresholdSpin->setValue(
        settings->value("Notifications/LatencyThreshold", 100).toInt());
    ui->packetLossThresholdSpin->setValue(
        settings->value("Notifications/PacketLossThreshold", 5).toInt());
    ui->jitterThresholdSpin->setValue(
        settings->value("Notifications/JitterThreshold", 10).toInt());

    // Advanced tab
    ui->historyRetentionSpin->setValue(
        settings->value("Advanced/HistoryRetention", 30).toInt());
    ui->metricsRetentionSpin->setValue(
        settings->value("Advanced/MetricsRetention", 7).toInt());

    int logLevel = settings->value("Advanced/LogLevel", static_cast<int>(Logger::INFO)).toInt();
    int logLevelIndex = ui->logLevelCombo->findData(logLevel);
    if (logLevelIndex >= 0) {
        ui->logLevelCombo->setCurrentIndex(logLevelIndex);
    }
    ui->logToFileCheck->setChecked(
        settings->value("Advanced/EnableFileLogging", true).toBool());
}

void SettingsDialog::saveSettings() {
    // General tab
    settings->setValue("General/StartWithSystem",
                      ui->startWithSystemCheck->isChecked());
    settings->setValue("General/MinimizeToTray",
                      ui->minimizeToTrayCheck->isChecked());
    settings->setValue("General/CloseToTray",
                      ui->closeToTrayCheck->isChecked());
    settings->setValue("General/Language",
                      ui->languageCombo->currentData().toString());

    // Network tab
    settings->setValue("Network/Timeout",
                      ui->timeoutSpin->value());
    settings->setValue("Network/Threads",
                      ui->threadsSpin->value());
    settings->setValue("Network/DefaultSubnet",
                      ui->subnetEdit->text());
    settings->setValue("Network/PingCount",
                      ui->pingCountSpin->value());
    settings->setValue("Network/PingInterval",
                      ui->pingIntervalSpin->value());

    // Appearance tab
    settings->setValue("Appearance/Theme",
                      ui->themeCombo->currentData().toString());
    settings->setValue("Appearance/FontSize",
                      ui->fontSizeSpin->value());

    // Notifications tab
    settings->setValue("Notifications/EnableAlerts",
                      ui->enableAlertsCheck->isChecked());
    settings->setValue("Notifications/AlertSound",
                      ui->alertSoundCheck->isChecked());
    settings->setValue("Notifications/SystemNotifications",
                      ui->systemNotificationsCheck->isChecked());
    settings->setValue("Notifications/LatencyThreshold",
                      ui->latencyThresholdSpin->value());
    settings->setValue("Notifications/PacketLossThreshold",
                      ui->packetLossThresholdSpin->value());
    settings->setValue("Notifications/JitterThreshold",
                      ui->jitterThresholdSpin->value());

    // Advanced tab
    settings->setValue("Advanced/HistoryRetention",
                      ui->historyRetentionSpin->value());
    settings->setValue("Advanced/MetricsRetention",
                      ui->metricsRetentionSpin->value());
    settings->setValue("Advanced/LogLevel",
                      ui->logLevelCombo->currentData().toInt());
    settings->setValue("Advanced/EnableFileLogging",
                      ui->logToFileCheck->isChecked());

    settings->sync();
}

void SettingsDialog::applySettings() {
    if (!validateSettings()) {
        return;
    }

    saveSettings();
    emit settingsApplied();

    Logger::info("Settings applied successfully");
    setModified(false);
}

void SettingsDialog::restoreDefaults() {
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Restore Defaults",
        "Are you sure you want to restore all settings to their default values?",
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No);

    if (reply != QMessageBox::Yes) {
        return;
    }

    // General tab defaults
    ui->startWithSystemCheck->setChecked(false);
    ui->minimizeToTrayCheck->setChecked(false);
    ui->closeToTrayCheck->setChecked(false);
    ui->languageCombo->setCurrentIndex(0); // English

    // Network tab defaults
    ui->timeoutSpin->setValue(1000);
    ui->threadsSpin->setValue(4);
    ui->subnetEdit->setText("192.168.1.0/24");
    ui->pingCountSpin->setValue(4);
    ui->pingIntervalSpin->setValue(1000);

    // Appearance tab defaults
    ui->themeCombo->setCurrentIndex(0); // System
    ui->fontSizeSpin->setValue(10);

    // Notifications tab defaults
    ui->enableAlertsCheck->setChecked(true);
    ui->alertSoundCheck->setChecked(true);
    ui->systemNotificationsCheck->setChecked(false);
    ui->latencyThresholdSpin->setValue(100);
    ui->packetLossThresholdSpin->setValue(5);
    ui->jitterThresholdSpin->setValue(10);

    // Advanced tab defaults
    ui->historyRetentionSpin->setValue(30);
    ui->metricsRetentionSpin->setValue(7);
    ui->logLevelCombo->setCurrentIndex(1); // Info
    ui->logToFileCheck->setChecked(true);

    setModified(true);
}

bool SettingsDialog::validateSettings() {
    // Validate subnet format (basic check)
    QString subnet = ui->subnetEdit->text();
    if (!subnet.contains('/')) {
        QMessageBox::warning(this, "Invalid Subnet",
                           "Subnet must be in CIDR notation (e.g., 192.168.1.0/24)");
        ui->tabWidget->setCurrentIndex(1); // Switch to Network tab
        ui->subnetEdit->setFocus();
        return false;
    }

    return true;
}

void SettingsDialog::setModified(bool mod) {
    modified = mod;
    updateApplyButton();
}

void SettingsDialog::updateApplyButton() {
    QPushButton* applyButton = ui->buttonBox->button(QDialogButtonBox::Apply);
    if (applyButton) {
        applyButton->setEnabled(modified);
    }
}

// Button handlers
void SettingsDialog::onApplyClicked() {
    applySettings();
}

void SettingsDialog::onOkClicked() {
    if (modified) {
        applySettings();
    }
    accept();
}

void SettingsDialog::onCancelClicked() {
    if (modified) {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this,
            "Discard Changes",
            "You have unsaved changes. Do you want to discard them?",
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::No);

        if (reply != QMessageBox::Yes) {
            return;
        }
    }
    reject();
}

void SettingsDialog::onRestoreDefaultsClicked() {
    restoreDefaults();
}

// General tab slots
void SettingsDialog::onStartWithSystemChanged(bool checked) {
    Q_UNUSED(checked);
    setModified(true);
}

void SettingsDialog::onMinimizeToTrayChanged(bool checked) {
    Q_UNUSED(checked);
    setModified(true);
}

void SettingsDialog::onCloseToTrayChanged(bool checked) {
    Q_UNUSED(checked);
    setModified(true);
}

void SettingsDialog::onLanguageChanged(int index) {
    Q_UNUSED(index);
    setModified(true);
}

// Network tab slots
void SettingsDialog::onTimeoutChanged(int value) {
    Q_UNUSED(value);
    setModified(true);
}

void SettingsDialog::onThreadsChanged(int value) {
    Q_UNUSED(value);
    setModified(true);
}

void SettingsDialog::onSubnetChanged(const QString& text) {
    Q_UNUSED(text);
    setModified(true);
}

void SettingsDialog::onPingCountChanged(int value) {
    Q_UNUSED(value);
    setModified(true);
}

void SettingsDialog::onPingIntervalChanged(int value) {
    Q_UNUSED(value);
    setModified(true);
}

// Appearance tab slots
void SettingsDialog::onThemeChanged(int index) {
    Q_UNUSED(index);
    setModified(true);
}

void SettingsDialog::onFontSizeChanged(int value) {
    Q_UNUSED(value);
    setModified(true);
}

// Notifications tab slots
void SettingsDialog::onEnableAlertsChanged(bool checked) {
    // Enable/disable dependent controls
    ui->alertSoundCheck->setEnabled(checked);
    ui->systemNotificationsCheck->setEnabled(checked);
    ui->latencyThresholdSpin->setEnabled(checked);
    ui->packetLossThresholdSpin->setEnabled(checked);
    ui->jitterThresholdSpin->setEnabled(checked);
    setModified(true);
}

void SettingsDialog::onAlertSoundChanged(bool checked) {
    Q_UNUSED(checked);
    setModified(true);
}

void SettingsDialog::onSystemNotificationsChanged(bool checked) {
    Q_UNUSED(checked);
    setModified(true);
}

void SettingsDialog::onLatencyThresholdChanged(int value) {
    Q_UNUSED(value);
    setModified(true);
}

void SettingsDialog::onPacketLossThresholdChanged(int value) {
    Q_UNUSED(value);
    setModified(true);
}

void SettingsDialog::onJitterThresholdChanged(int value) {
    Q_UNUSED(value);
    setModified(true);
}

// Advanced tab slots
void SettingsDialog::onHistoryRetentionChanged(int value) {
    Q_UNUSED(value);
    setModified(true);
}

void SettingsDialog::onMetricsRetentionChanged(int value) {
    Q_UNUSED(value);
    setModified(true);
}

void SettingsDialog::onLogLevelChanged(int index) {
    Q_UNUSED(index);
    setModified(true);
}

void SettingsDialog::onEnableFileLoggingChanged(bool checked) {
    Q_UNUSED(checked);
    setModified(true);
}
