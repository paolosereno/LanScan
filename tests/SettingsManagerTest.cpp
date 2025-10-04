#include <QtTest>
#include "config/SettingsManager.h"

class SettingsManagerTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testDefaultValues();
    void testGeneralSettings();
    void testScanSettings();
    void testUISettings();
    void testExportSettings();
    void testPersistence();
    void testReset();

private:
    SettingsManager* settings;
};

void SettingsManagerTest::initTestCase() {
    settings = SettingsManager::instance();
    settings->reset(); // Start with clean state
}

void SettingsManagerTest::cleanupTestCase() {
    settings->reset();
}

void SettingsManagerTest::testDefaultValues() {
    settings->reset();

    QCOMPARE(settings->getDefaultSubnet(), QString("192.168.1.0/24"));
    QCOMPARE(settings->getScanTimeout(), 1000);
    QCOMPARE(settings->getMaxThreads(), 10);
    QCOMPARE(settings->getTheme(), QString("light"));
    QCOMPARE(settings->getLanguage(), QString("en"));
    QCOMPARE(settings->getDefaultExportFormat(), QString("CSV"));
}

void SettingsManagerTest::testGeneralSettings() {
    settings->setDefaultSubnet("10.0.0.0/24");
    QCOMPARE(settings->getDefaultSubnet(), QString("10.0.0.0/24"));
}

void SettingsManagerTest::testScanSettings() {
    settings->setScanTimeout(2000);
    QCOMPARE(settings->getScanTimeout(), 2000);

    settings->setMaxThreads(20);
    QCOMPARE(settings->getMaxThreads(), 20);
}

void SettingsManagerTest::testUISettings() {
    settings->setTheme("dark");
    QCOMPARE(settings->getTheme(), QString("dark"));

    settings->setLanguage("it");
    QCOMPARE(settings->getLanguage(), QString("it"));
}

void SettingsManagerTest::testExportSettings() {
    settings->setDefaultExportPath("/tmp/exports");
    QCOMPARE(settings->getDefaultExportPath(), QString("/tmp/exports"));

    settings->setDefaultExportFormat("JSON");
    QCOMPARE(settings->getDefaultExportFormat(), QString("JSON"));
}

void SettingsManagerTest::testPersistence() {
    settings->setDefaultSubnet("172.16.0.0/16");
    settings->setScanTimeout(3000);
    settings->save();

    // Reload settings
    settings->load();

    QCOMPARE(settings->getDefaultSubnet(), QString("172.16.0.0/16"));
    QCOMPARE(settings->getScanTimeout(), 3000);
}

void SettingsManagerTest::testReset() {
    settings->setDefaultSubnet("192.168.100.0/24");
    settings->setScanTimeout(5000);
    settings->setTheme("dark");

    settings->reset();

    // Should be back to defaults
    QCOMPARE(settings->getDefaultSubnet(), QString("192.168.1.0/24"));
    QCOMPARE(settings->getScanTimeout(), 1000);
    QCOMPARE(settings->getTheme(), QString("light"));
}

QTEST_MAIN(SettingsManagerTest)
#include "SettingsManagerTest.moc"
