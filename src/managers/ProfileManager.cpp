#include "managers/ProfileManager.h"
#include "../utils/Logger.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QUuid>

ProfileManager::ProfileManager(const QString& storageFile)
    : storageFile(storageFile)
{
    loadFromFile();
    Logger::info("ProfileManager initialized with " + QString::number(profiles.size()) + " profiles");
}

ProfileManager::~ProfileManager() {
    saveToFile();
}

bool ProfileManager::saveProfile(const ScanProfile& profile) {
    if (!profile.isValid()) {
        Logger::error("Cannot save invalid profile");
        return false;
    }

    ScanProfile profileToSave = profile;

    // Generate ID if not set
    if (profileToSave.id.isEmpty()) {
        profileToSave.id = generateProfileId();
        profileToSave.createdAt = QDateTime::currentDateTime();
    }

    profileToSave.modifiedAt = QDateTime::currentDateTime();

    profiles[profileToSave.id] = profileToSave;
    saveToFile();

    Logger::info("Profile saved: " + profileToSave.name + " (ID: " + profileToSave.id + ")");
    return true;
}

ScanProfile ProfileManager::loadProfile(const QString& id) const {
    if (profiles.contains(id)) {
        return profiles[id];
    }

    Logger::warn("Profile not found: " + id);
    return ScanProfile();
}

QList<ScanProfile> ProfileManager::getAllProfiles() const {
    return profiles.values();
}

bool ProfileManager::deleteProfile(const QString& id) {
    if (!profiles.contains(id)) {
        Logger::warn("Cannot delete profile: not found (" + id + ")");
        return false;
    }

    profiles.remove(id);
    saveToFile();

    Logger::info("Profile deleted: " + id);
    return true;
}

bool ProfileManager::updateProfile(const ScanProfile& profile) {
    if (!profile.isValid() || !profiles.contains(profile.id)) {
        Logger::error("Cannot update profile: invalid or not found");
        return false;
    }

    ScanProfile updated = profile;
    updated.modifiedAt = QDateTime::currentDateTime();

    profiles[updated.id] = updated;
    saveToFile();

    Logger::info("Profile updated: " + profile.name);
    return true;
}

bool ProfileManager::profileExists(const QString& id) const {
    return profiles.contains(id);
}

ScanProfile ProfileManager::createQuickProfile(const QString& subnet, const QString& name) {
    ScanProfile profile;
    profile.id = generateProfileId();
    profile.name = name;
    profile.description = "Quick scan: Ping and DNS only";
    profile.subnet = subnet;
    profile.resolveDns = true;
    profile.resolveArp = false;
    profile.scanPorts = false;
    profile.timeout = 1000;
    profile.createdAt = QDateTime::currentDateTime();
    profile.modifiedAt = QDateTime::currentDateTime();

    return profile;
}

ScanProfile ProfileManager::createDeepProfile(const QString& subnet, const QString& name) {
    ScanProfile profile;
    profile.id = generateProfileId();
    profile.name = name;
    profile.description = "Deep scan: Ping, DNS, ARP, and common ports";
    profile.subnet = subnet;
    profile.resolveDns = true;
    profile.resolveArp = true;
    profile.scanPorts = true;
    profile.portsToScan = {21, 22, 23, 25, 53, 80, 110, 135, 139, 143, 443, 445, 3389, 8080};
    profile.timeout = 3000;
    profile.createdAt = QDateTime::currentDateTime();
    profile.modifiedAt = QDateTime::currentDateTime();

    return profile;
}

int ProfileManager::getProfileCount() const {
    return profiles.size();
}

void ProfileManager::clearAllProfiles() {
    profiles.clear();
    saveToFile();
    Logger::info("All profiles cleared");
}

void ProfileManager::loadFromFile() {
    QFile file(storageFile);

    if (!file.exists()) {
        Logger::info("Profile storage file does not exist, will create on save");
        return;
    }

    if (!file.open(QIODevice::ReadOnly)) {
        Logger::error("Cannot open profile storage file: " + storageFile);
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        Logger::error("Invalid JSON in profile storage file");
        return;
    }

    QJsonObject root = doc.object();
    QJsonArray profilesArray = root["profiles"].toArray();

    for (const QJsonValue& value : profilesArray) {
        if (value.isObject()) {
            ScanProfile profile = profileFromJson(value.toObject());
            if (profile.isValid()) {
                profiles[profile.id] = profile;
            }
        }
    }

    Logger::info("Loaded " + QString::number(profiles.size()) + " profiles from file");
}

void ProfileManager::saveToFile() {
    QJsonArray profilesArray;

    for (const ScanProfile& profile : profiles.values()) {
        profilesArray.append(profileToJson(profile));
    }

    QJsonObject root;
    root["version"] = "1.0";
    root["profiles"] = profilesArray;

    QJsonDocument doc(root);

    QFile file(storageFile);
    if (!file.open(QIODevice::WriteOnly)) {
        Logger::error("Cannot write to profile storage file: " + storageFile);
        return;
    }

    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    Logger::debug("Saved " + QString::number(profiles.size()) + " profiles to file");
}

QJsonObject ProfileManager::profileToJson(const ScanProfile& profile) const {
    QJsonObject json;

    json["id"] = profile.id;
    json["name"] = profile.name;
    json["description"] = profile.description;
    json["subnet"] = profile.subnet;
    json["resolveDns"] = profile.resolveDns;
    json["resolveArp"] = profile.resolveArp;
    json["scanPorts"] = profile.scanPorts;
    json["timeout"] = profile.timeout;
    json["createdAt"] = profile.createdAt.toString(Qt::ISODate);
    json["modifiedAt"] = profile.modifiedAt.toString(Qt::ISODate);

    // Convert ports list to JSON array
    QJsonArray portsArray;
    for (int port : profile.portsToScan) {
        portsArray.append(port);
    }
    json["portsToScan"] = portsArray;

    return json;
}

ScanProfile ProfileManager::profileFromJson(const QJsonObject& json) const {
    ScanProfile profile;

    profile.id = json["id"].toString();
    profile.name = json["name"].toString();
    profile.description = json["description"].toString();
    profile.subnet = json["subnet"].toString();
    profile.resolveDns = json["resolveDns"].toBool(true);
    profile.resolveArp = json["resolveArp"].toBool(true);
    profile.scanPorts = json["scanPorts"].toBool(false);
    profile.timeout = json["timeout"].toInt(3000);

    // Parse timestamps
    profile.createdAt = QDateTime::fromString(json["createdAt"].toString(), Qt::ISODate);
    profile.modifiedAt = QDateTime::fromString(json["modifiedAt"].toString(), Qt::ISODate);

    // Parse ports array
    QJsonArray portsArray = json["portsToScan"].toArray();
    for (const QJsonValue& value : portsArray) {
        profile.portsToScan.append(value.toInt());
    }

    return profile;
}

QString ProfileManager::generateProfileId() const {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}
