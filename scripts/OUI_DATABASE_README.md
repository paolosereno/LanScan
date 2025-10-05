# OUI Database - Vendor Identification

## What is the OUI Database?

The OUI (Organizationally Unique Identifier) database maps MAC address prefixes to hardware manufacturers. LanScan uses this database to automatically identify the manufacturer/vendor of network devices (e.g., "Apple", "Cisco", "Intel").

**Current database:**
- **38,169 vendors** from IEEE official registry
- **Size:** ~1.2 MB
- **Format:** Text file (OUI + Vendor Name)

---

## Included with LanScan

The OUI database is **already included** with LanScan installation:

**Windows:**
```
C:\Program Files\LanScan\oui_database.txt
```

**Linux:**
```
/usr/share/lanscan/oui_database.txt
```

**macOS:**
```
/Applications/LanScan.app/Contents/Resources/oui_database.txt
```

---

## Why Update?

The IEEE regularly assigns new OUI prefixes to manufacturers. Updating the database ensures:
- ✅ Recognition of newer devices
- ✅ Accurate vendor identification
- ✅ Better network visibility

**Update frequency:** Monthly or quarterly recommended

---

## How to Update

### Method 1: Using the Update Script (Recommended)

**Windows:**
```cmd
cd "C:\Program Files\LanScan\scripts"
python download_oui.py
```

**Linux/macOS:**
```bash
cd /usr/share/lanscan/scripts  # or appropriate path
sudo python3 download_oui.py
```

The script will:
1. Download latest database from IEEE
2. Parse and format it
3. Save as `oui_database.txt`
4. Show update summary

### Method 2: Manual Download

1. Download from GitHub releases:
   - Visit: https://github.com/paolosereno/LanScan/releases
   - Download latest `oui_database.txt`

2. Replace existing file:
   ```bash
   # Backup current database
   cp oui_database.txt oui_database.txt.bak

   # Copy new database
   cp ~/Downloads/oui_database.txt .
   ```

3. Restart LanScan

### Method 3: From IEEE Directly

1. Download raw file:
   ```bash
   wget https://standards-oui.ieee.org/oui/oui.txt
   ```

2. Run the conversion script:
   ```bash
   python3 download_oui.py --input oui.txt
   ```

---

## Verification

After updating, verify the database is loaded:

1. Open LanScan
2. Run a network scan
3. Check vendor information for devices
4. Look for message in logs:
   ```
   [INFO] Successfully loaded XXXXX OUI entries from external database
   ```

---

## Troubleshooting

### Database not loading

**Check file location:**
```bash
# Linux/macOS
ls -lh /usr/share/lanscan/oui_database.txt

# Windows
dir "C:\Program Files\LanScan\oui_database.txt"
```

**Check permissions:**
```bash
# Linux/macOS - should be readable
chmod 644 oui_database.txt
```

### "Unknown" vendors still showing

**Possible causes:**

1. **Locally Administered MAC:** Device uses virtual/non-registered MAC
   - Shows as "Locally Administered"
   - This is correct - not in IEEE database
   - Common for VMs, containers, VPNs

2. **Very new vendor:** Just registered with IEEE
   - Update database to latest version
   - May take a few weeks to appear

3. **Database not loading:** Check logs for errors
   ```
   [WARN] External OUI database not found, using built-in database
   ```

### Download script fails

**Error:** "HTTP 418" or connection timeout

**Solution:**
- Check internet connection
- Try again later (IEEE server may be busy)
- Use manual download method instead

---

## Technical Details

### Database Format

```
# IEEE OUI Database
# Format: OUI<TAB>Vendor Name
#
000000	XEROX CORPORATION
000001	XEROX CORPORATION
...
```

**Each line:**
- 6 hex characters (OUI prefix)
- TAB separator
- Vendor name
- Comments start with `#`

### File Locations (in priority order)

LanScan searches for database in this order:

1. `oui_database.txt` (current directory)
2. `<app_dir>/oui_database.txt` (installation directory)
3. `~/.lanscan/oui_database.txt` (user home)
4. `data/oui_database.txt` (data subdirectory)
5. Built-in database (36 vendors, fallback)

### Size Information

- **IEEE OUI registry:** 38,000+ entries (~1.2 MB)
- **Built-in fallback:** 36 entries (~2 KB)
- **Load time:** ~100ms
- **Memory usage:** ~5 MB

---

## Future Features

**Planned for future releases:**
- ✨ Automatic updates on startup (optional)
- ✨ Update notification in UI
- ✨ "Check for updates" menu option
- ✨ Update frequency settings
- ✨ Rollback to previous version

---

## FAQ

**Q: How often should I update?**
A: Monthly or quarterly is sufficient. Critical for recognizing new device models.

**Q: Does it require internet?**
A: No. Database is bundled with installation. Internet only needed for updates.

**Q: What happens if database is corrupted?**
A: App falls back to built-in database (36 common vendors). Re-download to fix.

**Q: Can I use a custom database?**
A: Yes! Place your formatted file in any of the search locations.

**Q: Database shows "Locally Administered" - is this an error?**
A: No. This means the device uses a virtual/software-generated MAC address (VM, container, VPN). These are not in the IEEE registry.

---

## Support

**Issues or questions?**
- GitHub: https://github.com/paolosereno/LanScan/issues
- Documentation: https://github.com/paolosereno/LanScan/docs

**Database source:**
- IEEE Official: https://standards-oui.ieee.org/
- License: Public domain (IEEE registry)
