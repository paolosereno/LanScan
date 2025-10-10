#include "views/ProfileDialog.h"
#include "ui_profiledialog.h"
#include "utils/Logger.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>

ProfileDialog::ProfileDialog(ProfileManager* profileManager, QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::ProfileDialog)
    , profileManager(profileManager)
{
    ui->setupUi(this);
    setupUI();
    setupConnections();
    loadProfiles();
}

ProfileDialog::~ProfileDialog() {
    delete ui;
}

ScanProfile ProfileDialog::getSelectedProfile() const {
    return selectedProfile;
}

void ProfileDialog::setSelectedProfile(const QString& profileId) {
    QListWidgetItem* item = findProfileItem(profileId);
    if (item) {
        ui->profileList->setCurrentItem(item);
    }
}

void ProfileDialog::setupUI() {
    // Set window properties
    setModal(true);
    resize(800, 600);

    // Set splitter sizes (30% left, 70% right)
    QList<int> sizes;
    sizes << 240 << 560;
    ui->splitter->setSizes(sizes);
}

void ProfileDialog::setupConnections() {
    // Profile list
    connect(ui->profileList, &QListWidget::itemSelectionChanged,
            this, &ProfileDialog::onProfileSelectionChanged);
    connect(ui->profileList, &QListWidget::itemDoubleClicked,
            this, &ProfileDialog::onProfileDoubleClicked);

    // Buttons
    connect(ui->newButton, &QPushButton::clicked,
            this, &ProfileDialog::onNewProfile);
    connect(ui->editButton, &QPushButton::clicked,
            this, &ProfileDialog::onEditProfile);
    connect(ui->deleteButton, &QPushButton::clicked,
            this, &ProfileDialog::onDeleteProfile);
    connect(ui->importButton, &QPushButton::clicked,
            this, &ProfileDialog::onImportProfile);
    connect(ui->exportButton, &QPushButton::clicked,
            this, &ProfileDialog::onExportProfile);

    // Template buttons
    connect(ui->templateHomeButton, &QPushButton::clicked,
            this, &ProfileDialog::onTemplateHome);
    connect(ui->templateEnterpriseButton, &QPushButton::clicked,
            this, &ProfileDialog::onTemplateEnterprise);
    connect(ui->templateSecurityButton, &QPushButton::clicked,
            this, &ProfileDialog::onTemplateSecurity);

    // Dialog buttons
    connect(ui->buttonBox, &QDialogButtonBox::accepted,
            this, &ProfileDialog::onUseProfile);
    connect(ui->buttonBox, &QDialogButtonBox::rejected,
            this, &ProfileDialog::onClose);
}

void ProfileDialog::loadProfiles() {
    ui->profileList->clear();

    QList<ScanProfile> profiles = profileManager->getAllProfiles();

    for (const ScanProfile& profile : profiles) {
        QString displayName = getProfileDisplayName(profile);
        QListWidgetItem* item = new QListWidgetItem(displayName, ui->profileList);
        item->setData(Qt::UserRole, profile.id);

        // Add usage statistics to tooltip
        int usageCount = profileManager->getUsageCount(profile.id);
        QDateTime lastUsed = profileManager->getLastUsed(profile.id);

        QString tooltip = profile.name + "\n" + profile.description;
        if (usageCount > 0) {
            tooltip += "\n\nUsed " + QString::number(usageCount) + " times";
            if (lastUsed.isValid()) {
                tooltip += "\nLast used: " + lastUsed.toString("yyyy-MM-dd hh:mm");
            }
        }
        item->setToolTip(tooltip);
    }

    Logger::debug("Loaded " + QString::number(profiles.size()) + " profiles into dialog");
}

