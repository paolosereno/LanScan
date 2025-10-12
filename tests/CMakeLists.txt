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

# Phase 3: Persistence & Data Management tests
add_executable(DeviceRepositoryTest
    DeviceRepositoryTest.cpp
    ${CMAKE_SOURCE_DIR}/src/database/DatabaseManager.cpp
    ${CMAKE_SOURCE_DIR}/src/database/DeviceCache.cpp
    ${CMAKE_SOURCE_DIR}/src/database/DeviceRepository.cpp
    ${CMAKE_SOURCE_DIR}/src/models/Device.cpp
    ${CMAKE_SOURCE_DIR}/src/models/PortInfo.cpp
    ${CMAKE_SOURCE_DIR}/src/models/NetworkMetrics.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/Logger.cpp
)
target_link_libraries(DeviceRepositoryTest PRIVATE Qt6::Test Qt6::Core Qt6::Sql)
add_test(NAME DeviceRepositoryTest COMMAND DeviceRepositoryTest)

add_executable(CsvExporterTest
    CsvExporterTest.cpp
    ${CMAKE_SOURCE_DIR}/src/export/CsvExporter.cpp
    ${CMAKE_SOURCE_DIR}/src/models/Device.cpp
    ${CMAKE_SOURCE_DIR}/src/models/PortInfo.cpp
    ${CMAKE_SOURCE_DIR}/src/models/NetworkMetrics.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/Logger.cpp
)
target_link_libraries(CsvExporterTest PRIVATE Qt6::Test Qt6::Core)
add_test(NAME CsvExporterTest COMMAND CsvExporterTest)

add_executable(JsonExporterTest
    JsonExporterTest.cpp
    ${CMAKE_SOURCE_DIR}/src/export/JsonExporter.cpp
    ${CMAKE_SOURCE_DIR}/src/models/Device.cpp
    ${CMAKE_SOURCE_DIR}/src/models/PortInfo.cpp
    ${CMAKE_SOURCE_DIR}/src/models/NetworkMetrics.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/Logger.cpp
)
target_link_libraries(JsonExporterTest PRIVATE Qt6::Test Qt6::Core)
add_test(NAME JsonExporterTest COMMAND JsonExporterTest)

add_executable(SettingsManagerTest
    SettingsManagerTest.cpp
    ${CMAKE_SOURCE_DIR}/src/config/SettingsManager.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/Logger.cpp
)
target_link_libraries(SettingsManagerTest PRIVATE Qt6::Test Qt6::Core)
add_test(NAME SettingsManagerTest COMMAND SettingsManagerTest)

add_executable(MacVendorLookupTest
    MacVendorLookupTest.cpp
    ${CMAKE_SOURCE_DIR}/src/network/services/MacVendorLookup.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/Logger.cpp
)
target_link_libraries(MacVendorLookupTest PRIVATE Qt6::Test Qt6::Core)
add_test(NAME MacVendorLookupTest COMMAND MacVendorLookupTest)

# Phase 6: Charts & Visualization tests
add_executable(ChartViewModelTest
    ChartViewModelTest.cpp
    ${CMAKE_SOURCE_DIR}/include/viewmodels/ChartViewModel.h
    ${CMAKE_SOURCE_DIR}/src/viewmodels/ChartViewModel.cpp
    ${CMAKE_SOURCE_DIR}/src/models/NetworkMetrics.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/Logger.cpp
)
target_include_directories(ChartViewModelTest PRIVATE
    ${CMAKE_SOURCE_DIR}/include/viewmodels
    ${CMAKE_SOURCE_DIR}/include/controllers
)
target_link_libraries(ChartViewModelTest PRIVATE Qt6::Test Qt6::Core Qt6::Network)
add_test(NAME ChartViewModelTest COMMAND ChartViewModelTest)

