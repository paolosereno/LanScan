#include "managers/LanguageManager.h"
#include "utils/Logger.h"
#include <QApplication>
#include <QDir>

LanguageManager::LanguageManager(QObject* parent)
    : QObject(parent)
    , m_currentLanguage(English)
    , m_translator(new QTranslator(this))
{
    Logger::info("LanguageManager: Initialized");
}

LanguageManager::~LanguageManager() {
    removeCurrentTranslation();
}

LanguageManager& LanguageManager::instance() {
    static LanguageManager instance;
    return instance;
}

bool LanguageManager::setLanguage(Language language) {
    if (language == m_currentLanguage) {
        Logger::debug("LanguageManager: Language already set to " + languageToName(language));
        return true;
    }

    // Remove current translation
    removeCurrentTranslation();

    // English is the default, no translation file needed
    if (language == English) {
        m_currentLanguage = English;
        emit languageChanged(language);
        Logger::info("LanguageManager: Switched to English (default)");
        return true;
    }

    // Load new translation
    if (loadTranslation(language)) {
        m_currentLanguage = language;
        emit languageChanged(language);
        Logger::info("LanguageManager: Switched to " + languageToName(language));
        return true;
    }

    Logger::warn("LanguageManager: Failed to load translation for " + languageToName(language));
    return false;
}

LanguageManager::Language LanguageManager::currentLanguage() const {
    return m_currentLanguage;
}

QString LanguageManager::currentLanguageCode() const {
    return languageToCode(m_currentLanguage);
}

QString LanguageManager::currentLanguageName() const {
    return languageToName(m_currentLanguage);
}

QMap<LanguageManager::Language, QString> LanguageManager::availableLanguages() const {
    QMap<Language, QString> languages;
    languages[English] = languageToName(English);
    languages[Italian] = languageToName(Italian);
    languages[Spanish] = languageToName(Spanish);
    languages[French] = languageToName(French);
    languages[German] = languageToName(German);
    return languages;
}

QString LanguageManager::languageToCode(Language language) {
    switch (language) {
        case English: return "en";
        case Italian: return "it";
        case Spanish: return "es";
        case French: return "fr";
        case German: return "de";
        default: return "en";
    }
}

QString LanguageManager::languageToName(Language language) {
    switch (language) {
        case English: return "English";
        case Italian: return "Italiano";
        case Spanish: return "Español";
        case French: return "Français";
        case German: return "Deutsch";
        default: return "English";
    }
}

LanguageManager::Language LanguageManager::codeToLanguage(const QString& code) {
    if (code == "it") return Italian;
    if (code == "es") return Spanish;
    if (code == "fr") return French;
    if (code == "de") return German;
    return English; // Default
}

bool LanguageManager::loadTranslation(Language language) {
    QString languageCode = languageToCode(language);
    QString fileName = QString("lanscan_%1").arg(languageCode);

    // Try to load from translations directory
    QStringList searchPaths;
    searchPaths << QApplication::applicationDirPath() + "/translations";
    searchPaths << ":/translations";  // Qt resource path
    searchPaths << "./translations";   // Current directory

    for (const QString& path : searchPaths) {
        QString fullPath = path + "/" + fileName;

        if (m_translator->load(fileName, path)) {
            // Install translator
            if (QApplication::installTranslator(m_translator)) {
                Logger::info(QString("LanguageManager: Loaded translation from %1").arg(fullPath));
                return true;
            } else {
                Logger::warn(QString("LanguageManager: Failed to install translator for %1").arg(fileName));
                return false;
            }
        }
    }

    Logger::warn(QString("LanguageManager: Translation file not found: %1.qm").arg(fileName));
    return false;
}

void LanguageManager::removeCurrentTranslation() {
    if (m_translator && QApplication::removeTranslator(m_translator)) {
        Logger::debug("LanguageManager: Removed current translation");
    }
}
