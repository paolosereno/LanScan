#include "PingService.h"
#include "../../utils/Logger.h"
#include <QRegularExpression>
#include <QStringList>

PingService::PingService(QObject* parent)
    : QObject(parent)
    , pingProcess(new QProcess(this))
    , continuousTimer(new QTimer(this))
    , currentCount(0)
    , isContinuous(false)
{
    connect(pingProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &PingService::onProcessFinished);
    connect(pingProcess, &QProcess::errorOccurred,
            this, &PingService::onProcessError);
    connect(continuousTimer, &QTimer::timeout,
            this, &PingService::onContinuousPingTimeout);
}

PingService::~PingService() {
    stopContinuousPing();
    if (pingProcess->state() == QProcess::Running) {
        pingProcess->kill();
        pingProcess->waitForFinished();
    }
}

void PingService::ping(const QString& host, int count) {
    if (pingProcess->state() == QProcess::Running) {
        Logger::warn("PingService: Ping already in progress");
        return;
    }

    currentHost = host;
    currentCount = count;
    currentResults.clear();
    isContinuous = false;

    QStringList args = buildPingCommand(host, count);
    QString program = args.takeFirst();

    Logger::debug(QString("PingService: Executing: %1 %2").arg(program, args.join(" ")));

    pingProcess->start(program, args);
}

PingService::PingResult PingService::pingSync(const QString& host, int timeout) {
    QProcess process;
    QStringList args = buildPingCommand(host, 1);
    QString program = args.takeFirst();

    process.start(program, args);

    if (!process.waitForFinished(timeout + 1000)) {
        PingResult result;
        result.host = host;
        result.success = false;
        result.errorMessage = "Timeout waiting for ping response";
        return result;
    }

    QString output = process.readAllStandardOutput();
    QVector<PingResult> results = parsePingOutput(output);

    if (!results.isEmpty()) {
        return results.first();
    }

    PingResult result;
    result.host = host;
    result.success = false;
    result.errorMessage = "Failed to parse ping output";
    return result;
}

void PingService::continuousPing(const QString& host, int interval) {
    currentHost = host;
    isContinuous = true;
    continuousTimer->start(interval);

    // Send first ping immediately
    onContinuousPingTimeout();

    Logger::info(QString("PingService: Started continuous ping to %1 (interval: %2ms)")
                 .arg(host).arg(interval));
}

void PingService::stopContinuousPing() {
    if (continuousTimer->isActive()) {
        continuousTimer->stop();
        isContinuous = false;
        Logger::info("PingService: Stopped continuous ping");
    }
}

bool PingService::isContinuousPingActive() const {
    return continuousTimer->isActive();
}

void PingService::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    QString output = pingProcess->readAllStandardOutput();
    QString errorOutput = pingProcess->readAllStandardError();

    if (exitStatus == QProcess::CrashExit || exitCode != 0) {
        Logger::warn(QString("PingService: Ping failed (exit code: %1): %2")
                       .arg(exitCode).arg(errorOutput));

        emit errorOccurred(QString("Ping failed: %1").arg(errorOutput));
        return;
    }

    QVector<PingResult> results = parsePingOutput(output);

    if (!isContinuous) {
        emit pingCompleted(results);
    } else {
        // For continuous ping, emit each result individually
        for (const PingResult& result : results) {
            emit pingResult(result);
        }
    }
}

void PingService::onProcessError(QProcess::ProcessError error) {
    QString errorMsg;
    switch (error) {
        case QProcess::FailedToStart:
            errorMsg = "Failed to start ping process";
            break;
        case QProcess::Crashed:
            errorMsg = "Ping process crashed";
            break;
        case QProcess::Timedout:
            errorMsg = "Ping process timed out";
            break;
        case QProcess::ReadError:
            errorMsg = "Read error from ping process";
            break;
        case QProcess::WriteError:
            errorMsg = "Write error to ping process";
            break;
        default:
            errorMsg = "Unknown ping process error";
    }

    Logger::error(QString("PingService: ") + errorMsg);
    emit errorOccurred(errorMsg);
}

void PingService::onContinuousPingTimeout() {
    ping(currentHost, 4);  // Collect 4 samples for balance between speed and statistical variance
}

QStringList PingService::buildPingCommand(const QString& host, int count) {
    QStringList command;
    QString platform = detectPlatform();

    if (platform == "windows") {
        command << "ping" << "-n" << QString::number(count) << host;
    } else {
        // Linux and macOS
        command << "ping" << "-c" << QString::number(count) << host;
    }

    return command;
}

