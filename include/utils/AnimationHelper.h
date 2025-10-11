#ifndef ANIMATIONHELPER_H
#define ANIMATIONHELPER_H

#include <QWidget>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QEasingCurve>

/**
 * @brief Utility class for creating smooth UI animations
 *
 * AnimationHelper provides static methods for common UI animations
 * such as fade-in, fade-out, and slide transitions.
 */
class AnimationHelper {
public:
    /**
     * @brief Fades in a widget
     * @param widget Widget to animate
     * @param duration Animation duration in milliseconds (default: 300)
     */
    static void fadeIn(QWidget* widget, int duration = 300);

    /**
     * @brief Fades out a widget
     * @param widget Widget to animate
     * @param duration Animation duration in milliseconds (default: 300)
     */
    static void fadeOut(QWidget* widget, int duration = 300);

    /**
     * @brief Slides a widget in from left
     * @param widget Widget to animate
     * @param duration Animation duration in milliseconds (default: 300)
     */
    static void slideInFromLeft(QWidget* widget, int duration = 300);

    /**
     * @brief Slides a widget in from right
     * @param widget Widget to animate
     * @param duration Animation duration in milliseconds (default: 300)
     */
    static void slideInFromRight(QWidget* widget, int duration = 300);

    /**
     * @brief Animates widget height expansion
     * @param widget Widget to animate
     * @param targetHeight Target height in pixels
     * @param duration Animation duration in milliseconds (default: 300)
     */
    static void expandHeight(QWidget* widget, int targetHeight, int duration = 300);

    /**
     * @brief Animates widget height collapse
     * @param widget Widget to animate
     * @param duration Animation duration in milliseconds (default: 300)
     */
    static void collapseHeight(QWidget* widget, int duration = 300);

    /**
     * @brief Creates a bounce effect on widget
     * @param widget Widget to animate
     * @param duration Animation duration in milliseconds (default: 500)
     */
    static void bounce(QWidget* widget, int duration = 500);

private:
    AnimationHelper() = delete; // Utility class, no instantiation
};

#endif // ANIMATIONHELPER_H
