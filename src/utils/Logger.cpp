#include "Logger.h"
#include <QDateTime>
#include <QDebug>
#include <iostream>

// Static member initialization
Logger::Level Logger::s_logLevel = Logger::INFO;
QFile Logger::s_logFile;
QTextStream Logger::s_logStream;
QMutex Logger::s_mutex;
bool Logger::s_consoleOutput = true;

void Logger::log(Level level, const QString& message)
{
    if (level < s_logLevel) {
        return;
    }

    QMutexLocker locker(&s_mutex);

    QString timestamp = getCurrentTimestamp();
    QString levelStr = levelToString(level);
    QString logMessage = QString("[%1] [%2] %3").arg(timestamp, levelStr, message);

    // Output to console
    if (s_consoleOutput) {
        if (level >= ERROR) {
            std::cerr << logMessage.toStdString() << std::endl;
        } else {
            std::cout << logMessage.toStdString() << std::endl;
        }
    }

    // Output to file
    if (s_logFile.isOpen()) {
        s_logStream << logMessage << Qt::endl;
        s_logStream.flush();
    }
}

void Logger::debug(const QString& message)
{
    log(DEBUG, message);
}

void Logger::info(const QString& message)
{
    log(INFO, message);
}

void Logger::warn(const QString& message)
{
    log(WARN, message);
}

void Logger::error(const QString& message)
{
    log(ERROR, message);
}

void Logger::setLogLevel(Level level)
{
    s_logLevel = level;
}

void Logger::setLogFile(const QString& filepath)
{
    QMutexLocker locker(&s_mutex);

    if (s_logFile.isOpen()) {
        s_logFile.close();
    }

    s_logFile.setFileName(filepath);
    if (s_logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        s_logStream.setDevice(&s_logFile);
    }
}

void Logger::enableConsoleOutput(bool enable)
{
    s_consoleOutput = enable;
}

QString Logger::levelToString(Level level)
{
    switch (level) {
        case DEBUG: return "DEBUG";
        case INFO: return "INFO";
        case WARN: return "WARN";
        case ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

QString Logger::getCurrentTimestamp()
{
    return QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
}
