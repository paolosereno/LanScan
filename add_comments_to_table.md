# Aggiunta Colonna Commenti alla Tabella Dispositivi

## Obiettivo
Aggiungere una colonna "Comments" alla destra della tabella dei dispositivi in MainWindow per permettere di inserire commenti personalizzati per ogni device di rete scoperto durante la scansione.

## Prerequisiti
- Il database SQLite verrà ricreato (eliminare il file lanscan.db esistente)
- Non è necessaria migrazione dello schema

---

## Checklist Implementazione (Work in Progress)

### Fase 1: Modello Device
- [ ] Modificare `src/models/Device.h`
  - [ ] Aggiungere campo privato `QString m_comments`
  - [ ] Aggiungere getter `QString getComments() const`
  - [ ] Aggiungere getter alias `QString comments() const`
  - [ ] Aggiungere setter `void setComments(const QString& comments)`
- [ ] Modificare `src/models/Device.cpp`
  - [ ] Inizializzare `m_comments("")` nel costruttore di default
  - [ ] Inizializzare `m_comments("")` nel costruttore con parametri
  - [ ] Implementare `getComments()`
  - [ ] Implementare `comments()`
  - [ ] Implementare `setComments()`
- [ ] Compilare e verificare che non ci siano errori

### Fase 2: Schema Database
- [ ] Modificare `src/database/DatabaseManager.cpp`
  - [ ] Aggiungere colonna `comments TEXT` nella `createDevicesTable()`
- [ ] Eliminare il database esistente `lanscan.db`
- [ ] Compilare e verificare che il database venga ricreato correttamente

### Fase 3: DeviceRepository - Persistenza
- [ ] Modificare `src/database/DeviceRepository.cpp`
  - [ ] Aggiungere `device.setComments(query.value("comments").toString())` in `mapFromQuery()`
  - [ ] Aggiungere `comments` nella query INSERT di `saveToDatabase()`
  - [ ] Aggiungere binding `:comments` con `device.getComments()` in `saveToDatabase()`
  - [ ] Aggiungere merge logic per comments in `updateInDatabase()`
  - [ ] Aggiungere `comments` nella query UPDATE di `updateInDatabase()`
  - [ ] Aggiungere binding `:comments` in `updateInDatabase()`
- [ ] Compilare e testare il salvataggio/lettura dal database

### Fase 4: DeviceTableViewModel - UI Editabile
- [ ] Modificare `include/viewmodels/DeviceTableViewModel.h`
  - [ ] Aggiungere `Comments` nell'enum Column (prima di ColumnCount)
  - [ ] Aggiungere dichiarazione `bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override`
  - [ ] Aggiungere dichiarazione `Qt::ItemFlags flags(const QModelIndex& index) const override`
- [ ] Modificare `src/viewmodels/DeviceTableViewModel.cpp`
  - [ ] Aggiungere case `Comments:` in `data()` per DisplayRole
  - [ ] Aggiungere case `Comments:` in `headerData()` con return `tr("Comments")`
  - [ ] Implementare metodo `setData()` completo
  - [ ] Implementare metodo `flags()` completo
- [ ] Compilare e testare l'editing della colonna Comments nell'UI

### Fase 5: Esportatori (OPZIONALE)
- [ ] Modificare `src/export/CsvExporter.cpp`
  - [ ] Aggiungere "Comments" nell'header CSV
  - [ ] Aggiungere `escapeField(device.getComments())` nelle righe
- [ ] Modificare `src/export/JsonExporter.cpp`
  - [ ] Aggiungere `deviceObj["comments"] = device.getComments()`
- [ ] Modificare `src/export/XmlExporter.cpp`
  - [ ] Aggiungere elemento `<comments>` con `device.getComments()`
- [ ] Modificare `src/export/HtmlReportGenerator.cpp`
  - [ ] Aggiungere `<th>Comments</th>` nell'header
  - [ ] Aggiungere `<td>` con `device.getComments()` nelle righe
- [ ] Testare tutti i formati di esportazione

