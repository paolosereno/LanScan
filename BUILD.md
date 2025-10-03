# LanScan - Build Instructions

## Quick Build Commands

### Standard Development Build (MinGW Debug)
```bash
rm -rf build && mkdir build && cd build
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH="C:/Qt/6.9.1/mingw_64" ..
cmake --build . -j12
```

### Run Tests
```bash
cd build
ctest --output-on-failure
```

### Clean Rebuild
```bash
rm -rf build && mkdir build && cd build
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH="C:/Qt/6.9.1/mingw_64" ..
cmake --build . -j12
ctest --output-on-failure
```

## Build Configuration

### Preferred Development Settings
- **Compiler**: MinGW Makefiles (GCC 13.1.0)
- **Build Type**: Debug
- **Qt Path**: C:/Qt/6.9.1/mingw_64
- **Parallel Jobs**: 12 cores (-j12)
- **Testing**: Run after every build

### Directory Structure
```
build/                     # Build output directory
├── LanScan.exe           # Main executable (Debug)
├── tests/                # Test executables
│   ├── DeviceTest.exe
│   ├── NetworkMetricsTest.exe
│   ├── IpAddressValidatorTest.exe
│   ├── StatisticsCalculatorTest.exe
│   └── LoggerTest.exe
└── Testing/              # CTest results
```

## Alternative Build Configurations

### Release Build (MSVC)
```bash
mkdir build && cd build
cmake -DCMAKE_PREFIX_PATH="C:/Qt/6.9.1/msvc2022_64" ..
cmake --build . --config Release -j12
```

### MinGW Release Build
```bash
rm -rf build && mkdir build && cd build
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="C:/Qt/6.9.1/mingw_64" ..
cmake --build . -j12
```

## Troubleshooting

### Common Issues

#### Qt DLLs Not Found
If tests crash with `0xc0000139` error, Qt DLLs are missing:
```bash
cp C:/Qt/6.9.1/mingw_64/bin/Qt6Core.dll build/tests/
cp C:/Qt/6.9.1/mingw_64/bin/Qt6Test.dll build/tests/
cp C:/Qt/6.9.1/mingw_64/bin/Qt6Network.dll build/tests/
```

#### MSVC Compilation Errors
Ensure you're using the correct Qt build:
- MinGW builds: Use `C:/Qt/6.9.1/mingw_64`
- MSVC builds: Use `C:/Qt/6.9.1/msvc2022_64`

#### CMake Cache Issues
Clean and regenerate:
```bash
rm -rf build
mkdir build && cd build
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH="C:/Qt/6.9.1/mingw_64" ..
```

## Running the Application

### From Build Directory
```bash
cd build
./LanScan.exe
```

### With Debugger (GDB)
```bash
cd build
gdb ./LanScan.exe
```

## Test Details

### Individual Test Execution
```bash
cd build/tests
./DeviceTest.exe
./NetworkMetricsTest.exe
./IpAddressValidatorTest.exe
./StatisticsCalculatorTest.exe
./LoggerTest.exe
```

### Verbose Test Output
```bash
cd build
ctest --verbose
```

### Run Specific Test
```bash
cd build
ctest -R DeviceTest --verbose
```

## Performance Notes

- **Parallel builds (-j12)**: Utilizes 12 CPU cores for faster compilation
- **Debug builds**: Include symbols for debugging, larger binaries
- **Release builds**: Optimized for performance, smaller binaries
- **MinGW**: Preferred for development (better debugging experience)
- **MSVC**: Optional for release builds (Windows-native toolchain)

## IDE Integration

### Qt Creator
1. Open `CMakeLists.txt` as project
2. Configure kit: Desktop Qt 6.9.1 MinGW 64-bit
3. Build configuration: Debug
4. CMake arguments: `-DCMAKE_PREFIX_PATH=C:/Qt/6.9.1/mingw_64`

### Visual Studio Code
Use CMake Tools extension with these settings in `.vscode/settings.json`:
```json
{
    "cmake.generator": "MinGW Makefiles",
    "cmake.buildDirectory": "${workspaceFolder}/build",
    "cmake.configureSettings": {
        "CMAKE_BUILD_TYPE": "Debug",
        "CMAKE_PREFIX_PATH": "C:/Qt/6.9.1/mingw_64"
    },
    "cmake.parallelJobs": 12
}
```
