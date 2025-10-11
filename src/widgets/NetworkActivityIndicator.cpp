#include "widgets/NetworkActivityIndicator.h"
#include <QPaintEvent>
#include <QRadialGradient>

NetworkActivityIndicator::NetworkActivityIndicator(QWidget* parent)
    : QWidget(parent)
    , m_currentState(Off)
    , m_color(QColor("#4CAF50")) // Green by default
    , m_blinkState(false)
{
    setFixedSize(20, 20);

    // Setup blink timer
    m_blinkTimer = new QTimer(this);
    m_blinkTimer->setInterval(500); // 500ms default blink interval
    connect(m_blinkTimer, &QTimer::timeout, this, &NetworkActivityIndicator::onBlinkTimer);
}

void NetworkActivityIndicator::setState(State state) {
    m_currentState = state;

    if (state == Blinking) {
        m_blinkTimer->start();
    } else {
        m_blinkTimer->stop();
        m_blinkState = (state == On);
    }

    update(); // Trigger repaint
}

NetworkActivityIndicator::State NetworkActivityIndicator::getState() const {
    return m_currentState;
}

void NetworkActivityIndicator::setColor(const QColor& color) {
    m_color = color;
    update();
}

QColor NetworkActivityIndicator::getColor() const {
    return m_color;
}

void NetworkActivityIndicator::setBlinkInterval(int milliseconds) {
    m_blinkTimer->setInterval(milliseconds);
}

QSize NetworkActivityIndicator::sizeHint() const {
    return QSize(20, 20);
}

QSize NetworkActivityIndicator::minimumSizeHint() const {
    return QSize(16, 16);
}

void NetworkActivityIndicator::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    drawLed(&painter);
}

void NetworkActivityIndicator::drawLed(QPainter* painter) {
    QRect rect = this->rect().adjusted(2, 2, -2, -2);

    // Determine if LED should be lit
    bool isLit = (m_currentState == On) || (m_currentState == Blinking && m_blinkState);

    if (isLit) {
        // LED is on - draw with radial gradient for glow effect
        QRadialGradient gradient(rect.center(), rect.width() / 2.0);
        gradient.setColorAt(0.0, m_color.lighter(150));
        gradient.setColorAt(0.5, m_color);
        gradient.setColorAt(1.0, m_color.darker(120));

        painter->setBrush(gradient);
        painter->setPen(QPen(m_color.darker(150), 1));
    } else {
        // LED is off - draw dark
        painter->setBrush(m_color.darker(300));
        painter->setPen(QPen(QColor("#808080"), 1));
    }

    painter->drawEllipse(rect);

    // Add highlight for 3D effect when lit
    if (isLit) {
        QRect highlightRect = rect.adjusted(2, 2, -rect.width() / 2, -rect.height() / 2);
        QRadialGradient highlightGradient(highlightRect.center(), highlightRect.width());
        highlightGradient.setColorAt(0.0, QColor(255, 255, 255, 180));
        highlightGradient.setColorAt(1.0, QColor(255, 255, 255, 0));

        painter->setBrush(highlightGradient);
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(highlightRect);
    }
}

void NetworkActivityIndicator::onBlinkTimer() {
    m_blinkState = !m_blinkState;
    update();
}
