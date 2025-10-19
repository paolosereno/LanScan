#include "AboutDialog.h"
#include "../services/SystemInfoCollector.h"
#include "../services/SystemValidator.h"
#include "../utils/IconLoader.h"
#include "version.h"
#include <QApplication>
#include <QClipboard>
#include <QMessageBox>
#include <QFont>
#include <QPixmap>
#include <QCoreApplication>
#include <QSvgRenderer>
#include <QPainter>
#include <QFrame>

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
    , m_tabWidget(nullptr)
    , m_mainLayout(nullptr)
    , m_aboutTab(nullptr)
    , m_logoLabel(nullptr)
    , m_appInfoLabel(nullptr)
    , m_featuresText(nullptr)
    , m_systemInfoTab(nullptr)
    , m_systemInfoText(nullptr)
    , m_copySystemInfoButton(nullptr)
    , m_validationTab(nullptr)
    , m_validationText(nullptr)
    , m_runValidationButton(nullptr)
    , m_copyValidationButton(nullptr)
    , m_validationSummaryLabel(nullptr)
    , m_buttonLayout(nullptr)
    , m_closeButton(nullptr)
{
    setupUI();
    
    // Populate tabs with initial content
    populateSystemInfo();
    populateValidationInfo();
}

void AboutDialog::setupUI()
{
    setWindowTitle(tr("About LanScan"));
    setModal(true);
    resize(600, 500);
    
    m_mainLayout = new QVBoxLayout(this);
    
    // Create tab widget
    m_tabWidget = new QTabWidget(this);
    
    setupAboutTab();
    setupSystemInfoTab();
    setupValidationTab();
    setupButtonBox();
    
    m_mainLayout->addWidget(m_tabWidget);
    m_mainLayout->addLayout(m_buttonLayout);
}

void AboutDialog::setupAboutTab()
{
    m_aboutTab = new QWidget;
    auto* layout = new QVBoxLayout(m_aboutTab);

    // Add spacing at the top
    layout->addSpacing(20);

    // Logo/Icon area
    m_logoLabel = new QLabel;
    m_logoLabel->setAlignment(Qt::AlignCenter);
    m_logoLabel->setMinimumHeight(120);

    // Load application icon from resources using IconLoader
    QPixmap iconPixmap = IconLoader::loadPixmap("lanscan-app-icon", QSize(96, 96));

    if (!iconPixmap.isNull()) {
        m_logoLabel->setPixmap(iconPixmap);
    } else {
        // Fallback: try loading directly from resources
        QString iconPath = ":/icons/lanscan-app-icon.svg";
        QSvgRenderer renderer(iconPath);
        if (renderer.isValid()) {
            QPixmap pixmap(96, 96);
            pixmap.fill(Qt::transparent);
            QPainter painter(&pixmap);
            renderer.render(&painter);
            m_logoLabel->setPixmap(pixmap);
        } else {
            // Last fallback to text if icon can't be loaded
            m_logoLabel->setStyleSheet("QLabel { background-color: #2c3e50; color: white; font-size: 32px; font-weight: bold; padding: 20px; border-radius: 8px; }");
            m_logoLabel->setText("LanScan");
        }
    }
    layout->addWidget(m_logoLabel);

    layout->addSpacing(10);
    
    // Application information
    m_appInfoLabel = new QLabel;
    m_appInfoLabel->setAlignment(Qt::AlignCenter);
    m_appInfoLabel->setWordWrap(true);
    m_appInfoLabel->setTextFormat(Qt::RichText);

    QString appInfo = tr("<h1 style='margin-bottom: 5px;'>LanScan</h1>"
                        "<p style='font-size: 11pt; color: gray; margin-top: 0px;'>Version %1</p>"
                        "<p style='font-size: 10pt; margin-top: 15px;'><b>Cross-platform network scanning and diagnostic tool</b></p>"
                        "<p style='font-size: 9pt; color: gray; margin-top: 10px;'>Built with Qt %2 and C++17</p>")
                     .arg(LANSCAN_VERSION)
                     .arg(QT_VERSION_STR);
    m_appInfoLabel->setText(appInfo);
    m_appInfoLabel->setOpenExternalLinks(true);
    layout->addWidget(m_appInfoLabel);

    layout->addSpacing(15);
    
    // Features list
    m_featuresText = new QTextEdit;
    m_featuresText->setReadOnly(true);
    m_featuresText->setMaximumHeight(220);
    m_featuresText->setFrameShape(QFrame::NoFrame);

    QString features = tr("<h3 style='color: #2c3e50;'>Key Features</h3>"
                         "<ul style='line-height: 1.6;'>"
                         "<li>Network device discovery with IP, hostname, and MAC detection</li>"
                         "<li>Multi-threaded IP range scanning</li>"
                         "<li>Real-time latency, jitter, and packet loss monitoring</li>"
                         "<li>Advanced diagnostics: Traceroute, MTU, Bandwidth, DNS</li>"
                         "<li>Service detection on 40+ common ports</li>"
                         "<li>MAC vendor identification (38,169+ manufacturers)</li>"
                         "<li>Export to multiple formats (CSV, JSON, XML, HTML)</li>"
                         "<li>Wake-on-LAN support</li>"
                         "<li>Dark/Light theme with multi-language support (5 languages)</li>"
                         "<li>Professional charts and metrics visualization</li>"
                         "</ul>");
    m_featuresText->setHtml(features);
    layout->addWidget(m_featuresText);

    layout->addSpacing(10);
    
    // Copyright and links
    auto* copyrightLabel = new QLabel;
    copyrightLabel->setAlignment(Qt::AlignCenter);
    copyrightLabel->setTextFormat(Qt::RichText);
    copyrightLabel->setText(tr("<p style='font-size: 9pt; color: gray;'>© 2025 Paolo Sereno</p>"
                              "<p style='font-size: 8pt; color: gray;'>Licensed under MIT License</p>"));
    layout->addWidget(copyrightLabel);

    layout->addSpacing(10);
    m_tabWidget->addTab(m_aboutTab, tr("About"));
}

