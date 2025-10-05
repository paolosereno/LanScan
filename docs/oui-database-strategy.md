# OUI Database Management Strategy

## Current Implementation (v1.0)

The application currently loads the OUI database from these locations (in order):
1. `oui_database.txt` (current directory)
2. `<app_directory>/oui_database.txt`
3. `~/.lanscan/oui_database.txt`
4. `data/oui_database.txt`
5. Built-in database (36 vendors) as fallback

## Production Deployment Strategies

### Strategy 1: Bundled Database (Recommended for v1.0)

**Implementation:**
- Include `oui_database.txt` in installation package
- Copy to application directory during installation
- Size: ~1.2 MB (38,169 vendors)

**Pros:**
- Works offline immediately
- No network dependency
- Simple deployment

**Cons:**
- Database becomes outdated
- Requires app updates for new vendors

**Installation locations by platform:**

```
Windows:  C:\Program Files\LanScan\oui_database.txt
Linux:    /usr/share/lanscan/oui_database.txt
macOS:    /Applications/LanScan.app/Contents/Resources/oui_database.txt
```

---

### Strategy 2: Auto-Update (Recommended for v2.0+)

**Features:**
- Check for updates on startup (optional)
- Download if database is older than 30 days
- Fallback to bundled database if download fails

**Implementation steps:**

1. **Add OUI update service:**
```cpp
class OuiUpdateService : public QObject
{
public:
    bool checkForUpdate();
    void downloadLatest();
    QDateTime getLastUpdateDate();

private:
    QString m_databasePath;
};
```

2. **Update workflow:**
   - Check file modification date
   - If > 30 days old, download in background
   - Verify downloaded file (check format/size)
   - Replace old database atomically
   - Reload MacVendorLookup singleton

3. **Download source:**
   - Primary: IEEE official (https://standards-oui.ieee.org/oui/oui.txt)
   - Fallback: GitHub release or own CDN

**User settings:**
```
- Auto-update: Enabled/Disabled
- Update frequency: Daily/Weekly/Monthly
- Use network: Only on WiFi/Always/Never
```

---

### Strategy 3: Hybrid Approach (Best for Production)

**Combination:**
1. **Bundle database** with installation (baseline)
2. **Auto-update** for fresh data (optional)
3. **Manual update** via Help > Update OUI Database
4. **Built-in fallback** for critical vendors

**Workflow:**
```
Application Start
    ├─> Load bundled database (guaranteed to work)
    ├─> Check if auto-update enabled
    │   ├─> Yes: Check last update date
    │   │   └─> If old: Download in background
    │   └─> No: Continue with current database
    └─> User can manually trigger update anytime
```

---

## Update Mechanism Design

### File Structure
```
~/.lanscan/
├── oui_database.txt          # Current database
├── oui_database.txt.new      # Download in progress
├── oui_database.txt.bak      # Previous version (rollback)
└── oui_update.json           # Metadata
```

### Metadata (oui_update.json)
```json
{
    "last_update": "2025-10-05T12:00:00Z",
    "version": "2025-10-05",
    "source": "https://standards-oui.ieee.org/oui/oui.txt",
    "entries": 38169,
    "checksum": "sha256:...",
    "auto_update": true,
    "update_frequency": "monthly"
}
```

### Update Process
```cpp
void OuiUpdateService::performUpdate()
{
    // 1. Download to temporary file
    downloadFile("https://...", "oui_database.txt.new");

    // 2. Parse and validate
    if (!validateDatabase("oui_database.txt.new")) {
        QFile::remove("oui_database.txt.new");
        return;
    }

    // 3. Backup current
    QFile::copy("oui_database.txt", "oui_database.txt.bak");

    // 4. Replace atomically
    QFile::rename("oui_database.txt.new", "oui_database.txt");

    // 5. Reload in MacVendorLookup
    MacVendorLookup::instance()->loadDefaultDatabase();

    // 6. Update metadata
    saveMetadata();

    emit updateCompleted(entries_count);
}
```

---

## Recommended Approach for LanScan v1.0

**Phase 1 (Current Release):**
1. ✅ Bundle `oui_database.txt` with installer
2. ✅ Include in Windows installer / Linux package / macOS .app
3. ✅ Document manual update procedure
4. ✅ Provide download script (`scripts/download_oui.py`)

**Phase 2 (Future Release):**
1. Add "Check for OUI updates" menu item
2. Implement background auto-update
3. Add update notification UI
4. Add settings for update preferences

---

## Manual Update Procedure (for v1.0)

**For users:**
```bash
# Windows
cd "C:\Program Files\LanScan"
python scripts\download_oui.py

# Linux
cd /usr/share/lanscan
sudo python3 scripts/download_oui.py

# macOS
cd /Applications/LanScan.app/Contents/Resources
python3 scripts/download_oui.py
```

**Or via releases:**
- Download latest `oui_database.txt` from GitHub releases
- Replace existing file in installation directory
- Restart LanScan

---

## Testing Update Mechanism

**Test cases:**
1. ✅ Fresh install with bundled database
2. ✅ Offline operation (no network)
3. ✅ Database corruption handling
4. ✅ Download failure recovery
5. ✅ Concurrent update during scan
6. ✅ Database version compatibility

---

## Database Versioning

**Version format:** `YYYY-MM-DD`
- Based on IEEE publication date
- Stored in first line of `oui_database.txt`:
  ```
  # IEEE OUI Database - Version 2025-10-05
  ```

**Compatibility:**
- Format should be backward compatible
- Old versions can read new databases
- New features may require database updates

---

## Monitoring & Analytics (Optional)

**Track:**
- Database version in use
- Last successful update
- Number of "Unknown" vendors (improvement metric)
- Update success/failure rate

**Privacy:**
- No personal data collected
- Anonymous usage statistics only (opt-in)
- Local-only tracking in settings
