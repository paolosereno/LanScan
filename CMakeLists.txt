cmake_minimum_required(VERSION 3.16)

project(LanScan VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# MSVC specific: Enable proper C++17 __cplusplus macro and strict conformance
if(MSVC)
    add_compile_options(/Zc:__cplusplus /permissive-)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt6 packages
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Widgets
    Network
    Charts
    Test
    Sql
    Concurrent
)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/models
    ${CMAKE_SOURCE_DIR}/include/utils
    ${CMAKE_SOURCE_DIR}/include/interfaces
    ${CMAKE_SOURCE_DIR}/include/network
    ${CMAKE_SOURCE_DIR}/include/database
    ${CMAKE_SOURCE_DIR}/include/export
    ${CMAKE_SOURCE_DIR}/include/config
    ${CMAKE_SOURCE_DIR}/include/coordinators
    ${CMAKE_SOURCE_DIR}/include/controllers
    ${CMAKE_SOURCE_DIR}/include/managers
    ${CMAKE_SOURCE_DIR}/include/viewmodels
    ${CMAKE_SOURCE_DIR}/include/views
    ${CMAKE_SOURCE_DIR}/include/delegates
    ${CMAKE_SOURCE_DIR}/include/charts
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/models
    ${CMAKE_SOURCE_DIR}/src/utils
    ${CMAKE_SOURCE_DIR}/src/interfaces
    ${CMAKE_SOURCE_DIR}/src/network
    ${CMAKE_SOURCE_DIR}/src/services
    ${CMAKE_SOURCE_DIR}/src/controllers
    ${CMAKE_SOURCE_DIR}/src/coordinators
    ${CMAKE_SOURCE_DIR}/src/managers
    ${CMAKE_SOURCE_DIR}/src/viewmodels
    ${CMAKE_SOURCE_DIR}/src/views
    ${CMAKE_SOURCE_DIR}/src/delegates
    ${CMAKE_SOURCE_DIR}/src/charts
    ${CMAKE_SOURCE_DIR}/src/persistence
    ${CMAKE_SOURCE_DIR}/src/ui
    ${CMAKE_SOURCE_DIR}/src/core
)

# Source files
set(SOURCES
    src/main.cpp
)

# Model sources
set(MODEL_SOURCES
    src/models/Device.cpp
    src/models/NetworkMetrics.cpp
    src/models/PortInfo.cpp
    src/models/NetworkInterface.cpp
)

# Utils sources
set(UTILS_SOURCES
    src/utils/Logger.cpp
    src/utils/IpAddressValidator.cpp
    src/utils/StringFormatter.cpp
    src/utils/TimeFormatter.cpp
    src/utils/StatisticsCalculator.cpp
)

# Network sources
set(NETWORK_SOURCES
    src/network/services/SubnetCalculator.cpp
    src/network/services/NetworkInterfaceDetector.cpp
    src/network/services/MacVendorLookup.cpp
    src/network/services/PortServiceMapper.cpp
    src/network/sockets/TcpSocketManager.cpp
    src/network/sockets/UdpSocketManager.cpp
    src/network/discovery/HostDiscovery.cpp
    src/network/discovery/DnsResolver.cpp
    src/network/discovery/ArpDiscovery.cpp
    src/network/scanner/IpScanner.cpp
    src/network/scanner/QuickScanStrategy.cpp
    src/network/scanner/DeepScanStrategy.cpp
    src/network/diagnostics/PingService.cpp
    src/network/diagnostics/LatencyCalculator.cpp
    src/network/diagnostics/JitterCalculator.cpp
    src/network/diagnostics/PacketLossCalculator.cpp
    src/network/diagnostics/QualityScoreCalculator.cpp
    src/network/diagnostics/MetricsAggregator.cpp
    src/network/diagnostics/PortScanner.cpp
)

# Database sources
set(DATABASE_SOURCES
    src/database/DatabaseManager.cpp
    src/database/DeviceCache.cpp
    src/database/DeviceRepository.cpp
)

# Export sources
set(EXPORT_SOURCES
    src/export/CsvExporter.cpp
    src/export/JsonExporter.cpp
)

# Config sources
set(CONFIG_SOURCES
    src/config/SettingsManager.cpp
    src/config/ConfigValidator.cpp
)

# Coordinator sources (Phase 4)
set(COORDINATOR_SOURCES
    src/coordinators/ScanCoordinator.cpp
)

