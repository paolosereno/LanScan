/**
 * @file systemvalidator.h
 * @brief Header file for the SystemValidator class
 * @author Paolo
 * @date 2025
 */

#ifndef SYSTEMVALIDATOR_H
#define SYSTEMVALIDATOR_H

#include <QString>
#include <QStringList>
#include <QPair>

/**
 * @brief Static class for validating system requirements and compatibility
 */
class SystemValidator
{
public:
    /**
     * @brief Enumeration for validation severity levels
     */
    enum ValidationLevel {
        INFO,     // Informational message
        WARNING,  // Warning message
        ERROR,    // Error message
        CRITICAL  // Critical error message
    };

    /**
     * @brief Structure representing a validation result
     */
    struct ValidationResult {
        bool passed;              // Whether the validation passed
        ValidationLevel level;    // Severity level of the result
        QString category;         // Category of the validation
        QString message;          // Validation message
        QString suggestion;       // Suggestion for resolving issues
        
        /**
         * @brief Constructor for ValidationResult
         * @param p Whether the validation passed
         * @param l Severity level
         * @param cat Category of validation
         * @param msg Validation message
         * @param sug Suggestion for resolving issues
         */
        ValidationResult(bool p = true, ValidationLevel l = INFO, 
                        const QString& cat = "", const QString& msg = "", 
                        const QString& sug = "")
            : passed(p), level(l), category(cat), message(msg), suggestion(sug) {}
    };

    /**
     * @brief Deleted constructor - this is a static utility class
     */
    SystemValidator() = delete;

    // System Requirements
    static constexpr quint64 MIN_RAM_BYTES = 2ULL * 1024 * 1024 * 1024; // Minimum RAM: 2 GB
    static constexpr quint64 MIN_DISK_SPACE_BYTES = 200 * 1024 * 1024; // Minimum disk space: 200 MB
    static constexpr int MIN_SCREEN_WIDTH = 1024;  // Minimum screen width
    static constexpr int MIN_SCREEN_HEIGHT = 768; // Minimum screen height

    // Individual validation functions
    /**
     * @brief Validates operating system compatibility
     * @return Validation result for operating system
     */
    static ValidationResult validateOperatingSystem();
    
    /**
     * @brief Validates available memory
     * @return Validation result for memory
     */
    static ValidationResult validateMemory();
    
    /**
     * @brief Validates available disk space
     * @return Validation result for disk space
     */
    static ValidationResult validateDiskSpace();
    
    /**
     * @brief Validates screen resolution and capabilities
     * @return Validation result for screen
     */
    static ValidationResult validateScreen();
    
    /**
     * @brief Validates Qt framework version
     * @return Validation result for Qt version
     */
    static ValidationResult validateQtVersion();
    
    /**
     * @brief Validates OpenGL support
     * @return Validation result for OpenGL
     */
    static ValidationResult validateOpenGL();
    
    /**
     * @brief Validates file system permissions
     * @return Validation result for file permissions
     */
    static ValidationResult validateFilePermissions();
    
    /**
     * @brief Validates system locale settings
     * @return Validation result for locale
     */
    static ValidationResult validateLocale();
    
    /**
     * @brief Validates network connectivity
     * @return Validation result for network
     */
    static ValidationResult validateNetwork();

    /**
     * @brief Validates network capabilities (raw socket permissions for ping)
     * @return Validation result for network capabilities
     */
    static ValidationResult validateNetworkCapabilities();

    /**
     * @brief Validates database access (SQLite driver availability)
     * @return Validation result for database access
     */
    static ValidationResult validateDatabaseAccess();

    // Comprehensive validation
    /**
     * @brief Runs all available validations
     * @return List of all validation results
     */
    static QList<ValidationResult> runAllValidations();
    
    /**
     * @brief Runs only critical validations
     * @return List of critical validation results
     */
    static QList<ValidationResult> runCriticalValidations();
    
    // Results analysis
    /**
     * @brief Checks if results contain any errors
     * @param results List of validation results to check
     * @return true if any errors are found
     */
    static bool hasErrors(const QList<ValidationResult>& results);
    
    /**
     * @brief Checks if results contain any critical errors
     * @param results List of validation results to check
     * @return true if any critical errors are found
     */
    static bool hasCriticalErrors(const QList<ValidationResult>& results);
    
    /**
     * @brief Checks if results contain any warnings
     * @param results List of validation results to check
     * @return true if any warnings are found
     */
    static bool hasWarnings(const QList<ValidationResult>& results);
    
    // Reporting
    /**
     * @brief Generates a text report of validation results
     * @param results List of validation results
     * @return Formatted text report
     */
    static QString getValidationReportText(const QList<ValidationResult>& results);
    
    /**
     * @brief Generates an HTML report of validation results
     * @param results List of validation results
     * @return Formatted HTML report
     */
    static QString getValidationReportHtml(const QList<ValidationResult>& results);
    
    /**
     * @brief Generates a summary text of validation results
     * @param results List of validation results
     * @return Summary text
     */
    static QString getSummaryText(const QList<ValidationResult>& results);
    
    // System compatibility check
    /**
     * @brief Checks overall system compatibility
     * @return true if system meets minimum requirements
     */
    static bool isSystemCompatible();
    
    /**
     * @brief Gets a comprehensive compatibility report
     * @return Compatibility report string
     */
    static QString getCompatibilityReport();
    
    // Utilities
    /**
     * @brief Converts validation level to string representation
     * @param level Validation level to convert
     * @return String representation of the level
     */
    static QString levelToString(ValidationLevel level);
    
    /**
     * @brief Gets color hex code for validation level
     * @param level Validation level
     * @return Hex color code for the level
     */
    static QString levelToColorHex(ValidationLevel level);

private:
    /**
     * @brief Checks if the current operating system is supported
     * @return true if operating system is supported
     */
    static bool isOperatingSystemSupported();
    
    /**
     * @brief Gets text describing minimum system requirements
     * @return Minimum requirements text
     */
    static QString getMinimumRequirementsText();
    
    /**
     * @brief Creates a validation result with the given parameters
     * @param passed Whether the validation passed
     * @param level Severity level
     * @param category Category of validation
     * @param message Validation message
     * @param suggestion Suggestion for resolving issues
     * @return Constructed ValidationResult
     */
    static ValidationResult createResult(bool passed, ValidationLevel level,
                                       const QString& category, const QString& message,
                                       const QString& suggestion = "");
};

#endif // SYSTEMVALIDATOR_H
