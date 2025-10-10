#ifndef FAVORITESWIDGET_H
#define FAVORITESWIDGET_H

#include <QWidget>
#include <QTreeWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include "managers/FavoritesManager.h"

/**
 * @brief Widget for displaying and managing favorite devices
 *
 * Provides a tree view organized by groups, with search and filter capabilities.
 * Supports adding, removing, editing favorites, and quick connect actions.
 */
class FavoritesWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param manager Favorites manager instance
     * @param parent Parent widget
     */
    explicit FavoritesWidget(FavoritesManager* manager, QWidget* parent = nullptr);

    /**
     * @brief Destructor
     */
    ~FavoritesWidget();

    /**
     * @brief Refresh the favorites display
     */
    void refresh();

    /**
     * @brief Get the currently selected favorite
     * @return Selected favorite device (empty if none selected)
     */
    FavoriteDevice getSelectedFavorite() const;

signals:
    /**
     * @brief Emitted when a favorite is selected
     * @param favorite Selected favorite device
     */
    void favoriteSelected(const FavoriteDevice& favorite);

    /**
     * @brief Emitted when quick connect is requested
     * @param favorite Favorite device to connect to
     */
    void quickConnectRequested(const FavoriteDevice& favorite);

    /**
     * @brief Emitted when a favorite is added
     * @param favorite Added favorite device
     */
    void favoriteAdded(const FavoriteDevice& favorite);

    /**
     * @brief Emitted when a favorite is removed
     * @param deviceId Removed device ID
     */
    void favoriteRemoved(const QString& deviceId);

    /**
     * @brief Emitted when a favorite is edited
     * @param favorite Edited favorite device
     */
    void favoriteEdited(const FavoriteDevice& favorite);

private slots:
    void onAddFavorite();
    void onRemoveFavorite();
    void onEditFavorite();
    void onQuickConnect();
    void onAddNote();
    void onViewNotes();
    void onChangeIcon();
    void onGroupSelected(int index);
    void onSearchTextChanged(const QString& text);
    void onFavoriteDoubleClicked(QTreeWidgetItem* item, int column);
    void onContextMenu(const QPoint& pos);
    void onCreateGroup();
    void onDeleteGroup();
    void onAddToGroup();
    void onRemoveFromGroup();

private:
    FavoritesManager* manager;

    // UI components
    QTreeWidget* favoritesTree;
    QLineEdit* searchBox;
    QComboBox* groupFilter;
    QPushButton* addButton;
    QPushButton* removeButton;
    QPushButton* editButton;
    QPushButton* connectButton;
    QPushButton* createGroupButton;

    void setupUI();
    void setupConnections();
    void loadFavorites();
    void filterFavorites(const QString& searchText);
    void organizeFavoritesByGroup();
    QString getSelectedDeviceId() const;
    QTreeWidgetItem* createDeviceItem(const FavoriteDevice& favorite);
    QIcon getDeviceIcon(const FavoriteDevice& favorite) const;
    void updateGroupFilter();
};

#endif // FAVORITESWIDGET_H
