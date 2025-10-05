#include <QApplication>
#include "views/MainWindow.h"
#include "controllers/ScanController.h"
#include "controllers/MetricsController.h"
#include "controllers/ExportController.h"
#include "coordinators/ScanCoordinator.h"
#include "../database/DatabaseManager.h"
#include "../database/DeviceRepository.h"
#include "../database/DeviceCache.h"
#include "../network/scanner/IpScanner.h"
#include "../network/diagnostics/PortScanner.h"
#include "../network/diagnostics/MetricsAggregator.h"
#include "../network/diagnostics/LatencyCalculator.h"
#include "../network/diagnostics/JitterCalculator.h"
#include "../network/diagnostics/PacketLossCalculator.h"
#include "../network/diagnostics/QualityScoreCalculator.h"
#include "../network/diagnostics/PingService.h"
#include "../export/CsvExporter.h"
#include "../export/JsonExporter.h"
#include "../utils/Logger.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("LanScan");
    app.setApplicationVersion("0.5.0-phase5");
    app.setOrganizationName("Paolo Sereno");

    // Initialize logger
    Logger::setLogLevel(Logger::DEBUG);  // Enable DEBUG logging
    Logger::info("LanScan v0.5.0-phase5 starting...");

    // ========== Infrastructure Setup ==========

    // Database
    DatabaseManager* db = DatabaseManager::instance();
    if (!db->open("lanscan.db")) {
        Logger::error("Failed to open database");
        return 1;
    }

    // Repositories and cache
    DeviceRepository* deviceRepo = new DeviceRepository(db);
    DeviceCache* cache = new DeviceCache();

    // ========== Network Services Setup ==========

    IpScanner* ipScanner = new IpScanner();
    PortScanner* portScanner = new PortScanner();

    // Metrics calculators
    PingService* pingService = new PingService();
    LatencyCalculator* latencyCalc = new LatencyCalculator();
    JitterCalculator* jitterCalc = new JitterCalculator();
    PacketLossCalculator* packetLossCalc = new PacketLossCalculator();
    QualityScoreCalculator* qualityCalc = new QualityScoreCalculator();

    MetricsAggregator* metricsAgg = new MetricsAggregator(
        latencyCalc,
        jitterCalc,
        packetLossCalc,
        qualityCalc
    );

    // ========== Coordinators Setup ==========

    ScanCoordinator* scanCoord = new ScanCoordinator(
        ipScanner,
        portScanner,
        metricsAgg
    );

    // ========== Controllers Setup ==========

    ScanController* scanCtrl = new ScanController(
        scanCoord,
        deviceRepo,
        cache
    );

    MetricsController* metricsCtrl = new MetricsController(
        metricsAgg,
        deviceRepo
    );

    // Exporters
    CsvExporter* csvExporter = new CsvExporter();
    JsonExporter* jsonExporter = new JsonExporter();

    ExportController* exportCtrl = new ExportController(deviceRepo);

    // ========== Main Window Setup ==========

    MainWindow mainWindow(
        scanCtrl,
        metricsCtrl,
        exportCtrl,
        deviceRepo
    );

    mainWindow.show();

    Logger::info("Application initialized successfully");

    int result = app.exec();

    // Cleanup
    delete exportCtrl;
    delete jsonExporter;
    delete csvExporter;
    delete metricsCtrl;
    delete scanCtrl;
    delete scanCoord;
    delete metricsAgg;
    delete qualityCalc;
    delete packetLossCalc;
    delete jitterCalc;
    delete latencyCalc;
    delete pingService;
    delete portScanner;
    delete ipScanner;
    delete cache;
    delete deviceRepo;
    db->close();

    Logger::info("Application terminated");

    return result;
}