# Phase 7: Advanced Diagnostics tests
add_executable(TraceRouteServiceTest
    TraceRouteServiceTest.cpp
    ${CMAKE_SOURCE_DIR}/src/diagnostics/TraceRouteService.cpp
    ${CMAKE_SOURCE_DIR}/include/diagnostics/TraceRouteService.h
    ${CMAKE_SOURCE_DIR}/src/models/TraceRouteHop.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/Logger.cpp
)
target_include_directories(TraceRouteServiceTest PRIVATE
    ${CMAKE_SOURCE_DIR}/include/diagnostics
    ${CMAKE_SOURCE_DIR}/include/models
)
target_link_libraries(TraceRouteServiceTest PRIVATE Qt6::Test Qt6::Core Qt6::Network)
add_test(NAME TraceRouteServiceTest COMMAND TraceRouteServiceTest)

add_executable(MtuDiscoveryTest
    MtuDiscoveryTest.cpp
    ${CMAKE_SOURCE_DIR}/src/diagnostics/MtuDiscovery.cpp
    ${CMAKE_SOURCE_DIR}/include/diagnostics/MtuDiscovery.h
    ${CMAKE_SOURCE_DIR}/src/utils/Logger.cpp
)
target_include_directories(MtuDiscoveryTest PRIVATE
    ${CMAKE_SOURCE_DIR}/include/diagnostics
)
target_link_libraries(MtuDiscoveryTest PRIVATE Qt6::Test Qt6::Core Qt6::Network)
add_test(NAME MtuDiscoveryTest COMMAND MtuDiscoveryTest)

add_executable(BandwidthTesterTest
    BandwidthTesterTest.cpp
    ${CMAKE_SOURCE_DIR}/src/diagnostics/BandwidthTester.cpp
    ${CMAKE_SOURCE_DIR}/include/diagnostics/BandwidthTester.h
    ${CMAKE_SOURCE_DIR}/src/utils/Logger.cpp
)
target_include_directories(BandwidthTesterTest PRIVATE
    ${CMAKE_SOURCE_DIR}/include/diagnostics
)
target_link_libraries(BandwidthTesterTest PRIVATE Qt6::Test Qt6::Core Qt6::Network)
add_test(NAME BandwidthTesterTest COMMAND BandwidthTesterTest)

add_executable(DnsDiagnosticsTest
    DnsDiagnosticsTest.cpp
    ${CMAKE_SOURCE_DIR}/src/diagnostics/DnsDiagnostics.cpp
    ${CMAKE_SOURCE_DIR}/include/diagnostics/DnsDiagnostics.h
    ${CMAKE_SOURCE_DIR}/src/utils/Logger.cpp
)
target_include_directories(DnsDiagnosticsTest PRIVATE
    ${CMAKE_SOURCE_DIR}/include/diagnostics
)
target_link_libraries(DnsDiagnosticsTest PRIVATE Qt6::Test Qt6::Core Qt6::Network)
add_test(NAME DnsDiagnosticsTest COMMAND DnsDiagnosticsTest)

# Phase 7.3: Monitoring Service tests
add_executable(AlertServiceTest
    AlertServiceTest.cpp
    ${CMAKE_SOURCE_DIR}/src/services/AlertService.cpp
    ${CMAKE_SOURCE_DIR}/include/services/AlertService.h
    ${CMAKE_SOURCE_DIR}/src/models/Alert.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/Logger.cpp
)
target_include_directories(AlertServiceTest PRIVATE
    ${CMAKE_SOURCE_DIR}/include/services
    ${CMAKE_SOURCE_DIR}/include/models
)
target_link_libraries(AlertServiceTest PRIVATE Qt6::Test Qt6::Core Qt6::Gui)
add_test(NAME AlertServiceTest COMMAND AlertServiceTest)

add_executable(HistoryServiceTest
    HistoryServiceTest.cpp
    ${CMAKE_SOURCE_DIR}/src/services/HistoryService.cpp
    ${CMAKE_SOURCE_DIR}/include/services/HistoryService.h
    ${CMAKE_SOURCE_DIR}/src/database/DatabaseManager.cpp
    ${CMAKE_SOURCE_DIR}/src/models/NetworkMetrics.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/Logger.cpp
)
target_include_directories(HistoryServiceTest PRIVATE
    ${CMAKE_SOURCE_DIR}/include/services
    ${CMAKE_SOURCE_DIR}/include/database
    ${CMAKE_SOURCE_DIR}/include/models
)
target_link_libraries(HistoryServiceTest PRIVATE Qt6::Test Qt6::Core Qt6::Sql)
add_test(NAME HistoryServiceTest COMMAND HistoryServiceTest)

