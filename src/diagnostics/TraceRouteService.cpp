#include "diagnostics/TraceRouteService.h"
#include "utils/Logger.h"
#include <QRegularExpression>
#include <QStringList>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

TraceRouteService::TraceRouteService(QObject* parent)
    : QObject(parent)
    , m_process(nullptr)
    , m_target("")
    , m_maxHops(30)
    , m_currentHop(0)
{
}

TraceRouteService::~TraceRouteService()
{
    cancel();
}

bool TraceRouteService::traceRoute(const QString& target, int maxHops, int timeout)
{
    if (isRunning()) {
        Logger::warn("TraceRouteService: Cannot start traceroute: already running");
        return false;
    }

    if (target.isEmpty()) {
        Logger::error("TraceRouteService: Cannot start traceroute: target is empty");
        return false;
    }

    // Clean up previous process if exists
    if (m_process) {
        delete m_process;
        m_process = nullptr;
    }

    m_target = target;
    m_maxHops = maxHops;
    m_hops.clear();
    m_outputBuffer.clear();
    m_currentHop = 0;

    // Create new process
    m_process = new QProcess(this);

    // Connect signals
    connect(m_process, &QProcess::readyReadStandardOutput,
            this, &TraceRouteService::onReadyReadStandardOutput);
    connect(m_process, &QProcess::readyReadStandardError,
            this, &TraceRouteService::onReadyReadStandardError);
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &TraceRouteService::onProcessFinished);
    connect(m_process, &QProcess::errorOccurred,
            this, &TraceRouteService::onProcessError);

    // Build platform-specific command
    QStringList args = buildTraceCommand(target, maxHops, timeout);
    if (args.isEmpty()) {
        Logger::error("TraceRouteService: Failed to build traceroute command");
        return false;
    }

    QString program = args.takeFirst();

    Logger::info(QString("TraceRouteService: Starting traceroute to %1 (max hops: %2)")
                 .arg(target).arg(maxHops));

    // Start the process
    m_process->start(program, args);

    if (!m_process->waitForStarted(3000)) {
        Logger::error("TraceRouteService: Failed to start traceroute process");
        return false;
    }

    return true;
}

void TraceRouteService::cancel()
{
    if (m_process && m_process->state() == QProcess::Running) {
        Logger::info("TraceRouteService: Cancelling traceroute");
        m_process->kill();
        m_process->waitForFinished(1000);
    }
}

QStringList TraceRouteService::buildTraceCommand(const QString& target, int maxHops, int timeout)
{
    QStringList command;

#ifdef Q_OS_WIN
    // Windows: tracert -h max_hops -w timeout target
    command << "tracert";
    command << "-h" << QString::number(maxHops);
    command << "-w" << QString::number(timeout);
    command << target;
#else
    // Linux/macOS: traceroute -m max_hops -w timeout_sec target
    command << "traceroute";
    command << "-m" << QString::number(maxHops);
    command << "-w" << QString::number(timeout / 1000); // Convert ms to seconds
    command << target;
#endif

    return command;
}

void TraceRouteService::onReadyReadStandardOutput()
{
    if (!m_process) return;

    QString output = QString::fromLocal8Bit(m_process->readAllStandardOutput());
    m_outputBuffer += output;

    // Process complete lines
    QStringList lines = m_outputBuffer.split('\n');

    // Keep the last incomplete line in buffer
    m_outputBuffer = lines.takeLast();

    for (const QString& line : lines) {
        if (line.trimmed().isEmpty()) continue;

        TraceRouteHop hop;
#ifdef Q_OS_WIN
        hop = parseWindowsLine(line);
#else
        hop = parseUnixLine(line);
#endif

        if (hop.hopNumber() > 0) {
            m_hops.append(hop);
            m_currentHop = hop.hopNumber();

            Logger::debug(QString("TraceRouteService: Hop discovered: %1").arg(hop.toString()));

            emit hopDiscovered(hop);
            emit progressUpdated(m_currentHop, m_maxHops);
        }
    }
}

void TraceRouteService::onReadyReadStandardError()
{
    if (!m_process) return;

    QString error = QString::fromLocal8Bit(m_process->readAllStandardError());
    if (!error.trimmed().isEmpty()) {
        Logger::warn(QString("TraceRouteService: Error output: %1").arg(error.trimmed()));
    }
}

void TraceRouteService::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Logger::info(QString("TraceRouteService: Traceroute finished (exit code: %1, hops: %2)")
                 .arg(exitCode).arg(m_hops.size()));

    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        emit traceCompleted(m_hops);
    } else if (exitStatus == QProcess::CrashExit) {
        emit traceError("Traceroute process crashed");
    } else if (!m_hops.isEmpty()) {
        // Even with non-zero exit code, if we got some hops, emit them
        emit traceCompleted(m_hops);
    } else {
        emit traceError(QString("Traceroute failed with exit code %1").arg(exitCode));
    }
}

void TraceRouteService::onProcessError(QProcess::ProcessError error)
{
    QString errorMsg;
    switch (error) {
        case QProcess::FailedToStart:
            errorMsg = "Failed to start traceroute (command not found or insufficient permissions)";
            break;
        case QProcess::Crashed:
            errorMsg = "Traceroute process crashed";
            break;
        case QProcess::Timedout:
            errorMsg = "Traceroute process timed out";
            break;
        case QProcess::ReadError:
            errorMsg = "Error reading from traceroute process";
            break;
        case QProcess::WriteError:
            errorMsg = "Error writing to traceroute process";
            break;
        default:
            errorMsg = "Unknown error occurred";
            break;
    }

    Logger::error(QString("TraceRouteService: %1").arg(errorMsg));
    emit traceError(errorMsg);
}

