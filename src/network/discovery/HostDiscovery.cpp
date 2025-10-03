#include "HostDiscovery.h"
#include "utils/Logger.h"
#include <QRegularExpression>

HostDiscovery::HostDiscovery(QObject *parent)
    : QObject(parent)
    , m_pingProcess(nullptr)
{
}

HostDiscovery::~HostDiscovery()
{
    stopPing();
}

void HostDiscovery::pingHost(const QString& ip)
{
    m_currentIp = ip;

    if (m_pingProcess) {
        delete m_pingProcess;
    }

    m_pingProcess = new QProcess(this);
    connect(m_pingProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &HostDiscovery::onPingFinished);

#ifdef Q_OS_WIN
    // Windows: ping -n 1 -w 1000 IP
    m_pingProcess->start("ping", QStringList() << "-n" << "1" << "-w" << "1000" << ip);
#else
    // Linux/macOS: ping -c 1 -W 1 IP
    m_pingProcess->start("ping", QStringList() << "-c" << "1" << "-W" << "1" << ip);
#endif

    Logger::debug(QString("Pinging host: %1").arg(ip));
}

bool HostDiscovery::isHostAlive(const QString& ip, int timeout)
{
    QProcess process;

#ifdef Q_OS_WIN
    process.start("ping", QStringList() << "-n" << "1" << "-w" << QString::number(timeout) << ip);
#else
    int timeoutSec = timeout / 1000;
    if (timeoutSec < 1) timeoutSec = 1;
    process.start("ping", QStringList() << "-c" << "1" << "-W" << QString::number(timeoutSec) << ip);
#endif

    if (!process.waitForFinished(timeout + 1000)) {
        process.kill();
        return false;
    }

    QString output = QString::fromLocal8Bit(process.readAllStandardOutput());
    return process.exitCode() == 0 && !output.isEmpty();
}

void HostDiscovery::stopPing()
{
    if (m_pingProcess) {
        if (m_pingProcess->state() != QProcess::NotRunning) {
            m_pingProcess->kill();
            m_pingProcess->waitForFinished(1000);
        }
        delete m_pingProcess;
        m_pingProcess = nullptr;
    }
}

void HostDiscovery::onPingFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitStatus == QProcess::CrashExit) {
        Logger::warn(QString("Ping process crashed for %1").arg(m_currentIp));
        emit hostNotFound(m_currentIp);
        return;
    }

    QString output = QString::fromLocal8Bit(m_pingProcess->readAllStandardOutput());

    if (exitCode == 0 && !output.isEmpty()) {
        double latency = extractLatency(output);
        Logger::debug(QString("Host %1 is alive (latency: %2 ms)").arg(m_currentIp).arg(latency));
        emit hostDiscovered(m_currentIp, latency);
    } else {
        Logger::debug(QString("Host %1 not found").arg(m_currentIp));
        emit hostNotFound(m_currentIp);
    }
}

QString HostDiscovery::parsePingOutput(const QString& output)
{
    // Extract relevant ping information
    return output;
}

double HostDiscovery::extractLatency(const QString& output)
{
#ifdef Q_OS_WIN
    // Windows format: "time=5ms" or "time<1ms"
    QRegularExpression re("time[=<](\\d+(?:\\.\\d+)?)ms", QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch match = re.match(output);

    if (match.hasMatch()) {
        return match.captured(1).toDouble();
    }
#else
    // Linux/macOS format: "time=5.0 ms"
    QRegularExpression re("time=(\\d+(?:\\.\\d+)?)\\s*ms", QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch match = re.match(output);

    if (match.hasMatch()) {
        return match.captured(1).toDouble();
    }
#endif

    return -1.0; // Unable to extract latency
}
