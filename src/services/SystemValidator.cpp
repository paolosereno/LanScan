#include "SystemValidator.h"
#include "SystemInfoCollector.h"
#include <QSysInfo>
#include <QGuiApplication>
#include <QScreen>
#include <QOperatingSystemVersion>
#include <QStandardPaths>
#include <QDir>
#include <QFileInfo>
#include <QLocale>
#include <QNetworkInterface>
#include <QOpenGLContext>
#include <QOffscreenSurface>
#include <qopenglfunctions.h>
#include <QCoreApplication>
#include <QSqlDatabase>
#include <QTemporaryFile>

#ifdef Q_OS_WIN
    #include <windows.h>
    // Undefine Windows macros that conflict with our code
    #ifdef ERROR
        #undef ERROR
    #endif
    #ifdef interface
        #undef interface
    #endif
#endif

#if defined(Q_OS_LINUX) || defined(Q_OS_MACOS)
    #include <unistd.h>
#endif

SystemValidator::ValidationResult SystemValidator::validateOperatingSystem()
{
    bool supported = isOperatingSystemSupported();
    QString osName = SystemInfoCollector::getOperatingSystem();
    
    if (supported) {
        return createResult(true, INFO, QCoreApplication::translate("SystemValidator", "Operating System"), 
                          QCoreApplication::translate("SystemValidator", "Supported OS: %1").arg(osName));
    } else {
        return createResult(false, WARNING, QCoreApplication::translate("SystemValidator", "Operating System"),
                          QCoreApplication::translate("SystemValidator", "OS may not be fully supported: %1").arg(osName),
                          QCoreApplication::translate("SystemValidator", "Consider upgrading to a newer OS version or check compatibility"));
    }
}

SystemValidator::ValidationResult SystemValidator::validateMemory()
{
    quint64 totalRam = SystemInfoCollector::getTotalPhysicalMemory();
    quint64 availableRam = SystemInfoCollector::getAvailablePhysicalMemory();
    
    if (totalRam == 0) {
        return createResult(false, WARNING, QCoreApplication::translate("SystemValidator", "Memory"), 
                          QCoreApplication::translate("SystemValidator", "Unable to determine system memory"),
                          QCoreApplication::translate("SystemValidator", "Memory information not available on this platform"));
    }
    
    QString totalStr = SystemInfoCollector::formatBytes(totalRam);
    QString availableStr = SystemInfoCollector::formatBytes(availableRam);
    
    if (totalRam < MIN_RAM_BYTES) {
        return createResult(false, ERROR, QCoreApplication::translate("SystemValidator", "Memory"),
                          QCoreApplication::translate("SystemValidator", "Insufficient RAM: %1 (minimum %2 required)")
                          .arg(totalStr, SystemInfoCollector::formatBytes(MIN_RAM_BYTES)),
                          QCoreApplication::translate("SystemValidator", "Close other applications or upgrade system memory"));
    }
    
    if (availableRam < MIN_RAM_BYTES / 2) {
        return createResult(false, WARNING, QCoreApplication::translate("SystemValidator", "Memory"),
                          QCoreApplication::translate("SystemValidator", "Low available memory: %1 of %2 total")
                          .arg(availableStr, totalStr),
                          QCoreApplication::translate("SystemValidator", "Close other applications to free up memory"));
    }
    
    return createResult(true, INFO, QCoreApplication::translate("SystemValidator", "Memory"),
                       QCoreApplication::translate("SystemValidator", "Memory OK: %1 total, %2 available")
                       .arg(totalStr, availableStr));
}

