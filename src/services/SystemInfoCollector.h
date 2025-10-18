/**
 * @file systeminfocollector.h
 * @brief Header file for the SystemInfoCollector class
 * @author Paolo
 * @date 2025
 */

#ifndef SYSTEMINFOCOLLECTOR_H
#define SYSTEMINFOCOLLECTOR_H

#include <QString>
#include <QStringList>
#include <QVariantMap>

/**
 * @brief Static class for collecting comprehensive system information
 */
class SystemInfoCollector
{
public:
    /**
     * @brief Deleted constructor - this is a static utility class
     */
    SystemInfoCollector() = delete;

    // Operating System Information
    /**
     * @brief Gets the operating system name
     * @return Operating system name
     */
    static QString getOperatingSystem();
    
    /**
     * @brief Gets the operating system version
     * @return Operating system version string
     */
    static QString getOperatingSystemVersion();
    
    /**
     * @brief Gets the kernel version
     * @return Kernel version string
     */
    static QString getKernelVersion();
    
    /**
     * @brief Gets the machine host name
     * @return Host name of the machine
     */
    static QString getMachineHostName();
    
    /**
     * @brief Gets the CPU architecture
     * @return CPU architecture string
     */
    static QString getCpuArchitecture();

    // Hardware Information
    /**
     * @brief Gets detailed processor information
     * @return Processor information string
     */
    static QString getProcessorInfo();
    
    /**
     * @brief Gets the number of processor cores
     * @return Number of CPU cores
     */
    static int getProcessorCoreCount();
    
    /**
     * @brief Gets total physical memory in bytes
     * @return Total physical memory
     */
    static quint64 getTotalPhysicalMemory();
    
    /**
     * @brief Gets available physical memory in bytes
     * @return Available physical memory
     */
    static quint64 getAvailablePhysicalMemory();
    
    /**
     * @brief Gets formatted memory information
     * @return Memory information string
     */
    static QString getMemoryInfo();

    // Display Information
    /**
     * @brief Gets information about all screens
     * @return List of screen information strings
     */
    static QStringList getScreenInfo();
    
    /**
     * @brief Gets information about the primary screen
     * @return Primary screen information string
     */
    static QString getPrimaryScreenInfo();

    // Storage Information
    /**
     * @brief Gets information about all storage devices
     * @return List of storage information strings
     */
    static QStringList getStorageInfo();
    
    /**
     * @brief Gets available disk space in bytes
     * @return Available disk space
     */
    static quint64 getAvailableDiskSpace();

    // Qt and Application Information
    /**
     * @brief Gets Qt framework version at compile time
     * @return Qt version string
     */
    static QString getQtVersion();
    
    /**
     * @brief Gets Qt framework version at runtime
     * @return Qt runtime version string
     */
    static QString getQtVersionRuntime();
    
    /**
     * @brief Gets compiler information used to build the application
     * @return Compiler information string
     */
    static QString getCompilerInfo();
    
    /**
     * @brief Gets application-specific information
     * @return Application information string
     */
    static QString getApplicationInfo();

    // Locale and Environment
    /**
     * @brief Gets system locale information
     * @return System locale string
     */
    static QString getSystemLocale();
    
    /**
     * @brief Gets environment variables information
     * @return Environment information string
     */
    static QString getEnvironmentInfo();

    // Graphics Information
    /**
     * @brief Gets graphics system information
     * @return Graphics information string
     */
    static QString getGraphicsInfo();

    // Network Information (basic)
    /**
     * @brief Gets basic network configuration information
     * @return Network information string
     */
    static QString getNetworkInfo();

    // Complete system report
    /**
     * @brief Gets complete system information as a variant map
     * @return Map containing all system information
     */
    static QVariantMap getCompleteSystemInfo();
    
    /**
     * @brief Gets complete system information as formatted text
     * @return System information text report
     */
    static QString getSystemInfoText();
    
    /**
     * @brief Gets complete system information as formatted HTML
     * @return System information HTML report
     */
    static QString getSystemInfoHtml();

    // Public utility methods
    /**
     * @brief Formats byte count into human-readable string
     * @param bytes Number of bytes to format
     * @return Formatted bytes string (e.g., "1.5 GB")
     */
    static QString formatBytes(quint64 bytes);
    
    /**
     * @brief Formats a percentage value
     * @param value Percentage value to format
     * @return Formatted percentage string
     */
    static QString formatPercentage(double value);

private:
    // Platform-specific helpers
    /**
     * @brief Gets physical memory on Windows systems
     * @return Physical memory in bytes
     */
    static quint64 getPhysicalMemoryWindows();
    
    /**
     * @brief Gets physical memory on Linux systems
     * @return Physical memory in bytes
     */
    static quint64 getPhysicalMemoryLinux();
    
    /**
     * @brief Gets physical memory on macOS systems
     * @return Physical memory in bytes
     */
    static quint64 getPhysicalMemoryMacOS();
    
    /**
     * @brief Gets available memory on Windows systems
     * @return Available memory in bytes
     */
    static quint64 getAvailableMemoryWindows();
    
    /**
     * @brief Gets available memory on Linux systems
     * @return Available memory in bytes
     */
    static quint64 getAvailableMemoryLinux();
    
    /**
     * @brief Gets available memory on macOS systems
     * @return Available memory in bytes
     */
    static quint64 getAvailableMemoryMacOS();
};

#endif // SYSTEMINFOCOLLECTOR_H