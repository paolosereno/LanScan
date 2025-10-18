# LanScan - Network Scanner and Diagnostic Tool

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![Qt](https://img.shields.io/badge/Qt-6.9.1-brightgreen.svg)](https://www.qt.io/)
[![CMake](https://img.shields.io/badge/CMake-3.16+-064F8C.svg)](https://cmake.org/)
[![Build](https://img.shields.io/badge/build-passing-brightgreen.svg)](https://github.com/paolosereno/LanScan)
[![Tests](https://img.shields.io/badge/tests-38%20suites%20%7C%20160%2B%20cases-brightgreen.svg)](https://github.com/paolosereno/LanScan)
[![Progress](https://img.shields.io/badge/progress-94%25-blue.svg)](https://github.com/paolosereno/LanScan)
[![LOC](https://img.shields.io/badge/LOC-32.7k-informational.svg)](https://github.com/paolosereno/LanScan)

Network scanner application with advanced diagnostics and metrics visualization.

## Features

### ✅ Implemented (Phase 0-6)

**Network Discovery & Scanning** (Phase 0-1)
- ✅ Network device discovery (IP, hostname, MAC)
- ✅ Multi-threaded IP range scanning
- ✅ Ping-based host discovery (cross-platform Windows/Linux/macOS)
- ✅ DNS reverse lookup with IP-as-hostname validation
- ✅ ARP table parsing with local interface detection
- ✅ **MAC vendor identification (38,169+ manufacturers)**
  - IEEE OUI database integration (1.2 MB, bundled)
  - Locally Administered Address (LAA) detection
  - Automatic vendor lookup for physical devices
  - VM/Container/VPN device identification
  - Singleton pattern for efficient database loading
- ✅ Service detection (40+ common ports)
- ✅ Two scan strategies (Quick & Deep)

**Metrics & Diagnostics** (Phase 2)
- ✅ Real-time latency monitoring (min/max/avg/median/stdDev)
- ✅ Jitter calculation (standard deviation & consecutive)
- ✅ Packet loss detection with burst pattern analysis
- ✅ Connection quality scoring (0-100 weighted algorithm)
- ✅ Continuous ping monitoring with history tracking
- ✅ Port scanning (TCP Quick/Full/Custom modes)
- ✅ Metrics aggregation with dependency injection

**Persistence & Data Management** (Phase 3)
- ✅ SQLite database with full schema (devices, ports, metrics)
- ✅ Repository pattern with CRUD operations
- ✅ In-memory LRU cache for performance
- ✅ Multi-format export: CSV, JSON, XML, HTML
- ✅ Cross-platform settings management (QSettings)
- ✅ Configuration validation

**Application Layer & Controllers** (Phase 4)
- ✅ Multi-threaded scan coordination (ScanCoordinator with QThreadPool)
- ✅ Thread-safe progress tracking (ProgressTracker with std::atomic)
- ✅ Scan workflow controller (quick/deep/custom scan modes)
- ✅ Metrics collection controller (continuous monitoring)
- ✅ Export operations controller (CSV/JSON/XML/HTML)
- ✅ Scan profile management with JSON persistence
- ✅ Favorite devices management with tagging support

**UI Foundation & Views** (Phase 5)
- ✅ MVVM architecture pattern implementation
- ✅ Qt Designer UI layouts (mainwindow, device table, scan dialog)
- ✅ Main window with menu bar, toolbar, status bar
- ✅ Device table widget with sorting and filtering
- ✅ Custom delegates (LED status indicator, quality score progress bar)
- ✅ Scan configuration dialog with input validation
- ✅ Context menu with device actions
- ✅ Full dependency injection architecture
- ✅ Signal/slot connections for async UI updates

**Charts & Metrics Visualization** (Phase 6)
- ✅ QtCharts integration with base ChartViewModel
- ✅ Real-time latency chart (line series: min/avg/max)
- ✅ Packet loss chart (bar series with severity-based colors)
- ✅ Jitter chart (smooth spline series)
- ✅ Auto-scaling axes with dynamic ranges
- ✅ Configurable max data points per chart (memory efficient)
- ✅ MetricsViewModel for monitoring management
- ✅ MetricsWidget with device selection and start/stop controls
- ✅ Summary panel with color-coded current metrics
- ✅ Three chart tabs for comprehensive visualization
- ✅ Thread-safe real-time updates via Qt signals

**Advanced Diagnostics** (Phase 7 - ✅ Complete)
- ✅ MetricsWidget integration into MainWindow with dock widget
- ✅ Enhanced MetricsController with proper device tracking
- ✅ Improved metrics collection with device context emission
- ✅ Multi-language ping parsing (Italian, German, French, Spanish)
- ✅ Enhanced error detection for timeout/unreachable states
- ✅ **Traceroute service with real-time hop discovery** (Phase 7.1)
  - Cross-platform support (Windows tracert / Linux traceroute)
  - Asynchronous execution with QProcess
  - Real-time hop discovery with Qt signals
  - TraceRouteHop model with RTT statistics (min/max/avg)
  - Comprehensive output parsing with regex
  - Cancellation support and error handling
  - 11 unit tests (100% passing)
- ✅ **Path MTU Discovery with binary search** (Phase 7.2)
  - Binary search algorithm (576-9000 bytes range)
  - Cross-platform ping with Don't Fragment flag (Windows -f / Linux -M do)
  - Fragmentation error detection in ping output
  - Real-time progress updates with current MTU range
  - 10 unit tests (100% passing)
- ✅ **Network bandwidth testing** (Phase 7.2)
  - TCP and UDP protocol support
  - Download and upload speed measurement
  - Configurable test duration (1-60 seconds) and packet size (1KB-1MB)
  - Results in Mbps (Megabits per second)
  - Real-time progress updates with current bandwidth
  - 11 unit tests (100% passing)
- ✅ **Advanced DNS diagnostics** (Phase 7.2)
  - Multiple DNS record types (A, AAAA, MX, NS, TXT, CNAME, PTR, SRV)
  - Forward and reverse DNS lookups
  - Custom DNS server support (e.g., 8.8.8.8)
  - Query time measurement with QElapsedTimer
  - Structured DnsRecord result type with TTL and priority
  - 15 unit tests (100% passing)
- ✅ **Monitoring service with alerts and history** (Phase 7.3)
  - Alert management with severity levels (Info, Warning, Critical)
  - Alert types: HighLatency, PacketLoss, HighJitter, DeviceOffline/Online
  - Historical metrics and events persistence in SQLite
  - Continuous device monitoring with configurable intervals
  - Threshold-based alert generation (latency, packet loss, jitter)
  - Device status change detection (online/offline transitions)
  - Alert acknowledgment and filtering
  - Automatic data pruning (LRU for alerts, time-based for history)
  - 32 unit tests across 3 test suites (AlertService, HistoryService, MonitoringService)
- ✅ **Device Detail Dialog with integrated diagnostics** (Phase 7.4, updated 2025-10-13)
  - Four-tab interface (Overview, Ports, Metrics, Diagnostics)
  - **Overview Tab**: Device information (IP, hostname, MAC, vendor, status, first/last seen, open ports)
  - **Ports Tab**: Open ports list with protocol, state, and service detection
  - **Metrics Tab**: Real-time monitoring with MetricsWidget and QualityGauge visualization
  - **Diagnostics Tab**: All Phase 7 diagnostic tools (Traceroute, MTU Discovery, Bandwidth Test, DNS Lookup)
  - Double-click and context menu support for opening dialog
  - ~~History Tab removed (2025-10-13) - caused crashes due to MonitoringService architecture mismatch~~
  - 3 new files: ~1,000 LOC (after History tab removal)

**Wake-on-LAN Support** (Phase 8.1 - ✅ Complete)
- ✅ **Wake-on-LAN magic packet sender**
  - WakeOnLanService for sending magic packets
  - Magic packet builder (6 bytes 0xFF + 16x MAC address)
  - MAC address validation (XX:XX:XX:XX:XX:XX or XX-XX-XX-XX-XX-XX)
  - UDP broadcast on port 9 (standard WoL port)
  - Cross-platform support (Windows/Linux/macOS)
- ✅ **UI Integration**
  - "Wake on LAN" context menu in device table
  - Confirmation dialog with device details
  - Error handling for missing MAC or unavailable service
  - Success/failure notifications
- ✅ **Qt Signals & Error Handling**
  - packetSent() signal for success
  - sendError() signal for failures
  - Real-time status updates
- ✅ **Unit Tests**
  - WakeOnLanServiceTest with 12 test cases
  - Magic packet validation tests
  - Signal emission tests
  - MAC address validation tests

**Advanced Export Formats** (Phase 8.2 - ✅ Complete)
- ✅ **XML Export with QXmlStreamWriter**
  - Structured XML hierarchy (LanScanExport > Devices > Device)
  - Device metadata as XML attributes
  - Detailed metrics with unit specifications (ms, %)
  - Port information with protocol, service, and state
  - Auto-formatted output with 2-space indentation
  - UTF-8 encoding support
- ✅ **HTML Report Generator**
  - Professional HTML5 structure with embedded CSS
  - Responsive design with CSS Grid layout
  - Modern UI with gradient summary cards
  - Color-coded quality indicators (Excellent/Good/Fair/Poor/Bad)
  - Status badges with Online/Offline states
  - Hover effects and interactive table
  - Mobile-friendly responsive layout
  - Self-contained reports (no external dependencies)
- ✅ **Export Controller Integration**
  - Four export formats: CSV, JSON, XML, HTML
  - Format auto-detection from file extension
  - Unified export interface (IExporter)
  - MainWindow file dialog with all format filters
- ✅ **Use Cases**
  - XML: Enterprise integration, API data exchange, automated processing
  - HTML: Executive reports, email distribution, printable documentation
- ✅ **Unit Tests**
  - XmlExporterTest with 6 test cases (structure, devices, metrics, ports)
  - HtmlReportGeneratorTest with 8 test cases (HTML structure, styling, badges)
  - Edge case testing (empty lists, special characters)

**Profile & Favorites** (Phase 8.3 - ✅ Complete)
- ✅ **ProfileManager Extensions**
  - Export/import profiles to/from JSON files
  - Three template profiles: Home Network, Enterprise Network, Security Audit
  - Usage statistics tracking (last used, usage count)
  - Profile templates with optimized port lists (6-62 ports)
- ✅ **ProfileDialog UI**
  - Split view with profile list and details panel
  - New/Edit/Delete/Import/Export buttons
  - Template buttons for quick profile creation
  - HTML-formatted details with usage statistics
  - Star indicator (⭐) for frequently used profiles (>10 uses)
- ✅ **FavoritesManager Extensions**
  - Group management (create, delete, organize devices)
  - Notes system (add, view, remove notes per device)
  - Custom icons support (PNG, JPG, SVG)
  - Extended JSON persistence for all metadata
- ✅ **FavoritesWidget UI**
  - Tree view organized by groups with "Ungrouped" section
  - Real-time search filtering by name/IP
  - Group filter dropdown with device counts
  - Context menu with device and group actions
  - Quick Connect via double-click
  - Custom icons with theme-based fallback
- ✅ **Implementation**
  - 6 new files created (~1,850 LOC)
  - ProfileManager: 9 new methods (~165 LOC)
  - FavoritesManager: 16 new methods (~233 LOC)
  - Full CMakeLists.txt integration

**History & Database** (Phase 8.4 - ✅ Complete)
- ✅ **HistoryDao - Event Persistence**
  - Event model with JSON metadata support
  - Event types: scan, status_change, alert, user_action
  - Query methods: findByDevice(), findByType(), findByDateRange()
  - Batch insert with transaction support
  - Cleanup methods: deleteOlderThan(), deleteByDevice()
  - Database indices for query optimization
- ✅ **MetricsDao - Metrics Persistence**
  - Network metrics temporal tracking
  - Statistical aggregation queries (avg, min, max)
  - getAverageMetrics() for comprehensive statistics
  - Specialized queries: getMaxLatency(), getMinLatency(), getAveragePacketLoss(), getAverageJitter()
  - Batch operations with transaction support
  - Metrics retention management
- ✅ **TrendsWidget - Temporal Visualization**
  - LatencyChart integration for trend display
  - Configurable time ranges: 1h, 6h, 24h, 7d, 30d, 90d, custom
  - Real-time statistics with data points count
  - Latency metrics (min/avg/max), packet loss, jitter, quality score
  - CSV export functionality
  - Auto-refresh on time range change
- ✅ **DatabaseManager Extensions**
  - database() method for transaction support
  - Maintains singleton pattern
- ✅ **Unit Tests**
  - HistoryDaoTest with 11 test cases
  - MetricsDaoTest with 13 test cases
- ✅ **Implementation**
  - 8 new files created (~1,433 LOC)
  - DatabaseManager: +5 LOC for database access
  - tests/CMakeLists.txt: +32 LOC for DAO tests

**Settings Dialog** (Phase 8.5 - ✅ Complete)
- ✅ **Comprehensive Settings Interface**
  - Five configuration tabs: General, Network, Appearance, Notifications, Advanced
  - QSettings integration for persistent platform-specific storage (Windows registry)
  - Input validation with user feedback (subnet CIDR format)
  - Modified state tracking with Apply button management
- ✅ **General Settings Tab**
  - Start with system checkbox
  - Minimize to system tray checkbox
  - Close to tray (don't quit) checkbox
  - Language selection: English, Italiano, Español, Français, Deutsch
- ✅ **Network Settings Tab**
  - Scan timeout (100-10000ms) with ms suffix
  - Thread count (1-16 threads)
  - Default subnet in CIDR notation with validation
  - Ping count (1-10 packets)
  - Ping interval (100-5000ms) with ms suffix
- ✅ **Appearance Settings Tab**
  - Theme selection: System Default, Light, Dark
  - Font size (8-24pt) with pt suffix
- ✅ **Notifications Settings Tab**
  - Enable alerts checkbox with dependent controls
  - Alert sound checkbox
  - System notifications checkbox
  - Alert thresholds: High latency (0-1000ms), Packet loss (0-100%), High jitter (0-100ms)
  - Threshold controls auto-enable/disable with alerts checkbox
- ✅ **Advanced Settings Tab**
  - History retention (1-365 days)
  - Metrics retention (1-90 days)
  - Log level selection: Debug, Info, Warning, Error
  - Enable file logging checkbox
- ✅ **Dialog Features**
  - Apply/OK/Cancel button handling with proper state management
  - Restore Defaults with confirmation dialog
  - settingsApplied() signal for MainWindow integration
  - Tab switching with proper layout management
  - Sensible default values for all settings
- ✅ **Implementation**
  - 3 new files created (~625 LOC)
  - SettingsDialog.h with comprehensive slot definitions (105 LOC)
  - SettingsDialog.cpp with load/save/validate methods (520 LOC)
  - settingsdialog.ui with 5-tab Qt Designer layout
  - MainWindow integration via Tools menu
  - Full CMakeLists.txt integration

**UI Polish & Theming** (Phase 9 - ✅ Complete)

**Theme System** (Phase 9.1 - ✅ Complete, Fixed 2025-10-13)
- ✅ **ThemeManager Singleton**
  - Application-wide theme switching with Light, Dark, and System modes
  - Windows system theme auto-detection via registry (AppsUseLightTheme)
  - Runtime theme switching without application restart
  - Qt signals for theme change notifications
  - **Dynamic font size support** with regex-based stylesheet replacement
- ✅ **Professional Stylesheets**
  - dark.qss - Modern dark theme (658 LOC, #1e1e1e background, #0e639c accent)
  - light.qss - Clean light theme (658 LOC, #ffffff background, #0078d4 accent)
  - Complete widget coverage: QMainWindow, QMenu, QToolBar, QPushButton, QLineEdit, QComboBox, QTableView, QScrollBar, QTabWidget, etc.
  - Consistent styling across all UI components
  - **Neutral status bar colors** (#2d2d30 dark, #f3f3f3 light)
  - **CSS-based ComboBox/SpinBox arrows** using border triangles (no external SVG dependencies)
- ✅ **Qt Resource System**
  - resources.qrc for embedded stylesheet resources
  - CMAKE_AUTORCC automatic compilation
  - **Fixed resource paths** (prefix "/" instead of "/styles" for correct QSS loading)
- ✅ **Integration**
  - SettingsDialog: Theme combo with instant preview and font size control (8-24pt)
  - main.cpp: Automatic theme and font size loading at startup from QSettings
  - Persistent theme and font preferences across sessions
  - **Fixed font size application** using ThemeManager::setFontSize() instead of qApp->setFont()
- ✅ **Critical Bug Fixes (2025-10-13)**
  - Fixed theme not applying due to incorrect QSS resource paths
  - Removed unwanted blue backgrounds from status bar
  - Implemented dynamic font size replacement in stylesheets
  - Fixed missing arrows in ComboBox and SpinBox widgets
  - All theme modes now fully functional with proper UI element visibility
- ✅ **Unit Tests**
  - ThemeManagerTest with 9 test cases (singleton, conversion, switching, loading)
- ✅ **Implementation**
  - 6 new files created (~2,039 LOC including stylesheets)
  - ThemeManager.h/cpp (332 LOC + setFontSize method)
  - dark.qss + light.qss (1,316 LOC + arrow fixes)
  - resources.qrc (23 LOC, fixed paths)
  - ThemeManagerTest.cpp (168 LOC)

**Custom Widgets** (Phase 9.2 - ✅ Complete)
- ✅ **QualityGauge Widget**
  - Circular gauge with color-coded arcs (0-100 range)
  - Quality levels: Excellent (90-100), Good (70-89), Fair (50-69), Poor (0-49)
  - Animated needle indicator with QPainter custom rendering
  - Auto-quality level detection with translatable labels
  - Theme-aware text colors
  - **Integrated into DeviceDetailDialog Metrics Tab** (2025-10-12)
    - Real-time quality visualization alongside text metrics
    - Horizontal layout for optimal space usage
    - Fixed needle angle calculation for Qt coordinate system
- ✅ **NetworkActivityIndicator Widget**
  - LED indicator with 3 states: Off, On, Blinking
  - QTimer-based blinking animation (configurable interval)
  - 3D glow effect with radial gradients
  - Configurable color and size (default: 20x20 pixels)
  - Status bar integration ready
- ✅ **GradientProgressBar Widget**
  - Extends QProgressBar with smooth color gradients
  - Color scheme: Red (0-30%), Orange (31-70%), Green (71-100%)
  - QPropertyAnimation for smooth value transitions
  - Rounded corners with QPainterPath
  - Animated value updates (default: 300ms)
- ✅ **Implementation**
  - 6 new files created (~600 LOC)
  - All widgets compiled and integrated into main executable

**UI Enhancements** (Phase 9.3 - ✅ Complete)
- ✅ **SVG Icons System**
  - 10 professional SVG icons (scan, stop, export, settings, refresh, details, favorite, copy, delete, power, clear)
  - IconLoader utility class with Qt6::Svg integration
  - SVG rendering at custom sizes (16x16 to 48x48)
  - Color customization via "currentColor" attribute
  - Qt resource system integration (:/icons/ prefix)
- ✅ **System Tray Integration**
  - QSystemTrayIcon in MainWindow with application icon
  - Context menu: Show/Hide, Quick Scan, Exit actions
  - Minimize to tray functionality (configurable in Settings)
  - Close to tray functionality (configurable in Settings)
  - Tray notifications for scan events
  - Double-click activation for show/hide
  - Windows/Linux/macOS support
- ✅ **Smooth Animations**
  - AnimationHelper utility class with reusable effects
  - Fade-in/fade-out animations (QGraphicsOpacityEffect)
  - Slide-in animations (left/right with QPropertyAnimation)
  - Expand/collapse height animations
  - Bounce effect animations (QEasingCurve::OutBounce)
  - All animations use Qt's animation framework
- ✅ **Rich HTML Tooltips**
  - TooltipHelper utility class for formatted tooltips
  - Device tooltips with HTML tables (IP, hostname, MAC, vendor, status, last seen)
  - Metrics tooltips with color-coded quality indicators
  - Scan action tooltips with descriptions
  - Export format tooltips
  - Generic rich tooltip and list tooltip creators
- ✅ **Implementation**
  - 16 new files created (~1,400 LOC)
  - Qt6::Svg component added to CMake
  - MainWindow extended with tray functionality
  - All utilities compiled and tested

**Localization** (Phase 9.4 - ✅ Complete)
- ✅ **LanguageManager Singleton**
  - Application-wide multi-language support
  - 5 languages: English, Italian (Italiano), Spanish (Español), French (Français), German (Deutsch)
  - Runtime language switching with Qt signals
  - Automatic translation file loading (.qm files)
  - Language code conversions (en, it, es, fr, de)
- ✅ **Qt Linguist Integration**
  - Qt6::LinguistTools component in CMake
  - qt6_add_translation() for .ts to .qm compilation
  - Post-build commands to copy .qm files to build/translations/
  - 4 translation files with 17 translations each
- ✅ **Translation Files**
  - lanscan_it.ts - Italian translations
  - lanscan_es.ts - Spanish translations
  - lanscan_fr.ts - French translations
  - lanscan_de.ts - German translations
  - MainWindow menu translations (File, Scan, View, Tools)
  - QualityGauge level translations (Excellent, Good, Fair, Poor, Unknown)
- ✅ **Settings Integration**
  - Language combo box in SettingsDialog General tab
  - Persistent language preference via QSettings
  - Immediate language application on settings save
  - Language initialization at application startup
- ✅ **Implementation**
  - 2 new files created (~200 LOC)
  - LanguageManager.h/cpp with singleton pattern
  - 4 translation files (~320 LOC total)
  - CMakeLists.txt updates (~25 LOC)
  - main.cpp startup language loading

**Testing & Quality Assurance** (Phase 10 - 🔄 In Progress)
- ✅ **Controller Tests** (Day 1-2 - Complete)
  - ScanControllerTest.cpp with 20 test cases
  - MetricsControllerTest.cpp with 15 test cases
  - ExportControllerTest.cpp with 16 test cases
  - Mock objects for ScanCoordinator and MetricsAggregator
  - Test coverage for scan workflows, metrics collection, and export operations
- ✅ **ViewModel Tests** (Day 3-4 - In Progress)
  - DeviceTableViewModelTest.cpp with 29 test cases
  - Tests for QAbstractTableModel interface, device management, signal emissions
  - MetricsViewModelTest.cpp (pending)
  - ScanConfigViewModelTest.cpp (pending)
- 🔄 **Integration Tests** (Day 8-9 - Planned)
  - Full scan workflow testing
  - Monitoring and alert integration tests
- 🔄 **Performance Tests** (Day 10-11 - Planned)
  - Scan performance benchmarks
  - Memory leak detection with Valgrind
  - Database query optimization
- 🔄 **Cross-platform Testing** (Day 12-14 - Planned)
  - Windows 10/11 verification
  - Linux (Ubuntu 22.04+) verification

### 🚧 Planned (Phase 11+)
- Documentation and user guides (Phase 11)
- Release preparation and deployment (Phase 12)

## Technology Stack

- **Framework**: Qt 6.x (Widgets, Network, Charts, Sql, Concurrent, Svg, LinguistTools)
- **Language**: C++ 17
- **Build System**: CMake 3.16+
- **Architecture**: MVVM pattern with dependency injection
- **Database**: SQLite with Repository pattern
- **Testing**: Qt Test framework (38 test suites, 160+ test cases)
- **Platform**: Cross-platform (Windows, Linux, macOS)
- **Localization**: Qt Linguist (5 languages: en, it, es, fr, de)

## Build Instructions

### Prerequisites
- Qt 6.2 or higher
- CMake 3.16 or higher
- C++17 compatible compiler (MSVC 2019+, GCC 9+, Clang 10+)

### Build Steps

#### Windows (MinGW)
```bash
# Clone repository
git clone https://github.com/paolosereno/LanScan.git
cd LanScan

# Clean build (recommended)
rm -rf build && mkdir build && cd build

# Configure with MinGW
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_PREFIX_PATH="C:/Qt/6.9.1/mingw_64" ..

# Build with 12 cores
cmake --build . -j12

# Run tests
ctest --output-on-failure

# Run application
./LanScan.exe
```

#### Windows (MSVC)
```bash
# Configure with MSVC
cmake -G "Visual Studio 17 2022" -A x64 \
  -DCMAKE_PREFIX_PATH="C:/Qt/6.9.1/msvc2022_64" ..

# Build Release
cmake --build . --config Release -j12
```

#### Linux
```bash
# Install dependencies
sudo apt-get install qt6-base-dev qt6-charts-dev cmake build-essential

# Configure and build
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . -j$(nproc)

# Run tests
ctest --output-on-failure
```

## Usage Guide

### Bandwidth Testing

The **Bandwidth Test** feature (available in the Diagnostics tab of Device Detail Dialog) measures network download and upload speeds between your computer and a target device using the **LanScan Bandwidth Test Protocol**.

#### Requirements

✅ **READY**: The bandwidth test requires the **LanScan-BandwidthServer** running on the target device. The server is included in the `LanScan-BandwidthServer` repository.

**⚠️ COMPATIBILITY NOTE**: LanScan uses a custom protocol and is **NOT compatible with iperf3 or other standard bandwidth testing tools**. You must use the LanScan-BandwidthServer.

#### Server Setup

**LanScan-BandwidthServer** is a lightweight Qt-based server that handles bandwidth test requests from LanScan clients.

**Building the Server:**
```bash
cd LanScan-BandwidthServer
mkdir build && cd build
cmake -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="C:/Qt/6.9.1/mingw_64" ..
mingw32-make -j16
```

**Running the Server:**
```bash
# Default port 5201
./LanScan-BandwidthServer.exe

# Custom port
./LanScan-BandwidthServer.exe --port 8080

# Custom max duration (default: 60 seconds)
./LanScan-BandwidthServer.exe --max-duration 120
```

**Server Features:**
- TCP and UDP protocol support
- Configurable maximum test duration (protection against long-running tests)
- Multi-client support (handles one client at a time per connection)
- Automatic buffer management (100 MB max buffer to ensure result delivery)
- Detailed logging with millisecond-precision timestamps
- Cross-platform (Windows, Linux, macOS)

#### Running a Bandwidth Test

1. **Start the server** on the target device:
   ```bash
   ./LanScan-BandwidthServer.exe
   ```

2. **Run test from LanScan client**:
   - Open LanScan and scan your network
   - Double-click a device to open Device Detail Dialog
   - Navigate to **Diagnostics** tab
   - Click **Test Bandwidth** button
   - Configure test parameters:
     - **Target IP**: Auto-filled with device IP
     - **Port**: Server port (default: 5201)
     - **Duration**: Test duration (1-60 seconds, default: 10)
     - **Protocol**: TCP (recommended) or UDP
     - **Direction**: Download or Upload
     - **Packet Size**: 1-1024 KB (default: 64)
   - Click **OK** to start the test

3. **View Results**:
   - Real-time progress during the test
   - Final bandwidth in Mbps (calculated by server)
   - Total bytes transferred
   - Test duration

#### Protocol Details

The **LanScan Bandwidth Test Protocol** is a custom text-based protocol with the following workflow:

**1. Handshake Phase:**
```
Client → Server:
LANSCAN_BW_TEST
VERSION:1.0
PROTOCOL:TCP
DIRECTION:DOWNLOAD
DURATION:10
PACKET_SIZE:65536
END

Server → Client:
LANSCAN_BW_OK
VERSION:1.0
READY
```

**2. Data Transfer Phase:**
- **DOWNLOAD**: Server sends test data to client as fast as possible
- **UPLOAD**: Client sends test data to server as fast as possible
- Buffer management: 100 MB maximum to prevent memory overflow
- Duration: Specified in handshake (1-60 seconds)

**3. Results Phase:**
```
Server → Client:
LANSCAN_BW_RESULTS
BYTES:4833214464
THROUGHPUT_MBPS:3866.57
DURATION_MS:10000
END
```

**Key Features:**
- **Server-side calculation**: Server calculates throughput for accuracy
- **Buffer draining**: Server waits for buffer to empty before sending results
- **Client timeout**: 30-second timeout for result reception
- **Marker-based parsing**: Client searches for `LANSCAN_BW_RESULTS` marker to identify results
- **Residual data handling**: Client automatically discards test data to find results message

**Bandwidth Calculation:**
```
Bandwidth (Mbps) = (Total Bytes × 8) / (Duration in milliseconds) × 1000 / 1,000,000
```

#### Performance Characteristics

**Tested Performance (Local Loopback):**
- TCP Download: 3.5-4 Gbps typical
- Buffer management ensures result delivery even with 100+ MB residual data
- Result delivery time: 10-20 seconds for buffer draining after test completes
- Client processes and displays server-calculated throughput

**Limitations:**
- One test per server instance at a time
- Maximum test duration: 60 seconds (configurable on server)
- TCP buffer: Limited to 100 MB to ensure timely result delivery
- Not compatible with iperf3, netperf, or other standard tools

## Database & Data Files

### OUI Database (Vendor Identification)

LanScan includes the IEEE OUI database for automatic vendor identification:

- **Database**: `oui_database.txt` (38,169+ vendors, ~1.2 MB)
- **Location**: Bundled with installation, loaded automatically
- **Update**: Manual updates via `scripts/download_oui.py`
- **Fallback**: Built-in database with 36 common vendors

For details, see [OUI Database README](scripts/OUI_DATABASE_README.md) and [Deployment Strategy](docs/oui-database-strategy.md).

### Device Database

- **Format**: SQLite
- **Location**: `lanscan.db` (created in application directory)
- **Schema**: Devices, ports, metrics with full history
- **Size**: Grows with scan data (~1-10 MB typical)

## Development

- See [PROJECT.md](PROJECT.md) for detailed architecture and development roadmap
- See [TODO.md](TODO.md) for current task list and progress tracking

### Branch Strategy
- `main` - Production releases
- `develop` - Development branch
- `feature/phaseX-description` - Feature branches (X = 0-11)
- `bugfix/description` - Bug fixes
- `hotfix/description` - Critical fixes

### Commit Convention
```
[Phase X] Brief description

Detailed description of changes.

Location: src/path/to/files
```

## Project Status

**Current Phase**: Phase 10 - Testing & Quality Assurance 🔄 **IN PROGRESS** (Day 1-4 of 14 - 29%)
**Next Milestone**: Phase 11 - Documentation & Release
**Progress**: 94% (~9.4 complete phases of ~12 total)
**Latest Release**: [v0.9.4-phase9.4](https://github.com/paolosereno/LanScan/releases/tag/v0.9.4-phase9.4)

### Metrics
- **Files**: 266 total (+4 new test files)
- **Lines of Code**: ~34,800 LOC (+1,508 LOC in Phase 10)
- **Test Coverage**: 38 suites, 160+ test cases (+4 suites, +71 cases)
- **Build Size**: 61 MB (Debug build)
- **Languages**: 5 (English, Italian, Spanish, French, German)

### Recent Updates
- **2025-10-18**: AboutDialog implementation with System Validation
  - **Feature**: Professional 3-tab About dialog with LanScan branding
    - **About Tab**: Application information, version, features list, copyright
    - **System Information Tab**: Comprehensive system details (OS, CPU, memory, Qt version, compiler, OpenGL)
    - **System Validation Tab**: 11 validation checks for system compatibility
  - **System Validators**:
    - Standard checks: OS, Memory (2GB min), Disk (200MB min), Screen (1024x768 min), Qt version, OpenGL, File permissions, Locale, Network connectivity
    - LanScan-specific: Network capabilities (admin/root for raw sockets), Database access (QSQLITE driver)
  - **Validation Severity**: INFO, WARNING, ERROR, CRITICAL levels with color-coded display
  - **Export Features**: Copy system info and validation reports to clipboard
  - **Windows Compatibility**: Fixed macro conflicts (ERROR, interface) with #undef directives
  - **Integration**: Replaced QMessageBox::about() with full AboutDialog in Help menu
  - **Source**: Adapted from IdeoRift project with LanScan customization
  - 6 new files created: ~1,800 LOC
    - AboutDialog.h/cpp with 3 tabs and clipboard export
    - SystemValidator.h/cpp with 11 validation methods and HTML/text reports
    - SystemInfoCollector.h/cpp with cross-platform system information gathering
  - Full Qt Linguist support with tr() for all UI strings
- **2025-10-18**: Device Metrics dock widget UI improvements
  - **Issue**: Device Metrics dock had usability issues
    1. No way to close the dock when monitoring was not needed
    2. Dock could be made floating via double-click (inconsistent UI)
    3. Opening dock from View menu used last device instead of selected device
    4. Context menu label "Ping Device" didn't match View menu "Device Metrics"
  - **Solutions**:
    1. Added custom toggle action in View menu (since NoDockWidgetFeatures disables built-in toggleViewAction)
    2. Set dock to NoDockWidgetFeatures (no close button, not movable, not floatable)
    3. Implemented device selection logic when opening from View menu
    4. Renamed context menu from "Ping Device" to "Device Metrics" for consistency
    5. Added automatic monitoring stop when dock is hidden
  - **Implementation Changes**:
    - MainWindow.cpp: Custom QAction for dock toggle with device selection logic
    - MainWindow.cpp: Connect visibilityChanged signal to stopMonitoring()
    - DeviceTableWidget.cpp: Renamed context menu action text
  - **Result**:
    - Dock is fixed in position (Right area), closable only via View menu
    - Opening from View menu now monitors currently selected device
    - Monitoring automatically stops when dock is hidden (saves resources)
    - Consistent naming across UI (Device Metrics everywhere)
  - 2 files modified: MainWindow.cpp (~40 LOC), DeviceTableWidget.cpp (1 LOC)
  - UI/UX: Cleaner, more predictable dock behavior with proper resource management
- **2025-10-17**: Quality column color visualization fix
  - **Issue**: Quality column in device table showed gray text instead of quality-specific colors (green/yellow/orange/red)
  - **Root Cause**:
    - QualityScoreDelegate expected format "Excellent (95)" with numeric score in parentheses
    - NetworkMetrics.getQualityScoreString() returned only "Excellent", "Good", "Poor", "Critical" without numbers
    - Delegate returned -1 for parseQualityScore() when no number found, triggering gray text rendering
    - Initial fix attempt used wrong mapping (0-100 score vs enum values 0-4)
  - **Solution**:
    - Simplified QualityScoreDelegate to render text-only display instead of progress bars
    - Removed complex bar rendering logic (gradient fills, score calculation)
    - Added direct quality string to color mapping (Excellent→Green, Good→Yellow-Green, Fair→Orange, Poor→Dark Orange, Critical→Red)
    - Made text bold for better visibility
  - **Implementation Changes**:
    - DeviceTableViewModel.cpp: Changed getQualityColor() from score-based (0-100) to enum-based mapping (0-4)
    - QualityScoreDelegate.cpp: Completely rewrote paintQualityBar() to simple text rendering with color
    - QualityScoreDelegate.cpp: Updated getColorForQuality() to include "Critical" case
  - **Result**: Quality column now displays colored text (Excellent=green, Good=yellow-green, Fair=orange, Poor=dark orange, Critical=red) with bold font
  - **UI Impact**: Cleaner, more readable quality display without overly long horizontal bars
  - 2 files modified: DeviceTableViewModel.cpp, QualityScoreDelegate.cpp (~60 LOC changed)
- **2025-10-17**: Packet loss detection for offline devices
  - **Issue**: Packet Loss chart showed no data when monitoring offline/powered-off devices
  - **Root Cause**:
    - PingService filtered out failed pings (success=false) in parsePingOutput()
    - MetricsAggregator returned empty metrics without emitting signals when all pings failed
    - Charts never received updates for 100% packet loss scenarios
  - **Solution**:
    - Modified parsePingOutput() to include both successful and failed ping results
    - Added fallback logic to create failed results when no responses detected
    - Updated MetricsAggregator::aggregate() to always calculate and emit packet loss metrics
    - Set latency/jitter to 0 when all pings fail (100% packet loss)
  - **Implementation Changes**:
    - PingService.cpp: Include failed pings in results, create fallback failed results for timeouts
    - MetricsAggregator.cpp: Calculate packet loss first, emit metrics even with 0 successful pings
  - **Result**: Packet Loss chart now correctly displays 100% (red bars) for offline devices
  - 2 files modified: PingService.cpp, MetricsAggregator.cpp
  - Behavior: Comprehensive packet loss tracking for all device states (online/offline/intermittent)
- **2025-10-17**: Metrics visualization and monitoring fixes
  - **Issue**: Multiple issues with metrics charts and monitoring control
    1. Latency chart showed only red line (max) instead of three lines (min/avg/max)
    2. Packet Loss chart showed no bars when packet loss was 0%
    3. Stop Monitoring button did not stop ping operations
  - **Root Causes**:
    1. PingService collected only 1 sample per execution, causing min=avg=max mathematically
    2. Packet Loss chart had no visual representation for 0% values (bars with height 0)
    3. MetricsViewModel did not call stopContinuousMonitoring on MetricsController
  - **Solutions**:
    1. Changed ping sample count from 1 to 4 for statistical variance (PingService.cpp:152)
    2. Added dynamic Y-axis range (0-5%) and title "No packet loss detected" for 0% values
    3. Integrated proper start/stop monitoring calls in MetricsViewModel
  - **Implementation Changes**:
    - PingService: Modified onContinuousPingTimeout() to collect 4 samples instead of 1
    - PacketLossChart: Dynamic range and title based on packet loss values
    - MetricsViewModel: Added metricsController->startContinuousMonitoring() in startMonitoring()
    - MetricsViewModel: Added metricsController->stopContinuousMonitoring() in stopMonitoring()
    - MetricsController: Improved cleanupTimer() to always stop aggregator collection
  - **Result**: All charts display correctly with proper visualization and monitoring stops immediately
  - 4 files modified: PingService.cpp, PacketLossChart.cpp, MetricsViewModel.cpp, MetricsController.cpp
  - Performance: Balanced speed (~4 seconds per update) with statistical accuracy
- **2025-10-13**: Theme system and UI styling fixes
  - **Issue**: Multiple theme and styling problems affecting user experience
    1. Light theme not applying due to incorrect QSS resource paths
    2. Unwanted blue status bar backgrounds in dark/system themes
    3. Font size settings not applying to UI elements
    4. Missing arrows in ComboBox and SpinBox widgets
  - **Root Causes**:
    1. resources.qrc had prefix="/styles" with file path "styles/light.qss", creating incorrect path ":/styles/styles/light.qss"
    2. QStatusBar had hardcoded blue colors (#007acc, #0078d4) instead of theme-neutral colors
    3. QSS hardcoded font-size values overrode programmatic font changes via qApp->setFont()
    4. QSS referenced missing :/icons/arrow-down.svg for ComboBox/SpinBox arrows
  - **Solutions**:
    1. Changed resources.qrc prefix from "/styles" to "/" for correct path ":/styles/light.qss"
    2. Replaced status bar colors with neutral theme-matching colors (#2d2d30 dark, #f3f3f3 light)
    3. Implemented ThemeManager::setFontSize() with regex to dynamically replace font-size in stylesheets
    4. Replaced missing SVG arrows with pure CSS triangles using border tricks
  - **Implementation Changes**:
    - ThemeManager: Added setFontSize() method with QRegularExpression for dynamic font-size replacement
    - SettingsDialog: Changed to use ThemeManager::setFontSize() instead of qApp->setFont()
    - main.cpp: Load and apply saved font size on startup
    - light.qss & dark.qss: Added CSS triangle arrows for ComboBox/SpinBox (no external dependencies)
  - **Result**: All theme modes (Light/Dark/System) work correctly with proper colors, font sizes, and visible UI elements
  - 7 files modified: ThemeManager.h/cpp, resources.qrc, light.qss, dark.qss, SettingsDialog.cpp, main.cpp
  - Git commit: 944db77 "fix: Resolve theme application and UI styling issues"
- **2025-10-13**: DeviceDetailDialog History tab removed
  - **Issue**: History tab was causing intermittent crashes when opening Device Details Dialog
  - **Root Cause**: History feature requires MonitoringService to be running at application level, not per-dialog
  - **Solution**: Completely removed History tab from DeviceDetailDialog
  - **Code Cleanup**: Removed 196 lines of code from 3 files (devicedetaildialog.ui, DeviceDetailDialog.h, DeviceDetailDialog.cpp)
  - **Removed Components**: setupHistoryTab(), loadHistory(), onRefreshHistoryClicked(), onTimeRangeChanged(), getStartTimeForRange()
  - **Result**: DeviceDetailDialog now has 4 tabs (Overview, Ports, Metrics, Diagnostics) and opens/closes reliably without crashes
  - **Stability**: All device detail dialogs tested successfully with multiple devices
  - 3 files modified (-196 LOC, +4 LOC comments)
- **2025-10-12**: QualityGauge integration into DeviceDetailDialog
  - **Widget Integration**: QualityGauge added to Metrics Tab with horizontal layout
  - **Real-time Updates**: Connected to MetricsViewModel for live quality visualization
  - **Needle Fix**: Corrected angle calculation from 210° to 90° (120° arc from bottom-left to bottom-right)
  - **UI Optimization**: Reduced dialog height from 950px to 650px for better UX
  - **Helper Function**: Added convertQualityScoreToValue() mapping (Excellent→95, Good→80, Fair→60, Poor→35, Critical→15)
  - 3 files modified (~50 LOC added)
- **2025-10-12**: DNS resolution improvements and critical bug fixes
  - **DNS Cache & Retry System**: Added in-memory DNS cache (max 1000 entries) with hit/miss statistics
  - **Exponential Backoff**: Implemented retry mechanism with progressive timeouts (1x, 1.5x, 2x)
  - **Configurable Timeouts**: DNS timeout increased from 2s to 3s (default), configurable per scan
  - **Enhanced Logging**: Detailed cache statistics, attempt tracking, and failure diagnostics
  - **Critical Race Condition Fix**: Fixed bug where different IPs showed same hostname
    - Root cause: Shared `m_currentIp` variable overwritten in concurrent DNS lookups
    - Solution: Added `QMap<lookupId, IP>` for thread-safe IP-to-callback tracking
    - Result: Each hostname now correctly associated with its IP address
  - **Hostname Persistence Fix**: Fixed bug where hostnames were lost in database
    - Root cause: Device updates during port scanning overwrote hostnames with empty strings
    - Solution: Data merging in `updateInDatabase()` - preserve existing values if new data has empty fields
    - Result: All resolved hostnames now properly saved and retained in database
  - **Performance Improvements**: First scan 85-95% hostname recognition, cached scans 98-100%
  - **Documentation**: Created docs/dns-improvements.md with detailed analysis and examples
  - 5 files modified, 4 commits: DNS improvements, race condition fix, persistence fix
- **2025-10-12**: Phase 10 started - Testing & Quality Assurance (Day 1-4)
  - Created ScanControllerTest.cpp with 20 test cases for scan workflows
  - Implemented MetricsControllerTest.cpp with 15 test cases for metrics collection
  - Built ExportControllerTest.cpp with 16 test cases for export operations
  - Developed DeviceTableViewModelTest.cpp with 29 test cases for UI model testing
  - Created mock objects (MockScanCoordinator, MockMetricsAggregator) for isolated testing
  - Updated CMakeLists.txt with 4 new test targets
  - 4 new test files: ~1,508 LOC
  - Total test coverage increased: 38 suites, 160+ test cases
  - Controller test coverage target: >70% (scan, metrics, export controllers)
  - ViewModel test coverage target: >80% (device table model)
- **2025-10-11**: Phase 9.4 completed - Localization
  - Implemented LanguageManager singleton for multi-language support
  - Created translation files for 5 languages (English, Italian, Spanish, French, German)
  - Integrated Qt6::LinguistTools with CMake for .qm file generation
  - Added language selection in SettingsDialog with persistent storage
  - Implemented automatic language loading at application startup
  - 17 translations per language file (MainWindow menus, QualityGauge levels)
  - Post-build commands to copy translation files to build directory
  - ~545 LOC added (LanguageManager + 4 translation files)
  - **Phase 9 (UI Polish & Theming) now 100% complete!**
- **2025-10-11**: Phase 9.3 completed - UI Enhancements
  - Created 10 SVG icons with IconLoader utility class
  - Implemented System Tray integration with notifications
  - Added AnimationHelper for smooth UI transitions
  - Created TooltipHelper for rich HTML tooltips
  - ~1,400 LOC added
- **2025-10-11**: Phase 9.2 completed - Custom Widgets
  - Implemented QualityGauge, NetworkActivityIndicator, GradientProgressBar
  - All widgets with custom QPainter rendering
  - ~600 LOC added
- **2025-10-10**: Phase 9.1 completed - Theme System
  - Implemented ThemeManager singleton for application-wide theme control
  - Created professional dark.qss and light.qss stylesheets (1,316 LOC total)
  - Built Qt Resource System integration for embedded stylesheet resources
  - Added Windows system theme auto-detection via registry (AppsUseLightTheme)
  - Implemented runtime theme switching without application restart
  - Integrated theme selection in SettingsDialog with instant preview
  - Added automatic theme loading at startup from QSettings
  - Created ThemeManagerTest with 9 comprehensive test cases
  - 6 new files created (~2,039 LOC including stylesheets)
  - Modified CMakeLists.txt, main.cpp, and SettingsDialog.cpp for integration
  - **Phase 9 (UI Polish & Theming) now 25% complete!**

- **2025-10-10**: Phase 8.5 completed - Settings Dialog
  - Implemented comprehensive SettingsDialog with 5 configuration tabs (General/Network/Appearance/Notifications/Advanced)
  - Integrated QSettings for persistent platform-specific storage (Windows registry)
  - Created General tab with startup options (start with system, minimize/close to tray) and language selection (5 languages)
  - Built Network tab with scan configuration (timeout, threads, subnet CIDR, ping settings)
  - Added Appearance tab with theme selection (System/Light/Dark) and font size control
  - Implemented Notifications tab with alert toggles and configurable thresholds (latency, packet loss, jitter)
  - Created Advanced tab with database retention settings (history/metrics days) and logging configuration
  - Added input validation with user feedback (subnet CIDR format check)
  - Implemented modified state tracking with Apply button management
  - Built Apply/OK/Cancel/Restore Defaults button handling with confirmation dialogs
  - Created settingsApplied() signal for MainWindow integration and future theme/font reloading
  - Added dependent control enable/disable (alert thresholds auto-enable with alerts checkbox)
  - Integrated dialog into MainWindow Tools menu, replacing placeholder implementation
  - 3 new files created (~625 LOC total)
  - Successfully tested all tabs and settings persistence
  - **Phase 8 (Advanced Features) now 100% complete!**

- **2025-10-10**: Phase 8.4 completed - History & Database
  - Implemented HistoryDao for event persistence with JSON metadata support
  - Created HistoryEvent model with event types (scan, status_change, alert, user_action)
  - Built MetricsDao for network metrics temporal tracking and statistical aggregation
  - Added specialized query methods: getAverageMetrics(), getMaxLatency(), getMinLatency(), getAveragePacketLoss(), getAverageJitter()
  - Implemented batch insert with transaction support for both DAOs
  - Created TrendsWidget for temporal metrics visualization with LatencyChart integration
  - Added configurable time ranges: 1h, 6h, 24h, 7d, 30d, 90d, and custom date picker
  - Implemented real-time statistics display (data points, latency min/avg/max, packet loss, jitter, quality score)
  - Added CSV export functionality for trend data
  - Extended DatabaseManager with database() method for transaction support
  - Created database indices for query optimization (device_id, event_type, timestamp)
  - Implemented cleanup methods: deleteOlderThan(), deleteByDevice() for data retention
  - Built comprehensive unit tests: HistoryDaoTest (11 test cases), MetricsDaoTest (13 test cases)
  - 8 new files created: ~1,433 LOC
  - Extended CMakeLists.txt with DAO test configuration (+32 LOC)
- **2025-10-10**: Phase 8.3 completed - Profile & Favorites
  - Extended ProfileManager with export/import functionality and JSON file support
  - Created three template profiles: Home Network (6 ports), Enterprise Network (25 ports), Security Audit (62 ports)
  - Implemented usage statistics tracking (last used date, usage count)
  - Built ProfileDialog with split view (30% list, 70% details panel)
  - Added template buttons for quick profile creation from predefined configurations
  - Extended FavoritesManager with group management (create, delete, organize)
  - Implemented notes system for devices (add, view, remove notes)
  - Added custom icons support (PNG, JPG, SVG) with theme-based fallback
  - Created FavoritesWidget with tree view organized by groups
  - Implemented real-time search filtering (by name and IP address)
  - Added group filter dropdown with device counts
  - Built context menu with device-specific and group-specific actions
  - Implemented Quick Connect via double-click
  - 6 new files: ~1,850 LOC (ProfileDialog, FavoritesWidget)
  - ProfileManager: 9 new methods (~165 LOC)
  - FavoritesManager: 16 new methods (~233 LOC)
  - Extended JSON persistence for groups, notes, and custom icons
- **2025-10-09**: Phase 8.2 completed - Advanced Export (XML, HTML)
  - Implemented XmlExporter with structured XML output and QXmlStreamWriter
  - Created HtmlReportGenerator with professional CSS styling and responsive design
  - Added gradient summary cards and color-coded quality indicators
  - Integrated XML and HTML formats into ExportController
  - Updated MainWindow file dialog with all 4 format filters (CSV, JSON, XML, HTML)
  - 14 unit tests (XmlExporter: 6, HtmlReportGenerator: 8)
  - 4 new files: ~800 LOC
- **2025-10-09**: Phase 8.1 completed - Wake-on-LAN Support
  - Implemented WakeOnLanService with magic packet builder (6 bytes 0xFF + 16x MAC address)
  - MAC address validation with regex (XX:XX:XX:XX:XX:XX or XX-XX-XX-XX-XX-XX formats)
  - UDP broadcast on port 9 (standard Wake-on-LAN port)
  - Cross-platform support (Windows/Linux/macOS)
  - DeviceTableWidget integration: "Wake on LAN" context menu item
  - Confirmation dialog with device details before sending
  - Error handling for missing MAC addresses or unavailable service
  - Qt signals for async notifications (packetSent, sendError)
  - MainWindow and main.cpp integration with dependency injection
  - Unit tests: WakeOnLanServiceTest with 12 test cases (all passing)
  - 3 new files: 551 lines of code (WakeOnLanService.h/cpp, WakeOnLanServiceTest.cpp)
- **2025-10-09**: Phase 7.4 completed - Device Detail Dialog
  - Comprehensive five-tab interface for device information (Overview, Ports, Metrics, History, Diagnostics)
  - Overview tab with device details, Ports tab with open ports list
  - Metrics tab with integrated MetricsWidget for real-time monitoring
  - History tab with historical events and time range filtering (Last Hour/6H/24H/7D/30D)
  - Diagnostics tab with all Phase 7 tools (Traceroute, MTU, Bandwidth, DNS)
  - Double-click and context menu support for opening dialog
  - Signal/slot connections for all diagnostic services with real-time updates
  - 3 new files: ~1,200 LOC (DeviceDetailDialog.h/cpp, devicedetaildialog.ui)
- **2025-10-09**: Phase 7.3 completed - Monitoring Service with Alerts and History
  - Implemented Alert model with severity levels (Info, Warning, Critical) and types
  - Created AlertService for alert management with acknowledgment and filtering
  - Built HistoryService for metrics and events persistence in SQLite
  - Designed MonitoringService for continuous device monitoring with threshold-based alerts
  - Device status change detection (online/offline transitions) with automatic alerts
  - Configurable monitoring intervals and alert thresholds per device
  - Automatic data pruning: LRU for alerts (max 1000), time-based for history (30 days default)
  - 32 unit tests created across 3 test suites (AlertService, HistoryService, MonitoringService)
  - 8 new files: ~1,500 lines of code
- **2025-10-09**: Phase 7.2 completed - Advanced Diagnostics (MTU, Bandwidth, DNS)
  - Implemented MtuDiscovery with binary search algorithm (576-9000 bytes)
  - Cross-platform ping with Don't Fragment flag for MTU discovery
  - Created BandwidthTester with TCP/UDP support for download/upload speed testing
  - Implemented DnsDiagnostics with multiple record types (A, AAAA, MX, NS, TXT, CNAME, PTR, SRV)
  - Forward and reverse DNS lookups with custom nameserver support
  - Query time measurement and structured DnsRecord result type
  - 36 unit tests created across 3 test suites (100% passing)
  - 9 new files: 2,671 lines of code
- **2025-10-09**: Phase 7.1 completed - Traceroute Service
  - Implemented cross-platform traceroute with Windows tracert and Linux/macOS traceroute support
  - Created TraceRouteHop model with RTT statistics (min/max/avg calculations)
  - Built TraceRouteService with asynchronous QProcess execution
  - Real-time hop discovery with Qt signals (hopDiscovered, traceCompleted, traceError, progressUpdated)
  - Platform-specific output parsing with regex for Windows and Unix formats
  - Comprehensive error handling and cancellation support
  - 11 unit tests created (100% passing)
  - 5 new files: 1029 lines of code
- **2025-10-07**: Phase 7.0 completed - Advanced diagnostics improvements
  - Enhanced MetricsController with currentMonitoringDevice tracking
  - Improved onMetricsUpdated to emit metricsCollected signal with device context
  - Added proper cleanup logic to stop collection only for current device
  - Enhanced PingService with multi-language support (Italian, German, French, Spanish)
  - Improved error message detection for timeout and unreachable states in ping parsing
- **2025-10-07**: Phase 6 completed - QtCharts integration with real-time metrics visualization
  - Implemented ChartViewModel base class with template helper methods
  - Created 3 chart widgets: LatencyChart (line), PacketLossChart (bar), JitterChart (spline)
  - Built MetricsViewModel with timer-based monitoring and history management
  - Designed MetricsWidget with Qt Designer UI, summary panel, and chart tabs
  - All charts feature auto-scaling, data pruning, and color-coded visualization
- **2025-10-05**: Added IEEE OUI database integration (38,169 vendors) with LAA detection
- **2025-10-05**: Fixed DNS hostname validation and local interface MAC detection
- **2025-10-05**: Implemented singleton pattern for efficient vendor lookup
- **2025-10-04**: Phase 5 completed - MVVM-based Qt GUI with ViewModels, Views, and custom delegates
- **2025-10-04**: Phase 4 completed - Application layer with multi-threaded controllers and management services
- **2025-10-04**: Phase 3 completed - Database persistence, export functionality, and settings management
- **2025-10-03**: Phase 2 completed - Added metrics calculation, ping service, and port scanning
- **2025-10-03**: Phase 1 completed - Implemented network discovery and IP scanning
- **2025-10-03**: Phase 0 completed - Project foundation and infrastructure

### Statistics
- **Files Created**: 266 total (Phase 10 added 4 new test files)
- **Lines of Code**: ~34,800+ lines (Phase 10 added ~1,508 LOC)
- **Test Coverage**: 160+ tests across 38 test suites (Phase 10: +71 tests, +4 suites)
- **Build Time**: ~50-60 seconds (Debug, 8-12 cores)
- **Executable Size**: 61 MB (Debug build)
- **Translation Files**: 4 languages × 17 translations = 68 translated strings

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Contributors

Paolo - Lead Developer