add_executable(MonitoringServiceTest
    MonitoringServiceTest.cpp
    ${CMAKE_SOURCE_DIR}/src/services/MonitoringService.cpp
    ${CMAKE_SOURCE_DIR}/include/services/MonitoringService.h
    ${CMAKE_SOURCE_DIR}/src/services/AlertService.cpp
    ${CMAKE_SOURCE_DIR}/include/services/AlertService.h
    ${CMAKE_SOURCE_DIR}/src/services/HistoryService.cpp
    ${CMAKE_SOURCE_DIR}/include/services/HistoryService.h
    ${CMAKE_SOURCE_DIR}/src/database/DatabaseManager.cpp
    ${CMAKE_SOURCE_DIR}/src/models/Alert.cpp
    ${CMAKE_SOURCE_DIR}/src/models/NetworkMetrics.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/Logger.cpp
)
target_include_directories(MonitoringServiceTest PRIVATE
    ${CMAKE_SOURCE_DIR}/include/services
    ${CMAKE_SOURCE_DIR}/include/controllers
    ${CMAKE_SOURCE_DIR}/include/database
    ${CMAKE_SOURCE_DIR}/include/models
)
target_link_libraries(MonitoringServiceTest PRIVATE Qt6::Test Qt6::Core Qt6::Network Qt6::Sql Qt6::Gui)
add_test(NAME MonitoringServiceTest COMMAND MonitoringServiceTest)

# Phase 8.1: Wake-on-LAN tests
add_executable(WakeOnLanServiceTest
    WakeOnLanServiceTest.cpp
    ${CMAKE_SOURCE_DIR}/src/services/WakeOnLanService.cpp
    ${CMAKE_SOURCE_DIR}/include/services/WakeOnLanService.h
    ${CMAKE_SOURCE_DIR}/src/models/Device.cpp
    ${CMAKE_SOURCE_DIR}/src/models/PortInfo.cpp
    ${CMAKE_SOURCE_DIR}/src/models/NetworkMetrics.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/Logger.cpp
)
target_include_directories(WakeOnLanServiceTest PRIVATE
    ${CMAKE_SOURCE_DIR}/include/services
    ${CMAKE_SOURCE_DIR}/include/models
)
target_link_libraries(WakeOnLanServiceTest PRIVATE Qt6::Test Qt6::Core Qt6::Network)
add_test(NAME WakeOnLanServiceTest COMMAND WakeOnLanServiceTest)

# Phase 8.2: Advanced Export tests
add_executable(XmlExporterTest
    XmlExporterTest.cpp
    ${CMAKE_SOURCE_DIR}/src/export/XmlExporter.cpp
    ${CMAKE_SOURCE_DIR}/include/export/XmlExporter.h
    ${CMAKE_SOURCE_DIR}/src/models/Device.cpp
    ${CMAKE_SOURCE_DIR}/src/models/PortInfo.cpp
    ${CMAKE_SOURCE_DIR}/src/models/NetworkMetrics.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/Logger.cpp
)
target_include_directories(XmlExporterTest PRIVATE
    ${CMAKE_SOURCE_DIR}/include/export
    ${CMAKE_SOURCE_DIR}/include/models
)
target_link_libraries(XmlExporterTest PRIVATE Qt6::Test Qt6::Core)
add_test(NAME XmlExporterTest COMMAND XmlExporterTest)