# Controller sources (Phase 4)
set(CONTROLLER_SOURCES
    src/controllers/ScanController.cpp
    src/controllers/MetricsController.cpp
    src/controllers/ExportController.cpp
)

# Manager sources (Phase 4)
set(MANAGER_SOURCES
    src/managers/ProfileManager.cpp
    src/managers/FavoritesManager.cpp
)

# ViewModel sources (Phase 5-6)
set(VIEWMODEL_SOURCES
    src/viewmodels/DeviceTableViewModel.cpp
    src/viewmodels/ScanConfigViewModel.cpp
    src/viewmodels/ChartViewModel.cpp
    src/viewmodels/MetricsViewModel.cpp
)

# Chart sources (Phase 6)
set(CHART_SOURCES
    src/charts/LatencyChart.cpp
    src/charts/PacketLossChart.cpp
    src/charts/JitterChart.cpp
)

# View sources (Phase 5-6)
set(VIEW_SOURCES
    src/views/MainWindow.cpp
    src/views/DeviceTableWidget.cpp
    src/views/ScanConfigDialog.cpp
    src/views/MetricsWidget.cpp
)

# Delegate sources (Phase 5)
set(DELEGATE_SOURCES
    src/delegates/StatusDelegate.cpp
    src/delegates/QualityScoreDelegate.cpp
)

# Header files (for MOC)
set(HEADERS
    src/models/Device.h
    src/models/NetworkMetrics.h
    src/models/PortInfo.h
    src/models/NetworkInterface.h
    src/utils/Logger.h
    src/utils/IpAddressValidator.h
    src/utils/StringFormatter.h
    src/utils/TimeFormatter.h
    src/utils/StatisticsCalculator.h
    src/interfaces/IScanStrategy.h
    src/interfaces/IMetricsCalculator.h
    src/interfaces/IExporter.h
    src/interfaces/IDeviceRepository.h
    include/coordinators/ScanCoordinator.h
    include/controllers/ScanController.h
    include/controllers/MetricsController.h
    include/controllers/ExportController.h
    include/managers/ProfileManager.h
    include/managers/FavoritesManager.h
    include/utils/ProgressTracker.h
    include/viewmodels/DeviceTableViewModel.h
    include/viewmodels/ScanConfigViewModel.h
    include/viewmodels/ChartViewModel.h
    include/viewmodels/MetricsViewModel.h
    include/charts/LatencyChart.h
    include/charts/PacketLossChart.h
    include/charts/JitterChart.h
    include/views/MainWindow.h
    include/views/DeviceTableWidget.h
    include/views/ScanConfigDialog.h
    include/views/MetricsWidget.h
    include/delegates/StatusDelegate.h
    include/delegates/QualityScoreDelegate.h
)

# UI files (Phase 5-6)
set(UI_SOURCES
    ui/mainwindow.ui
    ui/scanconfigdialog.ui
    ui/devicetablewidget.ui
    ui/metricswidget.ui
)

# Set AUTOUIC search paths for UI files
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_SOURCE_DIR}/ui)

# Create executable
add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${MODEL_SOURCES}
    ${UTILS_SOURCES}
    ${NETWORK_SOURCES}
    ${DATABASE_SOURCES}
    ${EXPORT_SOURCES}
    ${CONFIG_SOURCES}
    ${COORDINATOR_SOURCES}
    ${CONTROLLER_SOURCES}
    ${MANAGER_SOURCES}
    ${VIEWMODEL_SOURCES}
    ${VIEW_SOURCES}
    ${DELEGATE_SOURCES}
    ${CHART_SOURCES}
    ${HEADERS}
    ${UI_SOURCES}
)

# Link Qt libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
    Qt6::Charts
    Qt6::Sql
    Qt6::Concurrent
)

# Enable testing
enable_testing()

# Add test subdirectory
add_subdirectory(tests)

# Installation rules
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

# Install OUI database
install(FILES
    ${CMAKE_SOURCE_DIR}/scripts/oui_database.txt
    DESTINATION bin
    OPTIONAL
)

# Install scripts for manual updates
install(FILES
    ${CMAKE_SOURCE_DIR}/scripts/download_oui.py
    DESTINATION bin/scripts
    OPTIONAL
)

# Platform-specific settings
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

if(UNIX AND NOT APPLE)
    # Linux-specific settings
    install(FILES resources/lanscan.desktop
        DESTINATION share/applications
    )
endif()
