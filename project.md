# LanScan - Network Scanner and Diagnostic Tool

## Project Overview
Cross-platform desktop application developed in C++ with Qt for LAN scanning, monitoring, and network diagnostics.

## Key Features

### Network Discovery
- **IP Range Scanning**: CIDR notation support (e.g., 192.168.1.0/24)
- **Device Detection**: Automatic hostname and MAC address resolution
- **Vendor Identification**: IEEE OUI database integration
- **Port Scanning**: Quick scan for common services or custom port ranges

### Performance Metrics
- **Real-time Latency**: Continuous ping monitoring with statistical analysis
- **Jitter Measurement**: Network stability assessment through RTT variance
- **Packet Loss Detection**: Connection quality evaluation and alerting
- **Quality Scoring**: Automated network performance rating system

### Diagnostic Tools
- **Continuous Monitoring**: Real-time network change detection
- **Traceroute**: Visual hop-by-hop path analysis with latency per hop
- **Path MTU Discovery**: Maximum transmission unit detection
- **DNS Diagnostics**: Lookup times and multi-server testing

### Advanced Features
- **Wake-on-LAN**: Remote device wake-up functionality
- **Export Capabilities**: CSV, JSON, XML, and HTML report generation
- **Profiles & Favorites**: Save scan configurations and device collections
- **Dark/Light Themes**: Modern UI with customizable appearance

## Technology Stack

### Core Technologies
- **Framework**: Qt 6.x (Widgets + Network + Charts)
- **Language**: C++17 or higher
- **Build System**: CMake
- **UI Design**: Qt Designer (.ui files)

### Optional Libraries
- **libpcap/WinPcap**: Advanced packet capture capabilities
- **Boost.Asio**: Asynchronous networking operations

## Architecture

The project follows **Single Responsibility Principle (SRP)** with a clean layered architecture:

### Layer Structure
- **Presentation Layer**: Qt Widgets UI with MVVM pattern
- **Application Layer**: Controllers and service orchestration
- **Domain Layer**: Core business logic and metrics calculation
- **Infrastructure Layer**: Network operations and data persistence

### Key Design Patterns
- **Repository Pattern**: Data access abstraction
- **Strategy Pattern**: Interchangeable scanning algorithms
- **Observer Pattern**: Event-driven notifications
- **Dependency Injection**: Loose coupling between components

For detailed architectural information, see the [Architecture Overview](docs/architecture-overview.md).

## Development Roadmap

The project is structured in 12 development phases:

### Foundation Phases (0-2)
- **[Phase 0](docs/phases/phase-0-foundation-infrastructure.md)**: Project setup and infrastructure
- **[Phase 1](docs/phases/phase-1-network-discovery.md)**: Network layer and core discovery
- **[Phase 2](docs/phases/phase-2-metrics-diagnostics.md)**: Metrics and diagnostics core

### Core Development (3-6)
- **[Phase 3](docs/phases/phase-3-persistence-data-management.md)**: Persistence and data management
- **[Phase 4](docs/phases/phase-4-application-controllers.md)**: Application layer and controllers
- **[Phase 5](docs/phases/phase-5-ui-foundation-views.md)**: UI foundation and basic views
- **[Phase 6](docs/phases/phase-6-charts-visualization.md)**: Charts and metrics visualization

### Advanced Features (7-9)
- **[Phase 7](docs/phases/phase-7-advanced-diagnostics.md)**: Advanced diagnostics
- **[Phase 8](docs/phases/phase-8-advanced-features.md)**: Advanced features (WoL, export, profiles)
- **[Phase 9](docs/phases/phase-9-ui-polish-theming.md)**: UI polish and theming

### Quality & Release (10-12)
- **[Phase 10](docs/phases/phase-10-testing-quality-assurance.md)**: Testing and quality assurance
- **[Phase 11](docs/phases/phase-11-documentation-release.md)**: Documentation and release
- **[Phase 12](docs/phases/phase-12-post-release-maintenance.md)**: Post-release and maintenance

## Quick Start

### Prerequisites
- Qt 6.2 or higher
- CMake 3.16 or higher
- C++17 compatible compiler
- Administrative privileges (for raw socket operations)

### Building from Source
```bash
git clone https://github.com/username/LanScan.git
cd LanScan
mkdir build && cd build
cmake ..
cmake --build .
```

### System Requirements
- **Windows**: Windows 10/11
- **Linux**: Ubuntu 20.04+ or equivalent
- **Memory**: 512 MB RAM minimum
- **Disk Space**: 100 MB available space

## Documentation

### Development Documentation
- **[Architecture Overview](docs/architecture-overview.md)**: System design and patterns
- **[Development Guide](docs/development-guide.md)**: Setup and contribution guidelines
- **[API Reference](docs/api-reference.md)**: Generated API documentation
- **[Testing Guide](docs/testing-guide.md)**: Testing strategies and frameworks

### User Documentation
- **[User Manual](docs/user-manual.md)**: Comprehensive usage guide
- **[Quick Start Guide](docs/quick-start.md)**: Get started in 5 minutes
- **[FAQ](docs/faq.md)**: Frequently asked questions
- **[Troubleshooting](docs/troubleshooting.md)**: Common issues and solutions

## Project Management

### Git Workflow
The project uses GitFlow with the following branch structure:
- `main`: Production-ready releases
- `develop`: Integration branch for features
- `feature/phase-X-description`: Individual feature development
- `release/vX.Y.Z`: Release preparation
- `hotfix/description`: Critical bug fixes

### Task Management
Development tasks are tracked using a simple TODO.md system organized by development phases. See [TODO.md](TODO.md) for current progress.

### Quality Standards
- **Code Coverage**: Minimum 85% for unit tests
- **Documentation**: All public APIs must be documented
- **Code Review**: All changes require peer review
- **Testing**: Comprehensive unit and integration test suites

## Security Considerations

### Network Operations
- Input validation for all network parameters
- Rate limiting for scanning operations
- Secure handling of credentials
- Compliance with network security policies

### Privilege Management
- Administrative privileges required for raw socket operations
- Clear documentation of permission requirements
- Graceful degradation when privileges unavailable

## Contributing

We welcome contributions! Please see our [Contributing Guidelines](CONTRIBUTING.md) for:
- Code style and standards
- Development setup instructions
- Pull request process
- Issue reporting guidelines

## License

This project is licensed under the **MIT License** - see the [LICENSE](LICENSE) file for details.

## Support

- **Issues**: [GitHub Issues](https://github.com/username/LanScan/issues)
- **Discussions**: [GitHub Discussions](https://github.com/username/LanScan/discussions)
- **Documentation**: [Project Wiki](https://github.com/username/LanScan/wiki)

## Roadmap

### v1.0.0 (Initial Release)
- Core network scanning and metrics
- Basic UI with real-time charts
- Export capabilities
- Cross-platform support

### v1.1.0 (Near-term)
- Network topology visualization
- Enhanced monitoring and alerting
- Improved user experience
- Performance optimizations

### v2.0.0 (Long-term)
- Plugin architecture
- Web-based interface
- Cloud integration
- Enterprise features

For detailed feature planning, see individual phase documents in `docs/phases/`.

---

**LanScan aims to be the premier open-source network scanning and diagnostic tool, combining professional-grade features with an intuitive user interface and robust architecture.**