SystemValidator::ValidationResult SystemValidator::validateDiskSpace()
{
    quint64 availableSpace = SystemInfoCollector::getAvailableDiskSpace();
    
    if (availableSpace == 0) {
        return createResult(false, WARNING, QCoreApplication::translate("SystemValidator", "Disk Space"),
                          QCoreApplication::translate("SystemValidator", "Unable to determine available disk space"),
                          QCoreApplication::translate("SystemValidator", "Check disk space manually"));
    }
    
    QString spaceStr = SystemInfoCollector::formatBytes(availableSpace);
    
    if (availableSpace < MIN_DISK_SPACE_BYTES) {
        return createResult(false, ERROR, QCoreApplication::translate("SystemValidator", "Disk Space"),
                          QCoreApplication::translate("SystemValidator", "Insufficient disk space: %1 (minimum %2 required)")
                          .arg(spaceStr, SystemInfoCollector::formatBytes(MIN_DISK_SPACE_BYTES)),
                          QCoreApplication::translate("SystemValidator", "Free up disk space or choose a different installation location"));
    }
    
    if (availableSpace < MIN_DISK_SPACE_BYTES * 5) {
        return createResult(false, WARNING, QCoreApplication::translate("SystemValidator", "Disk Space"),
                          QCoreApplication::translate("SystemValidator", "Low disk space: %1 available").arg(spaceStr),
                          QCoreApplication::translate("SystemValidator", "Consider freeing up more disk space"));
    }
    
    return createResult(true, INFO, QCoreApplication::translate("SystemValidator", "Disk Space"),
                       QCoreApplication::translate("SystemValidator", "Disk space OK: %1 available").arg(spaceStr));
}

SystemValidator::ValidationResult SystemValidator::validateScreen()
{
    QScreen* primaryScreen = QGuiApplication::primaryScreen();
    
    if (!primaryScreen) {
        return createResult(false, ERROR, QCoreApplication::translate("SystemValidator", "Display"),
                          QCoreApplication::translate("SystemValidator", "No primary screen detected"),
                          QCoreApplication::translate("SystemValidator", "Check display connection and drivers"));
    }
    
    QSize screenSize = primaryScreen->geometry().size();
    int width = screenSize.width();
    int height = screenSize.height();
    
    if (width < MIN_SCREEN_WIDTH || height < MIN_SCREEN_HEIGHT) {
        return createResult(false, WARNING, QCoreApplication::translate("SystemValidator", "Display"),
                          QCoreApplication::translate("SystemValidator", "Screen resolution may be too low: %1x%2 (minimum %3x%4 recommended)")
                          .arg(width).arg(height).arg(MIN_SCREEN_WIDTH).arg(MIN_SCREEN_HEIGHT),
                          QCoreApplication::translate("SystemValidator", "Use higher resolution for better user experience"));
    }
    
    return createResult(true, INFO, QCoreApplication::translate("SystemValidator", "Display"),
                       QCoreApplication::translate("SystemValidator", "Screen resolution OK: %1x%2 at %3 DPI")
                       .arg(width).arg(height).arg(qRound(primaryScreen->logicalDotsPerInch())));
}

SystemValidator::ValidationResult SystemValidator::validateQtVersion()
{
    QString compileVersion = QT_VERSION_STR;
    QString runtimeVersion = qVersion();
    
    // Check if runtime version matches compile version
    if (compileVersion != runtimeVersion) {
        return createResult(false, WARNING, QCoreApplication::translate("SystemValidator", "Qt Version"),
                          QCoreApplication::translate("SystemValidator", "Qt version mismatch - Compiled: %1, Runtime: %2")
                          .arg(compileVersion, runtimeVersion),
                          QCoreApplication::translate("SystemValidator", "Reinstall application or update Qt runtime"));
    }
    
    // Check minimum Qt version (assuming 6.0 minimum)
    QVersionNumber currentVersion = QVersionNumber::fromString(runtimeVersion);
    QVersionNumber minVersion(6, 0, 0);
    
    if (currentVersion < minVersion) {
        return createResult(false, ERROR, QCoreApplication::translate("SystemValidator", "Qt Version"),
                          QCoreApplication::translate("SystemValidator", "Qt version too old: %1 (minimum 6.0.0 required)")
                          .arg(runtimeVersion),
                          QCoreApplication::translate("SystemValidator", "Update Qt runtime libraries"));
    }
    
    return createResult(true, INFO, QCoreApplication::translate("SystemValidator", "Qt Version"),
                       QCoreApplication::translate("SystemValidator", "Qt version OK: %1").arg(runtimeVersion));
}

