#ifndef STATUSDELEGATE_H
#define STATUSDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>
#include <QStyleOptionViewItem>

/**
 * @brief Custom delegate for status column
 *
 * Renders a colored LED indicator for online/offline status.
 */
class StatusDelegate : public QStyledItemDelegate {
    Q_OBJECT

public:
    explicit StatusDelegate(QObject* parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionViewItem& option,
               const QModelIndex& index) const override;

    QSize sizeHint(const QStyleOptionViewItem& option,
                   const QModelIndex& index) const override;

private:
    void paintStatusLed(QPainter* painter, const QRect& rect, bool isOnline) const;
};

#endif // STATUSDELEGATE_H