void ProfileDialog::displayProfileDetails(const ScanProfile& profile) {
    QString html = "<html><body>";
    html += "<h3>" + profile.name + "</h3>";
    html += "<p><i>" + profile.description + "</i></p>";
    html += "<hr>";
    html += "<p><b>Subnet:</b> " + profile.subnet + "</p>";
    html += "<p><b>Timeout:</b> " + QString::number(profile.timeout) + " ms</p>";
    html += "<p><b>DNS Resolution:</b> " + QString(profile.resolveDns ? "Enabled" : "Disabled") + "</p>";
    html += "<p><b>ARP Resolution:</b> " + QString(profile.resolveArp ? "Enabled" : "Disabled") + "</p>";
    html += "<p><b>Port Scanning:</b> " + QString(profile.scanPorts ? "Enabled" : "Disabled") + "</p>";

    if (profile.scanPorts && !profile.portsToScan.isEmpty()) {
        html += "<p><b>Ports to Scan:</b> ";
        QStringList portStrings;
        for (int port : profile.portsToScan) {
            portStrings.append(QString::number(port));
        }
        html += portStrings.join(", ") + "</p>";
    }

    html += "<hr>";
    html += "<p><small><b>Created:</b> " + profile.createdAt.toString("yyyy-MM-dd hh:mm") + "</small></p>";
    html += "<p><small><b>Modified:</b> " + profile.modifiedAt.toString("yyyy-MM-dd hh:mm") + "</small></p>";

    // Usage statistics
    int usageCount = profileManager->getUsageCount(profile.id);
    QDateTime lastUsed = profileManager->getLastUsed(profile.id);

    if (usageCount > 0) {
        html += "<p><small><b>Usage Count:</b> " + QString::number(usageCount) + "</small></p>";
        if (lastUsed.isValid()) {
            html += "<p><small><b>Last Used:</b> " + lastUsed.toString("yyyy-MM-dd hh:mm") + "</small></p>";
        }
    }

    html += "</body></html>";

    ui->detailsText->setHtml(html);
}

void ProfileDialog::clearProfileDetails() {
    ui->detailsText->setHtml("<p style='color: #888888; font-style: italic;'>Select a profile to view details</p>");
}

QString ProfileDialog::getProfileDisplayName(const ScanProfile& profile) const {
    QString name = profile.name;

    // Add indicator for frequently used profiles
    int usageCount = profileManager->getUsageCount(profile.id);
    if (usageCount > 10) {
        name += " ⭐";
    }

    return name;
}

QListWidgetItem* ProfileDialog::findProfileItem(const QString& profileId) const {
    for (int i = 0; i < ui->profileList->count(); ++i) {
        QListWidgetItem* item = ui->profileList->item(i);
        if (item->data(Qt::UserRole).toString() == profileId) {
            return item;
        }
    }
    return nullptr;
}

void ProfileDialog::onNewProfile() {
    bool ok;
    QString name = QInputDialog::getText(this, tr("New Profile"),
                                        tr("Profile name:"),
                                        QLineEdit::Normal,
                                        "", &ok);

    if (!ok || name.isEmpty()) {
        return;
    }

    // Create a basic profile
    ScanProfile profile = profileManager->createQuickProfile("192.168.1.0/24", name);

    if (profileManager->saveProfile(profile)) {
        loadProfiles();
        setSelectedProfile(profile.id);
        emit profileCreated(profile);
        Logger::info("Created new profile: " + name);
    } else {
        QMessageBox::warning(this, tr("Error"),
                           tr("Failed to create profile"));
    }
}

void ProfileDialog::onEditProfile() {
    if (!selectedProfile.isValid()) {
        return;
    }

    // TODO: Open profile editor dialog
    QMessageBox::information(this, tr("Edit Profile"),
                           tr("Profile editor not yet implemented.\n"
                              "You can manually edit the profile file or use import/export."));
}

void ProfileDialog::onDeleteProfile() {
    if (!selectedProfile.isValid()) {
        return;
    }

    int result = QMessageBox::question(this, tr("Delete Profile"),
                                      tr("Are you sure you want to delete the profile '%1'?")
                                      .arg(selectedProfile.name),
                                      QMessageBox::Yes | QMessageBox::No,
                                      QMessageBox::No);

    if (result == QMessageBox::Yes) {
        QString deletedId = selectedProfile.id;
        if (profileManager->deleteProfile(selectedProfile.id)) {
            selectedProfile = ScanProfile();
            loadProfiles();
            clearProfileDetails();
            emit profileDeleted(deletedId);
            Logger::info("Deleted profile: " + deletedId);
        } else {
            QMessageBox::warning(this, tr("Error"),
                               tr("Failed to delete profile"));
        }
    }
}

