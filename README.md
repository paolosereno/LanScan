# LanScan - Network Scanner and Diagnostic Tool

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![Qt](https://img.shields.io/badge/Qt-6.9.1-brightgreen.svg)](https://www.qt.io/)
[![CMake](https://img.shields.io/badge/CMake-3.16+-064F8C.svg)](https://cmake.org/)
[![Build](https://img.shields.io/badge/build-passing-brightgreen.svg)](https://github.com/paolosereno/LanScan)
[![Tests](https://img.shields.io/badge/tests-21%2F21%20passing-brightgreen.svg)](https://github.com/paolosereno/LanScan)

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

**Advanced Diagnostics** (Phase 7 - In Progress)
- ✅ MetricsWidget integration into MainWindow with dock widget
- ✅ Enhanced MetricsController with proper device tracking
- ✅ Improved metrics collection with device context emission
- ✅ Multi-language ping parsing (Italian, German, French, Spanish)
- ✅ Enhanced error detection for timeout/unreachable states
- 🚧 Traceroute service implementation (in progress)

### 🚧 Planned (Phase 7+)
- Advanced diagnostics (MTU discovery, bandwidth test, DNS diagnostics)
- UI polish with dark/light themes
- Historical data tracking with trend analysis
- Network topology visualization
- Wake-on-LAN support
- Chart export functionality
- Monitoring service with alerts

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

**Current Phase**: Phase 7 - Advanced Diagnostics 🚧 **IN PROGRESS**
**Next Phase**: Phase 8 - Advanced Features
**Progress**: 63% (6 complete + Phase 7 20%)
**Latest Release**: [v0.6.0-phase6](https://github.com/paolosereno/LanScan/releases/tag/v0.6.0-phase6) (pending)

### Recent Updates
- **2025-10-07**: Phase 7.0 progress - Advanced diagnostics improvements
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
- **Files Created**: 179 total (Phase 7.0: +4 modified files)
- **Lines of Code**: ~17,000+ (+300 in Phase 7.0 improvements)
- **Test Coverage**: 21/21 tests passing (100%)
- **Build Time**: ~40-45 seconds (Release, 12 cores)
- **Executable Size**: 39 MB (Release build)

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Contributors

Paolo - Lead Developer