add_executable(HtmlReportGeneratorTest
    HtmlReportGeneratorTest.cpp
    ${CMAKE_SOURCE_DIR}/src/export/HtmlReportGenerator.cpp
    ${CMAKE_SOURCE_DIR}/include/export/HtmlReportGenerator.h
    ${CMAKE_SOURCE_DIR}/src/models/Device.cpp
    ${CMAKE_SOURCE_DIR}/src/models/PortInfo.cpp
    ${CMAKE_SOURCE_DIR}/src/models/NetworkMetrics.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/Logger.cpp
)
target_include_directories(HtmlReportGeneratorTest PRIVATE
    ${CMAKE_SOURCE_DIR}/include/export
    ${CMAKE_SOURCE_DIR}/include/models
)
target_link_libraries(HtmlReportGeneratorTest PRIVATE Qt6::Test Qt6::Core)
add_test(NAME HtmlReportGeneratorTest COMMAND HtmlReportGeneratorTest)

# Phase 8.4: Database DAO tests
add_executable(HistoryDaoTest
    HistoryDaoTest.cpp
    ${CMAKE_SOURCE_DIR}/src/database/DatabaseManager.cpp
    ${CMAKE_SOURCE_DIR}/src/database/HistoryDao.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/Logger.cpp
)
target_include_directories(HistoryDaoTest PRIVATE
    ${CMAKE_SOURCE_DIR}/include/database
    ${CMAKE_SOURCE_DIR}/include/utils
    ${CMAKE_SOURCE_DIR}/include/models
)
target_link_libraries(HistoryDaoTest PRIVATE Qt6::Test Qt6::Core Qt6::Sql)
add_test(NAME HistoryDaoTest COMMAND HistoryDaoTest)

add_executable(MetricsDaoTest
    MetricsDaoTest.cpp
    ${CMAKE_SOURCE_DIR}/src/database/DatabaseManager.cpp
    ${CMAKE_SOURCE_DIR}/src/database/MetricsDao.cpp
    ${CMAKE_SOURCE_DIR}/src/models/NetworkMetrics.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/Logger.cpp
)
target_include_directories(MetricsDaoTest PRIVATE
    ${CMAKE_SOURCE_DIR}/include/database
    ${CMAKE_SOURCE_DIR}/include/utils
    ${CMAKE_SOURCE_DIR}/include/models
)
target_link_libraries(MetricsDaoTest PRIVATE Qt6::Test Qt6::Core Qt6::Sql)
add_test(NAME MetricsDaoTest COMMAND MetricsDaoTest)

# Phase 9.1: Theme Manager tests
add_executable(ThemeManagerTest
    ThemeManagerTest.cpp
    ${CMAKE_SOURCE_DIR}/src/managers/ThemeManager.cpp
    ${CMAKE_SOURCE_DIR}/include/managers/ThemeManager.h
    ${CMAKE_SOURCE_DIR}/src/utils/Logger.cpp
    ${CMAKE_SOURCE_DIR}/resources/resources.qrc
)
target_include_directories(ThemeManagerTest PRIVATE
    ${CMAKE_SOURCE_DIR}/include/managers
    ${CMAKE_SOURCE_DIR}/include/utils
)
target_link_libraries(ThemeManagerTest PRIVATE Qt6::Test Qt6::Core Qt6::Widgets)
add_test(NAME ThemeManagerTest COMMAND ThemeManagerTest)

# Phase 10: Controller Tests
add_executable(ScanControllerTest
    ScanControllerTest.cpp
    ${CMAKE_SOURCE_DIR}/src/controllers/ScanController.cpp
    ${CMAKE_SOURCE_DIR}/src/coordinators/ScanCoordinator.cpp
    ${CMAKE_SOURCE_DIR}/src/database/DeviceRepository.cpp
    ${CMAKE_SOURCE_DIR}/src/database/DeviceCache.cpp
    ${CMAKE_SOURCE_DIR}/src/database/DatabaseManager.cpp
    ${CMAKE_SOURCE_DIR}/src/models/Device.cpp
    ${CMAKE_SOURCE_DIR}/src/models/PortInfo.cpp
    ${CMAKE_SOURCE_DIR}/src/models/NetworkMetrics.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/Logger.cpp
)
target_include_directories(ScanControllerTest PRIVATE
    ${CMAKE_SOURCE_DIR}/include/controllers
    ${CMAKE_SOURCE_DIR}/include/coordinators
    ${CMAKE_SOURCE_DIR}/include/database
    ${CMAKE_SOURCE_DIR}/include/models
    ${CMAKE_SOURCE_DIR}/include/utils
)
target_link_libraries(ScanControllerTest PRIVATE Qt6::Test Qt6::Core Qt6::Network Qt6::Sql Qt6::Concurrent)
add_test(NAME ScanControllerTest COMMAND ScanControllerTest)

