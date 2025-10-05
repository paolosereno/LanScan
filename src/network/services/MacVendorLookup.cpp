#include "MacVendorLookup.h"
#include "utils/Logger.h"
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>
#include <QDir>
#include <QMutexLocker>

MacVendorLookup* MacVendorLookup::m_instance = nullptr;
QMutex MacVendorLookup::m_mutex;

MacVendorLookup::MacVendorLookup()
{
    // Don't load anything in constructor - use loadDefaultDatabase() instead
}

MacVendorLookup::~MacVendorLookup()
{
}

MacVendorLookup* MacVendorLookup::instance()
{
    if (m_instance == nullptr) {
        QMutexLocker locker(&m_mutex);
        if (m_instance == nullptr) {
            m_instance = new MacVendorLookup();
            m_instance->loadDefaultDatabase();
        }
    }
    return m_instance;
}

QString MacVendorLookup::lookupVendor(const QString& macAddress)
{
    QString oui = extractOui(macAddress);

    if (oui.isEmpty()) {
        return "Unknown";
    }

    // Check if this is a locally administered address (LAA)
    if (isLocallyAdministered(oui)) {
        return "Locally Administered";
    }

    return m_ouiDatabase.value(oui, "Unknown");
}

bool MacVendorLookup::isLocallyAdministered(const QString& oui)
{
    // Check the second bit of the first octet
    // If set, this is a locally administered address (not globally unique)
    if (oui.length() < 2) {
        return false;
    }

    bool ok;
    int firstByte = oui.left(2).toInt(&ok, 16);

    if (!ok) {
        return false;
    }

    // Bit 1 (second bit from right) indicates local/universal
    // 0 = Universally Administered (UAA), 1 = Locally Administered (LAA)
    return (firstByte & 0x02) != 0;
}

bool MacVendorLookup::loadOuiDatabase(const QString& filepath)
{
    QFile file(filepath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        Logger::error(QString("Failed to load OUI database: %1").arg(filepath));
        return false;
    }

    QTextStream in(&file);
    int count = 0;

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();

        // Skip comments and empty lines
        if (line.isEmpty() || line.startsWith('#')) {
            continue;
        }

        // Format: AA:BB:CC<TAB>Vendor Name
        QStringList parts = line.split('\t', Qt::SkipEmptyParts);

        if (parts.size() >= 2) {
            QString oui = parts[0].toUpper().remove(':').remove('-');
            QString vendor = parts[1].trimmed();

            m_ouiDatabase[oui] = vendor;
            count++;
        }
    }

    file.close();

    Logger::info(QString("Loaded %1 OUI entries from database").arg(count));
    return true;
}

void MacVendorLookup::loadBuiltinDatabase()
{
    // Common vendors (subset of IEEE OUI database)
    m_ouiDatabase["000000"] = "Xerox";
    m_ouiDatabase["000001"] = "Xerox";
    m_ouiDatabase["000D3A"] = "Microsoft";
    m_ouiDatabase["001C42"] = "Parallels";
    m_ouiDatabase["0050F2"] = "Microsoft";
    m_ouiDatabase["00155D"] = "Microsoft";
    m_ouiDatabase["001B21"] = "Intel";
    m_ouiDatabase["001E67"] = "Intel";
    m_ouiDatabase["0022FB"] = "Intel";
    m_ouiDatabase["003065"] = "Apple";
    m_ouiDatabase["0050E4"] = "Apple";
    m_ouiDatabase["001451"] = "Apple";
    m_ouiDatabase["001EC2"] = "Apple";
    m_ouiDatabase["002332"] = "Apple";
    m_ouiDatabase["D8A25E"] = "Apple";
    m_ouiDatabase["F0B479"] = "Apple";
    m_ouiDatabase["001A11"] = "Google";
    m_ouiDatabase["00241D"] = "Cisco";
    m_ouiDatabase["00D0BC"] = "Cisco";
    m_ouiDatabase["001B4F"] = "Cisco";
    m_ouiDatabase["001CFE"] = "Cisco";
    m_ouiDatabase["002618"] = "Cisco";
    m_ouiDatabase["002248"] = "Dell";
    m_ouiDatabase["0019B9"] = "Dell";
    m_ouiDatabase["001E4F"] = "Dell";
    m_ouiDatabase["00507B"] = "Dell";
    m_ouiDatabase["001B63"] = "Hewlett Packard";
    m_ouiDatabase["001CC4"] = "Hewlett Packard";
    m_ouiDatabase["002264"] = "Hewlett Packard";
    m_ouiDatabase["0024A5"] = "Hewlett Packard";
    m_ouiDatabase["001E0B"] = "ASUSTek";
    m_ouiDatabase["0026B6"] = "ASUSTek";
    m_ouiDatabase["50E549"] = "ASUSTek";
    m_ouiDatabase["00E04C"] = "Realtek";
    m_ouiDatabase["525400"] = "QEMU/KVM";
    m_ouiDatabase["020054"] = "Novell";

    Logger::debug(QString("Loaded %1 built-in OUI entries").arg(m_ouiDatabase.size()));
}

bool MacVendorLookup::loadDefaultDatabase()
{
    // Try to load external OUI database first
    QStringList searchPaths = {
        "oui_database.txt",                              // Current directory
        QCoreApplication::applicationDirPath() + "/oui_database.txt",  // App directory
        QDir::homePath() + "/.lanscan/oui_database.txt", // User home
        "data/oui_database.txt"                          // Data subdirectory
    };

    for (const QString& path : searchPaths) {
        QFile file(path);
        if (file.exists()) {
            Logger::info(QString("Found OUI database at: %1").arg(path));
            if (loadOuiDatabase(path)) {
                Logger::info(QString("Successfully loaded %1 OUI entries from external database").arg(m_ouiDatabase.size()));
                return true;
            }
        }
    }

    // Fallback to built-in database
    Logger::warn("External OUI database not found, using built-in database (limited coverage)");
    loadBuiltinDatabase();
    return false;
}

int MacVendorLookup::databaseSize() const
{
    return m_ouiDatabase.size();
}

QString MacVendorLookup::extractOui(const QString& macAddress)
{
    // Remove separators and take first 6 characters (OUI)
    QString cleaned = macAddress;
    cleaned = cleaned.remove(':').remove('-').remove('.').toUpper();

    if (cleaned.length() < 6) {
        return QString();
    }

    return cleaned.left(6);
}
