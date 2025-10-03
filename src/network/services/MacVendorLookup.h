#ifndef MACVENDORLOOKUP_H
#define MACVENDORLOOKUP_H

#include <QString>
#include <QMap>

class MacVendorLookup
{
public:
    MacVendorLookup();

    QString lookupVendor(const QString& macAddress);
    bool loadOuiDatabase(const QString& filepath);
    void loadBuiltinDatabase();

    int databaseSize() const;

private:
    QMap<QString, QString> m_ouiDatabase;

    QString extractOui(const QString& macAddress);
};

#endif // MACVENDORLOOKUP_H
