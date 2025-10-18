#include "SystemInfoCollector.h"
#include <QSysInfo>
#include <QThread>
#include <QScreen>
#include <QGuiApplication>
#include <QStorageInfo>
#include <QStandardPaths>
#include <QLocale>
#include <QOperatingSystemVersion>
#include <QDir>
#include <QCoreApplication>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOffscreenSurface>
#include <QNetworkInterface>

#ifdef Q_OS_WIN
    #include <windows.h>
    #include <sysinfoapi.h>
#elif defined(Q_OS_LINUX)
    #include <sys/sysinfo.h>
    #include <unistd.h>
    #include <fstream>
    #include <string>
#elif defined(Q_OS_MACOS)
    #include <sys/types.h>
    #include <sys/sysctl.h>
    #include <mach/mach.h>
#endif

QString SystemInfoCollector::getOperatingSystem()
{
    return QSysInfo::prettyProductName();
}

QString SystemInfoCollector::getOperatingSystemVersion()
{
    auto version = QOperatingSystemVersion::current();
    return QString("%1.%2.%3").arg(version.majorVersion())
                              .arg(version.minorVersion())
                              .arg(version.microVersion());
}

QString SystemInfoCollector::getKernelVersion()
{
    return QSysInfo::kernelVersion();
}

QString SystemInfoCollector::getMachineHostName()
{
    return QSysInfo::machineHostName();
}

QString SystemInfoCollector::getCpuArchitecture()
{
    return QSysInfo::currentCpuArchitecture();
}

QString SystemInfoCollector::getProcessorInfo()
{
    return QCoreApplication::translate("SystemInfoCollector", "Architecture: %1, Cores: %2")
           .arg(getCpuArchitecture())
           .arg(getProcessorCoreCount());
}

int SystemInfoCollector::getProcessorCoreCount()
{
    return QThread::idealThreadCount();
}

quint64 SystemInfoCollector::getTotalPhysicalMemory()
{
#ifdef Q_OS_WIN
    return getPhysicalMemoryWindows();
#elif defined(Q_OS_LINUX)
    return getPhysicalMemoryLinux();
#elif defined(Q_OS_MACOS)
    return getPhysicalMemoryMacOS();
#else
    return 0;
#endif
}

quint64 SystemInfoCollector::getAvailablePhysicalMemory()
{
#ifdef Q_OS_WIN
    return getAvailableMemoryWindows();
#elif defined(Q_OS_LINUX)
    return getAvailableMemoryLinux();
#elif defined(Q_OS_MACOS)
    return getAvailableMemoryMacOS();
#else
    return 0;
#endif
}

QString SystemInfoCollector::getMemoryInfo()
{
    quint64 total = getTotalPhysicalMemory();
    quint64 available = getAvailablePhysicalMemory();
    quint64 used = total - available;
    
    if (total == 0) return QCoreApplication::translate("SystemInfoCollector", "Memory info not available");
    
    double usedPercent = (double)used / total * 100.0;
    
    return QCoreApplication::translate("SystemInfoCollector", "Total: %1, Used: %2 (%3), Available: %4")
           .arg(formatBytes(total))
           .arg(formatBytes(used))
           .arg(formatPercentage(usedPercent))
           .arg(formatBytes(available));
}

QStringList SystemInfoCollector::getScreenInfo()
{
    QStringList screens;
    auto screenList = QGuiApplication::screens();
    
    for (int i = 0; i < screenList.size(); ++i) {
        QScreen* screen = screenList[i];
        QString info = QCoreApplication::translate("SystemInfoCollector", "Screen %1: %2x%3 at %4 DPI (%5 Hz)")
                       .arg(i + 1)
                       .arg(screen->geometry().width())
                       .arg(screen->geometry().height())
                       .arg(qRound(screen->logicalDotsPerInch()))
                       .arg(screen->refreshRate());
        screens << info;
    }
    
    return screens;
}

QString SystemInfoCollector::getPrimaryScreenInfo()
{
    QScreen* primary = QGuiApplication::primaryScreen();
    if (!primary) return QCoreApplication::translate("SystemInfoCollector", "No primary screen found");
    
    return QCoreApplication::translate("SystemInfoCollector", "Primary: %1x%2 at %3 DPI, %4 Hz")
           .arg(primary->geometry().width())
           .arg(primary->geometry().height())
           .arg(qRound(primary->logicalDotsPerInch()))
           .arg(primary->refreshRate());
}

