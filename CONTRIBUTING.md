# Contributing to LanScan

Thank you for your interest in contributing to LanScan! This document provides guidelines and instructions for contributing to the project.

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [Getting Started](#getting-started)
- [Development Setup](#development-setup)
- [Development Workflow](#development-workflow)
- [Coding Standards](#coding-standards)
- [Testing Guidelines](#testing-guidelines)
- [Commit Convention](#commit-convention)
- [Pull Request Process](#pull-request-process)
- [Project Structure](#project-structure)

## Code of Conduct

- Be respectful and inclusive
- Provide constructive feedback
- Focus on what is best for the community
- Show empathy towards other community members

## Getting Started

1. **Fork the repository** on GitHub
2. **Clone your fork** locally:
   ```bash
   git clone https://github.com/YOUR_USERNAME/LanScan.git
   cd LanScan
   ```
3. **Add upstream remote**:
   ```bash
   git remote add upstream https://github.com/paolosereno/LanScan.git
   ```

## Development Setup

### Prerequisites

- **Qt 6.2+** - Download from [qt.io](https://www.qt.io/download)
- **CMake 3.16+** - Build system
- **C++17 compatible compiler**:
  - MSVC 2019+ (Windows)
  - GCC 9+ (Linux)
  - Clang 10+ (macOS)

### Build Instructions

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Build the project
cmake --build .

# Run tests
ctest --output-on-failure

# Run the application
./LanScan  # or LanScan.exe on Windows
```

## Development Workflow

### Branch Strategy

- **`main`** - Production releases only
- **`develop`** - Active development branch
- **`feature/phaseX-description`** - New features (X = phase number 0-11)
- **`bugfix/description`** - Bug fixes
- **`hotfix/description`** - Critical production fixes

### Creating a Feature Branch

```bash
# Update develop branch
git checkout develop
git pull upstream develop

# Create feature branch
git checkout -b feature/phase1-network-discovery

# Make your changes...

# Push to your fork
git push origin feature/phase1-network-discovery
```

## Coding Standards

### C++ Style Guide

- **Standard**: C++17 or higher
- **Naming Conventions**:
  - Classes: `PascalCase` (e.g., `DeviceScanner`)
  - Functions/Methods: `camelCase` (e.g., `scanNetwork()`)
  - Variables: `camelCase` (e.g., `deviceList`)
  - Constants: `UPPER_CASE` (e.g., `MAX_THREADS`)
  - Private members: prefix with `m_` (e.g., `m_deviceList`)

- **Architecture**:
  - Follow **Single Responsibility Principle (SRP)**
  - Use **MVVM pattern** for UI components
  - Implement **Interface Segregation** for core services
  - Apply **Dependency Injection** where appropriate

- **Code Quality**:
  - Keep functions small and focused
  - Use smart pointers (`std::unique_ptr`, `std::shared_ptr`)
  - Prefer `const` correctness
  - Avoid raw `new`/`delete`
  - Use Qt containers when working with Qt APIs

### Qt-Specific Guidelines

- Use **Qt Designer (.ui files)** for UI layout
- Prefer **signals/slots** for event handling
- Use **Qt's parent-child ownership** for memory management
- Follow Qt naming conventions for signals/slots

## Testing Guidelines

### Unit Tests

- Write tests using **Qt Test framework**
- Place tests in corresponding `tests/` subdirectories
- Naming: `ClassNameTest.cpp` (e.g., `DeviceTest.cpp`)
- Test coverage expectations:
  - Models: 100%
  - Utils: 90%+
  - Services: 80%+
  - Controllers: 70%+

### Test Structure

```cpp
#include <QtTest>
#include "Device.h"

class DeviceTest : public QObject {
    Q_OBJECT
private slots:
    void initTestCase();
    void testDeviceCreation();
    void testIpAddressValidation();
    void cleanupTestCase();
};
```

### Running Tests

```bash
cd build
ctest --output-on-failure
```

## Commit Convention

Use the following format for commit messages:

```
[Phase X] Brief description

Detailed description of changes.
Include reasoning and impact.

Location: src/path/to/modified/files
```

### Examples:

```
[Phase 0] Add Device model class

Implement core Device entity with IP, hostname, MAC address properties.
Includes validation logic and getter/setter methods.

Location: src/models/Device.h, src/models/Device.cpp
```

```
[Phase 2] Implement LatencyCalculator service

Add calculator for min/avg/max/median latency metrics.
Supports continuous ping data aggregation.

Location: src/services/metrics/LatencyCalculator.cpp
```

### Commit Best Practices

- Keep commits atomic and focused
- Write clear, descriptive messages
- Reference issue numbers when applicable (e.g., `Fixes #42`)
- Don't commit commented-out code
- Don't commit debug prints or temporary files

## Pull Request Process

### Before Submitting

1. **Sync with upstream**:
   ```bash
   git checkout develop
   git pull upstream develop
   git checkout your-feature-branch
   git rebase develop
   ```

2. **Run tests**:
   ```bash
   cd build
   cmake --build .
   ctest --output-on-failure
   ```

3. **Check code style** (ensure consistency with existing code)

### Submitting PR

1. **Push to your fork**:
   ```bash
   git push origin your-feature-branch
   ```

2. **Create Pull Request** on GitHub:
   - Base branch: `develop`
   - Compare branch: `your-feature-branch`
   - Fill in PR template with:
     - Description of changes
     - Related phase/sprint
     - Testing performed
     - Screenshots (if UI changes)

3. **PR Title Format**:
   ```
   [Phase X] Brief description of changes
   ```

### PR Review Process

- At least one approval required
- All tests must pass
- Code must follow project standards
- Documentation updated if needed
- No merge conflicts

### After PR is Merged

```bash
# Update your local develop branch
git checkout develop
git pull upstream develop

# Delete feature branch
git branch -d your-feature-branch
git push origin --delete your-feature-branch
```

## Project Structure

```
LanScan/
├── src/
│   ├── models/           # Data models (Device, NetworkMetrics, etc.)
│   ├── utils/            # Utilities (Logger, Validators, Formatters)
│   ├── interfaces/       # Abstract interfaces
│   ├── services/         # Core services (Scanning, Metrics, Network)
│   │   ├── network/      # Network layer services
│   │   ├── metrics/      # Metrics calculators
│   │   └── diagnostics/  # Diagnostic tools
│   ├── controllers/      # Application controllers
│   ├── viewmodels/       # MVVM ViewModels
│   ├── views/            # Qt UI views
│   └── main.cpp
├── tests/                # Unit tests (mirrors src/ structure)
├── resources/            # Qt resources, icons, qrc files
├── docs/                 # Documentation
└── CMakeLists.txt
```

### Current Development Phase

**Phase 0: Foundation & Infrastructure (Weeks 1-2)**
- Focus: Models, Utils, Interfaces, base architecture
- See [TODO.md](TODO.md) for current task list
- See [project.md](project.md) for detailed roadmap

## Questions or Issues?

- Check existing [Issues](https://github.com/paolosereno/LanScan/issues)
- Create a new issue with:
  - Clear description
  - Steps to reproduce (for bugs)
  - Expected vs actual behavior
  - Environment details (OS, Qt version, compiler)

## License

By contributing, you agree that your contributions will be licensed under the MIT License.

---

**Happy Coding!** 🚀