### Test Finali
- [ ] Eliminare lanscan.db
- [ ] Ricompilare tutto il progetto (`cmake --build . -j12`)
- [ ] Eseguire `ctest --output-on-failure`
- [ ] Test manuale:
  - [ ] Avviare LanScan
  - [ ] Eseguire scansione di rete
  - [ ] Fare doppio click su una cella della colonna Comments
  - [ ] Inserire un commento e premere Invio
  - [ ] Riavviare l'applicazione e verificare persistenza
  - [ ] Testare esportazione in CSV/JSON/XML/HTML

---

## Modifiche da Implementare

### 1. Modello Device

#### File: `src/models/Device.h`

**Aggiungere nella sezione private:**
```cpp
private:
    QString m_id;
    QString m_ip;
    QString m_hostname;
    QString m_macAddress;
    QString m_vendor;
    bool m_isOnline;
    QDateTime m_lastSeen;
    QList<PortInfo> m_openPorts;
    NetworkMetrics m_metrics;
    QString m_comments;  // NUOVA RIGA
```

**Aggiungere nella sezione public getters (dopo getMetrics):**
```cpp
    QString getComments() const;
    QString comments() const;  // Alias per compatibilità
```

**Aggiungere nella sezione public setters (dopo setMetrics):**
```cpp
    void setComments(const QString& comments);
```

#### File: `src/models/Device.cpp`

**Aggiungere nel costruttore di default:**
```cpp
Device::Device()
    : m_isOnline(false)
    , m_comments("")  // NUOVA RIGA
{
}
```

**Aggiungere nel costruttore con parametri:**
```cpp
Device::Device(const QString& ip, const QString& hostname)
    : m_ip(ip)
    , m_hostname(hostname)
    , m_isOnline(false)
    , m_comments("")  // NUOVA RIGA
{
    m_id = QUuid::createUuid().toString();
}
```

**Aggiungere le implementazioni dei metodi getter/setter alla fine del file:**
```cpp
QString Device::getComments() const {
    return m_comments;
}

QString Device::comments() const {
    return m_comments;
}

void Device::setComments(const QString& comments) {
    m_comments = comments;
}
```

---

### 2. Schema Database

#### File: `src/database/DatabaseManager.cpp`

**Modificare il metodo `createDevicesTable()` alla riga ~135:**

Sostituire la query SQL esistente con:
```cpp
bool DatabaseManager::createDevicesTable() {
    QString query = R"(
        CREATE TABLE IF NOT EXISTS devices (
            id TEXT PRIMARY KEY,
            ip TEXT NOT NULL UNIQUE,
            hostname TEXT,
            mac_address TEXT,
            vendor TEXT,
            is_online INTEGER,
            last_seen DATETIME,
            comments TEXT,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            updated_at DATETIME DEFAULT CURRENT_TIMESTAMP
        )
    )";

    return executeQuery(query);
}
```

---

### 3. DeviceRepository - Lettura/Scrittura Database

#### File: `src/database/DeviceRepository.cpp`

**Modificare il metodo `mapFromQuery()` alla riga ~196:**

Aggiungere dopo `device.setLastSeen(...)`:
```cpp
Device DeviceRepository::mapFromQuery(const QSqlQuery& query) {
    Device device;
    device.setId(query.value("id").toString());
    device.setIp(query.value("ip").toString());
    device.setHostname(query.value("hostname").toString());
    device.setMacAddress(query.value("mac_address").toString());
    device.setVendor(query.value("vendor").toString());
    device.setOnline(query.value("is_online").toBool());
    device.setLastSeen(query.value("last_seen").toDateTime());
    device.setComments(query.value("comments").toString());  // NUOVA RIGA

    // Load ports for this device...
    // (resto del codice invariato)
```

**Modificare il metodo `saveToDatabase()` alla riga ~242:**

Sostituire la query INSERT con:
```cpp
void DeviceRepository::saveToDatabase(const Device& device) {
    QString query = R"(
        INSERT INTO devices (id, ip, hostname, mac_address, vendor, is_online, last_seen, comments)
        VALUES (:id, :ip, :hostname, :mac, :vendor, :online, :last_seen, :comments)
    )";

    QSqlQuery sqlQuery = db->prepareQuery(query);
    sqlQuery.bindValue(":id", device.getId());
    sqlQuery.bindValue(":ip", device.getIp());
    sqlQuery.bindValue(":hostname", device.getHostname());
    sqlQuery.bindValue(":mac", device.getMacAddress());
    sqlQuery.bindValue(":vendor", device.getVendor());
    sqlQuery.bindValue(":online", device.isOnline() ? 1 : 0);
    sqlQuery.bindValue(":last_seen", device.getLastSeen());
    sqlQuery.bindValue(":comments", device.getComments());  // NUOVA RIGA

    if (!sqlQuery.exec()) {
        Logger::error("DeviceRepository: Failed to save device: " + sqlQuery.lastError().text());
        return;
    }

    // Save ports...
    // (resto del codice invariato)
}
```

