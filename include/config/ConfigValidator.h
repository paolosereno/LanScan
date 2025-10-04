#ifndef CONFIGVALIDATOR_H
#define CONFIGVALIDATOR_H

#include <QString>

class ConfigValidator {
public:
    struct ValidationResult {
        bool isValid;
        QString errorMessage;

        ValidationResult(bool valid = true, const QString& error = "")
            : isValid(valid), errorMessage(error) {}
    };

    static ValidationResult validateSubnet(const QString& subnet);
    static ValidationResult validateTimeout(int timeout);
    static ValidationResult validateThreadCount(int threads);
    static ValidationResult validateExportPath(const QString& path);
    static ValidationResult validateTheme(const QString& theme);
    static ValidationResult validateLanguage(const QString& lang);

    static bool isValidIpRange(const QString& start, const QString& end);
    static bool isValidPort(int port);
    static bool isValidPortRange(int start, int end);

private:
    ConfigValidator() = delete; // Static class
};

#endif // CONFIGVALIDATOR_H
