#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QMutex>

class Logger
{
public:
    enum Level {
        DEBUG,
        INFO,
        WARN,
        ERROR
    };

    static void log(Level level, const QString& message);
    static void debug(const QString& message);
    static void info(const QString& message);
    static void warn(const QString& message);
    static void error(const QString& message);

    static void setLogLevel(Level level);
    static void setLogFile(const QString& filepath);
    static void enableConsoleOutput(bool enable);

private:
    Logger() = delete;
    static QString levelToString(Level level);
    static QString getCurrentTimestamp();

    static Level s_logLevel;
    static QFile s_logFile;
    static QTextStream s_logStream;
    static QMutex s_mutex;
    static bool s_consoleOutput;
};

#endif // LOGGER_H
