# Changelog

All notable changes to LanScan project will be documented in this file.

## [Unreleased] - Phase 10 (Testing & Quality Assurance)

### Fixed - 2025-10-20
- **Alert System Frontend Integration - CRITICAL FIX**
  - Issue: Alert system backend was functional but alerts were invisible to users
  - Root cause: No connection between MonitoringService signals and MainWindow UI
  - Solution: Implemented system tray notification integration
  - Features implemented:
    - System tray notifications with severity-based icons (Info/Warning/Critical)
    - Alert settings loaded from QSettings on startup (enable/disable, thresholds)
    - Dynamic settings reload when changed in Settings Dialog
    - Signal/slot connection: MonitoringService::alertTriggered → MainWindow::onAlertTriggered
    - Alert message formatting with device ID, severity, and alert type
  - Result: Alerts now visible to users via system tray notifications
  - Impact: Unblocked Phase 10 (Testing & Quality Assurance) - Critical blocker resolved
  - Documentation: Created ALERT_SYSTEM_TESTING.md with comprehensive test guide
  - 2 files modified: MainWindow.h/cpp (+105 LOC)
  - Implementation time: ~1.5 hours (estimated 1-2 days)
  - Git commit: e1d3650

### Added - 2025-10-18
- **AboutDialog Implementation** with System Validation
  - Professional 3-tab About dialog with LanScan branding
  - **About Tab**: Application information, version, features list, copyright
  - **System Information Tab**: Comprehensive system details (OS, CPU, memory, Qt version, compiler, OpenGL)
  - **System Validation Tab**: 11 validation checks for system compatibility
  - System validators: OS, Memory (2GB min), Disk (200MB min), Screen (1024x768 min), Qt version, OpenGL, File permissions, Locale, Network connectivity
  - LanScan-specific validators: Network capabilities (admin/root for raw sockets), Database access (QSQLITE driver)
  - Validation severity levels: INFO, WARNING, ERROR, CRITICAL with color-coded display
  - Export features: Copy system info and validation reports to clipboard
  - Windows compatibility: Fixed macro conflicts (ERROR, interface) with #undef directives
  - 6 new files created: ~1,800 LOC (AboutDialog, SystemValidator, SystemInfoCollector)
  - Full Qt Linguist support with tr() for all UI strings
  - Source: Adapted from IdeoRift project with LanScan customization

### Fixed - 2025-10-18
- **Device Metrics Dock Widget UI Improvements**
  - Issue: Dock had no close option, could become floating, opened with wrong device
  - Solutions:
    1. Added custom toggle action in View menu (NoDockWidgetFeatures disables built-in toggleViewAction)
    2. Set dock to NoDockWidgetFeatures (no close button, not movable, not floatable)
    3. Implemented device selection logic when opening from View menu
    4. Renamed context menu from "Ping Device" to "Device Metrics" for consistency
    5. Added automatic monitoring stop when dock is hidden
  - Result: Fixed dock in Right area, closable only via View menu, monitors selected device, auto-stops when hidden
  - 2 files modified: MainWindow.cpp (~40 LOC), DeviceTableWidget.cpp (1 LOC)

### Fixed - 2025-10-17
- **Quality Column Color Visualization**
  - Issue: Quality column showed gray text instead of quality-specific colors
  - Root cause: Delegate expected "Excellent (95)" format but received only "Excellent"
  - Solution: Simplified QualityScoreDelegate to text-only rendering with direct color mapping
  - Result: Quality column displays colored bold text (Excellent=green, Good=yellow-green, Fair=orange, Poor=dark orange, Critical=red)
  - 2 files modified: DeviceTableViewModel.cpp, QualityScoreDelegate.cpp (~60 LOC)

- **Packet Loss Detection for Offline Devices**
  - Issue: Packet Loss chart showed no data for offline/powered-off devices
  - Root cause: PingService filtered out failed pings, MetricsAggregator skipped emission for 100% loss
  - Solution: Include failed pings in results, emit metrics even with 0 successful pings
  - Result: Packet Loss chart correctly displays 100% (red bars) for offline devices
  - 2 files modified: PingService.cpp, MetricsAggregator.cpp

