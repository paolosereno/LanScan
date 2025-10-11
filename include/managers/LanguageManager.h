#ifndef LANGUAGEMANAGER_H
#define LANGUAGEMANAGER_H

#include <QObject>
#include <QTranslator>
#include <QString>
#include <QMap>

/**
 * @brief Singleton manager for application localization
 *
 * The LanguageManager handles loading and switching between different
 * language translations at runtime using Qt's translation system.
 *
 * Supported languages:
 * - English (en) - Default
 * - Italiano (it)
 * - Español (es)
 * - Français (fr)
 * - Deutsch (de)
 */
class LanguageManager : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Supported languages
     */
    enum Language {
        English,   ///< English (default)
        Italian,   ///< Italiano
        Spanish,   ///< Español
        French,    ///< Français
        German     ///< Deutsch
    };
    Q_ENUM(Language)

    /**
     * @brief Gets the singleton instance
     * @return Reference to the singleton instance
     */
    static LanguageManager& instance();

    /**
     * @brief Sets the application language
     * @param language Language to set
     * @return True if language was loaded successfully
     */
    bool setLanguage(Language language);

    /**
     * @brief Gets the current language
     * @return Current language enum
     */
    Language currentLanguage() const;

    /**
     * @brief Gets the current language code (e.g., "en", "it")
     * @return Language code string
     */
    QString currentLanguageCode() const;

    /**
     * @brief Gets the current language display name (e.g., "English", "Italiano")
     * @return Language display name
     */
    QString currentLanguageName() const;

    /**
     * @brief Gets all available languages
     * @return Map of language enum to display name
     */
    QMap<Language, QString> availableLanguages() const;

    /**
     * @brief Converts language enum to language code
     * @param language Language enum
     * @return Language code (e.g., "en", "it")
     */
    static QString languageToCode(Language language);

    /**
     * @brief Converts language enum to display name
     * @param language Language enum
     * @return Display name (e.g., "English", "Italiano")
     */
    static QString languageToName(Language language);

    /**
     * @brief Converts language code to enum
     * @param code Language code (e.g., "en", "it")
     * @return Language enum
     */
    static Language codeToLanguage(const QString& code);

signals:
    /**
     * @brief Emitted when the language changes
     * @param language New language
     */
    void languageChanged(Language language);

private:
    explicit LanguageManager(QObject* parent = nullptr);
    ~LanguageManager();

    // Singleton pattern - delete copy/move
    LanguageManager(const LanguageManager&) = delete;
    LanguageManager& operator=(const LanguageManager&) = delete;
    LanguageManager(LanguageManager&&) = delete;
    LanguageManager& operator=(LanguageManager&&) = delete;

    /**
     * @brief Loads a translation file
     * @param language Language to load
     * @return True if loaded successfully
     */
    bool loadTranslation(Language language);

    /**
     * @brief Removes current translation
     */
    void removeCurrentTranslation();

    Language m_currentLanguage;      ///< Current active language
    QTranslator* m_translator;       ///< Qt translator for .qm files
};

#endif // LANGUAGEMANAGER_H
