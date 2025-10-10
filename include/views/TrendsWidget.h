#ifndef TRENDSWIDGET_H
#define TRENDSWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QPushButton>
#include <QLabel>
#include "database/MetricsDao.h"
#include "charts/LatencyChart.h"

/**
 * @brief Widget for displaying historical metrics trends
 *
 * Provides temporal visualization of network metrics with configurable
 * time ranges and chart views.
 */
class TrendsWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param metricsDao Metrics DAO instance
     * @param parent Parent widget
     */
    explicit TrendsWidget(MetricsDao* metricsDao, QWidget* parent = nullptr);

    /**
     * @brief Destructor
     */
    ~TrendsWidget();

    /**
     * @brief Set the device to display trends for
     * @param deviceId Device identifier
     */
    void setDevice(const QString& deviceId);

    /**
     * @brief Set custom date range
     * @param start Start date/time
     * @param end End date/time
     */
    void setDateRange(const QDateTime& start, const QDateTime& end);

    /**
     * @brief Refresh the trends display
     */
    void refresh();

signals:
    /**
     * @brief Emitted when trends are loaded
     * @param metricsCount Number of metrics loaded
     */
    void trendsLoaded(int metricsCount);

    /**
     * @brief Emitted when an error occurs
     * @param error Error message
     */
    void errorOccurred(const QString& error);

private slots:
    void onLoadTrends();
    void onTimeRangeChanged(int index);
    void onStartDateChanged(const QDateTime& dateTime);
    void onEndDateChanged(const QDateTime& dateTime);
    void onRefreshClicked();
    void onExportClicked();

private:
    MetricsDao* metricsDao;
    QString currentDeviceId;
    QDateTime startDate;
    QDateTime endDate;

    // UI components
    LatencyChart* trendsChart;
    QComboBox* timeRangeCombo;
    QDateTimeEdit* startDateEdit;
    QDateTimeEdit* endDateEdit;
    QPushButton* refreshButton;
    QPushButton* exportButton;
    QLabel* deviceLabel;
    QLabel* statsLabel;

    void setupUI();
    void setupConnections();
    void loadHistoricalData();
    void displayTrends(const QList<NetworkMetrics>& metrics);
    void updateStatistics(const QList<NetworkMetrics>& metrics);
    void calculateAndDisplayDateRange(int rangeIndex);
};

#endif // TRENDSWIDGET_H
