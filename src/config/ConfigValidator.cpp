#include "config/ConfigValidator.h"
#include "utils/IpAddressValidator.h"
#include <QDir>
#include <QFileInfo>
#include <QHostAddress>

ConfigValidator::ValidationResult ConfigValidator::validateSubnet(const QString& subnet) {
    if (subnet.isEmpty()) {
        return ValidationResult(false, "Subnet cannot be empty");
    }

    if (!IpAddressValidator::isValidCidr(subnet)) {
        return ValidationResult(false, "Invalid CIDR notation. Expected format: x.x.x.x/y");
    }

    return ValidationResult(true);
}

ConfigValidator::ValidationResult ConfigValidator::validateTimeout(int timeout) {
    if (timeout < 100) {
        return ValidationResult(false, "Timeout must be at least 100 ms");
    }

    if (timeout > 60000) {
        return ValidationResult(false, "Timeout cannot exceed 60000 ms (60 seconds)");
    }

    return ValidationResult(true);
}

ConfigValidator::ValidationResult ConfigValidator::validateThreadCount(int threads) {
    if (threads < 1) {
        return ValidationResult(false, "Thread count must be at least 1");
    }

    if (threads > 100) {
        return ValidationResult(false, "Thread count cannot exceed 100");
    }

    return ValidationResult(true);
}

ConfigValidator::ValidationResult ConfigValidator::validateExportPath(const QString& path) {
    if (path.isEmpty()) {
        return ValidationResult(false, "Export path cannot be empty");
    }

    QFileInfo fileInfo(path);
    QDir dir = fileInfo.dir();

    if (!dir.exists()) {
        // Try to create the directory
        if (!QDir().mkpath(dir.absolutePath())) {
            return ValidationResult(false, "Cannot create directory: " + dir.absolutePath());
        }
    }

    // Check if directory is writable
    if (!QFileInfo(dir.absolutePath()).isWritable()) {
        return ValidationResult(false, "Export path is not writable: " + dir.absolutePath());
    }

    return ValidationResult(true);
}

ConfigValidator::ValidationResult ConfigValidator::validateTheme(const QString& theme) {
    QStringList validThemes = {"light", "dark"};

    if (!validThemes.contains(theme.toLower())) {
        return ValidationResult(false, "Invalid theme. Valid options: light, dark");
    }

    return ValidationResult(true);
}

ConfigValidator::ValidationResult ConfigValidator::validateLanguage(const QString& lang) {
    QStringList validLanguages = {"en", "it", "es", "fr", "de"};

    if (!validLanguages.contains(lang.toLower())) {
        return ValidationResult(false, "Invalid language code. Valid options: en, it, es, fr, de");
    }

    return ValidationResult(true);
}

bool ConfigValidator::isValidIpRange(const QString& start, const QString& end) {
    if (!IpAddressValidator::isValidIpv4(start) || !IpAddressValidator::isValidIpv4(end)) {
        return false;
    }

    QHostAddress startAddr(start);
    QHostAddress endAddr(end);

    // Check if end is greater than or equal to start
    return startAddr.toIPv4Address() <= endAddr.toIPv4Address();
}

bool ConfigValidator::isValidPort(int port) {
    return port >= 1 && port <= 65535;
}

bool ConfigValidator::isValidPortRange(int start, int end) {
    return isValidPort(start) && isValidPort(end) && start <= end;
}
