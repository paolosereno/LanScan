# Test configuration for LanScan

# Model tests
add_executable(DeviceTest
    models/DeviceTest.cpp
    ${CMAKE_SOURCE_DIR}/src/models/Device.cpp
    ${CMAKE_SOURCE_DIR}/src/models/PortInfo.cpp
    ${CMAKE_SOURCE_DIR}/src/models/NetworkMetrics.cpp
)
target_link_libraries(DeviceTest PRIVATE Qt6::Test Qt6::Core)
add_test(NAME DeviceTest COMMAND DeviceTest)

add_executable(NetworkMetricsTest
    models/NetworkMetricsTest.cpp
    ${CMAKE_SOURCE_DIR}/src/models/NetworkMetrics.cpp
)
target_link_libraries(NetworkMetricsTest PRIVATE Qt6::Test Qt6::Core)
add_test(NAME NetworkMetricsTest COMMAND NetworkMetricsTest)

# Utils tests
add_executable(IpAddressValidatorTest
    utils/IpAddressValidatorTest.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/IpAddressValidator.cpp
)
target_link_libraries(IpAddressValidatorTest PRIVATE Qt6::Test Qt6::Core Qt6::Network)
add_test(NAME IpAddressValidatorTest COMMAND IpAddressValidatorTest)

add_executable(StatisticsCalculatorTest
    utils/StatisticsCalculatorTest.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/StatisticsCalculator.cpp
)
target_link_libraries(StatisticsCalculatorTest PRIVATE Qt6::Test Qt6::Core)
add_test(NAME StatisticsCalculatorTest COMMAND StatisticsCalculatorTest)

add_executable(LoggerTest
    utils/LoggerTest.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/Logger.cpp
)
target_link_libraries(LoggerTest PRIVATE Qt6::Test Qt6::Core)
add_test(NAME LoggerTest COMMAND LoggerTest)

# Network tests
add_executable(SubnetCalculatorTest
    network/SubnetCalculatorTest.cpp
    ${CMAKE_SOURCE_DIR}/src/network/services/SubnetCalculator.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/IpAddressValidator.cpp
)
target_link_libraries(SubnetCalculatorTest PRIVATE Qt6::Test Qt6::Core Qt6::Network)
add_test(NAME SubnetCalculatorTest COMMAND SubnetCalculatorTest)

add_executable(HostDiscoveryTest
    network/HostDiscoveryTest.cpp
    ${CMAKE_SOURCE_DIR}/src/network/discovery/HostDiscovery.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/Logger.cpp
)
target_link_libraries(HostDiscoveryTest PRIVATE Qt6::Test Qt6::Core Qt6::Network)
add_test(NAME HostDiscoveryTest COMMAND HostDiscoveryTest)

add_executable(DnsResolverTest
    network/DnsResolverTest.cpp
    ${CMAKE_SOURCE_DIR}/src/network/discovery/DnsResolver.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/Logger.cpp
)
target_link_libraries(DnsResolverTest PRIVATE Qt6::Test Qt6::Core Qt6::Network)
add_test(NAME DnsResolverTest COMMAND DnsResolverTest)

add_executable(ArpDiscoveryTest
    network/ArpDiscoveryTest.cpp
    ${CMAKE_SOURCE_DIR}/src/network/discovery/ArpDiscovery.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/Logger.cpp
)
target_link_libraries(ArpDiscoveryTest PRIVATE Qt6::Test Qt6::Core Qt6::Network)
add_test(NAME ArpDiscoveryTest COMMAND ArpDiscoveryTest)

add_executable(IpScannerTest
    network/IpScannerTest.cpp
    ${CMAKE_SOURCE_DIR}/src/network/scanner/IpScanner.cpp
    ${CMAKE_SOURCE_DIR}/src/network/scanner/QuickScanStrategy.cpp
    ${CMAKE_SOURCE_DIR}/src/network/scanner/DeepScanStrategy.cpp
    ${CMAKE_SOURCE_DIR}/src/network/services/SubnetCalculator.cpp
    ${CMAKE_SOURCE_DIR}/src/network/services/MacVendorLookup.cpp
    ${CMAKE_SOURCE_DIR}/src/network/services/PortServiceMapper.cpp
    ${CMAKE_SOURCE_DIR}/src/network/discovery/HostDiscovery.cpp
    ${CMAKE_SOURCE_DIR}/src/network/discovery/DnsResolver.cpp
    ${CMAKE_SOURCE_DIR}/src/network/discovery/ArpDiscovery.cpp
    ${CMAKE_SOURCE_DIR}/src/network/sockets/TcpSocketManager.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/IpAddressValidator.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/Logger.cpp
    ${CMAKE_SOURCE_DIR}/src/models/Device.cpp
    ${CMAKE_SOURCE_DIR}/src/models/PortInfo.cpp
    ${CMAKE_SOURCE_DIR}/src/models/NetworkMetrics.cpp
)
target_link_libraries(IpScannerTest PRIVATE Qt6::Test Qt6::Core Qt6::Network)
add_test(NAME IpScannerTest COMMAND IpScannerTest)

# Phase 2: Diagnostics tests
add_executable(PingServiceTest
    network/PingServiceTest.cpp
    ${CMAKE_SOURCE_DIR}/src/network/diagnostics/PingService.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/Logger.cpp
)
target_link_libraries(PingServiceTest PRIVATE Qt6::Test Qt6::Core Qt6::Network)
add_test(NAME PingServiceTest COMMAND PingServiceTest)

add_executable(LatencyCalculatorTest
    network/LatencyCalculatorTest.cpp
    ${CMAKE_SOURCE_DIR}/src/network/diagnostics/LatencyCalculator.cpp
)
target_link_libraries(LatencyCalculatorTest PRIVATE Qt6::Test Qt6::Core)
add_test(NAME LatencyCalculatorTest COMMAND LatencyCalculatorTest)

add_executable(JitterCalculatorTest
    network/JitterCalculatorTest.cpp
    ${CMAKE_SOURCE_DIR}/src/network/diagnostics/JitterCalculator.cpp
)
target_link_libraries(JitterCalculatorTest PRIVATE Qt6::Test Qt6::Core)
add_test(NAME JitterCalculatorTest COMMAND JitterCalculatorTest)

add_executable(PacketLossCalculatorTest
    network/PacketLossCalculatorTest.cpp
    ${CMAKE_SOURCE_DIR}/src/network/diagnostics/PacketLossCalculator.cpp
)
target_link_libraries(PacketLossCalculatorTest PRIVATE Qt6::Test Qt6::Core)
add_test(NAME PacketLossCalculatorTest COMMAND PacketLossCalculatorTest)

add_executable(QualityScoreCalculatorTest
    network/QualityScoreCalculatorTest.cpp
    ${CMAKE_SOURCE_DIR}/src/network/diagnostics/QualityScoreCalculator.cpp
    ${CMAKE_SOURCE_DIR}/src/models/NetworkMetrics.cpp
)
target_link_libraries(QualityScoreCalculatorTest PRIVATE Qt6::Test Qt6::Core)
add_test(NAME QualityScoreCalculatorTest COMMAND QualityScoreCalculatorTest)