void ProfileDialog::onImportProfile() {
    QString fileName = QFileDialog::getOpenFileName(this,
                                                   tr("Import Profile"),
                                                   QString(),
                                                   tr("Profile Files (*.json)"));

    if (fileName.isEmpty()) {
        return;
    }

    ScanProfile profile = profileManager->importProfile(fileName);

    if (profile.isValid()) {
        // Ask user if they want to save the imported profile
        int result = QMessageBox::question(this, tr("Import Profile"),
                                          tr("Import profile '%1'?").arg(profile.name),
                                          QMessageBox::Yes | QMessageBox::No,
                                          QMessageBox::Yes);

        if (result == QMessageBox::Yes) {
            if (profileManager->saveProfile(profile)) {
                loadProfiles();
                setSelectedProfile(profile.id);
                emit profileCreated(profile);
                QMessageBox::information(this, tr("Success"),
                                       tr("Profile imported successfully"));
            } else {
                QMessageBox::warning(this, tr("Error"),
                                   tr("Failed to save imported profile"));
            }
        }
    } else {
        QMessageBox::warning(this, tr("Error"),
                           tr("Failed to import profile from file"));
    }
}

void ProfileDialog::onExportProfile() {
    if (!selectedProfile.isValid()) {
        return;
    }

    QString defaultName = selectedProfile.name.toLower().replace(" ", "_") + ".json";
    QString fileName = QFileDialog::getSaveFileName(this,
                                                   tr("Export Profile"),
                                                   defaultName,
                                                   tr("Profile Files (*.json)"));

    if (fileName.isEmpty()) {
        return;
    }

    if (profileManager->exportProfile(selectedProfile, fileName)) {
        QMessageBox::information(this, tr("Success"),
                               tr("Profile exported successfully"));
    } else {
        QMessageBox::warning(this, tr("Error"),
                           tr("Failed to export profile"));
    }
}

void ProfileDialog::onProfileSelectionChanged() {
    QListWidgetItem* item = ui->profileList->currentItem();

    if (item) {
        QString profileId = item->data(Qt::UserRole).toString();
        selectedProfile = profileManager->loadProfile(profileId);

        if (selectedProfile.isValid()) {
            displayProfileDetails(selectedProfile);
            ui->editButton->setEnabled(true);
            ui->deleteButton->setEnabled(true);
            ui->exportButton->setEnabled(true);
        } else {
            clearProfileDetails();
            ui->editButton->setEnabled(false);
            ui->deleteButton->setEnabled(false);
            ui->exportButton->setEnabled(false);
        }
    } else {
        selectedProfile = ScanProfile();
        clearProfileDetails();
        ui->editButton->setEnabled(false);
        ui->deleteButton->setEnabled(false);
        ui->exportButton->setEnabled(false);
    }
}

void ProfileDialog::onProfileDoubleClicked(QListWidgetItem* item) {
    if (item && selectedProfile.isValid()) {
        onUseProfile();
    }
}

void ProfileDialog::onTemplateHome() {
    ScanProfile profile = ProfileManager::createHomeNetworkProfile();

    if (profileManager->saveProfile(profile)) {
        loadProfiles();
        setSelectedProfile(profile.id);
        emit profileCreated(profile);
        QMessageBox::information(this, tr("Template Created"),
                               tr("Home Network profile template created"));
    } else {
        QMessageBox::warning(this, tr("Error"),
                           tr("Failed to create template profile"));
    }
}

void ProfileDialog::onTemplateEnterprise() {
    ScanProfile profile = ProfileManager::createEnterpriseNetworkProfile();

    if (profileManager->saveProfile(profile)) {
        loadProfiles();
        setSelectedProfile(profile.id);
        emit profileCreated(profile);
        QMessageBox::information(this, tr("Template Created"),
                               tr("Enterprise Network profile template created"));
    } else {
        QMessageBox::warning(this, tr("Error"),
                           tr("Failed to create template profile"));
    }
}

void ProfileDialog::onTemplateSecurity() {
    ScanProfile profile = ProfileManager::createSecurityAuditProfile();

    if (profileManager->saveProfile(profile)) {
        loadProfiles();
        setSelectedProfile(profile.id);
        emit profileCreated(profile);
        QMessageBox::information(this, tr("Template Created"),
                               tr("Security Audit profile template created"));
    } else {
        QMessageBox::warning(this, tr("Error"),
                           tr("Failed to create template profile"));
    }
}

void ProfileDialog::onUseProfile() {
    if (selectedProfile.isValid()) {
        emit profileSelected(selectedProfile);
        accept();
    } else {
        QMessageBox::warning(this, tr("No Profile Selected"),
                           tr("Please select a profile first"));
    }
}

void ProfileDialog::onClose() {
    reject();
}