QVector<PingService::PingResult> PingService::parsePingOutput(const QString& output) {
    QVector<PingResult> results;
    QStringList lines = output.split('\n');
    QString platform = detectPlatform();

    for (const QString& line : lines) {
        PingResult result;

        if (platform == "windows") {
            result = parseWindowsPing(line);
        } else {
            result = parseUnixPing(line);
        }

        if (result.success) {
            result.host = currentHost;
            results.append(result);
        }
    }

    return results;
}

PingService::PingResult PingService::parseWindowsPing(const QString& line) {
    PingResult result;

    // Define multi-language regex patterns for internationalization
    // Pattern structure: (reply_keyword) from IP: (bytes_keyword)=(value) (time_keyword)[=<](value)ms TTL=(value)

    QVector<QRegularExpression> patterns = {
        // English: "Reply from 192.168.1.1: bytes=32 time=5ms TTL=64"
        QRegularExpression(R"(Reply from .+: bytes=(\d+) time[=<](\d+)ms TTL=(\d+))"),

        // Italian: "Risposta da 192.168.1.1: byte=32 durata<1ms TTL=64"
        QRegularExpression(R"(Risposta da .+: byte[s]?=(\d+) durata[=<](\d+)ms TTL=(\d+))"),

        // German: "Antwort von 192.168.1.1: Bytes=32 Zeit<1ms TTL=64"
        QRegularExpression(R"(Antwort von .+: Bytes=(\d+) Zeit[=<](\d+)ms TTL=(\d+))"),

        // French: "Réponse de 192.168.1.1: octets=32 temps<1ms TTL=64"
        QRegularExpression(R"(R[ée]ponse de .+: octets=(\d+) temps[=<](\d+)ms TTL=(\d+))"),

        // Spanish: "Respuesta desde 192.168.1.1: bytes=32 tiempo<1ms TTL=64"
        QRegularExpression(R"(Respuesta desde .+: bytes=(\d+) tiempo[=<](\d+)ms TTL=(\d+))")
    };

    // Try each pattern
    for (const QRegularExpression& regex : patterns) {
        QRegularExpressionMatch match = regex.match(line);
        if (match.hasMatch()) {
            result.bytes = match.captured(1).toInt();
            result.latency = match.captured(2).toDouble();
            result.ttl = match.captured(3).toInt();
            result.success = true;
            return result;
        }
    }

    // Check for error messages in multiple languages
    QStringList timeoutKeywords = {"Request timed out", "Richiesta scaduta", "Zeitüberschreitung",
                                    "Délai d'attente", "Tiempo de espera agotado"};
    QStringList unreachableKeywords = {"Destination host unreachable", "Host di destinazione non raggiungibile",
                                        "Zielhost nicht erreichbar", "Hôte de destination inaccessible",
                                        "Host de destino inaccesible"};

    for (const QString& keyword : timeoutKeywords) {
        if (line.contains(keyword, Qt::CaseInsensitive)) {
            result.success = false;
            result.errorMessage = line.trimmed();
            return result;
        }
    }

    for (const QString& keyword : unreachableKeywords) {
        if (line.contains(keyword, Qt::CaseInsensitive)) {
            result.success = false;
            result.errorMessage = line.trimmed();
            return result;
        }
    }

    return result;
}

PingService::PingResult PingService::parseUnixPing(const QString& line) {
    PingResult result;

    // Parse: "64 bytes from 192.168.1.1: icmp_seq=1 ttl=64 time=5.0 ms"
    QRegularExpression regex(R"((\d+) bytes from .+: icmp_seq=\d+ ttl=(\d+) time=(\d+\.?\d*)\s*ms)");
    QRegularExpressionMatch match = regex.match(line);

    if (match.hasMatch()) {
        result.bytes = match.captured(1).toInt();
        result.ttl = match.captured(2).toInt();
        result.latency = match.captured(3).toDouble();
        result.success = true;
    } else if (line.contains("Destination Host Unreachable", Qt::CaseInsensitive) ||
               line.contains("Request timeout", Qt::CaseInsensitive) ||
               line.contains("100% packet loss", Qt::CaseInsensitive)) {
        result.success = false;
        result.errorMessage = line.trimmed();
    }

    return result;
}

QString PingService::detectPlatform() const {
#ifdef Q_OS_WIN
    return "windows";
#elif defined(Q_OS_LINUX)
    return "linux";
#elif defined(Q_OS_MACOS)
    return "macos";
#else
    return "unknown";
#endif
}