**Modificare il metodo `updateInDatabase()` alla riga ~268:**

Aggiungere il merge per comments e modificare la query UPDATE:
```cpp
void DeviceRepository::updateInDatabase(const Device& device) {
    // Get existing device to merge data
    Device existing = findById(device.getId());

    // Merge: keep existing hostname if new device has empty hostname
    QString hostname = device.getHostname();
    if (hostname.isEmpty() && !existing.getHostname().isEmpty()) {
        hostname = existing.getHostname();
        Logger::debug(QString("DeviceRepository: Preserving existing hostname '%1' for %2")
                     .arg(existing.getHostname()).arg(device.getIp()));
    }

    // Merge: keep existing MAC/vendor if new device doesn't have them
    QString mac = device.getMacAddress();
    if (mac.isEmpty() && !existing.getMacAddress().isEmpty()) {
        mac = existing.getMacAddress();
    }

    QString vendor = device.getVendor();
    if (vendor.isEmpty() && !existing.getVendor().isEmpty()) {
        vendor = existing.getVendor();
    }

    // Merge: keep existing comments if new device doesn't have them
    QString comments = device.getComments();
    if (comments.isEmpty() && !existing.getComments().isEmpty()) {
        comments = existing.getComments();
    }

    QString query = R"(
        UPDATE devices
        SET ip = :ip, hostname = :hostname, mac_address = :mac,
            vendor = :vendor, is_online = :online, last_seen = :last_seen,
            comments = :comments,
            updated_at = CURRENT_TIMESTAMP
        WHERE id = :id
    )";

    QSqlQuery sqlQuery = db->prepareQuery(query);
    sqlQuery.bindValue(":id", device.getId());
    sqlQuery.bindValue(":ip", device.getIp());
    sqlQuery.bindValue(":hostname", hostname);
    sqlQuery.bindValue(":mac", mac);
    sqlQuery.bindValue(":vendor", vendor);
    sqlQuery.bindValue(":online", device.isOnline() ? 1 : 0);
    sqlQuery.bindValue(":last_seen", device.getLastSeen());
    sqlQuery.bindValue(":comments", comments);  // NUOVA RIGA

    if (!sqlQuery.exec()) {
        Logger::error("DeviceRepository: Failed to update device: " + sqlQuery.lastError().text());
        return;
    }

    // (resto del codice invariato)
}
```

---

### 4. DeviceTableViewModel - Aggiunta Colonna

#### File: `include/viewmodels/DeviceTableViewModel.h`

**Modificare l'enum Column alla riga ~21:**
```cpp
enum Column {
    Status = 0,
    IpAddress,
    Hostname,
    MacAddress,
    Vendor,
    OpenPorts,
    Latency,
    QualityScore,
    Comments,        // NUOVA COLONNA
    ColumnCount
};
```

**Aggiungere dopo il metodo `headerData()` alla riga ~39:**
```cpp
QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;  // NUOVO
Qt::ItemFlags flags(const QModelIndex& index) const override;  // NUOVO
```

#### File: `src/viewmodels/DeviceTableViewModel.cpp`

**Modificare il metodo `data()` alla riga ~26, nel switch case DisplayRole:**

Aggiungere prima del default:
```cpp
case Comments:
    return device.getComments().isEmpty() ? tr("") : device.getComments();
```

**Modificare il metodo `data()` nel TextAlignmentRole (~54):**

Aggiungere Comments alla lista degli allineamenti a sinistra (o non aggiungere nulla se vuoi lasciarlo allineato a sinistra di default).

**Modificare il metodo `headerData()` alla riga ~86, nel switch:**

Aggiungere prima del default:
```cpp
case Comments:      return tr("Comments");
```

**Aggiungere alla fine del file i nuovi metodi `setData()` e `flags()`:**