- **Metrics Visualization and Monitoring**
  - Issues:
    1. Latency chart showed only red line (max) instead of three lines (min/avg/max)
    2. Packet Loss chart showed no bars for 0% packet loss
    3. Stop Monitoring button did not stop ping operations
  - Root causes:
    1. PingService collected only 1 sample (min=avg=max mathematically)
    2. No visual representation for 0% values
    3. MetricsViewModel didn't call stopContinuousMonitoring
  - Solutions:
    1. Changed ping samples from 1 to 4 for statistical variance
    2. Added dynamic Y-axis range (0-5%) and "No packet loss detected" title
    3. Integrated proper start/stop monitoring calls
  - Result: All charts display correctly, monitoring stops immediately
  - 4 files modified: PingService.cpp, PacketLossChart.cpp, MetricsViewModel.cpp, MetricsController.cpp

### Fixed - 2025-10-13
- **Theme System and UI Styling**
  - Issues:
    1. Light theme not applying due to incorrect QSS resource paths
    2. Unwanted blue status bar backgrounds
    3. Font size settings not applying
    4. Missing arrows in ComboBox and SpinBox widgets
  - Root causes:
    1. resources.qrc had wrong prefix creating path ":/styles/styles/light.qss"
    2. QStatusBar had hardcoded blue colors
    3. QSS hardcoded font-size overrode programmatic changes
    4. QSS referenced missing SVG arrows
  - Solutions:
    1. Changed resources.qrc prefix from "/styles" to "/"
    2. Replaced with neutral colors (#2d2d30 dark, #f3f3f3 light)
    3. Implemented ThemeManager::setFontSize() with regex replacement
    4. Replaced SVG arrows with pure CSS triangles
  - Result: All theme modes work correctly with proper colors, font sizes, visible elements
  - 7 files modified: ThemeManager, resources.qrc, stylesheets, SettingsDialog, main.cpp
  - Git commit: 944db77

- **DeviceDetailDialog History Tab Removal**
  - Issue: History tab caused intermittent crashes
  - Root cause: History feature requires MonitoringService at application level, not per-dialog
  - Solution: Completely removed History tab from DeviceDetailDialog
  - Result: Dialog now has 4 stable tabs (Overview, Ports, Metrics, Diagnostics)
  - 3 files modified (-196 LOC)

### Added - 2025-10-12
- **QualityGauge Integration**
  - Integrated QualityGauge into DeviceDetailDialog Metrics Tab
  - Real-time updates via MetricsViewModel
  - Fixed needle angle calculation (210° to 90° for 120° arc)
  - Reduced dialog height from 950px to 650px
  - Added convertQualityScoreToValue() helper function
  - 3 files modified (~50 LOC)

- **Phase 10 Started - Testing & Quality Assurance (Day 1-4)**
  - Created ScanControllerTest.cpp with 20 test cases
  - Implemented MetricsControllerTest.cpp with 15 test cases
  - Built ExportControllerTest.cpp with 16 test cases
  - Developed DeviceTableViewModelTest.cpp with 29 test cases
  - Created mock objects (MockScanCoordinator, MockMetricsAggregator)
  - Updated CMakeLists.txt with 4 new test targets
  - 4 new test files: ~1,508 LOC
  - Total: 38 test suites, 160+ test cases

### Fixed - 2025-10-12
- **DNS Resolution Improvements**
  - Added in-memory DNS cache (max 1000 entries) with statistics
  - Implemented exponential backoff retry mechanism (1x, 1.5x, 2x timeouts)
  - Increased DNS timeout from 2s to 3s (configurable)
  - Enhanced logging with cache statistics and diagnostics
  - **Critical race condition fix**: Different IPs showed same hostname
    - Root cause: Shared `m_currentIp` variable overwritten in concurrent lookups
    - Solution: Added `QMap<lookupId, IP>` for thread-safe tracking
    - Result: Each hostname correctly associated with its IP
  - **Hostname persistence fix**: Hostnames lost in database
    - Root cause: Port scanning overwrote hostnames with empty strings
    - Solution: Data merging in updateInDatabase() - preserve existing non-empty values
    - Result: All resolved hostnames properly saved and retained
  - Performance: First scan 85-95% recognition, cached scans 98-100%
  - Documentation: Created docs/dns-improvements.md
  - 5 files modified, 4 commits

## [v0.9.4] - Phase 9 Complete (UI Polish & Theming)

### Added - 2025-10-11
- **Phase 9.4 - Localization**
  - LanguageManager singleton for multi-language support
  - Translation files for 5 languages: English, Italian, Spanish, French, German
  - Qt6::LinguistTools integration with CMake
  - Language selection in SettingsDialog with persistent storage
  - Automatic language loading at startup
  - 17 translations per language (MainWindow menus, QualityGauge levels)
  - Post-build commands for .qm file deployment
  - ~545 LOC added

- **Phase 9.3 - UI Enhancements**
  - 10 professional SVG icons with IconLoader utility
  - System Tray integration with notifications and context menu
  - AnimationHelper for smooth UI transitions (fade, slide, expand, bounce)
  - TooltipHelper for rich HTML tooltips
  - ~1,400 LOC added

- **Phase 9.2 - Custom Widgets**
  - QualityGauge: Circular gauge with color-coded arcs and animated needle
  - NetworkActivityIndicator: LED with 3 states (Off/On/Blinking)
  - GradientProgressBar: Smooth color gradients with animations
  - ~600 LOC added

- **Phase 9.1 - Theme System**
  - ThemeManager singleton for application-wide theme control
  - Professional dark.qss and light.qss stylesheets (1,316 LOC)
  - Qt Resource System integration
  - Windows system theme auto-detection via registry
  - Runtime theme switching without restart
  - Theme selection in SettingsDialog with instant preview
  - ThemeManagerTest with 9 test cases
  - ~2,039 LOC added

## [v0.8.5] - Phase 8 Complete (Advanced Features)

### Added - 2025-10-10
- **Phase 8.5 - Settings Dialog**
  - Comprehensive SettingsDialog with 5 tabs (General/Network/Appearance/Notifications/Advanced)
  - QSettings integration for persistent storage (Windows registry)
  - Input validation with user feedback
  - Modified state tracking with Apply button management
  - General: Startup options, language selection
  - Network: Scan configuration, timeout, threads, subnet CIDR
  - Appearance: Theme selection, font size control
  - Notifications: Alert toggles, configurable thresholds
  - Advanced: Database retention, logging configuration
  - ~625 LOC added

- **Phase 8.4 - History & Database**
  - HistoryDao for event persistence with JSON metadata
  - MetricsDao for temporal tracking and statistical aggregation
  - TrendsWidget for metrics visualization with configurable time ranges
  - Query methods: getAverageMetrics(), getMaxLatency(), getMinLatency(), etc.
  - Batch operations with transaction support
  - Database indices for query optimization
  - Unit tests: HistoryDaoTest (11 cases), MetricsDaoTest (13 cases)
  - ~1,433 LOC added

- **Phase 8.3 - Profile & Favorites**
  - ProfileManager extensions: export/import, template profiles
  - Three templates: Home Network (6 ports), Enterprise (25 ports), Security Audit (62 ports)
  - Usage statistics tracking (last used, usage count)
  - ProfileDialog with split view and template buttons
  - FavoritesManager extensions: groups, notes, custom icons
  - FavoritesWidget with tree view and real-time search
  - ~1,850 LOC added

- **Phase 8.2 - Advanced Export (XML, HTML)**
  - XmlExporter with structured XML and QXmlStreamWriter
  - HtmlReportGenerator with professional CSS and responsive design
  - Gradient summary cards and color-coded quality indicators
  - ExportController integration (CSV/JSON/XML/HTML)
  - 14 unit tests
  - ~800 LOC added

- **Phase 8.1 - Wake-on-LAN Support**
  - WakeOnLanService with magic packet builder
  - MAC address validation (XX:XX:XX:XX:XX:XX or XX-XX-XX-XX-XX-XX)
  - UDP broadcast on port 9
  - DeviceTableWidget context menu integration
  - Confirmation dialog with device details
  - Qt signals for async notifications
  - WakeOnLanServiceTest with 12 test cases
  - ~551 LOC added

## [v0.7.4] - Phase 7 Complete (Advanced Diagnostics)

### Added - 2025-10-09
- **Phase 7.4 - Device Detail Dialog**
  - Five-tab interface: Overview, Ports, Metrics, History, Diagnostics
  - MetricsWidget integration for real-time monitoring
  - Historical events with time range filtering
  - All Phase 7 diagnostic tools in one dialog
  - Double-click and context menu support
  - ~1,200 LOC added

- **Phase 7.3 - Monitoring Service**
  - Alert model with severity levels (Info/Warning/Critical)
  - AlertService for alert management and acknowledgment
  - HistoryService for metrics/events persistence
  - MonitoringService for continuous device monitoring
  - Threshold-based alert generation
  - Device status change detection (online/offline)
  - Automatic data pruning (LRU/time-based)
  - 32 unit tests across 3 suites
  - ~1,500 LOC added

- **Phase 7.2 - MTU, Bandwidth, DNS**
  - MtuDiscovery with binary search (576-9000 bytes)
  - BandwidthTester with TCP/UDP support
  - DnsDiagnostics with multiple record types (A/AAAA/MX/NS/TXT/CNAME/PTR/SRV)
  - Custom DNS server support
  - Query time measurement with QElapsedTimer
  - 36 unit tests across 3 suites
  - ~2,671 LOC added

- **Phase 7.1 - Traceroute Service**
  - Cross-platform traceroute (Windows tracert / Linux traceroute)
  - TraceRouteHop model with RTT statistics
  - Asynchronous QProcess execution
  - Real-time hop discovery with Qt signals
  - Platform-specific output parsing
  - 11 unit tests
  - ~1,029 LOC added

### Improved - 2025-10-07
- **Phase 7.0 - Diagnostics Enhancements**
  - MetricsController with device tracking
  - PingService multi-language support (Italian/German/French/Spanish)
  - Enhanced error detection for timeout/unreachable states

## [v0.6.0] - Phase 6 Complete (Charts & Metrics Visualization)

### Added - 2025-10-07
- QtCharts integration with ChartViewModel base class
- LatencyChart with line series (min/avg/max)
- PacketLossChart with bar series and severity colors
- JitterChart with smooth spline series
- MetricsViewModel with timer-based monitoring
- MetricsWidget with Qt Designer UI, summary panel, chart tabs
- Auto-scaling axes with dynamic ranges
- Configurable max data points for memory efficiency

## [v0.5.0] - Phase 5 Complete (UI Foundation & Views)

### Added - 2025-10-04
- MVVM architecture pattern implementation
- Qt Designer UI layouts (mainwindow, device table, scan dialog)
- Main window with menu bar, toolbar, status bar
- Device table widget with sorting and filtering
- Custom delegates (LED status, quality score progress bar)
- Scan configuration dialog with validation
- Context menu with device actions
- Full dependency injection architecture
- Signal/slot connections for async updates

## [v0.4.0] - Phase 4 Complete (Application Layer & Controllers)

### Added - 2025-10-04
- Multi-threaded scan coordination (ScanCoordinator with QThreadPool)
- Thread-safe progress tracking (ProgressTracker with std::atomic)
- Scan workflow controller (quick/deep/custom modes)
- Metrics collection controller
- Export operations controller (CSV/JSON/XML/HTML)
- Scan profile management with JSON persistence
- Favorite devices management with tagging

## [v0.3.0] - Phase 3 Complete (Persistence & Data Management)

### Added - 2025-10-04
- SQLite database with full schema (devices, ports, metrics)
- Repository pattern with CRUD operations
- In-memory LRU cache for performance
- Multi-format export: CSV, JSON, XML, HTML
- Cross-platform settings management (QSettings)
- Configuration validation

## [v0.2.0] - Phase 2 Complete (Metrics & Diagnostics)

### Added - 2025-10-03
- Real-time latency monitoring (min/max/avg/median/stdDev)
- Jitter calculation (standard deviation & consecutive)
- Packet loss detection with burst pattern analysis
- Connection quality scoring (0-100 weighted algorithm)
- Continuous ping monitoring with history
- Port scanning (TCP Quick/Full/Custom modes)
- Metrics aggregation with dependency injection

## [v0.1.0] - Phase 1 Complete (Network Discovery)

### Added - 2025-10-03
- Network device discovery (IP, hostname, MAC)
- Multi-threaded IP range scanning
- Ping-based host discovery (cross-platform)
- DNS reverse lookup with validation
- ARP table parsing with local interface detection
- Service detection (40+ common ports)
- Two scan strategies (Quick & Deep)

### Added - 2025-10-05
- **IEEE OUI Database Integration**
  - 38,169+ manufacturer vendor identification
  - Locally Administered Address (LAA) detection
  - VM/Container/VPN device identification
  - Singleton pattern for efficient loading
  - Fallback database with 36 common vendors

- **DNS and ARP Improvements**
  - Fixed DNS hostname validation
  - Local interface MAC detection
  - IP-as-hostname validation logic

## [v0.0.1] - Phase 0 Complete (Project Foundation)

### Added - 2025-10-03
- Project infrastructure and build system
- CMake configuration with Qt 6.x
- C++17 standard setup
- Cross-platform support (Windows/Linux/macOS)
- Qt Test framework integration
- Repository structure and documentation
- Git workflow and branch strategy
- Initial README and PROJECT documentation
