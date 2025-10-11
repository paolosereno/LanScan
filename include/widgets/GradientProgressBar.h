#ifndef GRADIENTPROGRESSBAR_H
#define GRADIENTPROGRESSBAR_H

#include <QProgressBar>
#include <QPainter>
#include <QLinearGradient>
#include <QPropertyAnimation>

/**
 * @brief Enhanced progress bar with gradient colors and smooth animations
 *
 * The GradientProgressBar extends QProgressBar with:
 * - Smooth gradient colors based on progress percentage
 * - Animated value transitions
 * - Modern appearance with rounded corners
 * - Theme-aware styling
 *
 * Color scheme:
 * - 0-30%: Red gradient (slow progress)
 * - 31-70%: Orange/Yellow gradient (moderate progress)
 * - 71-100%: Green gradient (good progress)
 */
class GradientProgressBar : public QProgressBar {
    Q_OBJECT
    Q_PROPERTY(int animatedValue READ animatedValue WRITE setAnimatedValue)

public:
    /**
     * @brief Constructs a GradientProgressBar
     * @param parent Parent widget
     */
    explicit GradientProgressBar(QWidget* parent = nullptr);

    /**
     * @brief Sets whether to use animated transitions
     * @param enabled True to enable smooth animations
     */
    void setAnimated(bool enabled);

    /**
     * @brief Checks if animations are enabled
     * @return True if animations are enabled
     */
    bool isAnimated() const;

    /**
     * @brief Sets the animation duration
     * @param milliseconds Duration in milliseconds (default: 300)
     */
    void setAnimationDuration(int milliseconds);

    /**
     * @brief Sets the progress value with optional animation
     * @param value New value (respects minimum and maximum)
     */
    void setValue(int value);

protected:
    /**
     * @brief Custom paint event for gradient rendering
     */
    void paintEvent(QPaintEvent* event) override;

private:
    bool m_animated;                      ///< Animation enabled flag
    int m_animatedValue;                  ///< Current animated value
    QPropertyAnimation* m_animation;      ///< Value animation object
    int m_animationDuration;              ///< Animation duration in ms

    /**
     * @brief Gets the animated value (for property animation)
     * @return Current animated value
     */
    int animatedValue() const;

    /**
     * @brief Sets the animated value (for property animation)
     * @param value New animated value
     */
    void setAnimatedValue(int value);

    /**
     * @brief Gets gradient colors based on progress percentage
     * @param percent Progress percentage (0-100)
     * @return Linear gradient for the given percentage
     */
    QLinearGradient getGradientForPercent(int percent) const;

    /**
     * @brief Draws the progress bar background
     * @param painter QPainter instance
     * @param rect Drawing rectangle
     */
    void drawBackground(QPainter* painter, const QRect& rect);

    /**
     * @brief Draws the progress bar chunk (filled portion)
     * @param painter QPainter instance
     * @param rect Drawing rectangle
     */
    void drawChunk(QPainter* painter, const QRect& rect);

    /**
     * @brief Draws the progress text
     * @param painter QPainter instance
     * @param rect Drawing rectangle
     */
    void drawText(QPainter* painter, const QRect& rect);
};

#endif // GRADIENTPROGRESSBAR_H
