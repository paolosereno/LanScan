#include "delegates/QualityScoreDelegate.h"
#include <QApplication>
#include <QBrush>
#include <QPen>

QualityScoreDelegate::QualityScoreDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
}

void QualityScoreDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,
                                const QModelIndex& index) const {
    // Draw background
    QStyledItemDelegate::paint(painter, option, QModelIndex());

    // Get quality score string
    QString qualityText = index.data(Qt::DisplayRole).toString();

    // Paint custom quality bar
    paintQualityBar(painter, option.rect, qualityText);
}

QSize QualityScoreDelegate::sizeHint(const QStyleOptionViewItem& option,
                                    const QModelIndex& index) const {
    Q_UNUSED(option)
    Q_UNUSED(index)
    return QSize(120, 30);
}

void QualityScoreDelegate::paintQualityBar(QPainter* painter, const QRect& rect,
                                          const QString& quality) const {
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    // Parse quality score (format: "Excellent (95)" or "N/A")
    int score = parseQualityScore(quality);

    if (score < 0) {
        // Draw "N/A" text for invalid scores
        painter->setPen(QColor(100, 100, 100));
        painter->drawText(rect, Qt::AlignCenter, quality);
        painter->restore();
        return;
    }

    // Calculate bar dimensions
    int barWidth = rect.width() - 20;
    int barHeight = 16;
    int barX = rect.x() + 10;
    int barY = rect.center().y() - barHeight/2;
    QRect barRect(barX, barY, barWidth, barHeight);

    // Draw background bar
    painter->setBrush(QColor(230, 230, 230));
    painter->setPen(QPen(QColor(180, 180, 180), 1));
    painter->drawRoundedRect(barRect, 3, 3);

    // Calculate filled portion
    int fillWidth = (barWidth * score) / 100;
    QRect fillRect(barX, barY, fillWidth, barHeight);

    // Get color based on quality
    QColor fillColor = getColorForQuality(quality);

    // Draw filled bar with gradient
    QLinearGradient gradient(fillRect.topLeft(), fillRect.bottomLeft());
    gradient.setColorAt(0.0, fillColor.lighter(120));
    gradient.setColorAt(0.5, fillColor);
    gradient.setColorAt(1.0, fillColor.darker(110));

    painter->setBrush(QBrush(gradient));
    painter->setPen(QPen(fillColor.darker(120), 1));
    painter->drawRoundedRect(fillRect, 3, 3);

    // Draw score text
    painter->setPen(QColor(60, 60, 60));
    QFont font = painter->font();
    font.setPointSize(8);
    font.setBold(true);
    painter->setFont(font);
    painter->drawText(barRect, Qt::AlignCenter, QString::number(score));

    painter->restore();
}

QColor QualityScoreDelegate::getColorForQuality(const QString& quality) const {
    if (quality.contains("Excellent")) {
        return QColor(0, 200, 0);       // Green
    } else if (quality.contains("Good")) {
        return QColor(150, 200, 0);     // Yellow-Green
    } else if (quality.contains("Fair")) {
        return QColor(255, 165, 0);     // Orange
    } else if (quality.contains("Poor")) {
        return QColor(255, 100, 0);     // Dark Orange
    } else if (quality.contains("Bad")) {
        return QColor(255, 0, 0);       // Red
    }
    return QColor(150, 150, 150);       // Gray (N/A)
}

int QualityScoreDelegate::parseQualityScore(const QString& quality) const {
    // Extract score from format "Excellent (95)"
    int openParen = quality.indexOf('(');
    int closeParen = quality.indexOf(')');

    if (openParen >= 0 && closeParen > openParen) {
        QString scoreStr = quality.mid(openParen + 1, closeParen - openParen - 1);
        bool ok;
        int score = scoreStr.toInt(&ok);
        if (ok && score >= 0 && score <= 100) {
            return score;
        }
    }

    return -1;  // Invalid score
}
