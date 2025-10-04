#ifndef PROFILEMANAGER_H
#define PROFILEMANAGER_H

#include <QString>
#include <QList>
#include <QMap>
#include <QDateTime>
#include <QJsonObject>

/**
 * @brief Scan profile data structure
 */
class ScanProfile {
public:
    QString id;              ///< Unique profile identifier
    QString name;            ///< Profile name
    QString description;     ///< Profile description
    QString subnet;          ///< Target subnet
    bool resolveDns;         ///< Enable DNS resolution
    bool resolveArp;         ///< Enable ARP resolution
    bool scanPorts;          ///< Enable port scanning
    QList<int> portsToScan;  ///< Ports to scan (empty for default)
    int timeout;             ///< Timeout in milliseconds
    QDateTime createdAt;     ///< Creation timestamp
    QDateTime modifiedAt;    ///< Last modification timestamp

    /**
     * @brief Default constructor
     */
    ScanProfile()
        : resolveDns(true)
        , resolveArp(true)
        , scanPorts(false)
        , timeout(3000)
        , createdAt(QDateTime::currentDateTime())
        , modifiedAt(QDateTime::currentDateTime())
    {}

    /**
     * @brief Check if profile is valid
     * @return True if profile has required fields
     */
    bool isValid() const {
        return !id.isEmpty() && !name.isEmpty() && !subnet.isEmpty();
    }
};

/**
 * @brief Manages scan profiles (load, save, create, delete)
 */
class ProfileManager {
public:
    /**
     * @brief Constructor
     * @param storageFile Path to storage file (JSON)
     */
    explicit ProfileManager(const QString& storageFile);

    /**
     * @brief Destructor
     */
    ~ProfileManager();

    /**
     * @brief Save a profile
     * @param profile Profile to save
     * @return True if successful
     */
    bool saveProfile(const ScanProfile& profile);

    /**
     * @brief Load a profile by ID
     * @param id Profile identifier
     * @return Loaded profile (empty if not found)
     */
    ScanProfile loadProfile(const QString& id) const;

    /**
     * @brief Get all profiles
     * @return List of all profiles
     */
    QList<ScanProfile> getAllProfiles() const;

    /**
     * @brief Delete a profile
     * @param id Profile identifier
     * @return True if successful
     */
    bool deleteProfile(const QString& id);

    /**
     * @brief Update an existing profile
     * @param profile Profile to update
     * @return True if successful
     */
    bool updateProfile(const ScanProfile& profile);

    /**
     * @brief Check if a profile exists
     * @param id Profile identifier
     * @return True if profile exists
     */
    bool profileExists(const QString& id) const;

    /**
     * @brief Create a quick scan profile
     * @param subnet Target subnet
     * @param name Profile name (optional)
     * @return Quick scan profile
     */
    ScanProfile createQuickProfile(const QString& subnet, const QString& name = "Quick Scan");

    /**
     * @brief Create a deep scan profile
     * @param subnet Target subnet
     * @param name Profile name (optional)
     * @return Deep scan profile
     */
    ScanProfile createDeepProfile(const QString& subnet, const QString& name = "Deep Scan");

    /**
     * @brief Get the number of profiles
     * @return Profile count
     */
    int getProfileCount() const;

    /**
     * @brief Clear all profiles
     */
    void clearAllProfiles();

private:
    QString storageFile;
    QMap<QString, ScanProfile> profiles;

    void loadFromFile();
    void saveToFile();
    QJsonObject profileToJson(const ScanProfile& profile) const;
    ScanProfile profileFromJson(const QJsonObject& json) const;
    QString generateProfileId() const;
};

#endif // PROFILEMANAGER_H
