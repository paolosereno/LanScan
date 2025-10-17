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

    // Get color based on quality
    QColor textColor = getColorForQuality(quality);

    // Draw quality text with appropriate color
    painter->setPen(textColor);
    QFont font = painter->font();
    font.setBold(true);
    painter->setFont(font);
    painter->drawText(rect, Qt::AlignCenter, quality);

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
    } else if (quality.contains("Critical") || quality.contains("Bad")) {
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
