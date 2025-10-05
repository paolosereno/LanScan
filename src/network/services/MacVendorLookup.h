#ifndef MACVENDORLOOKUP_H
#define MACVENDORLOOKUP_H

#include <QString>
#include <QMap>
#include <QMutex>

class MacVendorLookup
{
public:
    // Singleton instance
    static MacVendorLookup* instance();

    QString lookupVendor(const QString& macAddress);
    bool loadOuiDatabase(const QString& filepath);
    void loadBuiltinDatabase();
    bool loadDefaultDatabase();

    int databaseSize() const;

private:
    MacVendorLookup();
    ~MacVendorLookup();

    // Disable copy
    MacVendorLookup(const MacVendorLookup&) = delete;
    MacVendorLookup& operator=(const MacVendorLookup&) = delete;

    static MacVendorLookup* m_instance;
    static QMutex m_mutex;

    QMap<QString, QString> m_ouiDatabase;
    QString extractOui(const QString& macAddress);
    bool isLocallyAdministered(const QString& oui);
};

#endif // MACVENDORLOOKUP_H
