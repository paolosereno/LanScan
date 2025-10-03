# LanScan - Network Scanner and Diagnostic Tool

Network scanner application with advanced diagnostics and metrics visualization.

## Features

- Network device discovery (IP, hostname, MAC)
- Real-time latency, jitter, and packet loss monitoring
- Port scanning
- Advanced diagnostics (traceroute, MTU discovery, bandwidth test)
- Modern Qt6 GUI with dark/light themes
- Export to CSV/JSON/XML/HTML

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

```bash
# Clone repository
git clone https://github.com/paolosereno/LanScan.git
cd LanScan

# Create build directory
mkdir build
cd build

# Configure
cmake ..

# Build
cmake --build .

# Run
./LanScan  # or LanScan.exe on Windows
```

## Development

- See [project.md](project.md) for detailed architecture and development roadmap
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

**Current Phase**: Fase 0 - Foundation & Infrastructure
**Sprint**: Sprint 1 (Week 1-2)

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Contributors

Paolo - Lead Developer
