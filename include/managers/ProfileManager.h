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

    // Export/Import profiles
    /**
     * @brief Export a profile to a file
     * @param profile Profile to export
     * @param filepath Path to export file
     * @return True if successful
     */
    bool exportProfile(const ScanProfile& profile, const QString& filepath);

    /**
     * @brief Import a profile from a file
     * @param filepath Path to import file
     * @return Imported profile (empty if failed)
     */
    ScanProfile importProfile(const QString& filepath);

    // Profile templates
    /**
     * @brief Create a home network profile template
     * @return Home network profile
     */
    static ScanProfile createHomeNetworkProfile();

    /**
     * @brief Create an enterprise network profile template
     * @return Enterprise network profile
     */
    static ScanProfile createEnterpriseNetworkProfile();

    /**
     * @brief Create a security audit profile template
     * @return Security audit profile
     */
    static ScanProfile createSecurityAuditProfile();

    // Profile statistics
    /**
     * @brief Get the last time a profile was used
     * @param profileId Profile identifier
     * @return Last used timestamp (null if never used)
     */
    QDateTime getLastUsed(const QString& profileId) const;

    /**
     * @brief Get the number of times a profile was used
     * @param profileId Profile identifier
     * @return Usage count
     */
    int getUsageCount(const QString& profileId) const;

    /**
     * @brief Update usage statistics for a profile
     * @param profileId Profile identifier
     */
    void updateUsageStats(const QString& profileId);

private:
    QString storageFile;
    QMap<QString, ScanProfile> profiles;
    QMap<QString, int> usageCounts;
    QMap<QString, QDateTime> lastUsedTimes;

    void loadFromFile();
    void saveToFile();
    QJsonObject profileToJson(const ScanProfile& profile) const;
    ScanProfile profileFromJson(const QJsonObject& json) const;
    QString generateProfileId() const;
};

#endif // PROFILEMANAGER_H
