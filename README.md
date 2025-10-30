# LanScan - Network Scanner and Diagnostic Tool

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![Qt](https://img.shields.io/badge/Qt-6.9.1-brightgreen.svg)](https://www.qt.io/)
[![CMake](https://img.shields.io/badge/CMake-3.16+-064F8C.svg)](https://cmake.org/)
[![Build](https://img.shields.io/badge/build-passing-brightgreen.svg)](https://github.com/paolosereno/LanScan)
[![Tests](https://img.shields.io/badge/tests-40%20suites%20%7C%2023%20passing-yellow.svg)](https://github.com/paolosereno/LanScan)
[![Progress](https://img.shields.io/badge/progress-94%25-blue.svg)](https://github.com/paolosereno/LanScan)
[![LOC](https://img.shields.io/badge/LOC-35.4k-informational.svg)](https://github.com/paolosereno/LanScan)

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
- ✅ **Monitoring service with alerts and history** (Phase 7.3, Frontend Integration: 2025-10-20)
  - Alert management with severity levels (Info, Warning, Critical)
  - Alert types: HighLatency, PacketLoss, HighJitter, DeviceOffline/Online
  - **System tray notifications with severity-based icons** ✅
  - **Alert settings loaded from QSettings on startup** ✅
  - **Configurable thresholds** (latency, packet loss, jitter) ✅
  - **Enable/disable alerts toggle** ✅
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

**Alert System Frontend Integration** (Critical Fix - ✅ Complete - 2025-10-20)
- ✅ **System Tray Notifications**
  - Alert notifications with severity-based icons (Info/Warning/Critical)
  - Signal/slot connection: MonitoringService → MainWindow
  - Alert message formatting with device ID and severity
- ✅ **Settings Integration**
  - Load alert settings from QSettings on startup
  - Dynamic settings reload when changed in Settings Dialog
  - Alert enable/disable toggle
  - Configurable thresholds (latency, packet loss, jitter)
- ✅ **Documentation**
  - ALERT_SYSTEM_TESTING.md with comprehensive test guide
  - Test cases for all alert types
  - Configuration instructions
- ✅ **Implementation**
  - 2 files modified: MainWindow.h/cpp (+105 LOC)
  - 1 file created: ALERT_SYSTEM_TESTING.md
  - Build successful (67 MB Debug build)
  - ~1.5 hours implementation time

**Testing & Quality Assurance** (Phase 10 - 🔄 In Progress - 85% Complete)
- ✅ **Critical Bug Fixes** (COMPLETED - 2025-10-30)
  - DnsDiagnosticsTest segfault resolved (signal disconnect issue)
  - DeviceTableViewModelTest compilation error fixed
  - 2 critical bugs fixed in ~1 hour
- ✅ **Test Quality Analysis** (COMPLETED - 2025-10-30)
  - Comprehensive test quality report created
  - 40 test suites analyzed (23 passing, 13 failing, 4 not compiled)
  - Root cause analysis for all failures
  - Performance targets established
- ✅ **Performance Testing Framework** (COMPLETED - 2025-10-30)
  - PerformanceTests.cpp with QBENCHMARK integration (~400 LOC)
  - Network benchmarks (Ping, DNS, Port Scan)
  - Database benchmarks (Insert, Query, Update, Bulk operations)
  - Export benchmarks (CSV, JSON - small & large datasets)
  - Stress tests (10,000 devices, concurrent operations)
- ✅ **Integration Test Suite** (COMPLETED - 2025-10-30)
  - IntegrationTests.cpp with end-to-end workflows (~700 LOC)
  - 10 integration tests covering major workflows
  - Full scan workflow (scan → persist → export)
  - Service integration (Alert, Monitoring, History)
  - Database integration (Repository-Cache sync)
- ⏳ **Code Coverage** (PENDING)
  - gcov/lcov configuration
  - Baseline coverage report
  - Target: 85% overall coverage
- ⏳ **Memory Leak Detection** (PENDING)
  - Dr. Memory setup and execution
  - Leak analysis and fixes

### 🚧 Planned (Phase 11+)
- Documentation and user guides (Phase 11)
- Release preparation and deployment (Phase 12)

## Technology Stack

- **Framework**: Qt 6.x (Widgets, Network, Charts, Sql, Concurrent, Svg, LinguistTools)
- **Language**: C++ 17
- **Build System**: CMake 3.16+
- **Architecture**: MVVM pattern with dependency injection
- **Database**: SQLite with Repository pattern
- **Testing**: Qt Test framework (40 test suites, 23 passing, performance & integration tests)
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
# Install Qt and build dependencies
sudo apt-get install qt6-base-dev qt6-charts-dev qt6-svg-dev cmake build-essential

# Install network tools (required for LanScan features)
sudo apt-get install net-tools traceroute iputils-ping

# Configure and build
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . -j$(nproc)

# Run tests
ctest --output-on-failure
```

**Required Qt Components:**
- `qt6-base-dev` - Qt base development files
- `qt6-charts-dev` - Qt Charts for metrics visualization
- `qt6-svg-dev` - Qt SVG for icon rendering (required for UI)

**Required Linux Network Tools:**
- `net-tools` - Provides `arp` command for ARP table parsing
- `traceroute` - Provides `traceroute` command for route tracing
- `iputils-ping` - Provides `ping` command (usually pre-installed)

**Installation on different distributions:**
```bash
# Debian/Ubuntu
sudo apt-get install qt6-base-dev qt6-charts-dev qt6-svg-dev cmake build-essential
sudo apt-get install net-tools traceroute iputils-ping

# Fedora/RHEL/CentOS
sudo dnf install qt6-qtbase-devel qt6-qtcharts-devel qt6-qtsvg-devel cmake gcc-c++
sudo dnf install net-tools traceroute iputils

# Arch Linux
sudo pacman -S qt6-base qt6-charts qt6-svg cmake base-devel
sudo pacman -S net-tools traceroute iputils
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

**Current Phase**: Phase 10 - Testing & Quality Assurance 🔄 **IN PROGRESS** (85% complete)
**Critical Fix**: DnsDiagnosticsTest Segfault ✅ **RESOLVED** (2025-10-30)
**Next Milestone**: Complete code coverage & memory leak detection
**Progress**: 94% (~9.85 complete phases of ~12 total)
**Latest Release**: [v0.9.4-phase9.4](https://github.com/paolosereno/LanScan/releases/tag/v0.9.4-phase9.4)

### Metrics
- **Files**: 267 total (+4 new test files)
- **Lines of Code**: ~35,397 LOC (+2,000 LOC test frameworks)
- **Test Coverage**: 40 suites (23 passing, 13 failing, 4 not compiled)
- **Build Size**: 67 MB (Debug build)
- **Languages**: 5 (English, Italian, Spanish, French, German)
- **Test Code**: ~9,600 LOC (performance & integration frameworks)

### Recent Updates

For complete project history, see [CHANGELOG.md](CHANGELOG.md).

#### Latest (October 2025)

- **2025-10-30**: Phase 10 Testing & Quality Assurance - 85% Complete 🔄
  - **Critical Bug Fixes** ✅
    - Fixed DnsDiagnosticsTest segfault (signal disconnect issue)
    - Fixed DeviceTableViewModelTest compilation error (PortInfo enum)
    - 2 files modified: ~5 LOC
  - **Test Quality Report** ✅
    - Comprehensive analysis of 40 test suites
    - Documented root causes for 13 failing tests
    - Established performance targets
    - File: PHASE_10_TEST_QUALITY_REPORT.md (~500 lines)
  - **Performance Testing Framework** ✅
    - Created PerformanceTests.cpp (~400 LOC)
    - Network, Database, Export benchmarks
    - Stress tests (10,000 devices, concurrent operations)
    - Performance targets: Quick Scan < 5s, DB Insert < 5ms, CSV Export < 200ms
  - **Integration Test Suite** ✅
    - Created IntegrationTests.cpp (~700 LOC)
    - 10 end-to-end workflow tests
    - Full scan workflow, Service integration, Database sync
  - **Phase Documentation** ✅
    - PHASE_10_SUMMARY.md with complete deliverables
    - Testing procedures and recommendations
  - **Pending Tasks** ⏳
    - Code coverage configuration with gcov/lcov
    - Memory leak detection with Dr. Memory
    - Fix remaining 13 test failures
  - **Status**: 7/9 tasks complete, 2 pending (code coverage, memory leak detection)

- **2025-10-20**: Alert System Frontend Integration ✅ **CRITICAL FIX**
  - Implemented system tray notifications for alerts
  - Connected MonitoringService::alertTriggered signal to MainWindow
  - Added alert settings loading from QSettings
  - Alert enable/disable toggle and configurable thresholds
  - Created comprehensive testing guide (ALERT_SYSTEM_TESTING.md)
  - 2 files modified: +105 LOC (~1.5 hours)
  - **Status**: Alerts now visible to users via system tray notifications
  - **Impact**: Unblocked Phase 10 (Testing & Quality Assurance)

- **2025-10-18**: AboutDialog with System Validation
  - Professional 3-tab About dialog (About, System Information, System Validation)
  - 11 validation checks for system compatibility
  - Export system info and validation reports to clipboard
  - 6 new files: ~1,800 LOC

- **2025-10-18**: Device Metrics dock widget UI improvements
  - Fixed dock usability issues (closing, floating, device selection)
  - Automatic monitoring stop when dock is hidden
  - Consistent naming across UI

- **2025-10-17**: Quality column color visualization fix
  - Simplified delegate to text-only rendering with color mapping
  - Quality displays as colored bold text (green/yellow/orange/red)

- **2025-10-17**: Packet loss detection for offline devices
  - Fixed chart to display 100% packet loss for offline devices
  - Comprehensive tracking for all device states

- **2025-10-17**: Metrics visualization and monitoring fixes
  - Fixed latency chart to show min/avg/max lines (increased ping samples to 4)
  - Added dynamic Y-axis for 0% packet loss visualization
  - Fixed Stop Monitoring button functionality

- **2025-10-13**: Theme system and UI styling fixes
  - Fixed light theme not applying (QSS resource paths)
  - Implemented dynamic font size with regex replacement
  - Added CSS triangle arrows for ComboBox/SpinBox

- **2025-10-13**: DeviceDetailDialog History tab removed
  - Removed crash-prone History tab
  - Dialog now stable with 4 tabs (Overview, Ports, Metrics, Diagnostics)

- **2025-10-12**: DNS resolution improvements
  - Added in-memory DNS cache (max 1000 entries)
  - Fixed race condition causing hostname mismatches
  - Fixed hostname persistence in database
  - Performance: 85-95% recognition (first scan), 98-100% (cached)

- **2025-10-12**: Phase 10 started - Testing & Quality Assurance
  - 4 new test suites: ScanController, MetricsController, ExportController, DeviceTableViewModel
  - 71 new test cases (~1,508 LOC)
  - Total: 38 test suites, 160+ test cases

### Statistics
- **Files Created**: 267 total (Phase 9: 263 + Phase 10: +4 test files)
- **Lines of Code**: ~35,397 lines (Phase 9: ~33,397 + Phase 10: +2,000 test LOC)
- **Test Coverage**: 40 test suites (23 passing/58%, 13 failing/32%, 4 not compiled/10%)
- **Build Time**: ~50-60 seconds (Debug, 8-12 cores)
- **Executable Size**: 67 MB (Debug build)
- **Translation Files**: 4 languages × 17 translations = 68 translated strings
- **Test Frameworks**: Performance (~400 LOC) + Integration (~700 LOC) + Quality Report (~500 lines)

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Contributors

Paolo - Lead Developer