SystemValidator::ValidationResult SystemValidator::validateOpenGL()
{
    QOpenGLContext context;
    if (!context.create()) {
        return createResult(false, ERROR, QCoreApplication::translate("SystemValidator", "OpenGL"),
                          QCoreApplication::translate("SystemValidator", "Unable to create OpenGL context"),
                          QCoreApplication::translate("SystemValidator", "Update graphics drivers or check OpenGL support"));
    }
    
    QOffscreenSurface surface;
    surface.setFormat(context.format());
    surface.create();
    
    if (!context.makeCurrent(&surface)) {
        return createResult(false, ERROR, QCoreApplication::translate("SystemValidator", "OpenGL"),
                          QCoreApplication::translate("SystemValidator", "Unable to make OpenGL context current"),
                          QCoreApplication::translate("SystemValidator", "Update graphics drivers or check OpenGL support"));
    }
    
    // Use Qt's OpenGL function wrapper to avoid direct GL linking
    QOpenGLFunctions* gl = context.functions();
    if (!gl) {
        context.doneCurrent();
        return createResult(false, ERROR, QCoreApplication::translate("SystemValidator", "OpenGL"),
                          QCoreApplication::translate("SystemValidator", "Unable to get OpenGL functions"),
                          QCoreApplication::translate("SystemValidator", "Update graphics drivers or check OpenGL support"));
    }
    
    // Comment out these parts because of errors
    // Get OpenGL version using Qt functions
    const GLubyte* versionPtr = gl->glGetString(GL_VERSION);
    const GLubyte* vendorPtr = gl->glGetString(GL_VENDOR);
    
    context.doneCurrent();
    
    if (!versionPtr || !vendorPtr) {
        return createResult(false, WARNING, QCoreApplication::translate("SystemValidator", "OpenGL"),
                          QCoreApplication::translate("SystemValidator", "OpenGL version information not available"),
                          QCoreApplication::translate("SystemValidator", "Graphics acceleration may not work properly"));
    }
    
    QString glVersion = QString::fromLatin1(reinterpret_cast<const char*>(versionPtr));
    QString glVendor = QString::fromLatin1(reinterpret_cast<const char*>(vendorPtr));
    
    return createResult(true, INFO, QCoreApplication::translate("SystemValidator", "OpenGL"),
                       QCoreApplication::translate("SystemValidator", "OpenGL OK: %1 (%2)").arg(glVersion, glVendor));
}

SystemValidator::ValidationResult SystemValidator::validateFilePermissions()
{
    QString appDir = QCoreApplication::applicationDirPath();
    QString tempDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    
    // Check if we can write to temp directory
    QFileInfo tempInfo(tempDir);
    if (!tempInfo.isWritable()) {
        return createResult(false, ERROR, QCoreApplication::translate("SystemValidator", "File Permissions"),
                          QCoreApplication::translate("SystemValidator", "Cannot write to temp directory: %1").arg(tempDir),
                          QCoreApplication::translate("SystemValidator", "Check file permissions or run as administrator"));
    }
    
    // Check if we can create data directory
    QDir dataDirectory(dataDir);
    if (!dataDirectory.exists() && !dataDirectory.mkpath(".")) {
        return createResult(false, WARNING, QCoreApplication::translate("SystemValidator", "File Permissions"),
                          QCoreApplication::translate("SystemValidator", "Cannot create data directory: %1").arg(dataDir),
                          QCoreApplication::translate("SystemValidator", "Check file permissions for user data location"));
    }
    
    return createResult(true, INFO, QCoreApplication::translate("SystemValidator", "File Permissions"), QCoreApplication::translate("SystemValidator", "File system access OK"));
}

SystemValidator::ValidationResult SystemValidator::validateLocale()
{
    QLocale locale = QLocale::system();
    QString localeName = locale.name();
    
    // Basic locale validation
    if (localeName.isEmpty() || localeName == "C") {
        return createResult(false, WARNING, QCoreApplication::translate("SystemValidator", "Locale"),
                          QCoreApplication::translate("SystemValidator", "System locale not properly set"),
                          QCoreApplication::translate("SystemValidator", "Configure system locale for proper localization"));
    }
    
    return createResult(true, INFO, QCoreApplication::translate("SystemValidator", "Locale"),
                       QCoreApplication::translate("SystemValidator", "System locale: %1").arg(localeName));
}

SystemValidator::ValidationResult SystemValidator::validateNetwork()
{
    auto interfaces = QNetworkInterface::allInterfaces();
    bool hasActiveInterface = false;
    
    for (const QNetworkInterface& interface : interfaces) {
        if (interface.flags().testFlag(QNetworkInterface::IsUp) &&
            interface.flags().testFlag(QNetworkInterface::IsRunning) &&
            !interface.flags().testFlag(QNetworkInterface::IsLoopBack)) {
            hasActiveInterface = true;
            break;
        }
    }
    
    if (!hasActiveInterface) {
        return createResult(false, WARNING, QCoreApplication::translate("SystemValidator", "Network"),
                          QCoreApplication::translate("SystemValidator", "No active network interfaces found"),
                          QCoreApplication::translate("SystemValidator", "Network features may not work properly"));
    }
    
    return createResult(true, INFO, QCoreApplication::translate("SystemValidator", "Network"), QCoreApplication::translate("SystemValidator", "Network connectivity available"));
}

