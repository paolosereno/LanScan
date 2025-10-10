#ifndef PROFILEDIALOG_H
#define PROFILEDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include "managers/ProfileManager.h"

namespace Ui {
class ProfileDialog;
}

/**
 * @brief Dialog for managing scan profiles
 *
 * Provides UI for creating, editing, deleting, importing and exporting
 * scan profiles. Includes template profiles for common scenarios.
 */
class ProfileDialog : public QDialog {
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param profileManager Profile manager instance
     * @param parent Parent widget
     */
    explicit ProfileDialog(ProfileManager* profileManager, QWidget* parent = nullptr);

    /**
     * @brief Destructor
     */
    ~ProfileDialog();

    /**
     * @brief Get the selected profile
     * @return Selected profile (empty if none selected)
     */
    ScanProfile getSelectedProfile() const;

    /**
     * @brief Set the selected profile by ID
     * @param profileId Profile identifier
     */
    void setSelectedProfile(const QString& profileId);

signals:
    /**
     * @brief Emitted when a profile is selected
     * @param profile Selected profile
     */
    void profileSelected(const ScanProfile& profile);

    /**
     * @brief Emitted when a profile is created
     * @param profile Created profile
     */
    void profileCreated(const ScanProfile& profile);

    /**
     * @brief Emitted when a profile is updated
     * @param profile Updated profile
     */
    void profileUpdated(const ScanProfile& profile);

    /**
     * @brief Emitted when a profile is deleted
     * @param profileId Deleted profile ID
     */
    void profileDeleted(const QString& profileId);

private slots:
    void onNewProfile();
    void onEditProfile();
    void onDeleteProfile();
    void onImportProfile();
    void onExportProfile();
    void onProfileSelectionChanged();
    void onProfileDoubleClicked(QListWidgetItem* item);
    void onTemplateHome();
    void onTemplateEnterprise();
    void onTemplateSecurity();
    void onUseProfile();
    void onClose();

private:
    Ui::ProfileDialog* ui;
    ProfileManager* profileManager;
    ScanProfile selectedProfile;

    void setupUI();
    void setupConnections();
    void loadProfiles();
    void displayProfileDetails(const ScanProfile& profile);
    void clearProfileDetails();
    QString getProfileDisplayName(const ScanProfile& profile) const;
    QListWidgetItem* findProfileItem(const QString& profileId) const;
};

#endif // PROFILEDIALOG_H