```cpp
bool DeviceTableViewModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (!index.isValid() || index.row() >= devices.count())
        return false;

    if (role != Qt::EditRole)
        return false;

    // Solo la colonna Comments è editabile
    if (index.column() != Comments)
        return false;

    Device& device = devices[index.row()];
    QString newComments = value.toString();

    // Aggiorna il commento nel device
    device.setComments(newComments);

    // Salva nel database
    if (repository) {
        repository->update(device);
        Logger::info(QString("Comments updated for device %1: %2")
                    .arg(device.getIp())
                    .arg(newComments.isEmpty() ? "(empty)" : newComments));
    }

    // Notifica la view del cambiamento
    emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});

    return true;
}

Qt::ItemFlags DeviceTableViewModel::flags(const QModelIndex& index) const {
    if (!index.isValid())
        return Qt::NoItemFlags;

    // La colonna Comments è editabile
    if (index.column() == Comments) {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    }

    // Altre colonne sono solo selezionabili
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}
```

---

### 5. Esportatori (Opzionale)

Se vuoi esportare anche i commenti nei vari formati:

#### File: `src/export/CsvExporter.cpp`

Modificare il metodo `exportDevices()` aggiungendo "Comments" nell'header e il valore nella riga:
```cpp
// Header
stream << "\"IP Address\",\"Hostname\",\"MAC Address\",\"Vendor\",\"Status\",\"Open Ports\",\"Latency (ms)\",\"Quality\",\"Comments\"\n";

// Per ogni device
stream << escapeField(device.getComments()) << "\n";
```

#### File: `src/export/JsonExporter.cpp`

Aggiungere il campo comments nell'oggetto JSON del device:
```cpp
deviceObj["comments"] = device.getComments();
```

#### File: `src/export/XmlExporter.cpp`

Aggiungere l'elemento comments:
```cpp
QDomElement commentsElem = doc.createElement("comments");
commentsElem.appendChild(doc.createTextNode(device.getComments()));
deviceElem.appendChild(commentsElem);
```

#### File: `src/export/HtmlReportGenerator.cpp`

Aggiungere la colonna Comments nella tabella HTML:
```cpp
// Nell'header della tabella
html += "            <th>Comments</th>\n";

// Nelle righe dei device
html += QString("            <td>%1</td>\n").arg(escapeHtml(device.getComments()));
```

---

## Istruzioni per l'Implementazione

1. **Eliminare il database esistente** prima di avviare l'applicazione:
   ```bash
   rm lanscan.db
   # oppure su Windows
   del lanscan.db
   ```

2. **Applicare tutte le modifiche** nell'ordine indicato sopra

3. **Ricompilare il progetto**:
   ```bash
   cd build
   cmake --build . -j12
   ```

4. **Eseguire i test** per verificare che tutto funzioni:
   ```bash
   cd build
   ctest --output-on-failure
   ```

5. **Testare manualmente**:
   - Avviare l'applicazione
   - Eseguire una scansione di rete
   - Fare doppio click sulla colonna "Comments" di un dispositivo
   - Inserire un commento e premere Invio
   - Verificare che il commento venga salvato (riavviare l'app e controllare che sia ancora presente)
   - Testare l'esportazione in CSV/JSON/XML/HTML per verificare che i commenti vengano esportati

---

## Note Tecniche

- La colonna Comments sarà **editabile** direttamente dalla tabella (doppio click sulla cella)
- I commenti vengono **salvati automaticamente** nel database SQLite quando modificati
- La colonna sarà posizionata alla **destra** della tabella (ultima colonna prima di ColumnCount)
- I commenti vengono **preservati** durante gli update del device (merge logic nel repository)
- Il campo è **opzionale** (può essere vuoto)
- L'allineamento del testo è a **sinistra** (default per Qt::AlignLeft | Qt::AlignVCenter)

---

## Estensioni Future (Opzionali)

1. **Editor multilinea**: Implementare un custom delegate per QTextEdit invece di QLineEdit se si vogliono commenti su più righe
2. **Limite caratteri**: Aggiungere validazione per limitare la lunghezza dei commenti
3. **Ricerca nei commenti**: Estendere il filtro di ricerca per includere anche i commenti
4. **Colori/tag**: Permettere di assegnare colori o tag ai commenti
5. **History**: Tracciare lo storico delle modifiche ai commenti
