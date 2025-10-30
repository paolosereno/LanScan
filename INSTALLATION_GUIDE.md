# LanScan Installation Guide
## Complete Build and Installation Instructions

**Version**: 1.0.0
**Last Updated**: 2025-10-30
**Target Audience**: End Users, Developers, System Administrators

---

## Table of Contents

1. [System Requirements](#1-system-requirements)
2. [Prerequisites](#2-prerequisites)
3. [Windows Installation](#3-windows-installation)
4. [Linux Installation](#4-linux-installation)
5. [macOS Installation](#5-macos-installation)
6. [Build Options](#6-build-options)
7. [Post-Installation](#7-post-installation)
8. [Troubleshooting](#8-troubleshooting)
9. [Uninstallation](#9-uninstallation)

---

## 1. System Requirements

### 1.1 Minimum Requirements

| Component | Requirement |
|-----------|-------------|
| **Operating System** | Windows 10/11, Linux (Ubuntu 20.04+, Fedora 35+, Arch), macOS 11+ |
| **CPU** | Dual-core processor (Intel Core i3 or equivalent) |
| **RAM** | 2 GB |
| **Disk Space** | 200 MB free space (1 GB for development) |
| **Display** | 1024x768 minimum resolution |
| **Network** | Ethernet or Wi-Fi adapter |

### 1.2 Recommended Requirements

| Component | Recommendation |
|-----------|----------------|
| **CPU** | Quad-core processor or better (Intel Core i5/i7, AMD Ryzen 5/7) |
| **RAM** | 4 GB or more |
| **Disk Space** | 2 GB free space (SSD recommended for database) |
| **Display** | 1920x1080 or higher |
| **Network** | Gigabit Ethernet for best performance |

### 1.3 Supported Platforms

**Windows:**
- Windows 10 (64-bit) - Build 1809 or later
- Windows 11 (64-bit) - All builds
- Windows Server 2019/2022

**Linux:**
- Ubuntu 20.04 LTS, 22.04 LTS, 24.04 LTS
- Fedora 35, 36, 37, 38, 39
- Arch Linux (rolling release)
- Debian 11 (Bullseye), 12 (Bookworm)
- openSUSE Leap 15.4+, Tumbleweed
- CentOS Stream 9, RHEL 9

**macOS:**
- macOS Big Sur 11.x
- macOS Monterey 12.x
- macOS Ventura 13.x
- macOS Sonoma 14.x

---

## 2. Prerequisites

### 2.1 Required Software

**All Platforms:**
- **Qt 6.2+** (Qt 6.9.1 recommended)
- **CMake 3.16+** (CMake 3.27+ recommended)
- **C++17 Compiler**:
  - Windows: MSVC 2019+ or MinGW 11+
  - Linux: GCC 9+ or Clang 10+
  - macOS: Xcode 12+ (Apple Clang)
- **Git** (for cloning repository)

**Platform-Specific:**

**Windows:**
- **npcap** or **WinPcap** (for packet capture, optional)
- **Visual Studio 2019/2022** (for MSVC builds) or **MinGW-w64**

**Linux:**
- **net-tools** (for `arp` command)
- **traceroute** (for traceroute diagnostics)
- **iputils-ping** (usually pre-installed)
- **libgl1-mesa-dev** (OpenGL for Qt rendering)

**macOS:**
- **Xcode Command Line Tools**
- **Homebrew** (recommended for package management)

### 2.2 Qt Installation

#### Windows

**Option A: Qt Online Installer (Recommended)**

1. Download Qt Online Installer from [https://www.qt.io/download-qt-installer](https://www.qt.io/download-qt-installer)

2. Run the installer:
   ```
   qt-unified-windows-x64-online.exe
   ```

3. Create/login to Qt Account

4. Select Components:
   - ✅ Qt 6.9.1 (or latest 6.x)
     - ✅ MSVC 2022 64-bit
     - ✅ MinGW 11.2.0 64-bit (optional)
     - ✅ Qt Charts
     - ✅ Qt Network Authorization
     - ✅ Additional Libraries
   - ✅ Qt Creator (optional, recommended for development)
   - ✅ CMake (if not already installed)
   - ✅ Ninja (optional build tool)

5. Installation path: `C:\Qt\6.9.1` (default)

6. Complete installation (~8 GB)

**Option B: Qt Offline Installer**

Download specific packages from [https://www.qt.io/offline-installers](https://www.qt.io/offline-installers)

#### Linux

**Ubuntu/Debian:**
```bash
# Add Qt PPA (if needed for latest version)
sudo add-apt-repository ppa:okirby/qt6-backports
sudo apt update

# Install Qt 6 packages
sudo apt install -y \
    qt6-base-dev \
    qt6-charts-dev \
    qt6-svg-dev \
    qt6-tools-dev \
    qt6-l10n-tools \
    libqt6sql6-sqlite \
    qmake6 \
    cmake \
    build-essential \
    git

# Install network tools
sudo apt install -y net-tools traceroute iputils-ping

# Install OpenGL libraries
sudo apt install -y libgl1-mesa-dev libglu1-mesa-dev
```

**Fedora/RHEL/CentOS:**
```bash
# Install Qt 6 packages
sudo dnf install -y \
    qt6-qtbase-devel \
    qt6-qtcharts-devel \
    qt6-qtsvg-devel \
    qt6-qttools-devel \
    qt6-linguist \
    cmake \
    gcc-c++ \
    git

# Install network tools
sudo dnf install -y net-tools traceroute iputils

# Install OpenGL libraries
sudo dnf install -y mesa-libGL-devel mesa-libGLU-devel
```

**Arch Linux:**
```bash
# Install Qt 6 packages
sudo pacman -S --needed \
    qt6-base \
    qt6-charts \
    qt6-svg \
    qt6-tools \
    cmake \
    base-devel \
    git

# Install network tools
sudo pacman -S --needed net-tools traceroute iputils
```

#### macOS

**Using Homebrew (Recommended):**
```bash
# Install Homebrew (if not already installed)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install Qt 6
brew install qt@6

# Link Qt to PATH
echo 'export PATH="/usr/local/opt/qt@6/bin:$PATH"' >> ~/.zshrc
source ~/.zshrc

# Install CMake
brew install cmake

# Install Git (if not already installed)
brew install git
```

**Using Official Qt Installer:**
1. Download Qt Online Installer for macOS
2. Follow same steps as Windows
3. Installation path: `/Users/<username>/Qt/6.9.1`

### 2.3 CMake Installation

**Windows:**
- Download from [https://cmake.org/download/](https://cmake.org/download/)
- Run installer: `cmake-3.27.x-windows-x86_64.msi`
- ✅ Add CMake to system PATH for all users

**Linux:**
```bash
# Ubuntu/Debian
sudo apt install cmake

# Fedora/RHEL
sudo dnf install cmake

# Arch
sudo pacman -S cmake
```

**macOS:**
```bash
brew install cmake
```

**Verify Installation:**
```bash
cmake --version
# Should output: cmake version 3.27.x or higher
```

### 2.4 Compiler Setup

**Windows - MSVC:**
1. Install Visual Studio 2019 or 2022 (Community Edition is free)
2. Select workloads:
   - ✅ Desktop development with C++
   - ✅ C++ CMake tools for Windows
3. Verify:
   ```cmd
   cl
   # Should show Microsoft (R) C/C++ Optimizing Compiler
   ```

**Windows - MinGW:**
1. Included with Qt installer (MinGW 11.2.0 64-bit)
2. Or download standalone from [https://www.mingw-w64.org/](https://www.mingw-w64.org/)
3. Add to PATH: `C:\Qt\Tools\mingw1120_64\bin`
4. Verify:
   ```cmd
   g++ --version
   # Should show: g++.exe (MinGW-W64) 11.2.0
   ```

**Linux:**
```bash
# Ubuntu/Debian
sudo apt install build-essential g++

# Fedora/RHEL
sudo dnf install gcc-c++

# Arch
sudo pacman -S base-devel

# Verify
g++ --version
```

**macOS:**
```bash
# Install Xcode Command Line Tools
xcode-select --install

# Verify
clang++ --version
```

---

## 3. Windows Installation

### 3.1 Clone Repository

```cmd
# Open Command Prompt or PowerShell
cd C:\Projects

# Clone repository
git clone https://github.com/paolosereno/LanScan.git
cd LanScan
```

### 3.2 Build with MinGW (Recommended for End Users)

**Step 1: Configure Build**
```cmd
# Create build directory
mkdir build
cd build

# Configure with CMake (MinGW)
cmake -G "MinGW Makefiles" ^
      -DCMAKE_BUILD_TYPE=Release ^
      -DCMAKE_PREFIX_PATH="C:/Qt/6.9.1/mingw_64" ^
      ..
```

**Step 2: Build**
```cmd
# Build with all CPU cores (adjust -j12 to your core count)
cmake --build . --config Release -j12
```

**Expected Output:**
```
[  1%] Building CXX object CMakeFiles/LanScan.dir/src/main.cpp.obj
[  2%] Building CXX object CMakeFiles/LanScan.dir/src/models/Device.cpp.obj
...
[100%] Linking CXX executable LanScan.exe
[100%] Built target LanScan
```

**Build Time:** ~2-5 minutes (depending on CPU)

**Step 3: Deploy Qt Libraries**
```cmd
# Deploy Qt DLLs to build directory
C:\Qt\6.9.1\mingw_64\bin\windeployqt.exe ^
    --release ^
    --no-compiler-runtime ^
    LanScan.exe
```

**Step 4: Run Application**
```cmd
# Run from build directory
.\LanScan.exe
```

### 3.3 Build with MSVC (Recommended for Developers)

**Step 1: Open Developer Command Prompt**
- Start → Visual Studio 2022 → Developer Command Prompt for VS 2022

**Step 2: Configure Build**
```cmd
cd C:\Projects\LanScan
mkdir build-msvc
cd build-msvc

cmake -G "Visual Studio 17 2022" ^
      -A x64 ^
      -DCMAKE_PREFIX_PATH="C:/Qt/6.9.1/msvc2022_64" ^
      ..
```

**Step 3: Build**
```cmd
# Release build
cmake --build . --config Release -j12

# OR Debug build
cmake --build . --config Debug -j12
```

**Step 4: Deploy and Run**
```cmd
cd Release
C:\Qt\6.9.1\msvc2022_64\bin\windeployqt.exe LanScan.exe
.\LanScan.exe
```

### 3.4 Building Tests (Optional)

```cmd
# Configure with tests enabled
cmake -G "MinGW Makefiles" ^
      -DCMAKE_BUILD_TYPE=Debug ^
      -DCMAKE_PREFIX_PATH="C:/Qt/6.9.1/mingw_64" ^
      -DBUILD_TESTS=ON ^
      ..

# Build
cmake --build . -j12

# Run tests
ctest --output-on-failure --parallel 4
```

### 3.5 Creating Installer (Advanced)

**Using NSIS:**
1. Install NSIS from [https://nsis.sourceforge.io/](https://nsis.sourceforge.io/)
2. Create `installer.nsi` script:
   ```nsis
   !define APP_NAME "LanScan"
   !define VERSION "1.0.0"

   OutFile "LanScan-Setup-1.0.0.exe"
   InstallDir "$PROGRAMFILES64\LanScan"

   Section "Install"
       SetOutPath $INSTDIR
       File /r "build\Release\*.*"
       CreateShortcut "$DESKTOP\LanScan.lnk" "$INSTDIR\LanScan.exe"
   SectionEnd
   ```
3. Compile: `makensis installer.nsi`

---

## 4. Linux Installation

### 4.1 Ubuntu/Debian

**Step 1: Install Dependencies**
```bash
# Install build dependencies
sudo apt update
sudo apt install -y \
    qt6-base-dev \
    qt6-charts-dev \
    qt6-svg-dev \
    qt6-tools-dev \
    libqt6sql6-sqlite \
    cmake \
    build-essential \
    git \
    net-tools \
    traceroute \
    iputils-ping

# Install OpenGL libraries (required for Qt rendering)
sudo apt install -y libgl1-mesa-dev libglu1-mesa-dev
```

**Step 2: Clone Repository**
```bash
cd ~/Projects
git clone https://github.com/paolosereno/LanScan.git
cd LanScan
```

**Step 3: Build**
```bash
# Create build directory
mkdir build && cd build

# Configure
cmake -DCMAKE_BUILD_TYPE=Release ..

# Build with all cores
cmake --build . -j$(nproc)
```

**Step 4: Install (Optional)**
```bash
# Install system-wide (requires sudo)
sudo cmake --install .

# OR run from build directory
./LanScan
```

**Step 5: Create Desktop Entry**
```bash
# Create application menu entry
sudo tee /usr/share/applications/lanscan.desktop > /dev/null <<EOF
[Desktop Entry]
Type=Application
Name=LanScan
Comment=Network Scanner and Diagnostic Tool
Exec=/usr/local/bin/LanScan
Icon=/usr/local/share/icons/lanscan.png
Terminal=false
Categories=Network;Utility;
EOF

# Update desktop database
sudo update-desktop-database
```

### 4.2 Fedora/RHEL/CentOS

**Step 1: Install Dependencies**
```bash
# Install build dependencies
sudo dnf install -y \
    qt6-qtbase-devel \
    qt6-qtcharts-devel \
    qt6-qtsvg-devel \
    qt6-qttools-devel \
    cmake \
    gcc-c++ \
    git \
    net-tools \
    traceroute \
    iputils

# Install OpenGL libraries
sudo dnf install -y mesa-libGL-devel mesa-libGLU-devel
```

**Step 2: Build and Install**
```bash
cd ~/Projects
git clone https://github.com/paolosereno/LanScan.git
cd LanScan

mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . -j$(nproc)

# Install
sudo cmake --install .
```

### 4.3 Arch Linux

**Step 1: Install Dependencies**
```bash
# Install build dependencies
sudo pacman -S --needed \
    qt6-base \
    qt6-charts \
    qt6-svg \
    qt6-tools \
    cmake \
    base-devel \
    git \
    net-tools \
    traceroute \
    iputils
```

**Step 2: Build and Install**
```bash
cd ~/Projects
git clone https://github.com/paolosereno/LanScan.git
cd LanScan

mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . -j$(nproc)

# Install
sudo cmake --install .
```

### 4.4 Creating .deb Package (Ubuntu/Debian)

```bash
# Install packaging tools
sudo apt install -y debhelper dh-make

# Create Debian package structure
cd LanScan
mkdir -p debian

# Create control file
cat > debian/control <<EOF
Source: lanscan
Section: net
Priority: optional
Maintainer: Paolo <paolo@example.com>
Build-Depends: debhelper (>= 11), cmake, qt6-base-dev, qt6-charts-dev
Standards-Version: 4.5.0

Package: lanscan
Architecture: amd64
Depends: \${shlibs:Depends}, \${misc:Depends}, libqt6core6, libqt6gui6, libqt6widgets6
Description: Network Scanner and Diagnostic Tool
 LanScan is a powerful cross-platform network scanner with advanced diagnostics.
EOF

# Build package
dpkg-buildpackage -us -uc

# Install
sudo dpkg -i ../lanscan_1.0.0_amd64.deb
```

### 4.5 Creating .rpm Package (Fedora/RHEL)

```bash
# Install packaging tools
sudo dnf install -y rpm-build rpmdevtools

# Create RPM build structure
rpmdev-setuptree

# Create spec file
cat > ~/rpmbuild/SPECS/lanscan.spec <<EOF
Name:           lanscan
Version:        1.0.0
Release:        1%{?dist}
Summary:        Network Scanner and Diagnostic Tool
License:        MIT
URL:            https://github.com/paolosereno/LanScan
Source0:        lanscan-1.0.0.tar.gz

BuildRequires:  cmake qt6-qtbase-devel qt6-qtcharts-devel
Requires:       qt6-qtbase qt6-qtcharts

%description
LanScan is a powerful cross-platform network scanner with advanced diagnostics.

%build
cmake -DCMAKE_BUILD_TYPE=Release .
make %{?_smp_mflags}

%install
make install DESTDIR=%{buildroot}

%files
/usr/local/bin/LanScan

%changelog
* Thu Oct 30 2025 Paolo <paolo@example.com> - 1.0.0-1
- Initial release
EOF

# Build RPM
rpmbuild -ba ~/rpmbuild/SPECS/lanscan.spec

# Install
sudo rpm -i ~/rpmbuild/RPMS/x86_64/lanscan-1.0.0-1.x86_64.rpm
```

---

## 5. macOS Installation

### 5.1 Using Homebrew (Recommended)

**Step 1: Install Dependencies**
```bash
# Install Homebrew (if not already installed)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install Qt 6 and CMake
brew install qt@6 cmake git

# Add Qt to PATH
echo 'export PATH="/usr/local/opt/qt@6/bin:$PATH"' >> ~/.zshrc
source ~/.zshrc
```

**Step 2: Clone and Build**
```bash
cd ~/Projects
git clone https://github.com/paolosereno/LanScan.git
cd LanScan

mkdir build && cd build

# Configure with Homebrew Qt
cmake -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_PREFIX_PATH="/usr/local/opt/qt@6" \
      ..

# Build
cmake --build . -j$(sysctl -n hw.ncpu)
```

**Step 3: Create .app Bundle**
```bash
# Deploy Qt frameworks
/usr/local/opt/qt@6/bin/macdeployqt LanScan.app

# OR create DMG installer
/usr/local/opt/qt@6/bin/macdeployqt LanScan.app -dmg
```

**Step 4: Install**
```bash
# Copy to Applications
cp -r LanScan.app /Applications/

# Run
open /Applications/LanScan.app
```

### 5.2 Using Official Qt Installer

**Step 1: Install Qt**
1. Download Qt Online Installer for macOS
2. Install to `/Users/<username>/Qt/6.9.1`

**Step 2: Build**
```bash
cd ~/Projects
git clone https://github.com/paolosereno/LanScan.git
cd LanScan

mkdir build && cd build

cmake -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_PREFIX_PATH="$HOME/Qt/6.9.1/macos" \
      ..

cmake --build . -j$(sysctl -n hw.ncpu)
```

**Step 3: Deploy and Install**
```bash
$HOME/Qt/6.9.1/macos/bin/macdeployqt LanScan.app -dmg
cp -r LanScan.app /Applications/
```

### 5.3 Code Signing (Optional)

```bash
# Create developer certificate in Keychain Access first

# Sign application
codesign --force --deep --sign "Developer ID Application: Your Name" \
         /Applications/LanScan.app

# Verify signature
codesign --verify --deep --verbose=2 /Applications/LanScan.app
```

---

## 6. Build Options

### 6.1 CMake Build Types

**Debug Build:**
```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
```
- Includes debug symbols
- No optimizations
- Assertions enabled
- Useful for development and debugging

**Release Build:**
```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
```
- Optimized for performance (-O3)
- No debug symbols
- Assertions disabled
- Recommended for production

**RelWithDebInfo Build:**
```bash
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
```
- Optimized but includes debug symbols
- Useful for profiling

**MinSizeRel Build:**
```bash
cmake -DCMAKE_BUILD_TYPE=MinSizeRel ..
```
- Optimized for size (-Os)
- Smaller executable

### 6.2 Custom Build Options

**Enable Tests:**
```bash
cmake -DBUILD_TESTS=ON ..
```

**Enable Code Coverage (GCC/Clang only):**
```bash
cmake -DCMAKE_BUILD_TYPE=Coverage \
      -DCMAKE_CXX_FLAGS="--coverage -fprofile-arcs -ftest-coverage" \
      ..
cmake --build .
ctest
lcov --capture --directory . --output-file coverage.info
genhtml coverage.info --output-directory coverage_html
```

**Custom Install Prefix:**
```bash
cmake -DCMAKE_INSTALL_PREFIX=/opt/lanscan ..
sudo cmake --install .
```

**Specify Qt Path:**
```bash
cmake -DCMAKE_PREFIX_PATH="/path/to/Qt/6.9.1/gcc_64" ..
```

**Parallel Build:**
```bash
# Windows
cmake --build . -j12

# Linux/macOS
cmake --build . -j$(nproc)    # Linux
cmake --build . -j$(sysctl -n hw.ncpu)  # macOS
```

### 6.3 Build Flags

**Compiler-Specific Flags:**

**GCC/Clang:**
```bash
cmake -DCMAKE_CXX_FLAGS="-Wall -Wextra -Wpedantic" ..
```

**MSVC:**
```bash
cmake -DCMAKE_CXX_FLAGS="/W4 /WX" ..
```

**Enable Link-Time Optimization:**
```bash
cmake -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON ..
```

---

## 7. Post-Installation

### 7.1 Verify Installation

```bash
# Check version
LanScan --version  # (if CLI support added)

# Check Qt libraries
# Windows
where Qt6Core.dll

# Linux
ldd /usr/local/bin/LanScan

# macOS
otool -L /Applications/LanScan.app/Contents/MacOS/LanScan
```

### 7.2 Database Initialization

LanScan automatically creates its database on first run:
- **Windows**: `C:\Users\<username>\AppData\Local\LanScan\lanscan.db`
- **Linux**: `~/.local/share/LanScan/lanscan.db`
- **macOS**: `~/Library/Application Support/LanScan/lanscan.db`

### 7.3 Configuration Files

**Settings:**
- **Windows**: Registry `HKEY_CURRENT_USER\Software\LanScan`
- **Linux**: `~/.config/LanScan/LanScan.conf`
- **macOS**: `~/Library/Preferences/com.lanscan.LanScan.plist`

**Profiles:**
- `<AppData>/LanScan/profiles.json`

**Favorites:**
- `<AppData>/LanScan/favorites.json`

### 7.4 Firewall Configuration

**Windows Firewall:**
```cmd
# Allow LanScan through firewall (run as Administrator)
netsh advfirewall firewall add rule ^
    name="LanScan" ^
    dir=in ^
    action=allow ^
    program="C:\Program Files\LanScan\LanScan.exe"
```

**Linux (UFW):**
```bash
# Allow LanScan (if needed)
sudo ufw allow from any to any proto tcp port 5201 comment 'LanScan Bandwidth Server'
```

**Linux (iptables):**
```bash
# Allow ICMP (ping)
sudo iptables -A INPUT -p icmp --icmp-type echo-request -j ACCEPT
sudo iptables -A OUTPUT -p icmp --icmp-type echo-reply -j ACCEPT
```

### 7.5 Permissions (Linux)

**Grant raw socket capabilities (for ping without sudo):**
```bash
sudo setcap cap_net_raw+ep /usr/local/bin/LanScan
```

**Add user to netdev group:**
```bash
sudo usermod -aG netdev $USER
```

---

## 8. Troubleshooting

### 8.1 Build Errors

#### Error: "Qt6 package not found"

**Problem:** CMake cannot find Qt installation

**Solutions:**
```bash
# Specify Qt path explicitly
cmake -DCMAKE_PREFIX_PATH="/path/to/Qt/6.9.1/gcc_64" ..

# OR set environment variable (Linux/macOS)
export CMAKE_PREFIX_PATH="/path/to/Qt/6.9.1/gcc_64"
cmake ..

# Windows
set CMAKE_PREFIX_PATH=C:\Qt\6.9.1\mingw_64
cmake ..
```

#### Error: "Could NOT find Qt6Charts"

**Problem:** Qt Charts component not installed

**Solutions:**
```bash
# Linux: Install Qt Charts
sudo apt install qt6-charts-dev  # Ubuntu/Debian
sudo dnf install qt6-qtcharts-devel  # Fedora
sudo pacman -S qt6-charts  # Arch

# Windows: Reinstall Qt with Charts component selected
```

#### Error: "CMake version is too old"

**Problem:** CMake version < 3.16

**Solutions:**
```bash
# Linux: Install from Kitware repository
wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc | sudo apt-key add -
sudo apt-add-repository 'deb https://apt.kitware.com/ubuntu/ focal main'
sudo apt update
sudo apt install cmake

# Windows: Download latest from cmake.org

# macOS: Update via Homebrew
brew upgrade cmake
```

#### Error: "undefined reference to `WinMain'"

**Problem:** Qt6::Test not linked correctly (Windows test build)

**Solutions:**
```cmake
# In tests/CMakeLists.txt, ensure:
target_link_libraries(MyTest PRIVATE Qt6::Test Qt6::Core Qt6::Gui)
```

#### Error: "MOC file not generated"

**Problem:** AUTOMOC not enabled

**Solutions:**
```cmake
# In CMakeLists.txt, add:
set(CMAKE_AUTOMOC ON)
```

### 8.2 Runtime Errors

#### Error: "Cannot find Qt6Core.dll" (Windows)

**Problem:** Qt DLLs not in PATH or not deployed

**Solutions:**
```cmd
# Option 1: Deploy Qt libraries
C:\Qt\6.9.1\mingw_64\bin\windeployqt.exe LanScan.exe

# Option 2: Add Qt bin to PATH
set PATH=C:\Qt\6.9.1\mingw_64\bin;%PATH%

# Option 3: Copy DLLs manually to build directory
copy C:\Qt\6.9.1\mingw_64\bin\Qt6*.dll .
```

#### Error: "Failed to open database"

**Problem:** SQLite not accessible or permissions issue

**Solutions:**
```bash
# Linux: Check permissions
chmod 755 ~/.local/share/LanScan
chmod 644 ~/.local/share/LanScan/lanscan.db

# Windows: Run as administrator once
# macOS: Grant full disk access in System Preferences → Privacy
```

#### Error: "Ping failed: Permission denied" (Linux)

**Problem:** Raw sockets require root privileges

**Solutions:**
```bash
# Option 1: Grant capabilities (recommended)
sudo setcap cap_net_raw+ep /usr/local/bin/LanScan

# Option 2: Run with sudo (not recommended for GUI)
sudo lanscan

# Option 3: Add user to group (distribution-specific)
sudo usermod -aG netdev $USER
```

#### Error: "Segmentation fault on startup"

**Problem:** Qt platform plugin not found

**Solutions:**
```bash
# Linux: Install Qt platform plugins
sudo apt install libqt6gui6 qt6-qpa-plugins

# Set QT_QPA_PLATFORM_PLUGIN_PATH
export QT_QPA_PLATFORM_PLUGIN_PATH=/usr/lib/x86_64-linux-gnu/qt6/plugins
```

### 8.3 Network Issues

#### Problem: "No devices found" after scan

**Solutions:**
1. Check firewall allows ICMP (ping)
2. Verify subnet is correct (use "Detect" button)
3. Disable AP Isolation on router
4. Run with admin/sudo privileges
5. Check network adapter is active

#### Problem: "MAC addresses not detected"

**Solutions:**
1. Ensure LanScan is on same subnet as targets
2. Install `net-tools` (Linux)
3. Enable ARP on network interface
4. Check router allows ARP queries

#### Problem: "DNS resolution fails"

**Solutions:**
1. Check DNS server is accessible (ping 8.8.8.8)
2. Verify firewall allows DNS (UDP port 53)
3. Use custom DNS server (8.8.8.8, 1.1.1.1)
4. Check `/etc/resolv.conf` (Linux)

### 8.4 Qt-Specific Issues

#### Error: "This application failed to start because no Qt platform plugin could be initialized"

**Problem:** Qt platform plugins missing

**Solutions:**
```bash
# Linux
sudo apt install qt6-qpa-plugins
export QT_QPA_PLATFORM_PLUGIN_PATH=/usr/lib/x86_64-linux-gnu/qt6/plugins

# Windows
# Ensure platforms/ directory exists in build directory with qwindows.dll

# macOS
# Ensure platforms/ directory exists with libqcocoa.dylib
```

#### Error: "Could not find the Qt platform plugin 'xcb'"

**Problem:** Missing X11 dependencies (Linux)

**Solutions:**
```bash
sudo apt install -y \
    libxcb-xinerama0 \
    libxcb-cursor0 \
    libxcb-icccm4 \
    libxcb-image0 \
    libxcb-keysyms1 \
    libxcb-randr0 \
    libxcb-render-util0 \
    libxcb-shape0
```

---

## 9. Uninstallation

### 9.1 Windows

**If installed via installer:**
1. Control Panel → Programs → Uninstall a program
2. Select "LanScan" → Uninstall

**If built from source:**
```cmd
# Delete build directory
rmdir /s /q C:\Projects\LanScan\build

# Delete application data
rmdir /s /q "%LOCALAPPDATA%\LanScan"

# Delete desktop shortcut
del "%USERPROFILE%\Desktop\LanScan.lnk"

# Remove from registry
reg delete HKEY_CURRENT_USER\Software\LanScan /f
```

### 9.2 Linux

**If installed system-wide:**
```bash
# Ubuntu/Debian
sudo apt remove lanscan

# Fedora/RHEL
sudo dnf remove lanscan

# Arch
sudo pacman -R lanscan

# From source (if CMake install was used)
cd ~/Projects/LanScan/build
sudo xargs rm < install_manifest.txt
```

**Remove user data:**
```bash
rm -rf ~/.local/share/LanScan
rm -rf ~/.config/LanScan
rm -f ~/.local/share/applications/lanscan.desktop
```

### 9.3 macOS

```bash
# Remove application
sudo rm -rf /Applications/LanScan.app

# Remove user data
rm -rf ~/Library/Application\ Support/LanScan
rm -rf ~/Library/Preferences/com.lanscan.LanScan.plist
rm -rf ~/Library/Logs/LanScan
```

---

## Appendix A: Build Directory Structure

```
LanScan/
├── build/               # Build output directory
│   ├── LanScan.exe     # Main executable (Windows)
│   ├── LanScan         # Main executable (Linux/macOS)
│   ├── *.dll           # Qt DLLs (Windows)
│   ├── platforms/      # Qt platform plugins
│   ├── styles/         # Qt style plugins
│   ├── translations/   # Qt translations
│   └── tests/          # Test executables
├── src/                # Source code
├── include/            # Header files
├── ui/                 # Qt Designer UI files
├── resources/          # Icons, stylesheets, translations
├── tests/              # Unit tests
├── CMakeLists.txt      # Main CMake configuration
└── README.md           # Project documentation
```

---

## Appendix B: Environment Variables

**Qt-Related:**
- `CMAKE_PREFIX_PATH` - Qt installation path
- `QT_QPA_PLATFORM_PLUGIN_PATH` - Qt platform plugins path
- `QT_PLUGIN_PATH` - Additional Qt plugins path
- `QT_DEBUG_PLUGINS` - Enable Qt plugin debug output (1)

**Build-Related:**
- `CMAKE_BUILD_TYPE` - Build configuration (Debug/Release)
- `CMAKE_INSTALL_PREFIX` - Installation directory
- `CMAKE_CXX_COMPILER` - C++ compiler path
- `CMAKE_GENERATOR` - CMake generator (Ninja, Unix Makefiles, etc.)

**LanScan-Specific:**
- `LANSCAN_LOG_LEVEL` - Log level (DEBUG/INFO/WARNING/ERROR)
- `LANSCAN_DB_PATH` - Custom database path
- `LANSCAN_CONFIG_PATH` - Custom config path

---

## Appendix C: Quick Reference Commands

**Clone and Build (All Platforms):**
```bash
git clone https://github.com/paolosereno/LanScan.git
cd LanScan
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="/path/to/Qt" ..
cmake --build . -j$(nproc)
```

**Run Tests:**
```bash
cmake -DBUILD_TESTS=ON ..
cmake --build . -j$(nproc)
ctest --output-on-failure
```

**Install System-Wide:**
```bash
sudo cmake --install .
```

**Clean Build:**
```bash
rm -rf build && mkdir build && cd build
cmake .. && cmake --build . -j$(nproc)
```

---

## Appendix D: Continuous Integration

**GitHub Actions Example (.github/workflows/build.yml):**
```yaml
name: Build LanScan

on: [push, pull_request]

jobs:
  build-linux:
    runs-on: ubuntu-22.04
    steps:
      - uses: actions/checkout@v3
      - name: Install Qt
        run: |
          sudo apt update
          sudo apt install -y qt6-base-dev qt6-charts-dev qt6-svg-dev cmake
      - name: Build
        run: |
          mkdir build && cd build
          cmake -DCMAKE_BUILD_TYPE=Release ..
          cmake --build . -j$(nproc)
      - name: Test
        run: |
          cd build
          ctest --output-on-failure

  build-windows:
    runs-on: windows-latest
    steps:
      - uses: actions/checkout@v3
      - name: Install Qt
        uses: jurplel/install-qt-action@v3
        with:
          version: '6.5.0'
          modules: 'qtcharts qtsvg'
      - name: Build
        run: |
          mkdir build
          cd build
          cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ..
          cmake --build . -j4

  build-macos:
    runs-on: macos-latest
    steps:
      - uses: actions/checkout@v3
      - name: Install Qt
        run: brew install qt@6
      - name: Build
        run: |
          mkdir build && cd build
          cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="/usr/local/opt/qt@6" ..
          cmake --build . -j$(sysctl -n hw.ncpu)
```

---

**End of Installation Guide**

*For user documentation, see [USER_MANUAL.md](USER_MANUAL.md)*
*For developer documentation, see [API_DOCUMENTATION.md](API_DOCUMENTATION.md)*

**LanScan © 2025** - Network Scanner and Diagnostic Tool
Licensed under MIT License
https://github.com/paolosereno/LanScan
