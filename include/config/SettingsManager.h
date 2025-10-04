#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QString>
#include <QSettings>
#include <QMutex>

class SettingsManager {
public:
    static SettingsManager* instance();

    // General Settings
    QString getDefaultSubnet() const;
    void setDefaultSubnet(const QString& subnet);

    // Scan Settings
    int getScanTimeout() const;
    void setScanTimeout(int ms);
    int getMaxThreads() const;
    void setMaxThreads(int threads);

    // UI Settings
    QString getTheme() const;
    void setTheme(const QString& theme);
    QString getLanguage() const;
    void setLanguage(const QString& lang);

    // Export Settings
    QString getDefaultExportPath() const;
    void setDefaultExportPath(const QString& path);
    QString getDefaultExportFormat() const;
    void setDefaultExportFormat(const QString& format);

    // Database Settings
    QString getDatabasePath() const;
    void setDatabasePath(const QString& path);

    // Persistence
    void save();
    void load();
    void reset();

private:
    SettingsManager();
    ~SettingsManager();
    SettingsManager(const SettingsManager&) = delete;
    SettingsManager& operator=(const SettingsManager&) = delete;

    void loadDefaults();

    QSettings* settings;
    static SettingsManager* _instance;
    static QMutex mutex;
};

#endif // SETTINGSMANAGER_H
