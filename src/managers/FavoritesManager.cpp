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

    // Load groups
    QJsonObject groupsObject = root["groups"].toObject();
    for (auto it = groupsObject.constBegin(); it != groupsObject.constEnd(); ++it) {
        QList<QString> deviceIds;
        QJsonArray deviceIdsArray = it.value().toArray();
        for (const QJsonValue& val : deviceIdsArray) {
            deviceIds.append(val.toString());
        }
        deviceGroups[it.key()] = deviceIds;
    }

    // Load notes
    QJsonObject notesObject = root["notes"].toObject();
    for (auto it = notesObject.constBegin(); it != notesObject.constEnd(); ++it) {
        QList<QString> notes;
        QJsonArray notesArray = it.value().toArray();
        for (const QJsonValue& val : notesArray) {
            notes.append(val.toString());
        }
        deviceNotes[it.key()] = notes;
    }

    // Load custom icons
    QJsonObject iconsObject = root["customIcons"].toObject();
    for (auto it = iconsObject.constBegin(); it != iconsObject.constEnd(); ++it) {
        customIcons[it.key()] = it.value().toString();
    }

    Logger::info("Loaded " + QString::number(favorites.size()) + " favorites from file");
}

void FavoritesManager::saveToFile() {
    QJsonArray favoritesArray;

    for (const FavoriteDevice& favorite : favorites.values()) {
        favoritesArray.append(favoriteToJson(favorite));
    }

    // Save groups
    QJsonObject groupsObject;
    for (auto it = deviceGroups.constBegin(); it != deviceGroups.constEnd(); ++it) {
        QJsonArray deviceIds;
        for (const QString& id : it.value()) {
            deviceIds.append(id);
        }
        groupsObject[it.key()] = deviceIds;
    }

    // Save notes
    QJsonObject notesObject;
    for (auto it = deviceNotes.constBegin(); it != deviceNotes.constEnd(); ++it) {
        QJsonArray notes;
        for (const QString& note : it.value()) {
            notes.append(note);
        }
        notesObject[it.key()] = notes;
    }

    // Save custom icons
    QJsonObject iconsObject;
    for (auto it = customIcons.constBegin(); it != customIcons.constEnd(); ++it) {
        iconsObject[it.key()] = it.value();
    }

    QJsonObject root;
    root["version"] = "1.0";
    root["favorites"] = favoritesArray;
    root["groups"] = groupsObject;
    root["notes"] = notesObject;
    root["customIcons"] = iconsObject;

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

// Groups management
void FavoritesManager::createGroup(const QString& groupName) {
    if (groupName.isEmpty()) {
        Logger::warn("Cannot create group with empty name");
        return;
    }

    if (!deviceGroups.contains(groupName)) {
        deviceGroups[groupName] = QList<QString>();
        saveToFile();
        Logger::info("Created group: " + groupName);
    } else {
        Logger::warn("Group already exists: " + groupName);
    }
}

void FavoritesManager::deleteGroup(const QString& groupName) {
    if (deviceGroups.contains(groupName)) {
        deviceGroups.remove(groupName);
        saveToFile();
        Logger::info("Deleted group: " + groupName);
    } else {
        Logger::warn("Cannot delete group: not found (" + groupName + ")");
    }
}

QList<QString> FavoritesManager::getGroups() const {
    return deviceGroups.keys();
}

void FavoritesManager::addToGroup(const QString& deviceId, const QString& groupName) {
    if (!favorites.contains(deviceId)) {
        Logger::warn("Cannot add device to group: device not found (" + deviceId + ")");
        return;
    }

    if (!deviceGroups.contains(groupName)) {
        createGroup(groupName);
    }

    if (!deviceGroups[groupName].contains(deviceId)) {
        deviceGroups[groupName].append(deviceId);
        saveToFile();
        Logger::debug("Added device " + deviceId + " to group: " + groupName);
    }
}

void FavoritesManager::removeFromGroup(const QString& deviceId, const QString& groupName) {
    if (deviceGroups.contains(groupName)) {
        deviceGroups[groupName].removeAll(deviceId);
        saveToFile();
        Logger::debug("Removed device " + deviceId + " from group: " + groupName);
    }
}

QList<FavoriteDevice> FavoritesManager::getDevicesInGroup(const QString& groupName) const {
    QList<FavoriteDevice> result;

    if (deviceGroups.contains(groupName)) {
        for (const QString& deviceId : deviceGroups[groupName]) {
            if (favorites.contains(deviceId)) {
                result.append(favorites[deviceId]);
            }
        }
    }

    return result;
}

QStringList FavoritesManager::getDeviceGroups(const QString& deviceId) const {
    QStringList groups;

    for (auto it = deviceGroups.constBegin(); it != deviceGroups.constEnd(); ++it) {
        if (it.value().contains(deviceId)) {
            groups.append(it.key());
        }
    }

    return groups;
}

// Notes management
void FavoritesManager::addNote(const QString& deviceId, const QString& note) {
    if (!favorites.contains(deviceId)) {
        Logger::warn("Cannot add note: device not found (" + deviceId + ")");
        return;
    }

    if (note.isEmpty()) {
        Logger::warn("Cannot add empty note");
        return;
    }

    if (!deviceNotes.contains(deviceId)) {
        deviceNotes[deviceId] = QList<QString>();
    }

    deviceNotes[deviceId].append(note);
    saveToFile();
    Logger::debug("Added note to device: " + deviceId);
}

QList<QString> FavoritesManager::getNotes(const QString& deviceId) const {
    if (deviceNotes.contains(deviceId)) {
        return deviceNotes[deviceId];
    }
    return QList<QString>();
}

void FavoritesManager::removeNote(const QString& deviceId, int noteIndex) {
    if (deviceNotes.contains(deviceId)) {
        if (noteIndex >= 0 && noteIndex < deviceNotes[deviceId].size()) {
            deviceNotes[deviceId].removeAt(noteIndex);
            saveToFile();
            Logger::debug("Removed note from device: " + deviceId);
        } else {
            Logger::warn("Invalid note index: " + QString::number(noteIndex));
        }
    }
}

void FavoritesManager::clearNotes(const QString& deviceId) {
    if (deviceNotes.contains(deviceId)) {
        deviceNotes.remove(deviceId);
        saveToFile();
        Logger::debug("Cleared notes for device: " + deviceId);
    }
}

// Custom icons management
void FavoritesManager::setCustomIcon(const QString& deviceId, const QString& iconPath) {
    if (!favorites.contains(deviceId)) {
        Logger::warn("Cannot set custom icon: device not found (" + deviceId + ")");
        return;
    }

    if (iconPath.isEmpty()) {
        removeCustomIcon(deviceId);
        return;
    }

    customIcons[deviceId] = iconPath;
    saveToFile();
    Logger::debug("Set custom icon for device: " + deviceId);
}

QString FavoritesManager::getCustomIcon(const QString& deviceId) const {
    if (customIcons.contains(deviceId)) {
        return customIcons[deviceId];
    }
    return QString();
}

void FavoritesManager::removeCustomIcon(const QString& deviceId) {
    if (customIcons.contains(deviceId)) {
        customIcons.remove(deviceId);
        saveToFile();
        Logger::debug("Removed custom icon for device: " + deviceId);
    }
}
