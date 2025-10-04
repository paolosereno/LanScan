#include "managers/FavoritesManager.h"
#include "../utils/Logger.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QUuid>

FavoritesManager::FavoritesManager(const QString& storageFile)
    : storageFile(storageFile)
{
    loadFromFile();
    Logger::info("FavoritesManager initialized with " + QString::number(favorites.size()) + " favorites");
}

FavoritesManager::~FavoritesManager() {
    saveToFile();
}

bool FavoritesManager::addFavorite(const FavoriteDevice& favorite) {
    if (!favorite.isValid()) {
        Logger::error("Cannot add invalid favorite device");
        return false;
    }

    FavoriteDevice deviceToAdd = favorite;

    // Generate ID if not set
    if (deviceToAdd.id.isEmpty()) {
        deviceToAdd.id = generateFavoriteId();
    }

    // Check if IP already exists
    if (isFavorite(deviceToAdd.ip) && !isFavoriteById(deviceToAdd.id)) {
        Logger::warn("Device with IP " + deviceToAdd.ip + " already in favorites");
        return false;
    }

    deviceToAdd.addedAt = QDateTime::currentDateTime();
    favorites[deviceToAdd.id] = deviceToAdd;
    saveToFile();

    Logger::info("Favorite added: " + deviceToAdd.name + " (" + deviceToAdd.ip + ")");
    return true;
}

bool FavoritesManager::removeFavorite(const QString& id) {
    if (!favorites.contains(id)) {
        Logger::warn("Cannot remove favorite: not found (" + id + ")");
        return false;
    }

    QString ip = favorites[id].ip;
    favorites.remove(id);
    saveToFile();

    Logger::info("Favorite removed: " + ip);
    return true;
}

bool FavoritesManager::removeFavoriteByIp(const QString& ip) {
    for (auto it = favorites.begin(); it != favorites.end(); ++it) {
        if (it.value().ip == ip) {
            QString id = it.key();
            favorites.erase(it);
            saveToFile();
            Logger::info("Favorite removed by IP: " + ip);
            return true;
        }
    }

    Logger::warn("Favorite not found with IP: " + ip);
    return false;
}

bool FavoritesManager::updateFavorite(const FavoriteDevice& favorite) {
    if (!favorite.isValid() || !favorites.contains(favorite.id)) {
        Logger::error("Cannot update favorite: invalid or not found");
        return false;
    }

    favorites[favorite.id] = favorite;
    saveToFile();

    Logger::info("Favorite updated: " + favorite.name);
    return true;
}

FavoriteDevice FavoritesManager::getFavorite(const QString& id) const {
    if (favorites.contains(id)) {
        return favorites[id];
    }

    Logger::warn("Favorite not found: " + id);
    return FavoriteDevice();
}

FavoriteDevice FavoritesManager::getFavoriteByIp(const QString& ip) const {
    for (const FavoriteDevice& fav : favorites.values()) {
        if (fav.ip == ip) {
            return fav;
        }
    }

    Logger::debug("Favorite not found with IP: " + ip);
    return FavoriteDevice();
}

QList<FavoriteDevice> FavoritesManager::getAllFavorites() const {
    return favorites.values();
}

QList<FavoriteDevice> FavoritesManager::getFavoritesByTag(const QString& tag) const {
    QList<FavoriteDevice> result;

    for (const FavoriteDevice& fav : favorites.values()) {
        if (fav.hasTag(tag)) {
            result.append(fav);
        }
    }

    return result;
}

bool FavoritesManager::isFavorite(const QString& ip) const {
    for (const FavoriteDevice& fav : favorites.values()) {
        if (fav.ip == ip) {
            return true;
        }
    }
    return false;
}

bool FavoritesManager::isFavoriteById(const QString& id) const {
    return favorites.contains(id);
}

QStringList FavoritesManager::getAllTags() const {
    QStringList allTags;

    for (const FavoriteDevice& fav : favorites.values()) {
        for (const QString& tag : fav.tags) {
            if (!allTags.contains(tag, Qt::CaseInsensitive)) {
                allTags.append(tag);
            }
        }
    }

    allTags.sort(Qt::CaseInsensitive);
    return allTags;
}

int FavoritesManager::getFavoriteCount() const {
    return favorites.size();
}

void FavoritesManager::clearAllFavorites() {
    favorites.clear();
    saveToFile();
    Logger::info("All favorites cleared");
}

void FavoritesManager::loadFromFile() {
    QFile file(storageFile);

    if (!file.exists()) {
        Logger::info("Favorites storage file does not exist, will create on save");
        return;
    }

    if (!file.open(QIODevice::ReadOnly)) {
        Logger::error("Cannot open favorites storage file: " + storageFile);
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        Logger::error("Invalid JSON in favorites storage file");
        return;
    }

    QJsonObject root = doc.object();
    QJsonArray favoritesArray = root["favorites"].toArray();

    for (const QJsonValue& value : favoritesArray) {
        if (value.isObject()) {
            FavoriteDevice favorite = favoriteFromJson(value.toObject());
            if (favorite.isValid()) {
                favorites[favorite.id] = favorite;
            }
        }
    }

    Logger::info("Loaded " + QString::number(favorites.size()) + " favorites from file");
}

void FavoritesManager::saveToFile() {
    QJsonArray favoritesArray;

    for (const FavoriteDevice& favorite : favorites.values()) {
        favoritesArray.append(favoriteToJson(favorite));
    }

    QJsonObject root;
    root["version"] = "1.0";
    root["favorites"] = favoritesArray;

    QJsonDocument doc(root);

    QFile file(storageFile);
    if (!file.open(QIODevice::WriteOnly)) {
        Logger::error("Cannot write to favorites storage file: " + storageFile);
        return;
    }

    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    Logger::debug("Saved " + QString::number(favorites.size()) + " favorites to file");
}

QJsonObject FavoritesManager::favoriteToJson(const FavoriteDevice& favorite) const {
    QJsonObject json;

    json["id"] = favorite.id;
    json["ip"] = favorite.ip;
    json["name"] = favorite.name;
    json["description"] = favorite.description;
    json["wakeOnLanEnabled"] = favorite.wakeOnLanEnabled;
    json["macAddress"] = favorite.macAddress;
    json["addedAt"] = favorite.addedAt.toString(Qt::ISODate);
    json["lastSeenAt"] = favorite.lastSeenAt.toString(Qt::ISODate);

    // Convert tags list to JSON array
    QJsonArray tagsArray;
    for (const QString& tag : favorite.tags) {
        tagsArray.append(tag);
    }
    json["tags"] = tagsArray;

    return json;
}

FavoriteDevice FavoritesManager::favoriteFromJson(const QJsonObject& json) const {
    FavoriteDevice favorite;

    favorite.id = json["id"].toString();
    favorite.ip = json["ip"].toString();
    favorite.name = json["name"].toString();
    favorite.description = json["description"].toString();
    favorite.wakeOnLanEnabled = json["wakeOnLanEnabled"].toBool(false);
    favorite.macAddress = json["macAddress"].toString();

    // Parse timestamps
    favorite.addedAt = QDateTime::fromString(json["addedAt"].toString(), Qt::ISODate);
    favorite.lastSeenAt = QDateTime::fromString(json["lastSeenAt"].toString(), Qt::ISODate);

    // Parse tags array
    QJsonArray tagsArray = json["tags"].toArray();
    for (const QJsonValue& value : tagsArray) {
        favorite.tags.append(value.toString());
    }

    return favorite;
}

QString FavoritesManager::generateFavoriteId() const {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}
