#ifndef FAVORITESMANAGER_H
#define FAVORITESMANAGER_H

#include <QString>
#include <QStringList>
#include <QList>
#include <QMap>
#include <QDateTime>
#include <QJsonObject>

/**
 * @brief Favorite device data structure
 */
class FavoriteDevice {
public:
    QString id;              ///< Unique device identifier
    QString ip;              ///< IP address
    QString name;            ///< User-defined name
    QString description;     ///< User notes/description
    QStringList tags;        ///< User tags (e.g., "router", "server")
    bool wakeOnLanEnabled;   ///< Wake-on-LAN enabled
    QString macAddress;      ///< MAC address (for WoL)
    QDateTime addedAt;       ///< When device was added to favorites
    QDateTime lastSeenAt;    ///< Last time device was seen online

    /**
     * @brief Default constructor
     */
    FavoriteDevice()
        : wakeOnLanEnabled(false)
        , addedAt(QDateTime::currentDateTime())
        , lastSeenAt(QDateTime::currentDateTime())
    {}

    /**
     * @brief Check if device is valid
     * @return True if device has required fields
     */
    bool isValid() const {
        return !id.isEmpty() && !ip.isEmpty();
    }

    /**
     * @brief Check if device has a specific tag
     * @param tag Tag to check
     * @return True if tag exists
     */
    bool hasTag(const QString& tag) const {
        return tags.contains(tag, Qt::CaseInsensitive);
    }

    /**
     * @brief Add a tag to the device
     * @param tag Tag to add
     */
    void addTag(const QString& tag) {
        if (!hasTag(tag)) {
            tags.append(tag);
        }
    }

    /**
     * @brief Remove a tag from the device
     * @param tag Tag to remove
     */
    void removeTag(const QString& tag) {
        tags.removeAll(tag);
    }
};

/**
 * @brief Manages favorite devices (add, remove, update, search)
 */
class FavoritesManager {
public:
    /**
     * @brief Constructor
     * @param storageFile Path to storage file (JSON)
     */
    explicit FavoritesManager(const QString& storageFile);

    /**
     * @brief Destructor
     */
    ~FavoritesManager();

    /**
     * @brief Add a device to favorites
     * @param favorite Favorite device to add
     * @return True if successful
     */
    bool addFavorite(const FavoriteDevice& favorite);

    /**
     * @brief Remove a device from favorites
     * @param id Device identifier
     * @return True if successful
     */
    bool removeFavorite(const QString& id);

    /**
     * @brief Remove a device from favorites by IP
     * @param ip IP address
     * @return True if successful
     */
    bool removeFavoriteByIp(const QString& ip);

    /**
     * @brief Update a favorite device
     * @param favorite Updated favorite device
     * @return True if successful
     */
    bool updateFavorite(const FavoriteDevice& favorite);

    /**
     * @brief Get a favorite device by ID
     * @param id Device identifier
     * @return Favorite device (empty if not found)
     */
    FavoriteDevice getFavorite(const QString& id) const;

    /**
     * @brief Get a favorite device by IP address
     * @param ip IP address
     * @return Favorite device (empty if not found)
     */
    FavoriteDevice getFavoriteByIp(const QString& ip) const;

    /**
     * @brief Get all favorite devices
     * @return List of all favorites
     */
    QList<FavoriteDevice> getAllFavorites() const;

    /**
     * @brief Get favorites by tag
     * @param tag Tag to filter by
     * @return List of favorites with the specified tag
     */
    QList<FavoriteDevice> getFavoritesByTag(const QString& tag) const;

    /**
     * @brief Check if an IP is in favorites
     * @param ip IP address
     * @return True if IP is favorited
     */
    bool isFavorite(const QString& ip) const;

    /**
     * @brief Check if a device ID is in favorites
     * @param id Device identifier
     * @return True if ID is favorited
     */
    bool isFavoriteById(const QString& id) const;

    /**
     * @brief Get all unique tags
     * @return List of all tags used
     */
    QStringList getAllTags() const;

    /**
     * @brief Get the number of favorites
     * @return Favorite count
     */
    int getFavoriteCount() const;

    /**
     * @brief Clear all favorites
     */
    void clearAllFavorites();

private:
    QString storageFile;
    QMap<QString, FavoriteDevice> favorites;

    void loadFromFile();
    void saveToFile();
    QJsonObject favoriteToJson(const FavoriteDevice& favorite) const;
    FavoriteDevice favoriteFromJson(const QJsonObject& json) const;
    QString generateFavoriteId() const;
};

#endif // FAVORITESMANAGER_H
