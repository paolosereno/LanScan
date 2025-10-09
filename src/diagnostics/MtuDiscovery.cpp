#include "diagnostics/MtuDiscovery.h"
#include "utils/Logger.h"
#include <QRegularExpression>

MtuDiscovery::MtuDiscovery(QObject* parent)
    : QObject(parent)
    , m_process(nullptr)
    , m_target("")
    , m_minMtu(576)
    , m_maxMtu(9000)
    , m_currentMtu(0)
    , m_discoveredMtu(0)
    , m_isRunning(false)
{
}

MtuDiscovery::~MtuDiscovery()
{
    cancel();
}

bool MtuDiscovery::discoverMtu(const QString& target, int minMtu, int maxMtu)
{
    if (m_isRunning) {
        Logger::warn("MtuDiscovery: Cannot start discovery: already running");
        return false;
    }

    if (target.isEmpty()) {
        Logger::error("MtuDiscovery: Cannot start discovery: target is empty");
        return false;
    }

    if (minMtu < 68 || minMtu > maxMtu) {
        Logger::error(QString("MtuDiscovery: Invalid MTU range: %1-%2").arg(minMtu).arg(maxMtu));
        return false;
    }

    m_target = target;
    m_minMtu = minMtu;
    m_maxMtu = maxMtu;
    m_discoveredMtu = 0;
    m_isRunning = true;

    Logger::info(QString("MtuDiscovery: Starting discovery for %1 (range: %2-%3)")
                 .arg(target).arg(minMtu).arg(maxMtu));

    // Start binary search
    performBinarySearchStep();

    return true;
}

void MtuDiscovery::cancel()
{
    if (m_process && m_process->state() == QProcess::Running) {
        Logger::info("MtuDiscovery: Cancelling discovery");
        m_process->kill();
        m_process->waitForFinished(1000);
    }
    m_isRunning = false;
}

void MtuDiscovery::performBinarySearchStep()
{
    if (!m_isRunning) {
        return;
    }

    // Check if search has converged
    if (m_minMtu > m_maxMtu) {
        // Discovery complete
        m_discoveredMtu = m_maxMtu;
        m_isRunning = false;

        Logger::info(QString("MtuDiscovery: Discovery completed, MTU = %1 bytes").arg(m_discoveredMtu));
        emit mtuDiscovered(m_discoveredMtu);
        return;
    }

    // Calculate midpoint for binary search
    m_currentMtu = (m_minMtu + m_maxMtu) / 2;

    Logger::debug(QString("MtuDiscovery: Testing MTU %1 (range: %2-%3)")
                  .arg(m_currentMtu).arg(m_minMtu).arg(m_maxMtu));

    emit progressUpdated(m_currentMtu, m_minMtu, m_maxMtu);

    // Test current MTU size
    testMtuSize(m_currentMtu);
}

bool MtuDiscovery::testMtuSize(int mtuSize)
{
    // Clean up previous process if exists
    if (m_process) {
        delete m_process;
        m_process = nullptr;
    }

    // ICMP header size: IP (20 bytes) + ICMP (8 bytes) = 28 bytes
    int icmpOverhead = 28;
    int packetSize = mtuSize - icmpOverhead;

    if (packetSize <= 0) {
        Logger::error(QString("MtuDiscovery: Invalid packet size %1 for MTU %2").arg(packetSize).arg(mtuSize));
        m_isRunning = false;
        emit discoveryError("Invalid MTU size");
        return false;
    }

    // Create new process
    m_process = new QProcess(this);
    m_outputBuffer.clear();

    // Connect signals
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &MtuDiscovery::onProcessFinished);
    connect(m_process, &QProcess::errorOccurred,
            this, &MtuDiscovery::onProcessError);

    // Build platform-specific command
    QStringList args = buildPingCommand(m_target, packetSize);
    if (args.isEmpty()) {
        Logger::error("MtuDiscovery: Failed to build ping command");
        m_isRunning = false;
        emit discoveryError("Failed to build ping command");
        return false;
    }

    QString program = args.takeFirst();

    Logger::debug(QString("MtuDiscovery: Executing: %1 %2").arg(program, args.join(" ")));

    // Start the process
    m_process->start(program, args);

    if (!m_process->waitForStarted(3000)) {
        Logger::error("MtuDiscovery: Failed to start ping process");
        m_isRunning = false;
        emit discoveryError("Failed to start ping process");
        return false;
    }

    return true;
}