QStringList SystemInfoCollector::getStorageInfo()
{
    QStringList storages;
    auto volumes = QStorageInfo::mountedVolumes();
    
    for (const QStorageInfo& storage : volumes) {
        if (storage.isValid() && storage.isReady()) {
            QString info = QCoreApplication::translate("SystemInfoCollector", "%1 (%2): %3 total, %4 available")
                          .arg(storage.rootPath())
                          .arg(storage.fileSystemType())
                          .arg(formatBytes(storage.bytesTotal()))
                          .arg(formatBytes(storage.bytesAvailable()));
            storages << info;
        }
    }
    
    return storages;
}

quint64 SystemInfoCollector::getAvailableDiskSpace()
{
    QString appPath = QCoreApplication::applicationDirPath();
    QStorageInfo storage(appPath);
    return storage.isValid() ? storage.bytesAvailable() : 0;
}

QString SystemInfoCollector::getQtVersion()
{
    return QCoreApplication::translate("SystemInfoCollector", "Compile: %1, Runtime: %2")
           .arg(QT_VERSION_STR)
           .arg(qVersion());
}

QString SystemInfoCollector::getQtVersionRuntime()
{
    return qVersion();
}

QString SystemInfoCollector::getCompilerInfo()
{
#if defined(Q_CC_MSVC)
    return QString("MSVC %1").arg(_MSC_VER);
#elif defined(Q_CC_GNU)
    return QString("GCC %1.%2.%3")
           .arg(__GNUC__)
           .arg(__GNUC_MINOR__)
           .arg(__GNUC_PATCHLEVEL__);
#elif defined(Q_CC_CLANG)
    return QString("Clang %1.%2.%3")
           .arg(__clang_major__)
           .arg(__clang_minor__)
           .arg(__clang_patchlevel__);
#else
    return QCoreApplication::translate("SystemInfoCollector", "Unknown compiler");
#endif
}

QString SystemInfoCollector::getApplicationInfo()
{
    return QCoreApplication::translate("SystemInfoCollector", "Name: %1, Version: %2, Path: %3")
           .arg(QCoreApplication::applicationName())
           .arg(QCoreApplication::applicationVersion().isEmpty() ? 
                QCoreApplication::translate("SystemInfoCollector", "Not set") : QCoreApplication::applicationVersion())
           .arg(QCoreApplication::applicationDirPath());
}

QString SystemInfoCollector::getSystemLocale()
{
    QLocale locale = QLocale::system();
    return QCoreApplication::translate("SystemInfoCollector", "Language: %1, Country: %2, Name: %3")
           .arg(QLocale::languageToString(locale.language()))
           .arg(QLocale::territoryToString(locale.territory()))
           .arg(locale.name());
}

QString SystemInfoCollector::getEnvironmentInfo()
{
    QStringList envVars = {"PATH", "TEMP", "TMP", "HOME", "USER", "USERNAME"};
    QStringList info;
    
    for (const QString& var : envVars) {
        QString value = qgetenv(var.toLatin1());
        if (!value.isEmpty()) {
            // Truncate long paths
            if (value.length() > 100) {
                value = value.left(50) + "..." + value.right(47);
            }
            info << QString("%1=%2").arg(var, value);
        }
    }
    
    return info.join("; ");
}

QString SystemInfoCollector::getGraphicsInfo()
{
    QString info = QCoreApplication::translate("SystemInfoCollector", "Graphics info not available");
    
    // Try to get OpenGL info
    QOpenGLContext context;
    QOffscreenSurface surface;
    surface.create();
    
    if (context.create()) {
        context.makeCurrent(&surface);
        QOpenGLFunctions* functions = context.functions();
        if (functions) {
            const char* vendor = reinterpret_cast<const char*>(functions->glGetString(GL_VENDOR));
            const char* renderer = reinterpret_cast<const char*>(functions->glGetString(GL_RENDERER));
            const char* version = reinterpret_cast<const char*>(functions->glGetString(GL_VERSION));
            
            info = QCoreApplication::translate("SystemInfoCollector", "Vendor: %1, Renderer: %2, Version: %3")
                   .arg(vendor ? vendor : QCoreApplication::translate("SystemInfoCollector", "Unknown"))
                   .arg(renderer ? renderer : QCoreApplication::translate("SystemInfoCollector", "Unknown"))
                   .arg(version ? version : QCoreApplication::translate("SystemInfoCollector", "Unknown"));
        }
        context.doneCurrent();
    }
    
    return info;
}