SystemValidator::ValidationResult SystemValidator::validateNetworkCapabilities()
{
    bool hasCapabilities = false;
    QString capabilityInfo;

#ifdef Q_OS_WIN
    // On Windows, check if running as administrator
    BOOL isAdmin = FALSE;
    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
    PSID AdministratorsGroup;

    if (AllocateAndInitializeSid(&NtAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID,
                                  DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0,
                                  &AdministratorsGroup)) {
        CheckTokenMembership(NULL, AdministratorsGroup, &isAdmin);
        FreeSid(AdministratorsGroup);
    }

    hasCapabilities = isAdmin;
    capabilityInfo = isAdmin ?
        QCoreApplication::translate("SystemValidator", "Running with administrator privileges") :
        QCoreApplication::translate("SystemValidator", "Not running as administrator");
#elif defined(Q_OS_LINUX) || defined(Q_OS_MACOS)
    // On Unix-like systems, check if running as root
    hasCapabilities = (geteuid() == 0);
    capabilityInfo = hasCapabilities ?
        QCoreApplication::translate("SystemValidator", "Running as root") :
        QCoreApplication::translate("SystemValidator", "Running as normal user");
#endif

    if (hasCapabilities) {
        return createResult(true, INFO, QCoreApplication::translate("SystemValidator", "Network Capabilities"),
                          QCoreApplication::translate("SystemValidator", "Full network access: %1").arg(capabilityInfo));
    } else {
        return createResult(true, WARNING, QCoreApplication::translate("SystemValidator", "Network Capabilities"),
                          QCoreApplication::translate("SystemValidator", "Limited network access: %1. Some features like raw ping may require elevated privileges").arg(capabilityInfo),
                          QCoreApplication::translate("SystemValidator", "Run as administrator/root for full network diagnostic capabilities"));
    }
}

SystemValidator::ValidationResult SystemValidator::validateDatabaseAccess()
{
    // Check if QSQLITE driver is available
    QStringList drivers = QSqlDatabase::drivers();

    if (!drivers.contains("QSQLITE")) {
        return createResult(false, ERROR, QCoreApplication::translate("SystemValidator", "Database Access"),
                          QCoreApplication::translate("SystemValidator", "SQLite driver not available"),
                          QCoreApplication::translate("SystemValidator", "Install Qt SQL SQLite driver"));
    }

    // Try to create a temporary database to verify write access
    QString tempDbPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/lanscan_db_test.db";
    QFile::remove(tempDbPath); // Remove if exists

    QSqlDatabase testDb = QSqlDatabase::addDatabase("QSQLITE", "validation_test");
    testDb.setDatabaseName(tempDbPath);

    if (!testDb.open()) {
        QSqlDatabase::removeDatabase("validation_test");
        return createResult(false, ERROR, QCoreApplication::translate("SystemValidator", "Database Access"),
                          QCoreApplication::translate("SystemValidator", "Cannot create test database"),
                          QCoreApplication::translate("SystemValidator", "Check file system permissions for temp directory"));
    }

    testDb.close();
    QSqlDatabase::removeDatabase("validation_test");
    QFile::remove(tempDbPath); // Clean up

    return createResult(true, INFO, QCoreApplication::translate("SystemValidator", "Database Access"),
                       QCoreApplication::translate("SystemValidator", "SQLite database access OK"));
}

QList<SystemValidator::ValidationResult> SystemValidator::runAllValidations()
{
    QList<ValidationResult> results;

    results << validateOperatingSystem();
    results << validateMemory();
    results << validateDiskSpace();
    results << validateScreen();
    results << validateQtVersion();
    results << validateOpenGL();
    results << validateFilePermissions();
    results << validateLocale();
    results << validateNetwork();
    results << validateNetworkCapabilities();
    results << validateDatabaseAccess();

    return results;
}

