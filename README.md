# LanScan - Network Scanner and Diagnostic Tool

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![Qt](https://img.shields.io/badge/Qt-6.9.1-brightgreen.svg)](https://www.qt.io/)
[![CMake](https://img.shields.io/badge/CMake-3.16+-064F8C.svg)](https://cmake.org/)
[![Build](https://img.shields.io/badge/build-passing-brightgreen.svg)](https://github.com/paolosereno/LanScan)
[![Tests](https://img.shields.io/badge/tests-28%20total-brightgreen.svg)](https://github.com/paolosereno/LanScan)

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
- ✅ CSV export functionality
- ✅ JSON export functionality
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
- ✅ **Device Detail Dialog with integrated diagnostics** (Phase 7.4)
  - Five-tab interface (Overview, Ports, Metrics, History, Diagnostics)
  - Integrated MetricsWidget for real-time monitoring
  - Historical events with time range filtering (Last Hour/6H/24H/7D/30D)
  - All Phase 7 diagnostic tools integrated (Traceroute, MTU, Bandwidth, DNS)
  - Double-click and context menu support for opening dialog
  - 3 new files: ~1,200 LOC

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

### 🚧 Planned (Phase 8+)
- Advanced export functionality (XML/HTML) (Phase 8.2)
- Scan profiles and favorites integration (Phase 8.3)
- History & database enhancement (Phase 8.4)
- Settings dialog with preferences (Phase 8.5)
- UI polish with dark/light themes (Phase 9)
- Network topology visualization (Phase 10)
- Chart export functionality (Phase 11)
- Advanced security scanning (Phase 12)

## Technology Stack

- **Framework**: Qt 6.x (Widgets, Network, Charts)
- **Language**: C++ 17
- **Build System**: CMake
- **Architecture**: SRP-compliant with MVVM pattern
- **Testing**: Qt Test framework

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

**Current Phase**: Phase 8 - Advanced Features 🚧 **IN PROGRESS** (1/5 modules - 20%)
**Next Phase**: Phase 9 - UI Polish & Theming
**Progress**: 77% (~7.2 complete phases of ~12 total)
**Latest Release**: [v0.8.1-phase8.1](https://github.com/paolosereno/LanScan/releases/tag/v0.8.1-phase8.1)

### Recent Updates
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
- **Files Created**: 214 total (Phase 7 completed: 25 new files total)
- **Lines of Code**: ~23,400+ lines (Phase 7.4 added final ~1,200 LOC)
- **Test Coverage**: 60+ tests across all test suites (Phase 7 added comprehensive diagnostic tests)
- **Build Time**: ~50-55 seconds (Debug, 12 cores)
- **Executable Size**: 48 MB (Debug build with all Phase 7 features)

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Contributors

Paolo - Lead Developer
