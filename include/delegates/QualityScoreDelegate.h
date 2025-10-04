#ifndef QUALITYSCOREDELEGATE_H
#define QUALITYSCOREDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>
#include <QStyleOptionViewItem>
#include <QColor>

/**
 * @brief Custom delegate for quality score column
 *
 * Renders a colored progress bar representing the quality score (0-100).
 */
class QualityScoreDelegate : public QStyledItemDelegate {
    Q_OBJECT

public:
    explicit QualityScoreDelegate(QObject* parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionViewItem& option,
               const QModelIndex& index) const override;

    QSize sizeHint(const QStyleOptionViewItem& option,
                   const QModelIndex& index) const override;

private:
    void paintQualityBar(QPainter* painter, const QRect& rect,
                        const QString& quality) const;
    QColor getColorForQuality(const QString& quality) const;
    int parseQualityScore(const QString& quality) const;
};

#endif // QUALITYSCOREDELEGATE_H