QList<SystemValidator::ValidationResult> SystemValidator::runCriticalValidations()
{
    QList<ValidationResult> results;
    
    results << validateMemory();
    results << validateDiskSpace();
    results << validateScreen();
    results << validateQtVersion();
    results << validateFilePermissions();
    
    return results;
}

bool SystemValidator::hasErrors(const QList<ValidationResult>& results)
{
    for (const ValidationResult& result : results) {
        if (!result.passed && (result.level == ERROR || result.level == CRITICAL)) {
            return true;
        }
    }
    return false;
}

bool SystemValidator::hasCriticalErrors(const QList<ValidationResult>& results)
{
    for (const ValidationResult& result : results) {
        if (!result.passed && result.level == CRITICAL) {
            return true;
        }
    }
    return false;
}

bool SystemValidator::hasWarnings(const QList<ValidationResult>& results)
{
    for (const ValidationResult& result : results) {
        if (!result.passed && result.level == WARNING) {
            return true;
        }
    }
    return false;
}

QString SystemValidator::getValidationReportText(const QList<ValidationResult>& results)
{
    QStringList lines;
    lines << QCoreApplication::translate("SystemValidator", "=== SYSTEM VALIDATION REPORT ===");
    lines << "";
    
    int passed = 0, warnings = 0, errors = 0;
    
    for (const ValidationResult& result : results) {
        QString status;
        if (result.passed) {
            status = QCoreApplication::translate("SystemValidator", "[PASS]");
            passed++;
        } else {
            switch (result.level) {
                case WARNING: status = QCoreApplication::translate("SystemValidator", "[WARN]"); warnings++; break;
                case ERROR: status = QCoreApplication::translate("SystemValidator", "[ERROR]"); errors++; break;
                case CRITICAL: status = QCoreApplication::translate("SystemValidator", "[CRITICAL]"); errors++; break;
                default: status = QCoreApplication::translate("SystemValidator", "[INFO]"); break;
            }
        }
        
        lines << QString("%1 %2: %3").arg(status, result.category, result.message);
        
        if (!result.passed && !result.suggestion.isEmpty()) {
            lines << QString(QCoreApplication::translate("SystemValidator", "    Suggestion: %1")).arg(result.suggestion);
        }
    }
    
    lines << "";
    lines << QCoreApplication::translate("SystemValidator", "Summary: %1 passed, %2 warnings, %3 errors")
             .arg(passed).arg(warnings).arg(errors);
    
    return lines.join("\n");
}

QString SystemValidator::getValidationReportHtml(const QList<ValidationResult>& results)
{
    QString html;
    html += QString("<h3>%1</h3>").arg(QCoreApplication::translate("SystemValidator", "System Validation Report"));
    html += "<table style='border-collapse: collapse; width: 100%; color: #ffffff;'>"; // Dark theme table
    html += QString("<tr style='background-color: #3c3c3c;'>"
            "<th style='padding: 5px; border: 1px solid #666666; color: #ffffff;'>%1</th>"
            "<th style='padding: 5px; border: 1px solid #666666; color: #ffffff;'>%2</th>"
            "<th style='padding: 5px; border: 1px solid #666666; color: #ffffff;'>%3</th>"
            "</tr>").arg(QCoreApplication::translate("SystemValidator", "Status"), QCoreApplication::translate("SystemValidator", "Category"), QCoreApplication::translate("SystemValidator", "Message"));
    
    int passed = 0, warnings = 0, errors = 0;
    
    for (const ValidationResult& result : results) {
        QString status, color;
        if (result.passed) {
            status = QCoreApplication::translate("SystemValidator", "PASS");
            color = "#28a745";
            passed++;
        } else {
            color = levelToColorHex(result.level);
            switch (result.level) {
                case WARNING: status = QCoreApplication::translate("SystemValidator", "WARN"); warnings++; break;
                case ERROR: status = QCoreApplication::translate("SystemValidator", "ERROR"); errors++; break;
                case CRITICAL: status = QCoreApplication::translate("SystemValidator", "CRITICAL"); errors++; break;
                default: status = QCoreApplication::translate("SystemValidator", "INFO"); break;
            }
        }
        
        html += QString("<tr><td style='padding: 5px; border: 1px solid #666666; color: %1; font-weight: bold;'>%2</td>"
                       "<td style='padding: 5px; border: 1px solid #666666; color: #ffffff;'>%3</td>"
                       "<td style='padding: 5px; border: 1px solid #666666; color: #ffffff;'>%4</td></tr>")
                .arg(color, status, result.category, result.message);
    }
    
    html += "</table>";
    html += QString("<p><strong>%1</strong> %2</p>")
            .arg(QCoreApplication::translate("SystemValidator", "Summary:"))
            .arg(QCoreApplication::translate("SystemValidator", "%1 passed, %2 warnings, %3 errors").arg(passed).arg(warnings).arg(errors));
    
    return html;
}

