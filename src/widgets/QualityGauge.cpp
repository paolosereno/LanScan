#include "widgets/QualityGauge.h"
#include <QPaintEvent>
#include <QtMath>

QualityGauge::QualityGauge(QWidget* parent)
    : QWidget(parent)
    , m_value(0)
    , m_qualityLevel("Unknown")
{
    setMinimumSize(150, 150);
}

void QualityGauge::setValue(int value) {
    // Clamp value between 0 and 100
    m_value = qBound(0, value, 100);

    // Auto-update quality level if not manually set
    m_qualityLevel = getQualityLevelFromValue(m_value);

    update(); // Trigger repaint
}

int QualityGauge::getValue() const {
    return m_value;
}

void QualityGauge::setQualityLevel(const QString& quality) {
    m_qualityLevel = quality;
    update();
}

QString QualityGauge::getQualityLevel() const {
    return m_qualityLevel;
}

QSize QualityGauge::sizeHint() const {
    return QSize(200, 200);
}

QSize QualityGauge::minimumSizeHint() const {
    return QSize(150, 150);
}

void QualityGauge::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    drawGauge(&painter);
    drawNeedle(&painter);
    drawValue(&painter);
}

void QualityGauge::drawGauge(QPainter* painter) {
    int side = qMin(width(), height());
    int margin = 20;
    QRect rect((width() - side) / 2 + margin,
               (height() - side) / 2 + margin,
               side - 2 * margin,
               side - 2 * margin);

    // Draw background arc (gray)
    painter->setPen(QPen(QColor("#E0E0E0"), 12, Qt::SolidLine, Qt::RoundCap));
    painter->drawArc(rect, 30 * 16, 120 * 16); // 30° to 150° (120° span)

    // Draw colored arc based on value
    QColor color = getColorForValue(m_value);
    painter->setPen(QPen(color, 12, Qt::SolidLine, Qt::RoundCap));

    // Map value (0-100) to angle span (0-120°)
    int spanAngle = (m_value * 120) / 100;
    painter->drawArc(rect, 30 * 16, spanAngle * 16);
}

void QualityGauge::drawNeedle(QPainter* painter) {
    int centerX = width() / 2;
    int centerY = height() / 2;
    int radius = qMin(width(), height()) / 2 - 30;

    // Calculate needle angle using Qt coordinate system
    // Qt angles: 0° = 3 o'clock, 90° = 12 o'clock (counter-clockwise)
    // We want: 0% at 210° (7 o'clock), 100% at 330° (5 o'clock)
    // This creates a 120° arc from bottom-left to bottom-right
    double angle = 210.0 - (m_value * 120.0 / 100.0);
    double radians = qDegreesToRadians(angle);

    // Calculate needle endpoint
    int needleX = centerX + static_cast<int>(radius * qCos(radians));
    int needleY = centerY - static_cast<int>(radius * qSin(radians));  // Negative because Y grows downward

    // Draw needle line
    painter->setPen(QPen(palette().text().color(), 3, Qt::SolidLine, Qt::RoundCap));
    painter->drawLine(centerX, centerY, needleX, needleY);

    // Draw center circle
    painter->setBrush(palette().text().color());
    painter->drawEllipse(QPoint(centerX, centerY), 6, 6);
}

void QualityGauge::drawValue(QPainter* painter) {
    painter->setPen(palette().text().color());

    // Draw percentage value
    QFont font = painter->font();
    font.setPointSize(20);
    font.setBold(true);
    painter->setFont(font);

    QString text = QString("%1%").arg(m_value);
    QRect textRect(0, height() / 2 + 20, width(), 40);
    painter->drawText(textRect, Qt::AlignCenter, text);

    // Draw quality level
    font.setPointSize(12);
    font.setBold(false);
    painter->setFont(font);

    QRect qualityRect(0, height() / 2 + 60, width(), 30);
    painter->drawText(qualityRect, Qt::AlignCenter, m_qualityLevel);
}

QColor QualityGauge::getColorForValue(int value) const {
    if (value >= 90) return QColor("#4CAF50");  // Green - Excellent
    if (value >= 70) return QColor("#8BC34A");  // Light Green - Good
    if (value >= 50) return QColor("#FF9800");  // Orange - Fair
    return QColor("#F44336");                    // Red - Poor
}

QString QualityGauge::getQualityLevelFromValue(int value) const {
    if (value >= 90) return tr("Excellent");
    if (value >= 70) return tr("Good");
    if (value >= 50) return tr("Fair");
    if (value > 0) return tr("Poor");
    return tr("Unknown");
}
