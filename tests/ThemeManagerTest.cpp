#include <QTest>
#include <QApplication>
#include <QSignalSpy>
#include "managers/ThemeManager.h"

/**
 * @brief Unit tests for ThemeManager
 *
 * Tests theme switching, system theme detection, and stylesheet loading.
 */
class ThemeManagerTest : public QObject {
    Q_OBJECT

private slots:
    /**
     * @brief Test singleton instance
     */
    void testSingleton() {
        ThemeManager& instance1 = ThemeManager::instance();
        ThemeManager& instance2 = ThemeManager::instance();
        QCOMPARE(&instance1, &instance2);
    }

    /**
     * @brief Test theme to string conversion
     */
    void testThemeToString() {
        QCOMPARE(ThemeManager::themeToString(ThemeManager::Theme::Light), QString("Light"));
        QCOMPARE(ThemeManager::themeToString(ThemeManager::Theme::Dark), QString("Dark"));
        QCOMPARE(ThemeManager::themeToString(ThemeManager::Theme::System), QString("System"));
    }

    /**
     * @brief Test string to theme conversion
     */
    void testStringToTheme() {
        QCOMPARE(ThemeManager::stringToTheme("Light"), ThemeManager::Theme::Light);
        QCOMPARE(ThemeManager::stringToTheme("light"), ThemeManager::Theme::Light);
        QCOMPARE(ThemeManager::stringToTheme("Dark"), ThemeManager::Theme::Dark);
        QCOMPARE(ThemeManager::stringToTheme("dark"), ThemeManager::Theme::Dark);
        QCOMPARE(ThemeManager::stringToTheme("System"), ThemeManager::Theme::System);
        QCOMPARE(ThemeManager::stringToTheme("system"), ThemeManager::Theme::System);

        // Invalid strings should default to System
        QCOMPARE(ThemeManager::stringToTheme("Invalid"), ThemeManager::Theme::System);
        QCOMPARE(ThemeManager::stringToTheme(""), ThemeManager::Theme::System);
    }

    /**
     * @brief Test setting light theme
     */
    void testSetLightTheme() {
        ThemeManager& tm = ThemeManager::instance();
        QSignalSpy spy(&tm, &ThemeManager::themeChanged);

        tm.setTheme(ThemeManager::Theme::Light);

        QCOMPARE(tm.currentTheme(), ThemeManager::Theme::Light);
        QCOMPARE(tm.effectiveTheme(), ThemeManager::Theme::Light);
        QCOMPARE(spy.count(), 1);

        // Check that stylesheet was applied
        QString stylesheet = qApp->styleSheet();
        QVERIFY(!stylesheet.isEmpty());
    }

    /**
     * @brief Test setting dark theme
     */
    void testSetDarkTheme() {
        ThemeManager& tm = ThemeManager::instance();
        QSignalSpy spy(&tm, &ThemeManager::themeChanged);

        tm.setTheme(ThemeManager::Theme::Dark);

        QCOMPARE(tm.currentTheme(), ThemeManager::Theme::Dark);
        QCOMPARE(tm.effectiveTheme(), ThemeManager::Theme::Dark);
        QCOMPARE(spy.count(), 1);

        // Check that stylesheet was applied
        QString stylesheet = qApp->styleSheet();
        QVERIFY(!stylesheet.isEmpty());
        QVERIFY(stylesheet.contains("#1e1e1e")); // Dark theme background color
    }

    /**
     * @brief Test setting system theme
     */
    void testSetSystemTheme() {
        ThemeManager& tm = ThemeManager::instance();
        QSignalSpy spy(&tm, &ThemeManager::themeChanged);

        tm.setTheme(ThemeManager::Theme::System);

        QCOMPARE(tm.currentTheme(), ThemeManager::Theme::System);

        // Effective theme should be Light or Dark (not System)
        QVERIFY(tm.effectiveTheme() == ThemeManager::Theme::Light ||
                tm.effectiveTheme() == ThemeManager::Theme::Dark);

        QCOMPARE(spy.count(), 1);

        // Check that stylesheet was applied
        QString stylesheet = qApp->styleSheet();
        QVERIFY(!stylesheet.isEmpty());
    }

    /**
     * @brief Test theme switching
     */
    void testThemeSwitching() {
        ThemeManager& tm = ThemeManager::instance();
        QSignalSpy spy(&tm, &ThemeManager::themeChanged);

        // Switch from Light to Dark
        tm.setTheme(ThemeManager::Theme::Light);
        QString lightStylesheet = qApp->styleSheet();

        tm.setTheme(ThemeManager::Theme::Dark);
        QString darkStylesheet = qApp->styleSheet();

        // Stylesheets should be different
        QVERIFY(lightStylesheet != darkStylesheet);

        // Should have emitted signal twice
        QCOMPARE(spy.count(), 2);
    }

    /**
     * @brief Test stylesheet loading
     */
    void testStylesheetLoading() {
        ThemeManager& tm = ThemeManager::instance();

        // Test light theme stylesheet
        tm.setTheme(ThemeManager::Theme::Light);
        QString lightStylesheet = qApp->styleSheet();
        QVERIFY(!lightStylesheet.isEmpty());
        QVERIFY(lightStylesheet.contains("#ffffff")); // Light theme background
        QVERIFY(lightStylesheet.contains("QWidget"));
        QVERIFY(lightStylesheet.contains("QPushButton"));

        // Test dark theme stylesheet
        tm.setTheme(ThemeManager::Theme::Dark);
        QString darkStylesheet = qApp->styleSheet();
        QVERIFY(!darkStylesheet.isEmpty());
        QVERIFY(darkStylesheet.contains("#1e1e1e")); // Dark theme background
        QVERIFY(darkStylesheet.contains("QWidget"));
        QVERIFY(darkStylesheet.contains("QPushButton"));
    }

    /**
     * @brief Test that multiple theme changes work correctly
     */
    void testMultipleThemeChanges() {
        ThemeManager& tm = ThemeManager::instance();
        QSignalSpy spy(&tm, &ThemeManager::themeChanged);

        // Change theme multiple times
        tm.setTheme(ThemeManager::Theme::Light);
        tm.setTheme(ThemeManager::Theme::Dark);
        tm.setTheme(ThemeManager::Theme::System);
        tm.setTheme(ThemeManager::Theme::Light);
        tm.setTheme(ThemeManager::Theme::Dark);

        // Should have emitted signal 5 times
        QCOMPARE(spy.count(), 5);

        // Final theme should be Dark
        QCOMPARE(tm.currentTheme(), ThemeManager::Theme::Dark);
        QCOMPARE(tm.effectiveTheme(), ThemeManager::Theme::Dark);
    }
};

QTEST_MAIN(ThemeManagerTest)
#include "ThemeManagerTest.moc"