QString SystemInfoCollector::getNetworkInfo()
{
    QStringList interfaces;
    auto networkInterfaces = QNetworkInterface::allInterfaces();
    
    for (const QNetworkInterface& netInterface : networkInterfaces) {
        if (netInterface.flags().testFlag(QNetworkInterface::IsUp) &&
            !netInterface.flags().testFlag(QNetworkInterface::IsLoopBack)) {
            interfaces << QCoreApplication::translate("SystemInfoCollector", "%1 (%2)")
                         .arg(netInterface.name())
                         .arg(netInterface.hardwareAddress());
        }
    }
    
    return interfaces.isEmpty() ? QCoreApplication::translate("SystemInfoCollector", "No active interfaces") : interfaces.join(", ");
}

QVariantMap SystemInfoCollector::getCompleteSystemInfo()
{
    QVariantMap info;
    
    // System
    info["os"] = getOperatingSystem();
    info["os_version"] = getOperatingSystemVersion();
    info["kernel"] = getKernelVersion();
    info["hostname"] = getMachineHostName();
    info["architecture"] = getCpuArchitecture();
    
    // Hardware
    info["processor"] = getProcessorInfo();
    info["memory"] = getMemoryInfo();
    info["primary_screen"] = getPrimaryScreenInfo();
    info["available_disk_space"] = formatBytes(getAvailableDiskSpace());
    
    // Software
    info["qt_version"] = getQtVersion();
    info["compiler"] = getCompilerInfo();
    info["application"] = getApplicationInfo();
    
    // Environment
    info["locale"] = getSystemLocale();
    info["graphics"] = getGraphicsInfo();
    info["network"] = getNetworkInfo();
    
    return info;
}

QString SystemInfoCollector::getSystemInfoText()
{
    QVariantMap info = getCompleteSystemInfo();
    QStringList lines;
    
    lines << QCoreApplication::translate("SystemInfoCollector", "=== SYSTEM INFORMATION ===");
    lines << "";
    lines << QCoreApplication::translate("SystemInfoCollector", "Operating System: ") + info["os"].toString();
    lines << QCoreApplication::translate("SystemInfoCollector", "OS Version: ") + info["os_version"].toString();
    lines << QCoreApplication::translate("SystemInfoCollector", "Kernel: ") + info["kernel"].toString();
    lines << QCoreApplication::translate("SystemInfoCollector", "Hostname: ") + info["hostname"].toString();
    lines << QCoreApplication::translate("SystemInfoCollector", "Architecture: ") + info["architecture"].toString();
    lines << "";
    lines << QCoreApplication::translate("SystemInfoCollector", "Processor: ") + info["processor"].toString();
    lines << QCoreApplication::translate("SystemInfoCollector", "Memory: ") + info["memory"].toString();
    lines << QCoreApplication::translate("SystemInfoCollector", "Primary Screen: ") + info["primary_screen"].toString();
    lines << QCoreApplication::translate("SystemInfoCollector", "Available Disk Space: ") + info["available_disk_space"].toString();
    lines << "";
    lines << QCoreApplication::translate("SystemInfoCollector", "Qt Version: ") + info["qt_version"].toString();
    lines << QCoreApplication::translate("SystemInfoCollector", "Compiler: ") + info["compiler"].toString();
    lines << QCoreApplication::translate("SystemInfoCollector", "Application: ") + info["application"].toString();
    lines << "";
    lines << QCoreApplication::translate("SystemInfoCollector", "System Locale: ") + info["locale"].toString();
    lines << QCoreApplication::translate("SystemInfoCollector", "Graphics: ") + info["graphics"].toString();
    lines << QCoreApplication::translate("SystemInfoCollector", "Network: ") + info["network"].toString();
    
    return lines.join("\n");
}

