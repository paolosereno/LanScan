#ifndef NETWORKACTIVITYINDICATOR_H
#define NETWORKACTIVITYINDICATOR_H

#include <QWidget>
#include <QPainter>
#include <QTimer>
#include <QColor>

/**
 * @brief Animated LED status indicator for network activity
 *
 * The NetworkActivityIndicator displays a circular LED that can be:
 * - Off: Inactive state (dark)
 * - On: Active state (solid color)
 * - Blinking: Activity state (animated blinking)
 *
 * Useful for showing network scanning status in the status bar.
 */
class NetworkActivityIndicator : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief LED states
     */
    enum State {
        Off,      ///< LED is off (dark)
        On,       ///< LED is on (solid)
        Blinking  ///< LED is blinking (animated)
    };
    Q_ENUM(State)

    /**
     * @brief Constructs a NetworkActivityIndicator widget
     * @param parent Parent widget
     */
    explicit NetworkActivityIndicator(QWidget* parent = nullptr);

    /**
     * @brief Sets the LED state
     * @param state New state (Off/On/Blinking)
     */
    void setState(State state);

    /**
     * @brief Gets the current LED state
     * @return Current state
     */
    State getState() const;

    /**
     * @brief Sets the LED color
     * @param color Color for the LED (default: green)
     */
    void setColor(const QColor& color);

    /**
     * @brief Gets the current LED color
     * @return Current color
     */
    QColor getColor() const;

    /**
     * @brief Sets the blink interval
     * @param milliseconds Blink interval in milliseconds (default: 500)
     */
    void setBlinkInterval(int milliseconds);

    /**
     * @brief Recommended size for the widget
     */
    QSize sizeHint() const override;

    /**
     * @brief Minimum size for the widget
     */
    QSize minimumSizeHint() const override;

protected:
    /**
     * @brief Custom paint event to draw the LED
     */
    void paintEvent(QPaintEvent* event) override;

private slots:
    /**
     * @brief Timer slot for blinking animation
     */
    void onBlinkTimer();

private:
    State m_currentState;     ///< Current LED state
    QColor m_color;           ///< LED color
    bool m_blinkState;        ///< Current blink state (true = on, false = off)
    QTimer* m_blinkTimer;     ///< Timer for blinking animation

    /**
     * @brief Draws the LED indicator
     * @param painter QPainter instance
     */
    void drawLed(QPainter* painter);
};

#endif // NETWORKACTIVITYINDICATOR_H
