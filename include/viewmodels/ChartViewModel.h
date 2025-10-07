#ifndef CHARTVIEWMODEL_H
#define CHARTVIEWMODEL_H

#include <QObject>
#include "models/NetworkMetrics.h"

class MetricsController;

/**
 * @class ChartViewModel
 * @brief Base ViewModel for chart components
 *
 * This abstract class provides the foundation for all chart ViewModels,
 * managing data updates, data point limits, and common chart operations.
 */
class ChartViewModel : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param metricsController Pointer to the MetricsController
     * @param parent Optional parent QObject
     */
    explicit ChartViewModel(MetricsController* metricsController, QObject* parent = nullptr);

    /**
     * @brief Virtual destructor
     */
    virtual ~ChartViewModel() = default;

    /**
     * @brief Update chart with new metrics data
     * @param metrics The network metrics to display
     *
     * Pure virtual method that must be implemented by derived classes
     * to handle chart-specific data updates.
     */
    virtual void updateChart(const NetworkMetrics& metrics) = 0;

    /**
     * @brief Clear all chart data
     *
     * Pure virtual method that must be implemented by derived classes
     * to reset chart state and remove all displayed data.
     */
    virtual void clearChart() = 0;

    /**
     * @brief Set maximum number of data points to display
     * @param max Maximum data points (default: 60)
     */
    void setMaxDataPoints(int max);

    /**
     * @brief Get current maximum data points setting
     * @return Maximum number of data points
     */
    int getMaxDataPoints() const;

signals:
    /**
     * @brief Emitted when chart data has been updated
     *
     * This signal can be used by views to refresh their display
     * or perform post-update operations.
     */
    void chartDataUpdated();

    /**
     * @brief Emitted when chart has been cleared
     */
    void chartCleared();

protected:
    MetricsController* metricsController;  ///< Pointer to metrics controller
    int maxDataPoints;                     ///< Maximum number of data points to retain

    /**
     * @brief Helper method to prune data lists to max size
     * @tparam T Type of data container (e.g., QList<QPointF>)
     * @param dataList Reference to the data list to prune
     */
    template<typename T>
    void pruneDataList(T& dataList) {
        while (dataList.size() > maxDataPoints) {
            dataList.removeFirst();
        }
    }
};

#endif // CHARTVIEWMODEL_H
