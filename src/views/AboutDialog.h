#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include <QTabWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QClipboard>

/**
 * @brief About dialog displaying application information and system details
 *
 * AboutDialog provides a comprehensive information dialog with multiple tabs
 * showing application details, system information, and system validation results.
 * Includes functionality for copying information to clipboard.
 *
 * @author Paolo
 * @date 2025
 */
class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Constructor for AboutDialog
     * @param parent Parent widget
     */
    explicit AboutDialog(QWidget *parent = nullptr);
    
    /**
     * @brief Destructor
     */
    ~AboutDialog() = default;

private slots:
    /**
     * @brief Copies system information to clipboard
     */
    void copySystemInfoToClipboard();
    
    /**
     * @brief Copies validation report to clipboard
     */
    void copyValidationReportToClipboard();
    
    /**
     * @brief Runs system validation checks
     */
    void runSystemValidation();

private:
    void setupUI();
    void setupAboutTab();
    void setupSystemInfoTab();
    void setupValidationTab();
    void setupButtonBox();
    
    void populateSystemInfo();
    void populateValidationInfo();
    
    // UI Components
    QTabWidget* m_tabWidget;
    QVBoxLayout* m_mainLayout;
    
    // About Tab
    QWidget* m_aboutTab;
    QLabel* m_logoLabel;
    QLabel* m_appInfoLabel;
    QTextEdit* m_featuresText;
    
    // System Info Tab
    QWidget* m_systemInfoTab;
    QTextEdit* m_systemInfoText;
    QPushButton* m_copySystemInfoButton;
    
    // Validation Tab
    QWidget* m_validationTab;
    QTextEdit* m_validationText;
    QPushButton* m_runValidationButton;
    QPushButton* m_copyValidationButton;
    QLabel* m_validationSummaryLabel;
    
    // Button Box
    QHBoxLayout* m_buttonLayout;
    QPushButton* m_closeButton;
    
    void updateValidationSummary();
};

#endif // ABOUTDIALOG_H