QStringList MtuDiscovery::buildPingCommand(const QString& target, int packetSize)
{
    QStringList command;

#ifdef Q_OS_WIN
    // Windows: ping -n 1 -l packetSize -f target
    // -n 1: send 1 packet
    // -l size: buffer size
    // -f: set Don't Fragment flag
    command << "ping";
    command << "-n" << "1";
    command << "-l" << QString::number(packetSize);
    command << "-f";
    command << "-w" << "5000";  // 5 second timeout
    command << target;
#else
    // Linux/macOS: ping -c 1 -s packetSize -M do target
    // -c 1: send 1 packet
    // -s size: packet size
    // -M do: set DF flag and don't fragment
    command << "ping";
    command << "-c" << "1";
    command << "-s" << QString::number(packetSize);
    command << "-M" << "do";
    command << "-W" << "5";  // 5 second timeout
    command << target;
#endif

    return command;
}

void MtuDiscovery::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (!m_process) return;

    // Read all output
    m_outputBuffer += QString::fromLocal8Bit(m_process->readAllStandardOutput());
    m_outputBuffer += QString::fromLocal8Bit(m_process->readAllStandardError());

    Logger::debug(QString("MtuDiscovery: Ping finished with exit code %1").arg(exitCode));
    Logger::debug(QString("MtuDiscovery: Output: %1").arg(m_outputBuffer.left(200)));

    // Analyze result
    bool success = analyzePingResult(m_outputBuffer, exitCode);

    if (success) {
        // MTU size is acceptable, try larger
        Logger::debug(QString("MtuDiscovery: MTU %1 is acceptable, trying larger").arg(m_currentMtu));
        m_minMtu = m_currentMtu + 1;
    } else {
        // MTU size is too large, try smaller
        Logger::debug(QString("MtuDiscovery: MTU %1 is too large, trying smaller").arg(m_currentMtu));
        m_maxMtu = m_currentMtu - 1;
    }

    // Continue binary search
    performBinarySearchStep();
}

void MtuDiscovery::onProcessError(QProcess::ProcessError error)
{
    QString errorMsg;
    switch (error) {
        case QProcess::FailedToStart:
            errorMsg = "Failed to start ping (command not found or insufficient permissions)";
            break;
        case QProcess::Crashed:
            errorMsg = "Ping process crashed";
            break;
        case QProcess::Timedout:
            errorMsg = "Ping process timed out";
            break;
        default:
            errorMsg = "Unknown error occurred";
            break;
    }

    Logger::error(QString("MtuDiscovery: %1").arg(errorMsg));
    m_isRunning = false;
    emit discoveryError(errorMsg);
}

bool MtuDiscovery::analyzePingResult(const QString& output, int exitCode)
{
    // Check for fragmentation errors (Windows)
    if (output.contains("Packet needs to be fragmented", Qt::CaseInsensitive) ||
        output.contains("needs to be fragmented", Qt::CaseInsensitive)) {
        return false;
    }

    // Check for fragmentation errors (Linux)
    if (output.contains("Message too long", Qt::CaseInsensitive) ||
        output.contains("mtu", Qt::CaseInsensitive)) {
        return false;
    }

    // Check for successful ping (exit code 0 and contains reply/response)
    if (exitCode == 0) {
        // Windows: look for "Reply from"
        if (output.contains("Reply from", Qt::CaseInsensitive) ||
            output.contains("Risposta da", Qt::CaseInsensitive) ||
            output.contains("Réponse de", Qt::CaseInsensitive) ||
            output.contains("Antwort von", Qt::CaseInsensitive) ||
            output.contains("Respuesta desde", Qt::CaseInsensitive)) {
            return true;
        }

        // Linux: look for "bytes from" or successful statistics
        if (output.contains("bytes from", Qt::CaseInsensitive) ||
            output.contains("1 received", Qt::CaseInsensitive) ||
            output.contains("1 packets transmitted, 1 received", Qt::CaseInsensitive)) {
            return true;
        }
    }

    // If exit code is not 0 or no success indicators, assume failure
    return false;
}
