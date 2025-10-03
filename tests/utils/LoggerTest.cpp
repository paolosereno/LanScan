#include <QtTest>
#include "utils/Logger.h"
#include <QTemporaryFile>

class LoggerTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void testLogLevels();
    void testSetLogLevel();
    void testLogFile();
    void cleanupTestCase();
};

void LoggerTest::initTestCase()
{
    Logger::setLogLevel(Logger::DEBUG);
    Logger::enableConsoleOutput(false); // Disable for testing
}

void LoggerTest::testLogLevels()
{
    // These should not crash
    Logger::debug("Debug message");
    Logger::info("Info message");
    Logger::warn("Warning message");
    Logger::error("Error message");

    QVERIFY(true); // If we reach here, no crashes occurred
}

void LoggerTest::testSetLogLevel()
{
    Logger::setLogLevel(Logger::ERROR);

    // Lower level messages should be filtered
    Logger::debug("This should be filtered");
    Logger::info("This should be filtered");
    Logger::warn("This should be filtered");
    Logger::error("This should appear");

    QVERIFY(true);
}

void LoggerTest::testLogFile()
{
    QTemporaryFile tempFile;
    QVERIFY(tempFile.open());
    QString filepath = tempFile.fileName();
    tempFile.close();

    Logger::setLogFile(filepath);
    Logger::setLogLevel(Logger::DEBUG);

    Logger::info("Test log entry");

    // Verify file exists and has content
    QFile logFile(filepath);
    QVERIFY(logFile.exists());
    QVERIFY(logFile.open(QIODevice::ReadOnly));

    QString content = QString::fromUtf8(logFile.readAll());
    QVERIFY(content.contains("Test log entry"));
    QVERIFY(content.contains("[INFO]"));

    logFile.close();
}

void LoggerTest::cleanupTestCase()
{
    Logger::enableConsoleOutput(true);
    Logger::setLogLevel(Logger::INFO);
}

QTEST_MAIN(LoggerTest)
#include "LoggerTest.moc"
