# LanScan User Manual
## Network Scanner and Diagnostic Tool

**Version**: 1.0.0
**Last Updated**: 2025-10-30
**Platform**: Windows, Linux, macOS

---

## Table of Contents

1. [Introduction](#1-introduction)
2. [Getting Started](#2-getting-started)
3. [Network Scanning](#3-network-scanning)
4. [Device Management](#4-device-management)
5. [Network Diagnostics](#5-network-diagnostics)
6. [Monitoring & Alerts](#6-monitoring--alerts)
7. [Export & Reports](#7-export--reports)
8. [Settings & Configuration](#8-settings--configuration)
9. [Advanced Features](#9-advanced-features)
10. [Troubleshooting & FAQ](#10-troubleshooting--faq)

---

## 1. Introduction

### 1.1 What is LanScan?

LanScan is a powerful, cross-platform network scanner and diagnostic tool designed for network administrators, IT professionals, and home users who need to monitor and analyze their local area network (LAN).

**Key Features:**
- **Network Discovery**: Automatically discover all devices on your network with detailed information (IP address, hostname, MAC address, vendor)
- **Advanced Diagnostics**: Ping, traceroute, MTU discovery, bandwidth testing, and DNS diagnostics
- **Real-time Monitoring**: Continuous monitoring with configurable alerts for high latency, packet loss, and device status changes
- **Metrics Visualization**: Interactive charts showing latency, packet loss, jitter, and quality scores over time
- **Multi-format Export**: Export scan results to CSV, JSON, XML, or professional HTML reports
- **Wake-on-LAN**: Remotely wake up sleeping devices with magic packets
- **Multi-language**: Support for English, Italian, Spanish, French, and German
- **Customizable Themes**: Light, dark, or system-matched themes with adjustable font sizes

### 1.2 System Requirements

**Minimum Requirements:**
- **OS**: Windows 10/11, Linux (Ubuntu 20.04+, Fedora 35+, Arch), macOS 11+
- **CPU**: Dual-core processor (Intel Core i3 or equivalent)
- **RAM**: 2 GB
- **Disk**: 200 MB free space
- **Network**: Ethernet or Wi-Fi adapter with administrative privileges

**Recommended:**
- **CPU**: Quad-core processor or better
- **RAM**: 4 GB or more
- **Display**: 1920x1080 resolution or higher

**Dependencies (Linux):**
- `net-tools` (for ARP functionality)
- `traceroute` (for traceroute diagnostics)
- `iputils-ping` (usually pre-installed)

### 1.3 Installation

**Windows:**
1. Download the installer (LanScan-Setup-1.0.0.exe) from the [Releases](https://github.com/paolosereno/LanScan/releases) page
2. Run the installer and follow the on-screen instructions
3. Launch LanScan from the Start Menu or desktop shortcut

**Linux:**
1. Install dependencies: `sudo apt-get install net-tools traceroute iputils-ping`
2. Download the .deb/.rpm package or compile from source
3. Install: `sudo dpkg -i LanScan-1.0.0.deb` (Debian/Ubuntu) or `sudo rpm -i LanScan-1.0.0.rpm` (Fedora/RHEL)
4. Launch: `lanscan` or find it in your application menu

**macOS:**
1. Download the .dmg file
2. Open the .dmg and drag LanScan to your Applications folder
3. Launch from Applications or Spotlight

For detailed build instructions from source, see [INSTALLATION_GUIDE.md](INSTALLATION_GUIDE.md).

---

## 2. Getting Started

### 2.1 First Run

When you launch LanScan for the first time, you'll see the main window with:

- **Menu Bar**: File, Scan, View, Tools menus for accessing all features
- **Toolbar**: Quick access buttons (Scan, Stop, Export, Settings, Refresh)
- **Device Table**: Central area showing all discovered devices
- **Status Bar**: Displays current operation status and scan progress
- **Metrics Dock** (optional): Real-time metrics visualization (can be hidden/shown via View menu)

### 2.2 Interface Tour

#### Main Window Components

**Device Table Columns:**
- **Status**: LED indicator (🟢 Online, 🔴 Offline)
- **IP Address**: Device IP address (e.g., 192.168.1.100)
- **Hostname**: Device name (e.g., "My-Computer", "Router")
- **MAC Address**: Hardware address (e.g., "AA:BB:CC:DD:EE:FF")
- **Vendor**: Manufacturer name (e.g., "Apple Inc.", "Dell")
- **Ports**: Number of open ports detected
- **Latency**: Average ping response time in milliseconds
- **Quality**: Network quality score (0-100, color-coded)

**Toolbar Buttons:**
- **Scan** 🔍: Start a network scan
- **Stop** ⏹: Stop the current scan operation
- **Export** 📄: Export scan results to file
- **Settings** ⚙: Open settings dialog
- **Refresh** 🔄: Refresh device list from database

**System Tray Icon:**
- **Show/Hide**: Double-click to show or hide the main window
- **Quick Scan**: Right-click → Quick Scan for fast scanning
- **Exit**: Right-click → Exit to close the application

### 2.3 Your First Scan

**Quick Scan (Recommended for beginners):**

1. Click the **Scan** button on the toolbar (or press `Ctrl+S`)
2. In the Scan Configuration Dialog:
   - **Scan Type**: Select "Quick Scan" (default)
   - **Subnet**: Your local subnet will be auto-detected (e.g., "192.168.1.0/24")
   - **Profile**: Leave as "Default" or select a preset (Home Network, Enterprise, Security Audit)
3. Click **Start Scan**
4. Watch the progress bar as LanScan discovers devices
5. View results in the device table when the scan completes

**What happens during a Quick Scan?**
- **Ping sweep**: Checks which IP addresses are responsive (fast host discovery)
- **DNS lookup**: Resolves hostnames for discovered devices
- **ARP table**: Retrieves MAC addresses and matches vendors
- **Quick port scan**: Tests 20 common ports (HTTP, HTTPS, SSH, FTP, DNS, SMTP, etc.)

**Scan Duration:** Typically 10-30 seconds for a /24 subnet (254 hosts)

---

## 3. Network Scanning

### 3.1 Scan Types

LanScan offers three scan strategies to balance speed and detail:

#### 3.1.1 Quick Scan (Fast)
- **Purpose**: Fast discovery for everyday use
- **Duration**: 10-30 seconds for /24 subnet
- **Ports Scanned**: 20 common ports (HTTP, HTTPS, SSH, FTP, SMB, RDP, etc.)
- **Best For**: Quick network overview, daily monitoring, home networks

**Common Ports in Quick Scan:**
- 20, 21 (FTP)
- 22 (SSH)
- 23 (Telnet)
- 25 (SMTP)
- 53 (DNS)
- 80, 443 (HTTP/HTTPS)
- 110, 143, 993, 995 (Email: POP3, IMAP)
- 135, 139, 445 (Windows: RPC, NetBIOS, SMB)
- 3306 (MySQL)
- 3389 (RDP)
- 5432 (PostgreSQL)
- 8080 (HTTP Alt)

#### 3.1.2 Deep Scan (Thorough)
- **Purpose**: Comprehensive security audit
- **Duration**: 2-10 minutes for /24 subnet
- **Ports Scanned**: 1000 most common ports
- **Best For**: Security audits, enterprise networks, detailed analysis

**Additional Ports in Deep Scan:**
- All Quick Scan ports
- Database ports (1433, 1521, 27017, etc.)
- Management ports (8443, 9090, 10000, etc.)
- Game servers (25565, 27015, etc.)
- IoT devices (8883, 1883 MQTT, etc.)

#### 3.1.3 Custom Scan (Flexible)
- **Purpose**: User-defined scanning with profiles
- **Duration**: Varies based on profile configuration
- **Ports Scanned**: Defined by selected profile
- **Best For**: Specific use cases, repeated scans with same settings

**Pre-defined Profiles:**
1. **Home Network** (6 ports): HTTP, HTTPS, SSH, FTP, SMB, RDP
2. **Enterprise Network** (25 ports): All home ports + database, email, VPN, LDAP, management
3. **Security Audit** (62 ports): Comprehensive security-focused port list

### 3.2 Scan Configuration

**Scan Configuration Dialog Options:**

#### Scan Type
- **Quick Scan**: Fast discovery with 20 common ports
- **Deep Scan**: Thorough analysis with 1000 ports
- **Custom Scan**: Use a saved profile with custom settings

#### Subnet
- **Format**: CIDR notation (e.g., "192.168.1.0/24")
- **Auto-detection**: Click "Detect" to auto-fill your local subnet
- **Custom ranges**: Enter any valid subnet (e.g., "10.0.0.0/16" for large networks)
- **Validation**: Red text indicates invalid CIDR format

**Common Subnet Sizes:**
- `/24` = 254 hosts (e.g., 192.168.1.1 - 192.168.1.254)
- `/16` = 65,534 hosts (e.g., 10.0.0.1 - 10.0.255.254)
- `/29` = 6 hosts (small office networks)
- `/28` = 14 hosts (small business networks)

#### Profile Selection
- **Default**: Standard quick scan settings
- **Home Network**: Optimized for home routers, PCs, printers, smart devices
- **Enterprise Network**: Includes database, email, and management ports
- **Security Audit**: Comprehensive port list for security assessment
- **Custom Profiles**: Create and save your own profiles with specific port lists

### 3.3 Scan Results Interpretation

#### Device Status Indicators
- **🟢 Green LED**: Device is online and responding to pings
- **🔴 Red LED**: Device is offline or unreachable
- **⚪ Gray LED**: Unknown status (no ping attempt yet)

#### Quality Scores
- **90-100 (Excellent)**: 🟢 Green - Very low latency, no packet loss, stable connection
- **70-89 (Good)**: 🟡 Yellow-Green - Acceptable latency, minimal packet loss
- **50-69 (Fair)**: 🟠 Orange - Moderate latency or packet loss, may affect performance
- **0-49 (Poor)**: 🔴 Red - High latency, packet loss, or unstable connection

**Quality Score Calculation:**
```
Quality Score = 100 - (Latency Weight × Latency Factor)
                    - (Jitter Weight × Jitter Factor)
                    - (Packet Loss Weight × Packet Loss)

Where:
  Latency Weight = 0.4 (40%)
  Jitter Weight = 0.3 (30%)
  Packet Loss Weight = 0.3 (30%)
```

#### Vendor Identification
LanScan uses the **IEEE OUI (Organizationally Unique Identifier) database** with 38,169+ manufacturers to automatically identify device vendors from MAC addresses.

**Examples:**
- `00:1A:2B:xx:xx:xx` → "Cisco Systems"
- `B8:27:EB:xx:xx:xx` → "Raspberry Pi Foundation"
- `00:50:56:xx:xx:xx` → "VMware, Inc."
- `02:xx:xx:xx:xx:xx` → "Locally Administered Address" (VM/Container/VPN)

**Special Cases:**
- **"Unknown"**: MAC address not in database (rare or new vendor)
- **"Locally Administered"**: Software-generated MAC (virtual machines, containers, VPNs)

### 3.4 Advanced Scan Options

#### Multi-threaded Scanning
LanScan uses a configurable thread pool for parallel scanning:
- **Default**: 4 threads
- **Range**: 1-16 threads (configured in Settings → Network)
- **Recommendation**: Use 4-8 threads for best performance without overloading the network

#### Timeout Settings
- **Scan Timeout**: Maximum wait time for each operation (default: 2000ms)
- **Configurable**: Settings → Network → Scan timeout (100-10000ms)
- **Impact**: Lower timeout = faster scans, may miss slow devices; Higher timeout = more thorough, slower

#### Ping Settings
- **Ping Count**: Number of ICMP packets per device (default: 4)
- **Ping Interval**: Time between packets (default: 1000ms)
- **Configurable**: Settings → Network → Ping count/interval

---

## 4. Device Management

### 4.1 Device Details

**Opening Device Details:**
1. **Double-click** a device row in the table, or
2. **Right-click** → "View Details", or
3. Select a device and press `Enter`

**Device Detail Dialog - Four Tabs:**

#### Overview Tab
Displays comprehensive device information:
- **IP Address**: Device IPv4 address
- **Hostname**: Resolved DNS name
- **MAC Address**: Hardware address (48-bit)
- **Vendor**: Manufacturer identified from MAC OUI
- **Status**: Online/Offline with last seen timestamp
- **First Seen**: When device was first discovered
- **Last Seen**: Most recent ping response
- **Open Ports**: List of detected open ports with services

#### Ports Tab
Lists all detected open ports with details:
- **Port Number**: TCP/UDP port (e.g., 80, 443, 22)
- **Protocol**: TCP or UDP
- **State**: Open, Closed, or Filtered
- **Service**: Identified service name (e.g., "HTTP", "SSH", "HTTPS")

**Common Services:**
- 80 → HTTP (Web server)
- 443 → HTTPS (Secure web)
- 22 → SSH (Secure shell)
- 3389 → RDP (Remote Desktop)
- 445 → SMB (File sharing)
- 5432 → PostgreSQL (Database)

#### Metrics Tab
Real-time network quality monitoring with visualizations:
- **QualityGauge**: Circular gauge showing current quality score (0-100)
- **Latency Chart**: Line graph with min/avg/max latency over time
- **Packet Loss Chart**: Bar chart showing packet loss percentage
- **Jitter Chart**: Smooth spline showing connection stability
- **Summary Panel**: Current metrics (latency, jitter, packet loss, quality)

**Start/Stop Monitoring:**
- Click "Start Monitoring" to begin continuous ping tests
- Click "Stop Monitoring" to halt metrics collection
- Metrics update every 5 seconds (configurable in Settings)

#### Diagnostics Tab
Advanced network diagnostic tools (see Section 5 for details):
- **Ping Test**: Custom ping with count and interval
- **Traceroute**: Discover network path to device
- **MTU Discovery**: Find optimal Maximum Transmission Unit
- **Bandwidth Test**: Measure download/upload speed
- **DNS Lookup**: Query DNS records (A, AAAA, MX, TXT, etc.)

### 4.2 Favorites

Mark frequently accessed devices as favorites for quick access.

**Adding Favorites:**
1. **Right-click** device → "Add to Favorites", or
2. Click ⭐ icon in device row (if shown in table customization)

**Managing Favorites:**
1. Open **View** → **Favorites** (or `Ctrl+F`)
2. Favorites Widget displays all favorited devices organized by groups

**Favorites Widget Features:**
- **Group Organization**: Organize favorites into custom groups (e.g., "Servers", "Printers", "IoT Devices")
- **Search Filter**: Real-time search by device name or IP address
- **Group Filter**: Dropdown to show devices from specific group or all groups
- **Quick Connect**: Double-click to open Device Detail Dialog
- **Notes**: Add personal notes to each favorite device
- **Custom Icons**: Assign custom icons (PNG, JPG, SVG) to devices
- **Context Menu**: Right-click for actions (Remove, Add Note, Change Icon, Move to Group)

**Creating Groups:**
1. Click "New Group" button in Favorites Widget
2. Enter group name (e.g., "Production Servers")
3. Drag devices into the group or use context menu → "Move to Group"

**Use Cases:**
- Group critical servers for quick health checks
- Organize IoT devices by room or function
- Add notes with credentials, contact info, or configuration details
- Assign custom icons for visual identification

### 4.3 Scan Profiles

Create reusable scan configurations for repeated tasks.

**Creating a Profile:**
1. Open **Tools** → **Profiles** (or `Ctrl+P`)
2. Click "New Profile" button
3. Enter profile details:
   - **Name**: Descriptive name (e.g., "Web Servers")
   - **Description**: Purpose and details
   - **Port List**: Comma-separated ports (e.g., "80,443,8080,8443")
4. Click "Save"

**Using Templates:**
LanScan includes three pre-built templates:

1. **Home Network Template** (6 ports):
   - 80, 443 (HTTP/HTTPS)
   - 22 (SSH)
   - 21 (FTP)
   - 445 (SMB)
   - 3389 (RDP)

2. **Enterprise Network Template** (25 ports):
   - All home ports
   - Database: 3306 (MySQL), 5432 (PostgreSQL), 1433 (SQL Server), 27017 (MongoDB)
   - Email: 25 (SMTP), 110 (POP3), 143 (IMAP), 993/995 (IMAPS/POP3S)
   - Management: 8080, 8443, 9090
   - LDAP: 389, 636

3. **Security Audit Template** (62 ports):
   - Comprehensive port list including:
   - All enterprise ports
   - Vulnerable services (Telnet 23, FTP 21, etc.)
   - Common malware ports
   - Backup and monitoring tools
   - IoT device ports

**Managing Profiles:**
- **Edit**: Modify existing profile settings
- **Delete**: Remove unused profiles
- **Export**: Save profile to JSON file for sharing
- **Import**: Load profiles from JSON files
- **Usage Stats**: View "Last Used" and "Usage Count" for each profile

**Profile Dialog Features:**
- **Split View**: Profile list (30%) + Details panel (70%)
- **Star Indicator**: ⭐ shown for frequently used profiles (>10 uses)
- **Template Buttons**: One-click to create from templates
- **HTML Details View**: Formatted display with usage statistics

---

## 5. Network Diagnostics

LanScan includes professional-grade diagnostic tools for network troubleshooting.

### 5.1 Ping Test

**Purpose**: Measure round-trip time (RTT) and packet loss to a target device.

**How to Use:**
1. Open Device Detail Dialog → Diagnostics Tab
2. Click "Ping" button
3. Configure test parameters:
   - **Target**: Device IP (auto-filled)
   - **Count**: Number of ping packets (1-100, default: 10)
   - **Interval**: Time between packets in ms (100-5000ms, default: 1000ms)
4. Click "Start"

**Results:**
- **Latency Min/Avg/Max**: Response time range in milliseconds
- **Packets**: Sent / Received / Lost
- **Packet Loss**: Percentage of lost packets
- **Jitter**: Standard deviation of latency (connection stability)

**Interpretation:**
- **Latency < 10ms**: Excellent (local network)
- **10-50ms**: Good (typical local network or nearby internet)
- **50-100ms**: Acceptable (distant internet servers)
- **> 100ms**: High latency, may affect real-time applications
- **Packet Loss > 1%**: Potential network issue, investigate further

**Supported Platforms:**
- **Windows**: Uses ICMP echo with multi-language parsing
- **Linux**: Uses `ping` command
- **macOS**: Uses `ping` command

### 5.2 Traceroute

**Purpose**: Discover the network path (hops) from your computer to the target device.

**How to Use:**
1. Open Device Detail Dialog → Diagnostics Tab
2. Click "Traceroute" button
3. Click "Start" (target IP is auto-filled)

**Results Display:**
Each hop shows:
- **Hop Number**: Sequential router/gateway number (1, 2, 3, ...)
- **IP Address**: Router IP at this hop
- **Hostname**: Resolved DNS name (if available)
- **RTT**: Round-trip time for this hop (min/avg/max in ms)
- **Status**: Successful or timeout

**Example Output:**
```
Hop 1: 192.168.1.1 (router.local) - 1.2ms / 1.5ms / 2.1ms
Hop 2: 10.0.0.1 (gateway.isp.com) - 8.3ms / 9.1ms / 10.2ms
Hop 3: 203.0.113.1 (border.isp.com) - 15.7ms / 16.4ms / 17.8ms
Hop 4: 198.51.100.50 (target-server.example.com) - 22.1ms / 23.5ms / 25.3ms
```

**Interpretation:**
- **Hop Count**: Number of routers between you and target
- **RTT Increase**: Each hop should add 1-10ms; sudden jumps indicate congestion or long-distance links
- **Timeouts (* * *)**: Router configured not to respond to traceroute, or firewall blocking
- **High Latency Hop**: Identifies bottleneck in network path

**Use Cases:**
- Diagnose where network slowdown occurs
- Verify routing path (ensure traffic goes through expected gateways)
- Identify ISP or backbone issues
- Troubleshoot connectivity problems

**Supported Platforms:**
- **Windows**: Uses `tracert` command
- **Linux**: Uses `traceroute` command
- **macOS**: Uses `traceroute` command

### 5.3 MTU Discovery

**Purpose**: Find the optimal Maximum Transmission Unit (MTU) size for the network path to avoid packet fragmentation.

**What is MTU?**
MTU is the largest packet size (in bytes) that can be transmitted without fragmentation. Fragmentation causes performance degradation and packet loss.

**How to Use:**
1. Open Device Detail Dialog → Diagnostics Tab
2. Click "MTU Discovery" button
3. Click "Start" (target IP is auto-filled)

**Discovery Process:**
LanScan uses a **binary search algorithm**:
1. Start with range: 576 bytes (min) to 9000 bytes (max)
2. Test middle value (e.g., 4788 bytes) with `ping -f` (Don't Fragment flag)
3. If successful → search upper half; if failed → search lower half
4. Repeat until optimal MTU found

**Results:**
- **Optimal MTU**: Largest packet size that doesn't fragment (e.g., 1472 bytes)
- **Recommended MTU**: Optimal MTU + 28 bytes for IP/ICMP headers (e.g., 1500 bytes)

**Common MTU Values:**
- **1500 bytes**: Standard Ethernet
- **1492 bytes**: PPPoE (DSL connections)
- **1280 bytes**: IPv6 minimum MTU
- **9000 bytes**: Jumbo Frames (enterprise networks)

**Interpretation:**
- **MTU = 1472**: Standard Ethernet (1500 - 28 header bytes)
- **MTU < 1472**: Possible tunneling (VPN, PPPoE) or network configuration issue
- **MTU > 1472**: Jumbo Frames enabled (enterprise/datacenter)

**Use Cases:**
- Optimize VPN performance
- Troubleshoot packet loss or fragmentation issues
- Configure network equipment (routers, firewalls)
- Verify path MTU for applications (VoIP, gaming, video streaming)

**Supported Platforms:**
- **Windows**: Uses `ping -f -l <size>` (Don't Fragment flag)
- **Linux**: Uses `ping -M do -s <size>` (Path MTU Discovery)

### 5.4 Bandwidth Test

**Purpose**: Measure actual network throughput (download/upload speed) between your computer and a target device.

**⚠️ Important**: Bandwidth testing requires **LanScan-BandwidthServer** running on the target device. See Section 5.4.1 for setup instructions.

#### 5.4.1 Bandwidth Server Setup

**Installation:**
1. Download LanScan-BandwidthServer from [GitHub Releases](https://github.com/paolosereno/LanScan-BandwidthServer/releases)
2. Copy the executable to the target device
3. Run the server:
   ```bash
   # Windows
   LanScan-BandwidthServer.exe

   # Linux/macOS
   ./LanScan-BandwidthServer
   ```

**Custom Port:**
```bash
LanScan-BandwidthServer.exe --port 8080
```

**Custom Max Duration:**
```bash
LanScan-BandwidthServer.exe --max-duration 120
```

**Server Features:**
- TCP and UDP protocol support
- Configurable maximum test duration (default: 60 seconds)
- Multi-client support (one at a time)
- Automatic buffer management (100 MB max)
- Detailed logging with timestamps

#### 5.4.2 Running a Bandwidth Test

**How to Use:**
1. Start **LanScan-BandwidthServer** on target device
2. Open Device Detail Dialog → Diagnostics Tab
3. Click "Test Bandwidth" button
4. Configure test parameters:
   - **Target IP**: Device IP (auto-filled)
   - **Port**: Server port (default: 5201)
   - **Duration**: Test duration in seconds (1-60, default: 10)
   - **Protocol**: TCP (recommended) or UDP
   - **Direction**: Download or Upload
   - **Packet Size**: 1-1024 KB (default: 64 KB)
5. Click "Start"

**Results:**
- **Bandwidth**: Throughput in Mbps (Megabits per second)
- **Total Bytes**: Amount of data transferred
- **Duration**: Actual test duration in seconds
- **Quality**: Connection quality during test

**Test Directions:**
- **Download**: Server sends data to client (measures inbound bandwidth)
- **Upload**: Client sends data to server (measures outbound bandwidth)

**Protocol Comparison:**
- **TCP**: Reliable, connection-oriented, recommended for most tests
- **UDP**: Faster, no retransmission, useful for testing raw throughput

**Interpretation:**
- **1 Gbps (1000 Mbps)**: Gigabit Ethernet, excellent
- **100 Mbps**: Fast Ethernet, good for most uses
- **10 Mbps**: Slow, may be network congestion or old equipment
- **< 1 Mbps**: Very slow, investigate network issues

**Performance Tips:**
- Use TCP for accurate bandwidth measurement
- Run multiple tests and average results
- Test during low network usage for baseline
- Compare download and upload speeds to identify asymmetry

**Compatibility:**
- ⚠️ **NOT compatible with iperf3, netperf, or other standard tools**
- Must use LanScan-BandwidthServer (custom protocol)

#### 5.4.3 Protocol Details

**LanScan Bandwidth Test Protocol:**

1. **Handshake Phase**:
   - Client sends test configuration (protocol, direction, duration, packet size)
   - Server responds with "LANSCAN_BW_OK" and "READY"

2. **Data Transfer Phase**:
   - Download: Server → Client
   - Upload: Client → Server
   - Duration: Specified in handshake (1-60 seconds)

3. **Results Phase**:
   - Server calculates throughput: `(Total Bytes × 8) / Duration (ms) × 1000 / 1,000,000`
   - Server sends results: Bytes, Throughput (Mbps), Duration (ms)
   - Client displays results

### 5.5 DNS Diagnostics

**Purpose**: Query DNS records for a target hostname or IP address.

**How to Use:**
1. Open Device Detail Dialog → Diagnostics Tab
2. Click "DNS Lookup" button
3. Configure query:
   - **Query**: Hostname (e.g., "google.com") or IP (reverse lookup)
   - **DNS Server**: Leave empty for system default, or specify (e.g., "8.8.8.8")
   - **Record Type**: Select from dropdown
4. Click "Lookup"

**Supported Record Types:**

- **A (Address)**: IPv4 address (e.g., 192.0.2.1)
- **AAAA (IPv6 Address)**: IPv6 address (e.g., 2001:db8::1)
- **MX (Mail Exchange)**: Email servers with priority
- **NS (Name Server)**: Authoritative DNS servers for domain
- **TXT (Text)**: Arbitrary text data (SPF, DKIM, verification codes)
- **CNAME (Canonical Name)**: Domain alias
- **PTR (Pointer)**: Reverse DNS (IP → hostname)
- **SRV (Service)**: Service location (e.g., _ldap._tcp.example.com)

**Results Display:**
- **Record Type**: Type of record returned
- **Value**: Record data (IP, hostname, text, etc.)
- **TTL**: Time To Live (seconds until record expires in cache)
- **Priority**: For MX and SRV records (lower = higher priority)
- **Query Time**: Time taken to resolve (ms)

**Example Queries:**

1. **Forward Lookup (A Record)**:
   - Query: `google.com`
   - Type: A
   - Result: `142.250.185.46` (TTL: 300s)

2. **Reverse Lookup (PTR Record)**:
   - Query: `8.8.8.8`
   - Type: PTR
   - Result: `dns.google` (TTL: 86400s)

3. **Mail Servers (MX Records)**:
   - Query: `example.com`
   - Type: MX
   - Results:
     - `mail1.example.com` (Priority: 10)
     - `mail2.example.com` (Priority: 20)

4. **SPF Record (TXT)**:
   - Query: `example.com`
   - Type: TXT
   - Result: `v=spf1 include:_spf.google.com ~all`

**Custom DNS Servers:**
- **Google Public DNS**: 8.8.8.8, 8.8.4.4
- **Cloudflare DNS**: 1.1.1.1, 1.0.0.1
- **Quad9 DNS**: 9.9.9.9
- **OpenDNS**: 208.67.222.222, 208.67.220.220

**Use Cases:**
- Verify DNS configuration
- Troubleshoot email delivery (MX records)
- Check SPF, DKIM, DMARC records (TXT)
- Identify authoritative name servers (NS)
- Reverse lookup for IP identification (PTR)
- Test DNS propagation after changes

**Performance:**
- Query time < 100ms: Excellent (local or cached)
- 100-500ms: Acceptable (remote DNS server)
- > 500ms: Slow, consider changing DNS server

---

## 6. Monitoring & Alerts

### 6.1 Continuous Monitoring

**Purpose**: Monitor one or more devices continuously and receive alerts when network quality degrades or devices go offline.

**Starting Monitoring:**

**Method 1: From Metrics Dock**
1. Open **View** → **Metrics Dock** (or `Ctrl+M`)
2. Select device from dropdown
3. Click "Start Monitoring"
4. Observe real-time charts and metrics

**Method 2: From Device Detail Dialog**
1. Open Device Detail Dialog → Metrics Tab
2. Click "Start Monitoring"
3. Watch QualityGauge and charts update in real-time

**Method 3: From Context Menu**
1. Right-click device in table
2. Select "Start Monitoring"
3. Metrics Dock opens automatically with selected device

**Stopping Monitoring:**
- Click "Stop Monitoring" button in Metrics Dock or Device Detail Dialog
- Close Metrics Dock (monitoring stops automatically)
- Select another device (previous monitoring stops)

**What is Monitored:**
- **Latency**: Continuous ping tests (default: every 5 seconds)
- **Packet Loss**: Percentage of failed pings
- **Jitter**: Variation in latency (connection stability)
- **Quality Score**: Overall connection quality (0-100)
- **Device Status**: Online/Offline transitions

**Monitoring Interval:**
- **Default**: 5 seconds between ping bursts
- **Configurable**: Settings → Network → Ping interval (1000-10000ms)

### 6.2 Alert Configuration

**Purpose**: Receive system tray notifications when network issues are detected.

**Configuring Alerts:**
1. Open **Tools** → **Settings** (or `Ctrl+,`)
2. Navigate to **Notifications** tab
3. Configure alert settings:
   - ☑ **Enable alerts**: Master toggle for all alerts
   - ☑ **Alert sound**: Play sound when alert triggers (optional)
   - ☑ **System notifications**: Show system tray notifications
   - **High latency threshold**: 0-1000ms (default: 100ms)
   - **Packet loss threshold**: 0-100% (default: 5%)
   - **High jitter threshold**: 0-100ms (default: 10ms)
4. Click "Apply" or "OK"

**Alert Types:**

1. **High Latency Alert** (⚠️ Warning)
   - **Trigger**: Average latency exceeds threshold
   - **Example**: "Device 192.168.1.100: High latency detected (125ms, threshold: 100ms)"
   - **Icon**: ⚠️ Yellow warning icon
   - **Severity**: Warning

2. **Packet Loss Alert** (⚠️ Warning)
   - **Trigger**: Packet loss exceeds threshold
   - **Example**: "Device 192.168.1.50: Packet loss detected (8.5%, threshold: 5%)"
   - **Icon**: ⚠️ Yellow warning icon
   - **Severity**: Warning

3. **High Jitter Alert** (⚠️ Warning)
   - **Trigger**: Jitter (latency variation) exceeds threshold
   - **Example**: "Device 192.168.1.200: High jitter detected (15ms, threshold: 10ms)"
   - **Icon**: ⚠️ Yellow warning icon
   - **Severity**: Warning

4. **Device Offline Alert** (🛑 Critical)
   - **Trigger**: Device stops responding to pings
   - **Example**: "Device 192.168.1.100 (Server-01) is now OFFLINE"
   - **Icon**: 🛑 Red critical icon
   - **Severity**: Critical

5. **Device Online Alert** (ℹ️ Info)
   - **Trigger**: Previously offline device comes back online
   - **Example**: "Device 192.168.1.100 (Server-01) is now ONLINE"
   - **Icon**: ℹ️ Blue info icon
   - **Severity**: Info

**Alert Notifications:**
- **System Tray**: Popup notification with icon, title, and message
- **Duration**: Notification displays for 5-10 seconds (OS-dependent)
- **Clickable**: Click notification to bring LanScan window to foreground
- **Persistent**: Alerts are logged to database for history

**Recommended Thresholds:**

**Home Network:**
- Latency: 50-100ms
- Packet Loss: 5-10%
- Jitter: 10-20ms

**Business Network:**
- Latency: 20-50ms
- Packet Loss: 1-5%
- Jitter: 5-10ms

**Critical Services (VoIP, Video Conferencing):**
- Latency: 10-30ms
- Packet Loss: 0.5-2%
- Jitter: 2-5ms

### 6.3 Alert History

**Viewing Alert History:**
Alerts are automatically saved to the database and can be viewed:

1. **From Device Detail Dialog**:
   - Open device → Overview Tab
   - View recent alerts for this device (if history feature enabled)

2. **From Database**:
   - Alerts are stored in SQLite database (`lanscan.db`)
   - Table: `alerts` with columns: id, device_id, alert_type, severity, description, timestamp
   - Query with SQL tools if needed

**Alert Retention:**
- **Default**: Alerts are kept indefinitely
- **Configurable**: Settings → Advanced → Alert retention (future feature)
- **Manual Cleanup**: Delete database to clear all alerts

---

## 7. Export & Reports

### 7.1 Export Formats

LanScan supports four professional export formats for scan results.

#### 7.1.1 CSV Export (Comma-Separated Values)

**Purpose**: Spreadsheet-friendly format for Excel, Google Sheets, LibreOffice Calc.

**How to Export:**
1. Complete a network scan
2. Click **Export** button (or **File** → **Export** → **CSV**)
3. Choose save location and filename (e.g., `network_scan_2025-10-30.csv`)
4. Click "Save"

**CSV Structure:**
```csv
IP,Hostname,MAC,Vendor,Status,Ports,Latency,Jitter,PacketLoss,Quality
192.168.1.1,router.local,00:11:22:33:44:55,Cisco Systems,Online,5,2.1,0.5,0.0,98
192.168.1.100,server-01,AA:BB:CC:DD:EE:FF,Dell Inc.,Online,12,8.5,1.2,0.5,92
192.168.1.150,printer,12:34:56:78:90:AB,HP Inc.,Offline,0,0.0,0.0,100.0,0
```

**Use Cases:**
- Import into Excel for analysis and charting
- Merge with other data sources
- Automated processing with scripts (Python, PowerShell)
- Quick sharing via email

#### 7.1.2 JSON Export (JavaScript Object Notation)

**Purpose**: Structured data format for APIs, web applications, and automation.

**How to Export:**
1. Click **Export** → **JSON**
2. Save file (e.g., `network_scan.json`)

**JSON Structure:**
```json
{
  "devices": [
    {
      "ip": "192.168.1.1",
      "hostname": "router.local",
      "mac": "00:11:22:33:44:55",
      "vendor": "Cisco Systems",
      "status": "Online",
      "ports": [
        {"port": 80, "protocol": "TCP", "service": "HTTP", "state": "Open"},
        {"port": 443, "protocol": "TCP", "service": "HTTPS", "state": "Open"}
      ],
      "metrics": {
        "latencyAvg": 2.1,
        "jitter": 0.5,
        "packetLoss": 0.0,
        "qualityScore": 98
      },
      "firstSeen": "2025-10-30T10:15:32Z",
      "lastSeen": "2025-10-30T15:42:18Z"
    }
  ],
  "scanInfo": {
    "timestamp": "2025-10-30T15:42:18Z",
    "subnet": "192.168.1.0/24",
    "scanType": "Quick Scan",
    "deviceCount": 15
  }
}
```

**Use Cases:**
- REST API integration
- Web dashboard data source
- Configuration management databases (CMDB)
- Automated network inventory
- Cross-platform data exchange

#### 7.1.3 XML Export (Extensible Markup Language)

**Purpose**: Enterprise integration, SOAP APIs, configuration management.

**How to Export:**
1. Click **Export** → **XML**
2. Save file (e.g., `network_scan.xml`)

**XML Structure:**
```xml
<?xml version="1.0" encoding="UTF-8"?>
<LanScanExport version="1.0" timestamp="2025-10-30T15:42:18Z">
  <ScanInfo>
    <Subnet>192.168.1.0/24</Subnet>
    <ScanType>Quick Scan</ScanType>
    <DeviceCount>15</DeviceCount>
  </ScanInfo>
  <Devices>
    <Device ip="192.168.1.1" hostname="router.local" mac="00:11:22:33:44:55" vendor="Cisco Systems" status="Online">
      <Metrics latencyAvg="2.1ms" jitter="0.5ms" packetLoss="0.0%" qualityScore="98"/>
      <Ports>
        <Port number="80" protocol="TCP" service="HTTP" state="Open"/>
        <Port number="443" protocol="TCP" service="HTTPS" state="Open"/>
      </Ports>
    </Device>
  </Devices>
</LanScanExport>
```

**Use Cases:**
- Enterprise application integration
- SOAP web services
- Automated parsing with XPath/XSLT
- Configuration management tools
- Compliance reporting

#### 7.1.4 HTML Report (Web Page)

**Purpose**: Professional, human-readable reports for presentations and documentation.

**How to Export:**
1. Click **Export** → **HTML Report**
2. Save file (e.g., `network_report.html`)
3. Open in web browser (Chrome, Firefox, Edge, Safari)

**HTML Report Features:**
- **Executive Summary**: Total devices, online/offline counts, scan timestamp
- **Responsive Design**: Works on desktop, tablet, mobile
- **Color-Coded Quality**: Visual indicators for device status and quality
- **Professional Styling**: Modern CSS with gradient cards and hover effects
- **Self-Contained**: No external dependencies, can be emailed or printed
- **Printable**: Optimized for printing to PDF or paper

**Report Sections:**
1. **Header**: LanScan logo, report title, generation timestamp
2. **Summary Cards**:
   - Total Devices
   - Online Devices
   - Offline Devices
   - Average Quality Score
3. **Device Table**: Sortable table with all devices and metrics
4. **Status Badges**: 🟢 Online / 🔴 Offline indicators
5. **Quality Colors**: Color-coded quality scores (Excellent/Good/Fair/Poor/Bad)

**Use Cases:**
- Executive presentations
- Email reports to management
- Documentation for audits
- Printable network diagrams
- SharePoint or wiki documentation

### 7.2 Scheduling Exports (Future Feature)

Automated scheduled exports are planned for future releases. You can currently use external task schedulers:

**Windows Task Scheduler:**
1. Create task to run LanScan with command-line arguments
2. Schedule frequency (daily, weekly, etc.)
3. Auto-export to specified location

**Linux Cron:**
```bash
0 2 * * * /usr/bin/lanscan --scan --export /path/to/reports/scan_$(date +\%Y\%m\%d).csv
```

**Example Command (Future):**
```bash
lanscan --scan --subnet 192.168.1.0/24 --export /reports/daily_scan.csv
```

---

## 8. Settings & Configuration

### 8.1 General Settings

**Access**: Tools → Settings → General Tab

#### Startup Options
- ☑ **Start with system**: Launch LanScan automatically when you log in to Windows/Linux/macOS
  - **Windows**: Creates registry entry in `HKEY_CURRENT_USER\SOFTWARE\Microsoft\Windows\CurrentVersion\Run`
  - **Linux**: Creates `~/.config/autostart/lanscan.desktop`
  - **macOS**: Adds to Login Items

#### System Tray Behavior
- ☑ **Minimize to system tray**: When you click Minimize, hide window to system tray instead of taskbar
- ☑ **Close to tray (don't quit)**: When you click Close (X), hide to tray instead of exiting application
  - **Use Case**: Keep LanScan running in background for continuous monitoring
  - **Exit**: Right-click tray icon → Exit to fully quit application

#### Language Selection
Select interface language from dropdown:
- **English** (default)
- **Italiano** (Italian)
- **Español** (Spanish)
- **Français** (French)
- **Deutsch** (German)

**Note**: Language change requires application restart for full effect.

### 8.2 Network Settings

**Access**: Tools → Settings → Network Tab

#### Scan Timeout
- **Range**: 100-10000ms
- **Default**: 2000ms (2 seconds)
- **Description**: Maximum wait time for network operations (ping, DNS, port scan)
- **Impact**:
  - Lower (e.g., 500ms): Faster scans, may miss slow devices
  - Higher (e.g., 5000ms): More thorough, detects slow devices, longer scan time

#### Thread Count
- **Range**: 1-16 threads
- **Default**: 4 threads
- **Description**: Number of parallel scan operations
- **Recommendations**:
  - **1-2 threads**: Slow but minimal network load, use for congested networks
  - **4-8 threads**: Balanced speed and network load (recommended)
  - **12-16 threads**: Fast but high network load, use for dedicated networks or powerful routers

#### Default Subnet
- **Format**: CIDR notation (e.g., `192.168.1.0/24`)
- **Default**: Auto-detected from your network adapter
- **Description**: Subnet to pre-fill in scan dialog
- **Validation**: Red text if invalid CIDR format
- **Examples**:
  - `192.168.1.0/24` (254 hosts, typical home network)
  - `10.0.0.0/16` (65,534 hosts, large enterprise)
  - `172.16.0.0/12` (1,048,574 hosts, very large network)

#### Ping Settings
- **Ping count**: 1-10 packets (default: 4)
  - More packets = more accurate average, longer scan time
- **Ping interval**: 100-5000ms (default: 1000ms)
  - Time between consecutive ping packets to same host

### 8.3 Appearance Settings

**Access**: Tools → Settings → Appearance Tab

#### Theme Selection
- **System Default**: Match your operating system theme (auto-switches between light/dark)
- **Light**: Bright theme with white backgrounds (#ffffff) and blue accents (#0078d4)
- **Dark**: Modern dark theme with dark gray backgrounds (#1e1e1e) and blue accents (#0e639c)

**Theme Features:**
- Professional stylesheets with complete widget coverage
- Consistent styling across all UI components
- Theme-aware icons and colors
- Smooth transitions when switching themes

**Theme Preview:** Changes apply immediately (no restart required)

#### Font Size
- **Range**: 8-24 pt
- **Default**: 10 pt
- **Description**: Base font size for entire application
- **Impact**: Affects all text in tables, menus, dialogs, and tooltips
- **Recommendations**:
  - **8-10 pt**: Compact, more content visible
  - **12-14 pt**: Standard, comfortable for most users
  - **16-24 pt**: Large text, accessibility, high-DPI displays

**Accessibility:** Use larger font sizes for users with visual impairments or high-resolution displays.

### 8.4 Notifications Settings

**Access**: Tools → Settings → Notifications Tab

#### Master Control
- ☑ **Enable alerts**: Master toggle for all alert functionality
  - **Enabled**: Alerts are generated and notifications are shown
  - **Disabled**: All alerts are suppressed (monitoring still works)

#### Alert Options
- ☑ **Alert sound**: Play notification sound when alert triggers (future feature)
- ☑ **System notifications**: Show system tray popup notifications
  - **Windows**: Windows Notification Center
  - **Linux**: libnotify (notify-send)
  - **macOS**: Notification Center

#### Alert Thresholds
**High latency threshold:**
- **Range**: 0-1000ms
- **Default**: 100ms
- **Description**: Trigger alert when average latency exceeds this value
- **Examples**:
  - 50ms: Strict, for high-performance networks
  - 100ms: Balanced (default)
  - 200ms: Lenient, for WAN or internet monitoring

**Packet loss threshold:**
- **Range**: 0-100%
- **Default**: 5%
- **Description**: Trigger alert when packet loss exceeds this percentage
- **Examples**:
  - 1%: Strict, for mission-critical services
  - 5%: Balanced (default)
  - 10%: Lenient, for unreliable networks

**High jitter threshold:**
- **Range**: 0-100ms
- **Default**: 10ms
- **Description**: Trigger alert when jitter (latency variation) exceeds this value
- **Examples**:
  - 5ms: Strict, for VoIP or real-time applications
  - 10ms: Balanced (default)
  - 20ms: Lenient, for general web browsing

**Dependent Controls:** Threshold spinboxes are auto-enabled/disabled based on "Enable alerts" checkbox.

### 8.5 Advanced Settings

**Access**: Tools → Settings → Advanced Tab

#### Data Retention
**History retention:**
- **Range**: 1-365 days
- **Default**: 30 days
- **Description**: How long to keep historical events (scans, status changes, user actions)
- **Storage**: Events older than this are automatically deleted
- **Impact**: Longer retention = larger database file size

**Metrics retention:**
- **Range**: 1-90 days
- **Default**: 7 days
- **Description**: How long to keep historical metrics (latency, packet loss, jitter measurements)
- **Storage**: Metrics older than this are automatically deleted
- **Impact**: Longer retention = larger database file size, more data for trend analysis

#### Logging
**Log level:**
- **Debug**: Verbose logging, all internal operations (developers)
- **Info**: General information messages (default)
- **Warning**: Only warnings and errors
- **Error**: Only critical errors

**Enable file logging:**
- ☑ **Enabled**: Write logs to file (`lanscan.log` in application directory)
- ☐ **Disabled**: Logs only to console (if launched from terminal)

**Log File Location:**
- **Windows**: `C:\Users\<username>\AppData\Local\LanScan\lanscan.log`
- **Linux**: `~/.local/share/LanScan/lanscan.log`
- **macOS**: `~/Library/Application Support/LanScan/lanscan.log`

**Log File Rotation:**
- Max size: 10 MB
- When full, renames to `lanscan.log.old` and starts new file

### 8.6 Restoring Defaults

**How to Reset:**
1. Open Settings dialog
2. Click "Restore Defaults" button (bottom-left)
3. Confirm in dialog: "Are you sure you want to restore all settings to defaults?"
4. Click "Yes"

**What Gets Reset:**
- All five tabs (General, Network, Appearance, Notifications, Advanced)
- Does NOT affect:
  - Scan profiles (kept)
  - Favorites (kept)
  - Database (kept)
  - Scan history (kept)

**Default Values:**
- Scan timeout: 2000ms
- Threads: 4
- Theme: System Default
- Font size: 10pt
- Latency threshold: 100ms
- Packet loss threshold: 5%
- Jitter threshold: 10ms
- History retention: 30 days
- Metrics retention: 7 days
- Log level: Info

---

## 9. Advanced Features

### 9.1 Wake-on-LAN

**Purpose**: Remotely power on a sleeping or shutdown computer over the network.

**Requirements:**
1. **Target Computer BIOS**: Wake-on-LAN must be enabled in BIOS/UEFI
2. **Network Adapter**: Must support WoL (most modern NICs do)
3. **Operating System**: WoL configured in OS network settings
4. **Power State**: Computer must be connected to power (not battery on laptop)
5. **Network**: Target must be on same subnet (broadcast domain)

#### 9.1.1 Enabling Wake-on-LAN (Target Computer)

**Windows:**
1. Open Device Manager → Network Adapters
2. Right-click your network adapter → Properties
3. **Power Management** tab:
   - ☑ Allow this device to wake the computer
   - ☑ Only allow a magic packet to wake the computer
4. **Advanced** tab:
   - Find "Wake on Magic Packet" → Set to **Enabled**
5. BIOS/UEFI:
   - Restart, enter BIOS (usually Del, F2, or F12 during boot)
   - Find "Power Management" or "Advanced" section
   - Enable "Wake on LAN" or "Wake on PCI-E"

**Linux:**
1. Check WoL support: `sudo ethtool eth0 | grep Wake-on`
2. Enable WoL: `sudo ethtool -s eth0 wol g`
3. Make persistent (Ubuntu): Edit `/etc/network/interfaces`:
   ```bash
   iface eth0 inet dhcp
       post-up /sbin/ethtool -s eth0 wol g
   ```
4. Or use NetworkManager: `nmcli connection modify eth0 802-3-ethernet.wake-on-lan magic`

**macOS:**
1. System Preferences → Energy Saver
2. ☑ Wake for network access
3. ☑ Wake for Ethernet network access (if available)

#### 9.1.2 Using Wake-on-LAN in LanScan

**Method 1: Context Menu**
1. Right-click device in table
2. Select "Wake on LAN"
3. Confirm in dialog: "Send magic packet to [device name] (IP: x.x.x.x, MAC: xx:xx:xx:xx:xx:xx)?"
4. Click "Yes"

**Method 2: Device Detail Dialog**
1. Open Device Detail Dialog
2. Click "Wake on LAN" button (if available)
3. Confirm in dialog

**Success:**
- LanScan sends magic packet to device's MAC address
- Target computer should power on within 5-10 seconds
- System tray notification: "Wake-on-LAN packet sent to [device name]"

**Failure:**
- **Missing MAC address**: Device must have been discovered with MAC (scan required)
- **Not on same subnet**: WoL only works within same broadcast domain (not across routers)
- **Firewall blocking**: Some firewalls block WoL packets (UDP port 9)

**Magic Packet Format:**
```
6 bytes of 0xFF: FF FF FF FF FF FF
Followed by 16 repetitions of target MAC: [MAC] x 16
```

**Use Cases:**
- Remote desktop: Wake computer before connecting via RDP/VNC
- File access: Wake NAS or file server to access files
- Energy savings: Keep computers asleep when not in use
- Maintenance: Wake servers for scheduled backups or updates

### 9.2 Keyboard Shortcuts

**File Operations:**
- `Ctrl+S` - Start Scan
- `Ctrl+E` - Export scan results
- `Ctrl+Q` - Quit application

**View:**
- `Ctrl+M` - Show/Hide Metrics Dock
- `Ctrl+F` - Show/Hide Favorites Widget
- `Ctrl+P` - Open Profile Manager

**Settings:**
- `Ctrl+,` - Open Settings Dialog
- `F1` - Open Help (this manual)

**Device Table:**
- `Enter` - Open Device Detail Dialog for selected device
- `Delete` - Remove selected device from table (not from database)
- `Ctrl+A` - Select all devices
- `Ctrl+C` - Copy selected device IP to clipboard

**Navigation:**
- `Tab` - Move focus to next element
- `Shift+Tab` - Move focus to previous element
- `Arrow Keys` - Navigate device table rows
- `Page Up/Down` - Scroll device table

**Customization:**
Keyboard shortcuts are currently fixed. Custom shortcuts may be added in future releases.

### 9.3 Command-Line Arguments (Future Feature)

LanScan GUI currently does not support command-line arguments. A CLI version is planned for automation:

**Planned Commands:**
```bash
# Run scan and export to CSV
lanscan --scan --subnet 192.168.1.0/24 --export scan.csv

# Quick scan with specific profile
lanscan --scan --profile "Enterprise Network" --export results.json

# Generate HTML report
lanscan --report --input scan.csv --output report.html

# Monitor device and alert
lanscan --monitor 192.168.1.100 --latency-threshold 50 --alert-email admin@example.com

# Export from database
lanscan --export-db --output all_devices.json
```

**Current Workaround:**
Use GUI for all operations. For automation, use external scripts to:
1. Launch LanScan
2. Send UI automation commands (AutoHotkey, xdotool, AppleScript)
3. Read database directly (`lanscan.db` in application directory)

### 9.4 Database Management

**Database Location:**
- **Windows**: `C:\Users\<username>\AppData\Local\LanScan\lanscan.db`
- **Linux**: `~/.local/share/LanScan/lanscan.db`
- **macOS**: `~/Library/Application Support/LanScan/lanscan.db`

**Database Schema:**
```sql
-- Devices table
CREATE TABLE devices (
    ip TEXT PRIMARY KEY,
    hostname TEXT,
    mac TEXT,
    vendor TEXT,
    status TEXT,
    first_seen DATETIME,
    last_seen DATETIME
);

-- Ports table
CREATE TABLE ports (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    device_ip TEXT,
    port INTEGER,
    protocol TEXT,
    service TEXT,
    state TEXT,
    FOREIGN KEY (device_ip) REFERENCES devices(ip)
);

-- Metrics table
CREATE TABLE metrics (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    device_ip TEXT,
    latency_avg REAL,
    latency_min REAL,
    latency_max REAL,
    jitter REAL,
    packet_loss REAL,
    quality_score REAL,
    timestamp DATETIME,
    FOREIGN KEY (device_ip) REFERENCES devices(ip)
);

-- Alerts table
CREATE TABLE alerts (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    device_ip TEXT,
    alert_type TEXT,
    severity TEXT,
    description TEXT,
    timestamp DATETIME,
    acknowledged INTEGER DEFAULT 0,
    FOREIGN KEY (device_ip) REFERENCES devices(ip)
);
```

**Querying Database:**
You can use any SQLite tool to query the database directly:

```bash
# SQLite3 CLI
sqlite3 lanscan.db "SELECT * FROM devices WHERE status='Online'"

# Export to CSV
sqlite3 -header -csv lanscan.db "SELECT * FROM devices" > devices.csv

# Count devices by vendor
sqlite3 lanscan.db "SELECT vendor, COUNT(*) FROM devices GROUP BY vendor"
```

**Backup:**
```bash
# Copy database file
cp lanscan.db lanscan_backup_2025-10-30.db

# SQLite backup
sqlite3 lanscan.db ".backup lanscan_backup.db"
```

**Reset Database:**
1. Close LanScan
2. Delete `lanscan.db`
3. Restart LanScan (new empty database will be created)

---

## 10. Troubleshooting & FAQ

### 10.1 Common Issues

#### Issue: "No devices found" after scan

**Possible Causes:**
1. **Wrong subnet**: Verify subnet in scan configuration matches your network
   - Solution: Click "Detect" to auto-fill correct subnet
2. **Firewall blocking**: Windows Firewall or antivirus blocking ICMP/network access
   - Solution: Add LanScan to firewall exceptions
   - Windows: Settings → Windows Security → Firewall & network protection → Allow an app through firewall → Add LanScan
3. **No network connection**: Computer not connected to network
   - Solution: Check network adapter status, ensure connected to router/switch
4. **Network isolation**: Some routers have "AP Isolation" or "Guest Network" mode
   - Solution: Disable AP Isolation in router settings, or connect to main network
5. **Insufficient permissions**: LanScan needs admin privileges for raw sockets (ping, ARP)
   - Solution: Run as administrator (Windows) or with sudo (Linux)

#### Issue: Scan is very slow

**Possible Causes:**
1. **Too many threads**: High thread count overwhelming router
   - Solution: Reduce thread count in Settings → Network → Thread count (try 2-4)
2. **High timeout**: Waiting too long for unresponsive devices
   - Solution: Lower timeout in Settings → Network → Scan timeout (try 1000ms)
3. **Large subnet**: Scanning thousands of IPs takes time
   - Solution: Use smaller subnets (/24 instead of /16)
4. **Network congestion**: Network busy with other traffic
   - Solution: Scan during low-usage periods

#### Issue: Hostnames not resolving

**Possible Causes:**
1. **DNS server slow/unavailable**: DNS timeouts
   - Solution: Configure faster DNS in router (use Google 8.8.8.8 or Cloudflare 1.1.1.1)
2. **No reverse DNS**: Some devices don't have PTR records
   - Solution: This is normal; not all devices have hostnames
3. **Hostname caching**: Old hostname cached
   - Solution: Restart LanScan to clear cache

#### Issue: MAC addresses or vendors missing

**Possible Causes:**
1. **Remote subnet**: Devices not on local subnet (ARP doesn't cross routers)
   - Solution: LanScan must be on same subnet as target devices for MAC detection
2. **Router restrictions**: Some routers don't allow ARP table access
   - Solution: Enable "Allow ARP" in router settings (if available)
3. **Unknown vendor**: MAC OUI not in database (new or rare vendor)
   - Solution: This is normal; database has 38,169 vendors but not all exist

#### Issue: Port scan finds no open ports

**Possible Causes:**
1. **Firewall on target**: Target device firewall blocking port scans
   - Solution: Temporarily disable firewall on target (for testing only)
2. **No services running**: Device has no listening services
   - Solution: This is normal for some devices (printers, cameras, IoT)
3. **Wrong ports scanned**: Custom services on non-standard ports
   - Solution: Create custom profile with specific port list
4. **Network security**: IDS/IPS blocking port scans
   - Solution: Use authorized scanning only; configure IDS to allow LanScan

#### Issue: Metrics charts not updating

**Possible Causes:**
1. **Monitoring not started**: "Start Monitoring" button not clicked
   - Solution: Click "Start Monitoring" in Metrics Dock or Device Detail Dialog
2. **Device offline**: Cannot collect metrics from offline device
   - Solution: Verify device is online first
3. **Metrics Dock closed**: Monitoring stops when dock is closed
   - Solution: Keep dock open while monitoring, or re-open to resume

#### Issue: Alerts not showing

**Possible Causes:**
1. **Alerts disabled**: Master toggle off
   - Solution: Settings → Notifications → Enable alerts
2. **Thresholds too high**: Alert thresholds set higher than current metrics
   - Solution: Lower thresholds to test (e.g., latency threshold to 10ms)
3. **System notifications disabled**: OS blocking notifications
   - Solution:
     - Windows: Settings → System → Notifications → Allow notifications from apps → Enable for LanScan
     - Linux: Check notification daemon (notify-osd, dunst)
     - macOS: System Preferences → Notifications → LanScan → Allow Notifications
4. **Not monitoring**: Alerts only trigger during active monitoring
   - Solution: Start monitoring on device (Metrics Dock or Device Detail Dialog)

#### Issue: Bandwidth test fails

**Possible Causes:**
1. **Server not running**: LanScan-BandwidthServer not started on target
   - Solution: Launch server on target device first
2. **Wrong port**: Server running on different port
   - Solution: Verify port in server output, enter correct port in test dialog
3. **Firewall blocking**: Firewall blocking TCP port 5201
   - Solution: Add firewall exception for port 5201 (or custom port)
4. **Network timeout**: Test duration too long or network congestion
   - Solution: Reduce test duration to 10 seconds, try TCP instead of UDP

### 10.2 Frequently Asked Questions (FAQ)

**Q: Does LanScan work over the internet?**
A: Partially. LanScan is designed for local networks (LANs). Ping, traceroute, and DNS tools work over internet, but:
- MAC addresses and vendor identification only work on local subnet
- Port scanning across internet may be blocked by firewalls or ISPs
- Wake-on-LAN does not work across routers (broadcast-based)

**Q: Can I scan multiple subnets at once?**
A: Not directly in the GUI. Workaround:
1. Scan first subnet (e.g., 192.168.1.0/24)
2. Scan second subnet (e.g., 192.168.2.0/24)
3. Results accumulate in database
4. Export all devices from database

**Q: How do I scan a single IP address?**
A: Use `/32` subnet mask:
- Enter `192.168.1.100/32` in subnet field
- This scans only one IP address

**Q: What's the difference between Quick Scan and Deep Scan?**
A:
- **Quick Scan**: 20 common ports, 10-30 seconds, everyday use
- **Deep Scan**: 1000 ports, 2-10 minutes, security audits
- See Section 3.1 for full comparison

**Q: Can I schedule automatic scans?**
A: Not yet in GUI. Use external schedulers:
- Windows: Task Scheduler
- Linux: Cron jobs
- macOS: launchd
CLI support is planned for future releases.

**Q: How do I export only online devices?**
A: Currently, export includes all devices in table. Workaround:
1. Sort by Status column
2. Manually select online devices (Ctrl+Click)
3. Copy to clipboard (Ctrl+C)
4. Paste into spreadsheet
Alternative: Query database directly:
```bash
sqlite3 lanscan.db "SELECT * FROM devices WHERE status='Online'" > online_devices.csv
```

**Q: Does LanScan require admin/root privileges?**
A:
- **Windows**: No, but some features work better with admin rights (raw sockets for ping)
- **Linux**: Yes, for ICMP ping and ARP. Run with `sudo lanscan` or set capabilities:
  ```bash
  sudo setcap cap_net_raw+ep /usr/bin/lanscan
  ```
- **macOS**: No, but raw sockets may require sudo

**Q: Can I import scan results from other tools (nmap, Angry IP Scanner)?**
A: Not directly. LanScan uses its own database format. Workaround:
1. Export from other tool to CSV
2. Manually import to spreadsheet
3. Format to match LanScan CSV structure
4. Import using database tools (future feature)

**Q: How do I backup my scan history and favorites?**
A:
1. Close LanScan
2. Copy database file:
   - Windows: `C:\Users\<username>\AppData\Local\LanScan\lanscan.db`
   - Linux: `~/.local/share/LanScan/lanscan.db`
   - macOS: `~/Library/Application Support/LanScan/lanscan.db`
3. Copy favorites file (if separate): `favorites.json` in same directory

**Q: Does LanScan work on Wi-Fi?**
A: Yes, fully supported on both Ethernet and Wi-Fi. Note:
- Wi-Fi may be slower than Ethernet (higher latency, packet loss)
- Some Wi-Fi routers have "Client Isolation" that prevents device discovery
- 5 GHz Wi-Fi generally faster than 2.4 GHz

**Q: Can I monitor devices 24/7?**
A: Yes, but:
1. Keep LanScan running (minimize to system tray)
2. Enable "Start with system" in Settings → General
3. Configure alerts in Settings → Notifications
4. Note: Continuous monitoring uses CPU and network bandwidth

**Q: How do I update LanScan?**
A:
1. Check for updates: Help → Check for Updates (future feature)
2. Download latest version from [GitHub Releases](https://github.com/paolosereno/LanScan/releases)
3. Install new version (keeps database and settings)

**Q: Is my network scan data sent to the cloud?**
A: **No.** LanScan is 100% offline:
- All data stored locally in SQLite database
- No telemetry or analytics sent
- No internet connection required (except for public DNS servers if you use them)

**Q: Can I use LanScan for commercial purposes?**
A: Yes, LanScan is open-source under MIT License:
- Free for personal and commercial use
- Modify and redistribute freely
- No warranty provided
See [LICENSE](LICENSE) file for full details.

### 10.3 Performance Tips

**Optimize Scan Speed:**
1. Use Quick Scan for routine monitoring
2. Reduce subnet size (/24 instead of /16)
3. Adjust thread count based on router capacity
4. Lower timeout for fast networks (1000ms)
5. Disable unnecessary features (e.g., port scanning if you only need IP/MAC)

**Reduce Resource Usage:**
1. Close Metrics Dock when not monitoring
2. Set shorter metrics retention (3 days instead of 7)
3. Use lower thread count (2-4)
4. Limit monitoring to critical devices only
5. Export and delete old scan data periodically

**Improve Accuracy:**
1. Use higher timeout for slow/unreliable networks (5000ms)
2. Increase ping count (8-10 packets for better average)
3. Run multiple scans and compare results
4. Use Deep Scan for comprehensive port discovery
5. Enable file logging (Debug level) to diagnose issues

### 10.4 Getting Help

**Documentation:**
- **User Manual**: This document (USER_MANUAL.md)
- **API Documentation**: For developers (API_DOCUMENTATION.md)
- **Installation Guide**: Build and installation instructions (INSTALLATION_GUIDE.md)
- **README**: Project overview (README.md)
- **Changelog**: Version history and updates (CHANGELOG.md)

**Online Resources:**
- **GitHub Repository**: [https://github.com/paolosereno/LanScan](https://github.com/paolosereno/LanScan)
- **Issue Tracker**: Report bugs or request features at [GitHub Issues](https://github.com/paolosereno/LanScan/issues)
- **Discussions**: Ask questions at [GitHub Discussions](https://github.com/paolosereno/LanScan/discussions)

**Community Support:**
- Search existing issues before creating new ones
- Provide detailed information: OS version, LanScan version, error messages, steps to reproduce
- Include logs (Settings → Advanced → Enable file logging → attach `lanscan.log`)

**Contact:**
- **Author**: Paolo
- **Email**: [GitHub profile contact](https://github.com/paolosereno)

---

## Appendix A: Glossary

**ARP (Address Resolution Protocol)**: Protocol for mapping IP addresses to MAC addresses on a local network.

**Bandwidth**: Maximum data transfer rate of a network connection, measured in Mbps or Gbps.

**CIDR (Classless Inter-Domain Routing)**: Notation for specifying IP address ranges (e.g., 192.168.1.0/24).

**DNS (Domain Name System)**: System for resolving hostnames (e.g., google.com) to IP addresses.

**ICMP (Internet Control Message Protocol)**: Network layer protocol used for ping and traceroute.

**Jitter**: Variation in latency over time; high jitter indicates unstable connection.

**Latency**: Time delay between sending and receiving a network packet, measured in milliseconds (ms).

**MAC Address (Media Access Control Address)**: Unique hardware identifier for network interfaces (e.g., 00:11:22:33:44:55).

**MTU (Maximum Transmission Unit)**: Largest packet size that can be transmitted without fragmentation.

**OUI (Organizationally Unique Identifier)**: First 24 bits of MAC address identifying the manufacturer.

**Packet Loss**: Percentage of network packets that fail to reach their destination.

**Port**: Logical endpoint for network communication (0-65535); identifies specific services on a device.

**PTR Record**: DNS record for reverse lookup (IP to hostname).

**Quality Score**: LanScan's 0-100 metric combining latency, jitter, and packet loss.

**Subnet**: Logical subdivision of an IP network (e.g., 192.168.1.0/24 = 254 addresses).

**TTL (Time To Live)**: Number of hops a packet can traverse before being discarded, or lifetime of DNS cache entry.

**WoL (Wake-on-LAN)**: Technology for remotely powering on sleeping/shutdown computers via magic packet.

---

## Appendix B: Port Reference

**Common Ports Quick Reference:**

| Port | Service | Description |
|------|---------|-------------|
| 20-21 | FTP | File Transfer Protocol |
| 22 | SSH | Secure Shell (remote login) |
| 23 | Telnet | Unencrypted remote login (insecure) |
| 25 | SMTP | Email sending |
| 53 | DNS | Domain Name System |
| 80 | HTTP | Web server (unencrypted) |
| 110 | POP3 | Email retrieval |
| 143 | IMAP | Email retrieval (advanced) |
| 443 | HTTPS | Secure web server |
| 445 | SMB | File/printer sharing (Windows) |
| 3306 | MySQL | MySQL database |
| 3389 | RDP | Remote Desktop Protocol (Windows) |
| 5432 | PostgreSQL | PostgreSQL database |
| 8080 | HTTP Alt | Alternate HTTP port (web proxies) |

Full port list available in scan profile templates (Home/Enterprise/Security Audit).

---

## Appendix C: Quality Score Formula

**LanScan Quality Score Calculation:**

```
Quality Score = 100 - (W_latency × F_latency + W_jitter × F_jitter + W_packetloss × P)

Where:
  W_latency = 0.4 (40% weight)
  W_jitter = 0.3 (30% weight)
  W_packetloss = 0.3 (30% weight)

  F_latency = min(latency_avg / 200, 100)  [normalized to 0-100]
  F_jitter = min(jitter / 50, 100)          [normalized to 0-100]
  P = packet_loss_percentage                [already 0-100]

Result clamped to [0, 100]
```

**Interpretation:**
- **90-100**: Excellent - Suitable for all applications including VoIP, gaming, video
- **70-89**: Good - Acceptable for most uses, minor degradation possible
- **50-69**: Fair - Noticeable latency or packet loss, may affect real-time apps
- **0-49**: Poor - Significant issues, troubleshooting recommended

---

## Appendix D: Version History

**Version 1.0.0** (2025-10-30)
- Initial stable release
- Complete network scanning with Quick/Deep/Custom modes
- Advanced diagnostics (Ping, Traceroute, MTU, Bandwidth, DNS)
- Real-time monitoring with configurable alerts
- Multi-format export (CSV, JSON, XML, HTML)
- Multi-language support (5 languages)
- Professional themes (Light/Dark/System)
- Wake-on-LAN support
- Comprehensive testing and QA

For detailed version history, see [CHANGELOG.md](CHANGELOG.md).

---

**End of User Manual**

*For technical documentation, see [API_DOCUMENTATION.md](API_DOCUMENTATION.md)*
*For installation instructions, see [INSTALLATION_GUIDE.md](INSTALLATION_GUIDE.md)*

**LanScan © 2025** - Network Scanner and Diagnostic Tool
Licensed under MIT License
https://github.com/paolosereno/LanScan
