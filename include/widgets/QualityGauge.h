#ifndef QUALITYGAUGE_H
#define QUALITYGAUGE_H

#include <QWidget>
#include <QPainter>
#include <QColor>

/**
 * @brief Circular gauge widget for displaying quality score (0-100)
 *
 * The QualityGauge provides a visual representation of network quality
 * with color-coded arcs and a needle indicator.
 *
 * Color mapping:
 * - Green (#4CAF50): 90-100 (Excellent)
 * - Light Green (#8BC34A): 70-89 (Good)
 * - Orange (#FF9800): 50-69 (Fair)
 * - Red (#F44336): 0-49 (Poor)
 */
class QualityGauge : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Constructs a QualityGauge widget
     * @param parent Parent widget
     */
    explicit QualityGauge(QWidget* parent = nullptr);

    /**
     * @brief Sets the current value of the gauge
     * @param value Value between 0 and 100
     */
    void setValue(int value);

    /**
     * @brief Gets the current value
     * @return Current gauge value (0-100)
     */
    int getValue() const;

    /**
     * @brief Sets the quality level text
     * @param quality Quality description (e.g., "Excellent", "Good", "Fair", "Poor")
     */
    void setQualityLevel(const QString& quality);

    /**
     * @brief Gets the current quality level text
     * @return Quality level description
     */
    QString getQualityLevel() const;

    /**
     * @brief Recommended minimum size for the widget
     */
    QSize sizeHint() const override;

    /**
     * @brief Recommended minimum size
     */
    QSize minimumSizeHint() const override;

protected:
    /**
     * @brief Custom paint event to draw the gauge
     */
    void paintEvent(QPaintEvent* event) override;

private:
    int m_value;              ///< Current value (0-100)
    QString m_qualityLevel;   ///< Quality level description

    /**
     * @brief Draws the gauge background and colored arc
     * @param painter QPainter instance
     */
    void drawGauge(QPainter* painter);

    /**
     * @brief Draws the needle indicator
     * @param painter QPainter instance
     */
    void drawNeedle(QPainter* painter);

    /**
     * @brief Draws the value text and quality level
     * @param painter QPainter instance
     */
    void drawValue(QPainter* painter);

    /**
     * @brief Gets the appropriate color for a given value
     * @param value Quality value (0-100)
     * @return QColor corresponding to the value
     */
    QColor getColorForValue(int value) const;

    /**
     * @brief Automatically determines quality level from value
     * @param value Quality value (0-100)
     * @return Quality level string
     */
    QString getQualityLevelFromValue(int value) const;
};

#endif // QUALITYGAUGE_H
