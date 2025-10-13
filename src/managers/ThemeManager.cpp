#include "managers/ThemeManager.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QRegularExpression>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

ThemeManager::ThemeManager()
    : m_currentTheme(Theme::System)
    , m_effectiveTheme(Theme::Light)
    , m_fontSize(10)  // Default font size
{
    // Initialize with system theme detection
    setTheme(Theme::System);
}

ThemeManager& ThemeManager::instance() {
    static ThemeManager instance;
    return instance;
}

void ThemeManager::setTheme(Theme theme) {
    m_currentTheme = theme;

    // Resolve effective theme
    bool useDarkTheme = false;
    if (theme == Theme::System) {
        useDarkTheme = detectSystemDarkMode();
        m_effectiveTheme = useDarkTheme ? Theme::Dark : Theme::Light;
    } else {
        useDarkTheme = (theme == Theme::Dark);
        m_effectiveTheme = theme;
    }

    // Apply theme to application
    applyThemeToApplication(useDarkTheme);

    // Emit signal
    emit themeChanged(m_effectiveTheme);

    qDebug() << "ThemeManager: Theme changed to" << themeToString(m_effectiveTheme)
             << "(requested:" << themeToString(theme) << ")";
}

bool ThemeManager::detectSystemDarkMode() const {
#ifdef Q_OS_WIN
    // Windows: Read registry key for Apps Use Light Theme
    // HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Themes\Personalize
    // AppsUseLightTheme = 0 (Dark) or 1 (Light)

    HKEY hKey;
    LONG result = RegOpenKeyExW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        0,
        KEY_READ,
        &hKey
    );

    if (result != ERROR_SUCCESS) {
        qWarning() << "ThemeManager: Failed to open registry key, defaulting to Light theme";
        return false;
    }

    DWORD value = 1; // Default to light
    DWORD size = sizeof(DWORD);
    result = RegQueryValueExW(
        hKey,
        L"AppsUseLightTheme",
        nullptr,
        nullptr,
        reinterpret_cast<LPBYTE>(&value),
        &size
    );

    RegCloseKey(hKey);

    if (result != ERROR_SUCCESS) {
        qWarning() << "ThemeManager: Failed to read registry value, defaulting to Light theme";
        return false;
    }

    // AppsUseLightTheme = 0 means dark mode
    bool isDark = (value == 0);
    qDebug() << "ThemeManager: System theme detected -" << (isDark ? "Dark" : "Light")
             << "(AppsUseLightTheme =" << value << ")";
    return isDark;

#elif defined(Q_OS_LINUX)
    // Linux: Future implementation - check GTK/KDE settings
    // For now, default to light theme
    qDebug() << "ThemeManager: Linux system theme detection not implemented, defaulting to Light";
    return false;

#elif defined(Q_OS_MACOS)
    // macOS: Future implementation - check NSAppearance
    // For now, default to light theme
    qDebug() << "ThemeManager: macOS system theme detection not implemented, defaulting to Light";
    return false;

#else
    qDebug() << "ThemeManager: Unknown platform, defaulting to Light theme";
    return false;
#endif
}

void ThemeManager::applyThemeToApplication(bool isDark) {
    QString stylesheetPath = isDark ? ":/styles/dark.qss" : ":/styles/light.qss";
    qDebug() << "ThemeManager: Attempting to load stylesheet from" << stylesheetPath;
    QString stylesheet = loadStylesheet(stylesheetPath);

    if (stylesheet.isEmpty()) {
        qWarning() << "ThemeManager: Failed to load stylesheet from" << stylesheetPath;
        // Clear any existing stylesheet
        qApp->setStyleSheet("");
        return;
    }

    // Replace font-size in stylesheet with current font size
    QString fontSizePattern = "font-size: \\d+pt;";
    QString newFontSize = QString("font-size: %1pt;").arg(m_fontSize);
    stylesheet.replace(QRegularExpression(fontSizePattern), newFontSize);

    // Apply stylesheet to application
    qApp->setStyleSheet(stylesheet);
    qDebug() << "ThemeManager: Successfully applied stylesheet from" << stylesheetPath
             << "with font size" << m_fontSize << "pt"
             << "(" << stylesheet.length() << " chars)";
}

QString ThemeManager::loadStylesheet(const QString& resourcePath) const {
    QFile file(resourcePath);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qWarning() << "ThemeManager: Failed to open stylesheet file:" << resourcePath
                   << "- Error:" << file.errorString();
        return QString();
    }

    QTextStream stream(&file);
    QString stylesheet = stream.readAll();
    file.close();

    return stylesheet;
}

void ThemeManager::setFontSize(int fontSize) {
    if (fontSize < 8 || fontSize > 24) {
        qWarning() << "ThemeManager: Invalid font size" << fontSize << "(valid range: 8-24)";
        return;
    }

    m_fontSize = fontSize;

    // Reapply current theme with new font size
    bool useDarkTheme = (m_effectiveTheme == Theme::Dark);
    applyThemeToApplication(useDarkTheme);

    qDebug() << "ThemeManager: Font size changed to" << fontSize << "pt";
}

QString ThemeManager::themeToString(Theme theme) {
    switch (theme) {
        case Theme::Light:
            return "Light";
        case Theme::Dark:
            return "Dark";
        case Theme::System:
            return "System";
        default:
            return "Unknown";
    }
}

ThemeManager::Theme ThemeManager::stringToTheme(const QString& str) {
    QString lower = str.toLower();
    if (lower == "light") {
        return Theme::Light;
    } else if (lower == "dark") {
        return Theme::Dark;
    } else if (lower == "system") {
        return Theme::System;
    } else {
        qWarning() << "ThemeManager: Unknown theme string:" << str << "- defaulting to System";
        return Theme::System;
    }
}
