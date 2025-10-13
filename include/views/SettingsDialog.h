#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QSettings>

namespace Ui {
class SettingsDialog;
}

/**
 * @brief Dialog for managing application settings
 *
 * Provides a comprehensive settings interface with 5 tabs:
 * - General: Startup options, language
 * - Network: Scan defaults, ping settings
 * - Appearance: Theme, font size
 * - Notifications: Alert settings and thresholds
 * - Advanced: Database retention, logging
 *
 * All settings are persisted using QSettings and integrated with SettingsManager.
 */
class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget* parent = nullptr);
    ~SettingsDialog();

signals:
    /**
     * @brief Emitted when settings have been applied
     */
    void settingsApplied();

private slots:
    // Button handlers
    void onApplyClicked();
    void onOkClicked();
    void onCancelClicked();
    void onRestoreDefaultsClicked();

    // General tab
    void onStartWithSystemChanged(bool checked);
    void onMinimizeToTrayChanged(bool checked);
    void onCloseToTrayChanged(bool checked);
    void onLanguageChanged(int index);

    // Network tab
    void onTimeoutChanged(int value);
    void onThreadsChanged(int value);
    void onSubnetChanged(const QString& text);
    void onPingCountChanged(int value);
    void onPingIntervalChanged(int value);

    // Appearance tab
    void onThemeChanged(int index);
    void onFontSizeChanged(int value);

    // Notifications tab
    void onEnableAlertsChanged(bool checked);
    void onAlertSoundChanged(bool checked);
    void onSystemNotificationsChanged(bool checked);
    void onLatencyThresholdChanged(int value);
    void onPacketLossThresholdChanged(int value);
    void onJitterThresholdChanged(int value);

    // Advanced tab
    void onHistoryRetentionChanged(int value);
    void onMetricsRetentionChanged(int value);
    void onLogLevelChanged(int index);
    void onEnableFileLoggingChanged(bool checked);

private:
    Ui::SettingsDialog* ui;
    QSettings* settings;
    bool modified;

    // Original values for cancel/revert
    QString m_originalTheme;
    int m_originalFontSize;

    // Initialization
    void setupConnections();
    void loadSettings();
    void setupTabGeneral();
    void setupTabNetwork();
    void setupTabAppearance();
    void setupTabNotifications();
    void setupTabAdvanced();

    // Settings management
    void saveSettings();
    void applySettings();
    void restoreDefaults();

    // Validation
    bool validateSettings();

    // UI state management
    void setModified(bool modified);
    void updateApplyButton();

    // Appearance helpers
    void storeOriginalAppearance();
    void restoreOriginalAppearance();
};

#endif // SETTINGSDIALOG_H