add_executable(MetricsControllerTest
    MetricsControllerTest.cpp
    ${CMAKE_SOURCE_DIR}/src/controllers/MetricsController.cpp
    ${CMAKE_SOURCE_DIR}/src/network/diagnostics/MetricsAggregator.cpp
    ${CMAKE_SOURCE_DIR}/src/network/diagnostics/PingService.cpp
    ${CMAKE_SOURCE_DIR}/src/network/diagnostics/LatencyCalculator.cpp
    ${CMAKE_SOURCE_DIR}/src/network/diagnostics/JitterCalculator.cpp
    ${CMAKE_SOURCE_DIR}/src/network/diagnostics/PacketLossCalculator.cpp
    ${CMAKE_SOURCE_DIR}/src/network/diagnostics/QualityScoreCalculator.cpp
    ${CMAKE_SOURCE_DIR}/src/database/DeviceRepository.cpp
    ${CMAKE_SOURCE_DIR}/src/database/DatabaseManager.cpp
    ${CMAKE_SOURCE_DIR}/src/models/Device.cpp
    ${CMAKE_SOURCE_DIR}/src/models/PortInfo.cpp
    ${CMAKE_SOURCE_DIR}/src/models/NetworkMetrics.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/Logger.cpp
)
target_include_directories(MetricsControllerTest PRIVATE
    ${CMAKE_SOURCE_DIR}/include/controllers
    ${CMAKE_SOURCE_DIR}/include/network/diagnostics
    ${CMAKE_SOURCE_DIR}/include/database
    ${CMAKE_SOURCE_DIR}/include/models
    ${CMAKE_SOURCE_DIR}/include/utils
)
target_link_libraries(MetricsControllerTest PRIVATE Qt6::Test Qt6::Core Qt6::Network Qt6::Sql)
add_test(NAME MetricsControllerTest COMMAND MetricsControllerTest)

add_executable(ExportControllerTest
    ExportControllerTest.cpp
    ${CMAKE_SOURCE_DIR}/src/controllers/ExportController.cpp
    ${CMAKE_SOURCE_DIR}/src/export/CsvExporter.cpp
    ${CMAKE_SOURCE_DIR}/src/export/JsonExporter.cpp
    ${CMAKE_SOURCE_DIR}/src/export/XmlExporter.cpp
    ${CMAKE_SOURCE_DIR}/src/export/HtmlReportGenerator.cpp
    ${CMAKE_SOURCE_DIR}/src/database/DeviceRepository.cpp
    ${CMAKE_SOURCE_DIR}/src/database/DatabaseManager.cpp
    ${CMAKE_SOURCE_DIR}/src/models/Device.cpp
    ${CMAKE_SOURCE_DIR}/src/models/PortInfo.cpp
    ${CMAKE_SOURCE_DIR}/src/models/NetworkMetrics.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/Logger.cpp
)
target_include_directories(ExportControllerTest PRIVATE
    ${CMAKE_SOURCE_DIR}/include/controllers
    ${CMAKE_SOURCE_DIR}/include/export
    ${CMAKE_SOURCE_DIR}/include/interfaces
    ${CMAKE_SOURCE_DIR}/include/database
    ${CMAKE_SOURCE_DIR}/include/models
    ${CMAKE_SOURCE_DIR}/include/utils
)
target_link_libraries(ExportControllerTest PRIVATE Qt6::Test Qt6::Core Qt6::Sql)
add_test(NAME ExportControllerTest COMMAND ExportControllerTest)

