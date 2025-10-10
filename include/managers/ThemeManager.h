#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QObject>
#include <QString>
#include <QApplication>

/**
 * @class ThemeManager
 * @brief Singleton manager for application theme management
 *
 * Manages application-wide theme switching between Light, Dark, and System themes.
 * Provides automatic system theme detection on Windows and applies QSS stylesheets.
 *
 * Features:
 * - Light/Dark/System theme modes
 * - Windows system theme detection via registry
 * - QSS stylesheet loading from Qt resources
 * - Runtime theme switching without restart
 * - Signal emission on theme changes
 *
 * @author LanScan Team
 * @date 2025-10-10
 */
class ThemeManager : public QObject {
    Q_OBJECT

public:
    /**
     * @enum Theme
     * @brief Available theme modes
     */
    enum class Theme {
        Light,   ///< Light theme with bright colors
        Dark,    ///< Dark theme with dark colors
        System   ///< Follows system theme preference
    };
    Q_ENUM(Theme)

    /**
     * @brief Get singleton instance
     * @return Reference to ThemeManager instance
     */
    static ThemeManager& instance();

    /**
     * @brief Set application theme
     * @param theme Theme to apply
     *
     * If Theme::System is selected, automatically detects system preference.
     * Loads appropriate QSS stylesheet and applies to QApplication.
     */
    void setTheme(Theme theme);

    /**
     * @brief Get current theme
     * @return Current theme setting (Light/Dark/System)
     */
    Theme currentTheme() const { return m_currentTheme; }

    /**
     * @brief Get effective theme (resolved System to Light/Dark)
     * @return Actual theme being displayed
     */
    Theme effectiveTheme() const { return m_effectiveTheme; }

    /**
     * @brief Convert theme enum to string
     * @param theme Theme enum value
     * @return Theme name as string ("Light"/"Dark"/"System")
     */
    static QString themeToString(Theme theme);

    /**
     * @brief Convert string to theme enum
     * @param str Theme name string
     * @return Theme enum value (defaults to System if invalid)
     */
    static Theme stringToTheme(const QString& str);

signals:
    /**
     * @brief Emitted when theme changes
     * @param theme New theme that was applied
     */
    void themeChanged(Theme theme);

private:
    /**
     * @brief Private constructor (singleton pattern)
     */
    ThemeManager();

    /**
     * @brief Destructor
     */
    ~ThemeManager() override = default;

    // Disable copy and move
    ThemeManager(const ThemeManager&) = delete;
    ThemeManager& operator=(const ThemeManager&) = delete;
    ThemeManager(ThemeManager&&) = delete;
    ThemeManager& operator=(ThemeManager&&) = delete;

    /**
     * @brief Detect system theme preference
     * @return True if system uses dark mode, false for light mode
     *
     * Windows: Reads HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Themes\Personalize\AppsUseLightTheme
     * Linux/macOS: Defaults to Light (future: check system settings)
     */
    bool detectSystemDarkMode() const;

    /**
     * @brief Apply theme stylesheet to application
     * @param isDark True for dark theme, false for light theme
     *
     * Loads QSS file from Qt resources (:/styles/dark.qss or :/styles/light.qss)
     * and applies to QApplication::setStyleSheet()
     */
    void applyThemeToApplication(bool isDark);

    /**
     * @brief Load stylesheet from Qt resources
     * @param resourcePath Path to QSS file in resources (e.g., ":/styles/dark.qss")
     * @return Stylesheet content as QString
     */
    QString loadStylesheet(const QString& resourcePath) const;

    Theme m_currentTheme;    ///< User-selected theme (Light/Dark/System)
    Theme m_effectiveTheme;  ///< Actual theme being displayed (Light/Dark only)
};

#endif // THEMEMANAGER_H