TraceRouteHop TraceRouteService::parseWindowsLine(const QString& line)
{
    // Windows tracert format:
    //   1    <1 ms    <1 ms    <1 ms  192.168.1.1
    //   2     5 ms     4 ms     5 ms  gateway.example.com [10.0.0.1]
    //   3     *        *        *     Request timed out.

    TraceRouteHop hop;
    QString trimmed = line.trimmed();

    // Skip header lines
    if (trimmed.contains("Tracing route", Qt::CaseInsensitive) ||
        trimmed.contains("over a maximum", Qt::CaseInsensitive) ||
        trimmed.isEmpty()) {
        return hop;
    }

    // Extract hop number (first number on the line)
    QRegularExpression hopRegex("^\\s*(\\d+)");
    QRegularExpressionMatch hopMatch = hopRegex.match(trimmed);

    if (!hopMatch.hasMatch()) {
        return hop;
    }

    int hopNumber = hopMatch.captured(1).toInt();
    hop.setHopNumber(hopNumber);

    // Check for timeout
    if (trimmed.contains("Request timed out", Qt::CaseInsensitive) ||
        trimmed.contains("* * *")) {
        hop.setTimeout(true);
        return hop;
    }

    // Extract RTT values (look for numbers followed by "ms")
    QRegularExpression rttRegex("(<?\\d+)\\s*ms");
    QRegularExpressionMatchIterator rttIter = rttRegex.globalMatch(trimmed);

    while (rttIter.hasNext()) {
        QRegularExpressionMatch match = rttIter.next();
        QString rttStr = match.captured(1);
        rttStr.remove('<'); // Remove '<' from "<1 ms"
        double rtt = rttStr.toDouble();
        hop.addRtt(rtt);
    }

    // Extract IP address and hostname
    // Format: "hostname [ip]" or just "ip"
    QRegularExpression ipRegex("\\[([0-9.]+)\\]|([0-9.]+)$");
    QRegularExpressionMatch ipMatch = ipRegex.match(trimmed);

    if (ipMatch.hasMatch()) {
        QString ip = ipMatch.captured(1).isEmpty() ? ipMatch.captured(2) : ipMatch.captured(1);
        hop.setIpAddress(ip);

        // Extract hostname if present (text before [ip])
        if (ipMatch.captured(1).isEmpty()) {
            // Just IP, no hostname
        } else {
            // Format: "hostname [ip]"
            QRegularExpression hostRegex("\\d+\\s+(?:<?\\d+\\s+ms\\s+){1,3}\\s+([^\\[]+)\\s+\\[");
            QRegularExpressionMatch hostMatch = hostRegex.match(trimmed);
            if (hostMatch.hasMatch()) {
                hop.setHostname(hostMatch.captured(1).trimmed());
            }
        }
    }

    return hop;
}

TraceRouteHop TraceRouteService::parseUnixLine(const QString& line)
{
    // Linux/macOS traceroute format:
    //  1  192.168.1.1 (192.168.1.1)  0.823 ms  0.765 ms  0.712 ms
    //  2  gateway.example.com (10.0.0.1)  5.234 ms  5.123 ms  5.456 ms
    //  3  * * *

    TraceRouteHop hop;
    QString trimmed = line.trimmed();

    // Skip header and empty lines
    if (trimmed.isEmpty()) {
        return hop;
    }

    // Extract hop number (first number on the line)
    QRegularExpression hopRegex("^\\s*(\\d+)");
    QRegularExpressionMatch hopMatch = hopRegex.match(trimmed);

    if (!hopMatch.hasMatch()) {
        return hop;
    }

    int hopNumber = hopMatch.captured(1).toInt();
    hop.setHopNumber(hopNumber);

    // Check for timeout
    if (trimmed.contains("* * *")) {
        hop.setTimeout(true);
        return hop;
    }

    // Extract hostname and IP address
    // Format: "hostname (ip)" or just "ip"
    QRegularExpression hostIpRegex("([a-zA-Z0-9.-]+)\\s+\\(([0-9.]+)\\)");
    QRegularExpressionMatch hostIpMatch = hostIpRegex.match(trimmed);

    if (hostIpMatch.hasMatch()) {
        QString hostname = hostIpMatch.captured(1);
        QString ip = hostIpMatch.captured(2);

        hop.setHostname(hostname != ip ? hostname : "");
        hop.setIpAddress(ip);
    } else {
        // Try to extract just IP
        QRegularExpression ipOnlyRegex("\\d+\\s+([0-9.]+)");
        QRegularExpressionMatch ipMatch = ipOnlyRegex.match(trimmed);
        if (ipMatch.hasMatch()) {
            hop.setIpAddress(ipMatch.captured(1));
        }
    }

    // Extract RTT values (numbers followed by "ms")
    QRegularExpression rttRegex("([0-9.]+)\\s*ms");
    QRegularExpressionMatchIterator rttIter = rttRegex.globalMatch(trimmed);

    while (rttIter.hasNext()) {
        QRegularExpressionMatch match = rttIter.next();
        double rtt = match.captured(1).toDouble();
        hop.addRtt(rtt);
    }

    return hop;
}

QString TraceRouteService::extractIpAddress(const QString& text) const
{
    QRegularExpression ipRegex("\\b([0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3})\\b");
    QRegularExpressionMatch match = ipRegex.match(text);
    return match.hasMatch() ? match.captured(1) : QString();
}

QString TraceRouteService::extractHostname(const QString& text) const
{
    QRegularExpression hostRegex("([a-zA-Z0-9.-]+)\\s*\\(");
    QRegularExpressionMatch match = hostRegex.match(text);
    return match.hasMatch() ? match.captured(1) : QString();
}
