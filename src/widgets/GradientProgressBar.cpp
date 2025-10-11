#include "widgets/GradientProgressBar.h"
#include <QPaintEvent>
#include <QPainterPath>

GradientProgressBar::GradientProgressBar(QWidget* parent)
    : QProgressBar(parent)
    , m_animated(true)
    , m_animatedValue(0)
    , m_animationDuration(300)
{
    // Setup animation
    m_animation = new QPropertyAnimation(this, "animatedValue");
    m_animation->setDuration(m_animationDuration);
    m_animation->setEasingCurve(QEasingCurve::OutCubic);

    // Set default appearance
    setTextVisible(true);
    setMinimumHeight(20);
}

void GradientProgressBar::setAnimated(bool enabled) {
    m_animated = enabled;
}

bool GradientProgressBar::isAnimated() const {
    return m_animated;
}

void GradientProgressBar::setAnimationDuration(int milliseconds) {
    m_animationDuration = milliseconds;
    m_animation->setDuration(milliseconds);
}

void GradientProgressBar::setValue(int value) {
    // Call base implementation
    QProgressBar::setValue(value);

    if (m_animated) {
        // Animate to new value
        m_animation->stop();
        m_animation->setStartValue(m_animatedValue);
        m_animation->setEndValue(value);
        m_animation->start();
    } else {
        // Set immediately
        m_animatedValue = value;
        update();
    }
}

int GradientProgressBar::animatedValue() const {
    return m_animatedValue;
}

void GradientProgressBar::setAnimatedValue(int value) {
    m_animatedValue = value;
    update(); // Trigger repaint
}

void GradientProgressBar::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect rect = this->rect();

    // Draw background and chunk
    drawBackground(&painter, rect);
    drawChunk(&painter, rect);

    // Draw text if visible
    if (isTextVisible()) {
        drawText(&painter, rect);
    }
}

void GradientProgressBar::drawBackground(QPainter* painter, const QRect& rect) {
    // Background with subtle border
    painter->setPen(QPen(palette().dark().color(), 1));
    painter->setBrush(palette().base().color());

    QPainterPath path;
    path.addRoundedRect(rect, 4, 4);
    painter->drawPath(path);
}

void GradientProgressBar::drawChunk(QPainter* painter, const QRect& rect) {
    if (maximum() == minimum()) {
        return; // Avoid division by zero
    }

    // Calculate progress percentage
    int range = maximum() - minimum();
    int currentValue = m_animated ? m_animatedValue : value();
    int percent = ((currentValue - minimum()) * 100) / range;

    // Calculate chunk width
    int chunkWidth = (rect.width() * (currentValue - minimum())) / range;

    if (chunkWidth <= 0) {
        return; // Nothing to draw
    }

    // Get gradient for current percentage
    QLinearGradient gradient = getGradientForPercent(percent);
    gradient.setStart(rect.topLeft());
    gradient.setFinalStop(rect.topRight());

    // Draw chunk with rounded corners
    QRect chunkRect = rect.adjusted(0, 0, -(rect.width() - chunkWidth), 0);

    painter->setPen(Qt::NoPen);
    painter->setBrush(gradient);

    QPainterPath path;
    path.addRoundedRect(chunkRect, 4, 4);
    painter->drawPath(path);
}

void GradientProgressBar::drawText(QPainter* painter, const QRect& rect) {
    painter->setPen(palette().text().color());

    QFont font = painter->font();
    font.setBold(true);
    painter->setFont(font);

    QString progressText = text();
    painter->drawText(rect, Qt::AlignCenter, progressText);
}

QLinearGradient GradientProgressBar::getGradientForPercent(int percent) const {
    QLinearGradient gradient;

    if (percent <= 30) {
        // Red gradient for low progress
        gradient.setColorAt(0.0, QColor("#F44336")); // Red
        gradient.setColorAt(1.0, QColor("#E53935")); // Darker red
    } else if (percent <= 70) {
        // Orange/Yellow gradient for moderate progress
        gradient.setColorAt(0.0, QColor("#FF9800")); // Orange
        gradient.setColorAt(1.0, QColor("#FB8C00")); // Darker orange
    } else {
        // Green gradient for good progress
        gradient.setColorAt(0.0, QColor("#4CAF50")); // Green
        gradient.setColorAt(1.0, QColor("#43A047")); // Darker green
    }

    return gradient;
}