QString SystemValidator::getSummaryText(const QList<ValidationResult>& results)
{
    int passed = 0, warnings = 0, errors = 0;
    
    for (const ValidationResult& result : results) {
        if (result.passed) {
            passed++;
        } else {
            if (result.level == WARNING) warnings++;
            else errors++;
        }
    }
    
    return QCoreApplication::translate("SystemValidator", "Validation Summary: %1 passed, %2 warnings, %3 errors")
           .arg(passed).arg(warnings).arg(errors);
}

bool SystemValidator::isSystemCompatible()
{
    QList<ValidationResult> critical = runCriticalValidations();
    return !hasCriticalErrors(critical);
}

QString SystemValidator::getCompatibilityReport()
{
    QList<ValidationResult> results = runAllValidations();
    
    QString report = QCoreApplication::translate("SystemValidator", "System Compatibility Report") + "\n";
    report += "===========================\n\n";
    
    if (hasCriticalErrors(results)) {
        report += QCoreApplication::translate("SystemValidator", "❌ System NOT compatible - Critical errors found") + "\n\n";
    } else if (hasErrors(results)) {
        report += QCoreApplication::translate("SystemValidator", "⚠️  System compatible with issues") + "\n\n";
    } else if (hasWarnings(results)) {
        report += QCoreApplication::translate("SystemValidator", "✅ System compatible with minor warnings") + "\n\n";
    } else {
        report += QCoreApplication::translate("SystemValidator", "✅ System fully compatible") + "\n\n";
    }
    
    report += getValidationReportText(results);
    
    return report;
}

QString SystemValidator::levelToString(ValidationLevel level)
{
    switch (level) {
        case INFO: return QCoreApplication::translate("SystemValidator", "Info");
        case WARNING: return QCoreApplication::translate("SystemValidator", "Warning");
        case ERROR: return QCoreApplication::translate("SystemValidator", "Error");
        case CRITICAL: return QCoreApplication::translate("SystemValidator", "Critical");
        default: return QCoreApplication::translate("SystemValidator", "Unknown");
    }
}

QString SystemValidator::levelToColorHex(ValidationLevel level)
{
    switch (level) {
        case INFO: return "#17a2b8";      // Blue
        case WARNING: return "#ffc107";   // Yellow
        case ERROR: return "#dc3545";     // Red
        case CRITICAL: return "#6f42c1";  // Purple
        default: return "#6c757d";        // Gray
    }
}

bool SystemValidator::isOperatingSystemSupported()
{
    // Define supported OS versions
    auto currentVersion = QOperatingSystemVersion::current();
    
#ifdef Q_OS_WIN
    // Windows 10 and later
    return currentVersion >= QOperatingSystemVersion::Windows10;
#elif defined(Q_OS_MACOS)
    // macOS 10.15 and later
    return currentVersion >= QOperatingSystemVersion::MacOSCatalina;
#elif defined(Q_OS_LINUX)
    // Most Linux distributions are supported
    return true;
#else
    return false;
#endif
}

QString SystemValidator::getMinimumRequirementsText()
{
    return QCoreApplication::translate("SystemValidator",
        "Minimum System Requirements:\n"
        "- RAM: %1\n"
        "- Disk Space: %2\n"
        "- Screen Resolution: %3x%4\n"
        "- Qt Version: 6.0.0 or later\n"
        "- OpenGL support recommended"
    ).arg(SystemInfoCollector::formatBytes(MIN_RAM_BYTES))
     .arg(SystemInfoCollector::formatBytes(MIN_DISK_SPACE_BYTES))
     .arg(MIN_SCREEN_WIDTH)
     .arg(MIN_SCREEN_HEIGHT);
}

SystemValidator::ValidationResult SystemValidator::createResult(bool passed, ValidationLevel level,
                                                               const QString& category, const QString& message,
                                                               const QString& suggestion)
{
    return ValidationResult(passed, level, category, message, suggestion);
}
