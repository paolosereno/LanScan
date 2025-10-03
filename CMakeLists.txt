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
)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/models
    ${CMAKE_SOURCE_DIR}/src/utils
    ${CMAKE_SOURCE_DIR}/src/interfaces
    ${CMAKE_SOURCE_DIR}/src/network
    ${CMAKE_SOURCE_DIR}/src/services
    ${CMAKE_SOURCE_DIR}/src/controllers
    ${CMAKE_SOURCE_DIR}/src/viewmodels
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

# Header files
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
)

# UI files
set(UI_SOURCES
    ui/MainWindow.ui
)

# Create executable
add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${MODEL_SOURCES}
    ${UTILS_SOURCES}
    ${HEADERS}
    ${UI_SOURCES}
)

# Link Qt libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
    Qt6::Charts
)

# Enable testing
enable_testing()

# Add test subdirectory
add_subdirectory(tests)

# Installation rules
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
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
