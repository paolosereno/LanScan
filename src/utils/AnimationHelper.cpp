#include "utils/AnimationHelper.h"

void AnimationHelper::fadeIn(QWidget* widget, int duration) {
    if (!widget) return;

    QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect(widget);
    widget->setGraphicsEffect(effect);

    QPropertyAnimation* animation = new QPropertyAnimation(effect, "opacity");
    animation->setDuration(duration);
    animation->setStartValue(0.0);
    animation->setEndValue(1.0);
    animation->setEasingCurve(QEasingCurve::InOutQuad);

    widget->show();
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void AnimationHelper::fadeOut(QWidget* widget, int duration) {
    if (!widget) return;

    QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect(widget);
    widget->setGraphicsEffect(effect);

    QPropertyAnimation* animation = new QPropertyAnimation(effect, "opacity");
    animation->setDuration(duration);
    animation->setStartValue(1.0);
    animation->setEndValue(0.0);
    animation->setEasingCurve(QEasingCurve::InOutQuad);

    // Hide widget when animation completes
    QObject::connect(animation, &QPropertyAnimation::finished, widget, &QWidget::hide);

    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void AnimationHelper::slideInFromLeft(QWidget* widget, int duration) {
    if (!widget) return;

    QRect finalGeometry = widget->geometry();
    QRect startGeometry = finalGeometry;
    startGeometry.moveLeft(-finalGeometry.width());

    widget->setGeometry(startGeometry);
    widget->show();

    QPropertyAnimation* animation = new QPropertyAnimation(widget, "geometry");
    animation->setDuration(duration);
    animation->setStartValue(startGeometry);
    animation->setEndValue(finalGeometry);
    animation->setEasingCurve(QEasingCurve::OutCubic);

    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void AnimationHelper::slideInFromRight(QWidget* widget, int duration) {
    if (!widget) return;

    QRect finalGeometry = widget->geometry();
    QRect startGeometry = finalGeometry;

    // Get parent width to slide from right edge
    if (widget->parentWidget()) {
        startGeometry.moveLeft(widget->parentWidget()->width());
    } else {
        startGeometry.moveLeft(finalGeometry.width() * 2);
    }

    widget->setGeometry(startGeometry);
    widget->show();

    QPropertyAnimation* animation = new QPropertyAnimation(widget, "geometry");
    animation->setDuration(duration);
    animation->setStartValue(startGeometry);
    animation->setEndValue(finalGeometry);
    animation->setEasingCurve(QEasingCurve::OutCubic);

    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void AnimationHelper::expandHeight(QWidget* widget, int targetHeight, int duration) {
    if (!widget) return;

    int startHeight = widget->height();

    QPropertyAnimation* animation = new QPropertyAnimation(widget, "maximumHeight");
    animation->setDuration(duration);
    animation->setStartValue(startHeight);
    animation->setEndValue(targetHeight);
    animation->setEasingCurve(QEasingCurve::InOutQuad);

    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void AnimationHelper::collapseHeight(QWidget* widget, int duration) {
    if (!widget) return;

    int startHeight = widget->height();

    QPropertyAnimation* animation = new QPropertyAnimation(widget, "maximumHeight");
    animation->setDuration(duration);
    animation->setStartValue(startHeight);
    animation->setEndValue(0);
    animation->setEasingCurve(QEasingCurve::InOutQuad);

    // Hide widget when animation completes
    QObject::connect(animation, &QPropertyAnimation::finished, widget, &QWidget::hide);

    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void AnimationHelper::bounce(QWidget* widget, int duration) {
    if (!widget) return;

    QRect originalGeometry = widget->geometry();
    QRect bounceGeometry = originalGeometry;
    bounceGeometry.moveTop(originalGeometry.top() - 20);

    QPropertyAnimation* animation = new QPropertyAnimation(widget, "geometry");
    animation->setDuration(duration);
    animation->setStartValue(originalGeometry);
    animation->setKeyValueAt(0.5, bounceGeometry);
    animation->setEndValue(originalGeometry);
    animation->setEasingCurve(QEasingCurve::OutBounce);

    animation->start(QAbstractAnimation::DeleteWhenStopped);
}