QString SystemInfoCollector::getSystemInfoHtml()
{
    QVariantMap info = getCompleteSystemInfo();
    QString html;
    
    html += QString("<h3>%1</h3>").arg(QCoreApplication::translate("SystemInfoCollector", "System Information"));
    html += "<table style='border-collapse: collapse; width: 100%; color: #ffffff;'>";
    html += QString("<tr style='background-color: #3c3c3c;'><th style='padding: 5px; text-align: left; border: 1px solid #666666; color: #ffffff;'>%1</th><th style='padding: 5px; text-align: left; border: 1px solid #666666; color: #ffffff;'>%2</th></tr>")
            .arg(QCoreApplication::translate("SystemInfoCollector", "Category"))
            .arg(QCoreApplication::translate("SystemInfoCollector", "Value"));
    
    auto addRow = [&html](const QString& key, const QString& value) {
        html += QString("<tr><td style='padding: 5px; border: 1px solid #666666; color: #ffffff;'><b>%1</b></td><td style='padding: 5px; border: 1px solid #666666; color: #ffffff;'>%2</td></tr>")
                .arg(key, value);
    };
    
    addRow(QCoreApplication::translate("SystemInfoCollector", "Operating System"), info["os"].toString());
    addRow(QCoreApplication::translate("SystemInfoCollector", "OS Version"), info["os_version"].toString());
    addRow(QCoreApplication::translate("SystemInfoCollector", "Kernel"), info["kernel"].toString());
    addRow(QCoreApplication::translate("SystemInfoCollector", "Hostname"), info["hostname"].toString());
    addRow(QCoreApplication::translate("SystemInfoCollector", "Architecture"), info["architecture"].toString());
    addRow(QCoreApplication::translate("SystemInfoCollector", "Processor"), info["processor"].toString());
    addRow(QCoreApplication::translate("SystemInfoCollector", "Memory"), info["memory"].toString());
    addRow(QCoreApplication::translate("SystemInfoCollector", "Primary Screen"), info["primary_screen"].toString());
    addRow(QCoreApplication::translate("SystemInfoCollector", "Available Disk Space"), info["available_disk_space"].toString());
    addRow(QCoreApplication::translate("SystemInfoCollector", "Qt Version"), info["qt_version"].toString());
    addRow(QCoreApplication::translate("SystemInfoCollector", "Compiler"), info["compiler"].toString());
    addRow(QCoreApplication::translate("SystemInfoCollector", "Application"), info["application"].toString());
    addRow(QCoreApplication::translate("SystemInfoCollector", "System Locale"), info["locale"].toString());
    addRow(QCoreApplication::translate("SystemInfoCollector", "Graphics"), info["graphics"].toString());
    addRow(QCoreApplication::translate("SystemInfoCollector", "Network"), info["network"].toString());
    
    html += "</table>";
    return html;
}

// Platform-specific implementations
quint64 SystemInfoCollector::getPhysicalMemoryWindows()
{
#ifdef Q_OS_WIN
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    if (GlobalMemoryStatusEx(&memInfo)) {
        return memInfo.ullTotalPhys;
    }
#endif
    return 0;
}

quint64 SystemInfoCollector::getPhysicalMemoryLinux()
{
#ifdef Q_OS_LINUX
    struct sysinfo info;
    if (sysinfo(&info) == 0) {
        return info.totalram * info.mem_unit;
    }
#endif
    return 0;
}

quint64 SystemInfoCollector::getPhysicalMemoryMacOS()
{
#ifdef Q_OS_MACOS
    int64_t memsize;
    size_t size = sizeof(memsize);
    if (sysctlbyname("hw.memsize", &memsize, &size, nullptr, 0) == 0) {
        return memsize;
    }
#endif
    return 0;
}

quint64 SystemInfoCollector::getAvailableMemoryWindows()
{
#ifdef Q_OS_WIN
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    if (GlobalMemoryStatusEx(&memInfo)) {
        return memInfo.ullAvailPhys;
    }
#endif
    return 0;
}

quint64 SystemInfoCollector::getAvailableMemoryLinux()
{
#ifdef Q_OS_LINUX
    std::ifstream meminfo("/proc/meminfo");
    std::string line;
    quint64 available = 0;
    
    while (std::getline(meminfo, line)) {
        if (line.find("MemAvailable:") == 0) {
            // Extract number (in kB)
            size_t pos = line.find_first_of("0123456789");
            if (pos != std::string::npos) {
                available = std::stoull(line.substr(pos)) * 1024; // Convert kB to bytes
                break;
            }
        }
    }
    
    return available;
#endif
    return 0;
}

quint64 SystemInfoCollector::getAvailableMemoryMacOS()
{
#ifdef Q_OS_MACOS
    vm_statistics_data_t vmstat;
    mach_msg_type_number_t count = HOST_VM_INFO_COUNT;
    
    if (host_statistics(mach_host_self(), HOST_VM_INFO, 
                       reinterpret_cast<host_info_t>(&vmstat), &count) == KERN_SUCCESS) {
        return vmstat.free_count * vm_page_size;
    }
#endif
    return 0;
}

QString SystemInfoCollector::formatBytes(quint64 bytes)
{
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int unitIndex = 0;
    double size = bytes;
    
    while (size >= 1024 && unitIndex < 4) {
        size /= 1024.0;
        unitIndex++;
    }
    
    return QString("%1 %2").arg(size, 0, 'f', unitIndex > 0 ? 2 : 0).arg(units[unitIndex]);
}

QString SystemInfoCollector::formatPercentage(double value)
{
    return QString("%1%").arg(value, 0, 'f', 1);
}