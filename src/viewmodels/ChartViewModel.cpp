#include "viewmodels/ChartViewModel.h"
#include "controllers/MetricsController.h"
#include "utils/Logger.h"

ChartViewModel::ChartViewModel(MetricsController* metricsController, QObject* parent)
    : QObject(parent)
    , metricsController(metricsController)
    , maxDataPoints(60) // Default: 60 seconds of data at 1 Hz sampling
{
    if (!metricsController) {
        Logger::warn("ChartViewModel: Created with null MetricsController");
    }
}

void ChartViewModel::setMaxDataPoints(int max) {
    if (max < 1) {
        Logger::warn(QString("ChartViewModel: Invalid maxDataPoints: %1, using default 60").arg(max));
        maxDataPoints = 60;
        return;
    }

    if (max > 10000) {
        Logger::warn(QString("ChartViewModel: MaxDataPoints too large: %1, capping at 10000").arg(max));
        maxDataPoints = 10000;
    } else {
        maxDataPoints = max;
    }

    Logger::debug(QString("ChartViewModel: MaxDataPoints set to %1").arg(maxDataPoints));
}

int ChartViewModel::getMaxDataPoints() const {
    return maxDataPoints;
}