# Phase 10: ViewModel Tests
add_executable(DeviceTableViewModelTest
    DeviceTableViewModelTest.cpp
    ${CMAKE_SOURCE_DIR}/src/viewmodels/DeviceTableViewModel.cpp
    ${CMAKE_SOURCE_DIR}/src/database/DeviceRepository.cpp
    ${CMAKE_SOURCE_DIR}/src/database/DatabaseManager.cpp
    ${CMAKE_SOURCE_DIR}/src/models/Device.cpp
    ${CMAKE_SOURCE_DIR}/src/models/PortInfo.cpp
    ${CMAKE_SOURCE_DIR}/src/models/NetworkMetrics.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/Logger.cpp
)
target_include_directories(DeviceTableViewModelTest PRIVATE
    ${CMAKE_SOURCE_DIR}/include/viewmodels
    ${CMAKE_SOURCE_DIR}/include/database
    ${CMAKE_SOURCE_DIR}/include/models
    ${CMAKE_SOURCE_DIR}/include/utils
    ${CMAKE_SOURCE_DIR}/src/models
)
target_link_libraries(DeviceTableViewModelTest PRIVATE Qt6::Test Qt6::Core Qt6::Sql Qt6::Widgets)
add_test(NAME DeviceTableViewModelTest COMMAND DeviceTableViewModelTest)

add_executable(MetricsViewModelTest
    MetricsViewModelTest.cpp
    ${CMAKE_SOURCE_DIR}/src/viewmodels/MetricsViewModel.cpp
    ${CMAKE_SOURCE_DIR}/src/controllers/MetricsController.cpp
    ${CMAKE_SOURCE_DIR}/src/network/diagnostics/MetricsAggregator.cpp
    ${CMAKE_SOURCE_DIR}/src/network/diagnostics/PingService.cpp
    ${CMAKE_SOURCE_DIR}/src/network/diagnostics/LatencyCalculator.cpp
    ${CMAKE_SOURCE_DIR}/src/network/diagnostics/JitterCalculator.cpp
    ${CMAKE_SOURCE_DIR}/src/network/diagnostics/PacketLossCalculator.cpp
    ${CMAKE_SOURCE_DIR}/src/network/diagnostics/QualityScoreCalculator.cpp
    ${CMAKE_SOURCE_DIR}/src/models/Device.cpp
    ${CMAKE_SOURCE_DIR}/src/models/PortInfo.cpp
    ${CMAKE_SOURCE_DIR}/src/models/NetworkMetrics.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/Logger.cpp
)
target_include_directories(MetricsViewModelTest PRIVATE
    ${CMAKE_SOURCE_DIR}/include/viewmodels
    ${CMAKE_SOURCE_DIR}/include/controllers
    ${CMAKE_SOURCE_DIR}/include/network/diagnostics
    ${CMAKE_SOURCE_DIR}/include/interfaces
    ${CMAKE_SOURCE_DIR}/include/models
    ${CMAKE_SOURCE_DIR}/include/utils
    ${CMAKE_SOURCE_DIR}/src/models
)
target_link_libraries(MetricsViewModelTest PRIVATE Qt6::Test Qt6::Core Qt6::Network)
add_test(NAME MetricsViewModelTest COMMAND MetricsViewModelTest)

add_executable(ScanConfigViewModelTest
    ScanConfigViewModelTest.cpp
    ${CMAKE_SOURCE_DIR}/src/viewmodels/ScanConfigViewModel.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/IpAddressValidator.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/Logger.cpp
)
target_include_directories(ScanConfigViewModelTest PRIVATE
    ${CMAKE_SOURCE_DIR}/include/viewmodels
    ${CMAKE_SOURCE_DIR}/include/coordinators
    ${CMAKE_SOURCE_DIR}/include/utils
)
target_link_libraries(ScanConfigViewModelTest PRIVATE Qt6::Test Qt6::Core Qt6::Network)
add_test(NAME ScanConfigViewModelTest COMMAND ScanConfigViewModelTest)
