#include "delegates/StatusDelegate.h"
#include <QApplication>
#include <QBrush>
#include <QPen>

StatusDelegate::StatusDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
}

void StatusDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,
                          const QModelIndex& index) const {
    // Draw background
    QStyledItemDelegate::paint(painter, option, QModelIndex());

    // Get status text (● for online, ○ for offline)
    QString statusText = index.data(Qt::DisplayRole).toString();
    bool isOnline = (statusText == "●");

    // Paint custom LED indicator
    paintStatusLed(painter, option.rect, isOnline);
}

QSize StatusDelegate::sizeHint(const QStyleOptionViewItem& option,
                              const QModelIndex& index) const {
    Q_UNUSED(option)
    Q_UNUSED(index)
    return QSize(60, 30);
}

void StatusDelegate::paintStatusLed(QPainter* painter, const QRect& rect, bool isOnline) const {
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    // Calculate LED position (centered)
    int ledSize = 12;
    QPoint center = rect.center();
    QRect ledRect(center.x() - ledSize/2, center.y() - ledSize/2, ledSize, ledSize);

    // Set colors based on status
    QColor ledColor = isOnline ? QColor(0, 200, 0) : QColor(150, 150, 150);
    QColor borderColor = isOnline ? QColor(0, 150, 0) : QColor(100, 100, 100);

    // Draw LED circle with gradient
    QRadialGradient gradient(ledRect.center(), ledSize/2);
    gradient.setColorAt(0.0, ledColor.lighter(150));
    gradient.setColorAt(0.5, ledColor);
    gradient.setColorAt(1.0, ledColor.darker(120));

    painter->setBrush(QBrush(gradient));
    painter->setPen(QPen(borderColor, 1));
    painter->drawEllipse(ledRect);

    // Add highlight for online status
    if (isOnline) {
        QRect highlightRect(ledRect.x() + 2, ledRect.y() + 2, ledSize/3, ledSize/3);
        QRadialGradient highlightGradient(highlightRect.center(), ledSize/4);
        highlightGradient.setColorAt(0.0, QColor(255, 255, 255, 200));
        highlightGradient.setColorAt(1.0, QColor(255, 255, 255, 0));
        painter->setBrush(QBrush(highlightGradient));
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(highlightRect);
    }

    painter->restore();
}
