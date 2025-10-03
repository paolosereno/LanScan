#include <QApplication>
#include <QMainWindow>
#include "utils/Logger.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Initialize logger
    Logger::info("LanScan v1.0.0 starting...");

    // TODO: Initialize main window when UI is ready
    // MainWindow window;
    // window.show();

    Logger::info("Application initialized successfully");

    return app.exec();
}
