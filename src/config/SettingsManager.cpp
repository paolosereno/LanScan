#include "config/SettingsManager.h"
#include "utils/Logger.h"
#include <QMutexLocker>
#include <QStandardPaths>
#include <QDir>

SettingsManager* SettingsManager::_instance = nullptr;
QMutex SettingsManager::mutex;

SettingsManager::SettingsManager() {
    settings = new QSettings("LanScan", "LanScan");
    loadDefaults();
}

SettingsManager::~SettingsManager() {
    delete settings;
}

SettingsManager* SettingsManager::instance() {
    if (_instance == nullptr) {
        QMutexLocker locker(&mutex);
        if (_instance == nullptr) {
            _instance = new SettingsManager();
        }
    }
    return _instance;
}

// General Settings
QString SettingsManager::getDefaultSubnet() const {
    return settings->value("general/default_subnet", "192.168.1.0/24").toString();
}

void SettingsManager::setDefaultSubnet(const QString& subnet) {
    settings->setValue("general/default_subnet", subnet);
}

// Scan Settings
int SettingsManager::getScanTimeout() const {
    return settings->value("scan/timeout", 1000).toInt();
}

void SettingsManager::setScanTimeout(int ms) {
    settings->setValue("scan/timeout", ms);
}

int SettingsManager::getMaxThreads() const {
    return settings->value("scan/max_threads", 10).toInt();
}

void SettingsManager::setMaxThreads(int threads) {
    settings->setValue("scan/max_threads", threads);
}

// UI Settings
QString SettingsManager::getTheme() const {
    return settings->value("ui/theme", "light").toString();
}

void SettingsManager::setTheme(const QString& theme) {
    settings->setValue("ui/theme", theme);
}

QString SettingsManager::getLanguage() const {
    return settings->value("ui/language", "en").toString();
}

void SettingsManager::setLanguage(const QString& lang) {
    settings->setValue("ui/language", lang);
}

// Export Settings
QString SettingsManager::getDefaultExportPath() const {
    QString defaultPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    return settings->value("export/default_path", defaultPath).toString();
}

void SettingsManager::setDefaultExportPath(const QString& path) {
    settings->setValue("export/default_path", path);
}

QString SettingsManager::getDefaultExportFormat() const {
    return settings->value("export/default_format", "CSV").toString();
}

void SettingsManager::setDefaultExportFormat(const QString& format) {
    settings->setValue("export/default_format", format);
}

// Database Settings
QString SettingsManager::getDatabasePath() const {
    QString defaultPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/lanscan.db";
    return settings->value("database/path", defaultPath).toString();
}

void SettingsManager::setDatabasePath(const QString& path) {
    settings->setValue("database/path", path);
}

// Persistence
void SettingsManager::save() {
    settings->sync();
    Logger::info("SettingsManager: Settings saved");
}

void SettingsManager::load() {
    settings->sync();
    Logger::info("SettingsManager: Settings loaded");
}

void SettingsManager::reset() {
    settings->clear();
    loadDefaults();
    Logger::info("SettingsManager: Settings reset to defaults");
}

void SettingsManager::loadDefaults() {
    // Set defaults if not already set
    if (!settings->contains("general/default_subnet")) {
        setDefaultSubnet("192.168.1.0/24");
    }
    if (!settings->contains("scan/timeout")) {
        setScanTimeout(1000);
    }
    if (!settings->contains("scan/max_threads")) {
        setMaxThreads(10);
    }
    if (!settings->contains("ui/theme")) {
        setTheme("light");
    }
    if (!settings->contains("ui/language")) {
        setLanguage("en");
    }
    if (!settings->contains("export/default_format")) {
        setDefaultExportFormat("CSV");
    }

    // Ensure database directory exists
    QString dbPath = getDatabasePath();
    QDir().mkpath(QFileInfo(dbPath).absolutePath());
}