void AboutDialog::setupSystemInfoTab()
{
    m_systemInfoTab = new QWidget;
    auto* layout = new QVBoxLayout(m_systemInfoTab);
    
    // Info text area
    m_systemInfoText = new QTextEdit;
    m_systemInfoText->setReadOnly(true);
    m_systemInfoText->setFont(QFont("Consolas", 9)); // Monospace font for better alignment
    layout->addWidget(m_systemInfoText);
    
    // Copy button
    m_copySystemInfoButton = new QPushButton(tr("Copy to Clipboard"));
    connect(m_copySystemInfoButton, &QPushButton::clicked,
            this, &AboutDialog::copySystemInfoToClipboard);
    
    auto* buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_copySystemInfoButton);
    layout->addLayout(buttonLayout);
    
    m_tabWidget->addTab(m_systemInfoTab, tr("System Information"));
}

void AboutDialog::setupValidationTab()
{
    m_validationTab = new QWidget;
    auto* layout = new QVBoxLayout(m_validationTab);
    
    // Summary label
    m_validationSummaryLabel = new QLabel;
    m_validationSummaryLabel->setAlignment(Qt::AlignCenter);
    QFont summaryFont = m_validationSummaryLabel->font();
    summaryFont.setBold(true);
    m_validationSummaryLabel->setFont(summaryFont);
    layout->addWidget(m_validationSummaryLabel);
    
    // Validation results text area
    m_validationText = new QTextEdit;
    m_validationText->setReadOnly(true);
    m_validationText->setFont(QFont("Consolas", 9));
    layout->addWidget(m_validationText);
    
    // Buttons
    auto* buttonLayout = new QHBoxLayout;
    
    m_runValidationButton = new QPushButton(tr("Run Validation"));
    connect(m_runValidationButton, &QPushButton::clicked,
            this, &AboutDialog::runSystemValidation);
    
    m_copyValidationButton = new QPushButton(tr("Copy Report"));
    connect(m_copyValidationButton, &QPushButton::clicked,
            this, &AboutDialog::copyValidationReportToClipboard);
    
    buttonLayout->addWidget(m_runValidationButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_copyValidationButton);
    layout->addLayout(buttonLayout);
    
    m_tabWidget->addTab(m_validationTab, tr("System Validation"));
}

void AboutDialog::setupButtonBox()
{
    m_buttonLayout = new QHBoxLayout;
    
    m_closeButton = new QPushButton(tr("Close"));
    connect(m_closeButton, &QPushButton::clicked, this, &QDialog::accept);
    
    m_buttonLayout->addStretch();
    m_buttonLayout->addWidget(m_closeButton);
}

void AboutDialog::populateSystemInfo()
{
    QString systemInfoHtml = SystemInfoCollector::getSystemInfoHtml();
    m_systemInfoText->setHtml(systemInfoHtml);
}

void AboutDialog::populateValidationInfo()
{
    // Run initial validation
    runSystemValidation();
}

void AboutDialog::copySystemInfoToClipboard()
{
    QString systemInfo = SystemInfoCollector::getSystemInfoText();
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(systemInfo);
    
    QMessageBox::information(this, tr("Copied"), 
                           tr("System information copied to clipboard"));
}

void AboutDialog::copyValidationReportToClipboard()
{
    auto results = SystemValidator::runAllValidations();
    QString report = SystemValidator::getValidationReportText(results);
    
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(report);
    
    QMessageBox::information(this, tr("Copied"), 
                           tr("Validation report copied to clipboard"));
}

void AboutDialog::runSystemValidation()
{
    // Show progress (for heavy validations)
    m_runValidationButton->setEnabled(false);
    m_runValidationButton->setText(tr("Running..."));
    
    // Force UI update
    QApplication::processEvents();
    
    // Run validation
    auto results = SystemValidator::runAllValidations();
    
    // Update UI with results
    QString reportHtml = SystemValidator::getValidationReportHtml(results);
    m_validationText->setHtml(reportHtml);
    
    updateValidationSummary();
    
    // Re-enable button
    m_runValidationButton->setEnabled(true);
    m_runValidationButton->setText(tr("Run Validation"));
}

void AboutDialog::updateValidationSummary()
{
    auto results = SystemValidator::runAllValidations();
    QString summary = SystemValidator::getSummaryText(results);
    
    // Set color based on results
    QString color = "green";
    if (SystemValidator::hasCriticalErrors(results)) {
        color = "red";
        summary = "❌ " + summary;
    } else if (SystemValidator::hasErrors(results)) {
        color = "orange";
        summary = "⚠️ " + summary;
    } else if (SystemValidator::hasWarnings(results)) {
        color = "gold";
        summary = "⚠️ " + summary;
    } else {
        color = "green";
        summary = "✅ " + summary;
    }
    
    m_validationSummaryLabel->setStyleSheet(QString("QLabel { color: %1; }").arg(color));
    m_validationSummaryLabel->setText(summary);
}