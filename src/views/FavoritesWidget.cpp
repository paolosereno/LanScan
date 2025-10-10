#include "views/FavoritesWidget.h"
#include "utils/Logger.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>
#include <QMenu>
#include <QHeaderView>

FavoritesWidget::FavoritesWidget(FavoritesManager* manager, QWidget* parent)
    : QWidget(parent)
    , manager(manager)
{
    setupUI();
    setupConnections();
    loadFavorites();
}

FavoritesWidget::~FavoritesWidget() {
}

void FavoritesWidget::refresh() {
    loadFavorites();
}

FavoriteDevice FavoritesWidget::getSelectedFavorite() const {
    QString deviceId = getSelectedDeviceId();
    if (!deviceId.isEmpty()) {
        return manager->getFavorite(deviceId);
    }
    return FavoriteDevice();
}

void FavoritesWidget::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(5, 5, 5, 5);

    // Title
    QLabel* titleLabel = new QLabel("<b>Favorites</b>", this);
    mainLayout->addWidget(titleLabel);

    // Search box
    QHBoxLayout* searchLayout = new QHBoxLayout();
    QLabel* searchLabel = new QLabel("Search:", this);
    searchBox = new QLineEdit(this);
    searchBox->setPlaceholderText("Filter favorites...");
    searchLayout->addWidget(searchLabel);
    searchLayout->addWidget(searchBox);
    mainLayout->addLayout(searchLayout);

    // Group filter
    QHBoxLayout* filterLayout = new QHBoxLayout();
    QLabel* filterLabel = new QLabel("Group:", this);
    groupFilter = new QComboBox(this);
    groupFilter->addItem("All Groups", "");
    filterLayout->addWidget(filterLabel);
    filterLayout->addWidget(groupFilter);
    filterLayout->addStretch();

    createGroupButton = new QPushButton("New Group", this);
    filterLayout->addWidget(createGroupButton);
    mainLayout->addLayout(filterLayout);

    // Favorites tree
    favoritesTree = new QTreeWidget(this);
    favoritesTree->setHeaderLabels(QStringList() << "Name" << "IP Address" << "Status");
    favoritesTree->setAlternatingRowColors(true);
    favoritesTree->setRootIsDecorated(true);
    favoritesTree->setContextMenuPolicy(Qt::CustomContextMenu);
    favoritesTree->header()->setStretchLastSection(false);
    favoritesTree->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    favoritesTree->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    favoritesTree->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    mainLayout->addWidget(favoritesTree);

    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();

    addButton = new QPushButton("Add", this);
    removeButton = new QPushButton("Remove", this);
    removeButton->setEnabled(false);
    editButton = new QPushButton("Edit", this);
    editButton->setEnabled(false);
    connectButton = new QPushButton("Quick Connect", this);
    connectButton->setEnabled(false);

    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(removeButton);
    buttonLayout->addWidget(editButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(connectButton);

    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
}

void FavoritesWidget::setupConnections() {
    // Buttons
    connect(addButton, &QPushButton::clicked,
            this, &FavoritesWidget::onAddFavorite);
    connect(removeButton, &QPushButton::clicked,
            this, &FavoritesWidget::onRemoveFavorite);
    connect(editButton, &QPushButton::clicked,
            this, &FavoritesWidget::onEditFavorite);
    connect(connectButton, &QPushButton::clicked,
            this, &FavoritesWidget::onQuickConnect);
    connect(createGroupButton, &QPushButton::clicked,
            this, &FavoritesWidget::onCreateGroup);

    // Search and filter
    connect(searchBox, &QLineEdit::textChanged,
            this, &FavoritesWidget::onSearchTextChanged);
    connect(groupFilter, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &FavoritesWidget::onGroupSelected);

    // Tree widget
    connect(favoritesTree, &QTreeWidget::itemSelectionChanged,
            this, [this]() {
                bool hasSelection = !getSelectedDeviceId().isEmpty();
                removeButton->setEnabled(hasSelection);
                editButton->setEnabled(hasSelection);
                connectButton->setEnabled(hasSelection);
            });
    connect(favoritesTree, &QTreeWidget::itemDoubleClicked,
            this, &FavoritesWidget::onFavoriteDoubleClicked);
    connect(favoritesTree, &QTreeWidget::customContextMenuRequested,
            this, &FavoritesWidget::onContextMenu);
}

void FavoritesWidget::loadFavorites() {
    favoritesTree->clear();
    updateGroupFilter();

    QString selectedGroup = groupFilter->currentData().toString();
    QString searchText = searchBox->text().toLower();

    if (selectedGroup.isEmpty()) {
        // Show all favorites organized by groups
        organizeFavoritesByGroup();
    } else {
        // Show favorites in selected group
        QList<FavoriteDevice> favorites = manager->getDevicesInGroup(selectedGroup);

        for (const FavoriteDevice& fav : favorites) {
            if (!searchText.isEmpty()) {
                QString name = fav.name.toLower();
                QString ip = fav.ip.toLower();
                if (!name.contains(searchText) && !ip.contains(searchText)) {
                    continue;
                }
            }

            QTreeWidgetItem* item = createDeviceItem(fav);
            favoritesTree->addTopLevelItem(item);
        }
    }

    favoritesTree->expandAll();
}

void FavoritesWidget::filterFavorites(const QString& searchText) {
    loadFavorites();
}

void FavoritesWidget::organizeFavoritesByGroup() {
    QList<QString> groups = manager->getGroups();
    QString searchText = searchBox->text().toLower();

    // Add ungrouped favorites
    QTreeWidgetItem* ungroupedItem = new QTreeWidgetItem(QStringList() << "Ungrouped" << "" << "");
    ungroupedItem->setData(0, Qt::UserRole, "");
    QFont font = ungroupedItem->font(0);
    font.setBold(true);
    ungroupedItem->setFont(0, font);

    QList<FavoriteDevice> allFavorites = manager->getAllFavorites();
    for (const FavoriteDevice& fav : allFavorites) {
        QStringList deviceGroups = manager->getDeviceGroups(fav.id);

        if (deviceGroups.isEmpty()) {
            if (!searchText.isEmpty()) {
                QString name = fav.name.toLower();
                QString ip = fav.ip.toLower();
                if (!name.contains(searchText) && !ip.contains(searchText)) {
                    continue;
                }
            }

            QTreeWidgetItem* item = createDeviceItem(fav);
            ungroupedItem->addChild(item);
        }
    }

    if (ungroupedItem->childCount() > 0) {
        favoritesTree->addTopLevelItem(ungroupedItem);
    } else {
        delete ungroupedItem;
    }

    // Add groups
    for (const QString& group : groups) {
        QTreeWidgetItem* groupItem = new QTreeWidgetItem(QStringList() << group << "" << "");
        groupItem->setData(0, Qt::UserRole + 1, group);
        QFont font = groupItem->font(0);
        font.setBold(true);
        groupItem->setFont(0, font);

        QList<FavoriteDevice> favorites = manager->getDevicesInGroup(group);
        for (const FavoriteDevice& fav : favorites) {
            if (!searchText.isEmpty()) {
                QString name = fav.name.toLower();
                QString ip = fav.ip.toLower();
                if (!name.contains(searchText) && !ip.contains(searchText)) {
                    continue;
                }
            }

            QTreeWidgetItem* item = createDeviceItem(fav);
            groupItem->addChild(item);
        }

        if (groupItem->childCount() > 0) {
            favoritesTree->addTopLevelItem(groupItem);
        } else {
            delete groupItem;
        }
    }
}

QString FavoritesWidget::getSelectedDeviceId() const {
    QTreeWidgetItem* item = favoritesTree->currentItem();
    if (item && item->parent()) {  // Only device items have parents
        return item->data(0, Qt::UserRole).toString();
    }
    return QString();
}

QTreeWidgetItem* FavoritesWidget::createDeviceItem(const FavoriteDevice& favorite) {
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0, favorite.name);
    item->setText(1, favorite.ip);
    item->setText(2, favorite.wakeOnLanEnabled ? "WoL" : "-");
    item->setData(0, Qt::UserRole, favorite.id);
    item->setIcon(0, getDeviceIcon(favorite));

    // Add tooltip with device details
    QString tooltip = favorite.name + "\n" +
                     "IP: " + favorite.ip + "\n" +
                     "Description: " + favorite.description;

    if (!favorite.tags.isEmpty()) {
        tooltip += "\nTags: " + favorite.tags.join(", ");
    }

    QStringList groups = manager->getDeviceGroups(favorite.id);
    if (!groups.isEmpty()) {
        tooltip += "\nGroups: " + groups.join(", ");
    }

    QList<QString> notes = manager->getNotes(favorite.id);
    if (!notes.isEmpty()) {
        tooltip += "\n\nNotes:\n" + notes.join("\n");
    }

    item->setToolTip(0, tooltip);
    item->setToolTip(1, tooltip);
    item->setToolTip(2, tooltip);

    return item;
}

QIcon FavoritesWidget::getDeviceIcon(const FavoriteDevice& favorite) const {
    // Check for custom icon
    QString customIcon = manager->getCustomIcon(favorite.id);
    if (!customIcon.isEmpty()) {
        QIcon icon(customIcon);
        if (!icon.isNull()) {
            return icon;
        }
    }

    // Default icon based on tags
    if (favorite.hasTag("router")) {
        return QIcon::fromTheme("network-wireless");
    } else if (favorite.hasTag("server")) {
        return QIcon::fromTheme("network-server");
    } else if (favorite.hasTag("printer")) {
        return QIcon::fromTheme("printer");
    } else {
        return QIcon::fromTheme("computer");
    }
}

void FavoritesWidget::updateGroupFilter() {
    QString currentGroup = groupFilter->currentData().toString();
    groupFilter->clear();
    groupFilter->addItem("All Groups", "");

    QList<QString> groups = manager->getGroups();
    for (const QString& group : groups) {
        int deviceCount = manager->getDevicesInGroup(group).size();
        groupFilter->addItem(group + " (" + QString::number(deviceCount) + ")", group);
    }

    // Restore selection if possible
    int index = groupFilter->findData(currentGroup);
    if (index >= 0) {
        groupFilter->setCurrentIndex(index);
    }
}

void FavoritesWidget::onAddFavorite() {
    // TODO: Open add favorite dialog
    QMessageBox::information(this, tr("Add Favorite"),
                           tr("Add favorite dialog not yet implemented.\n"
                              "Use the device table context menu to add favorites."));
}

void FavoritesWidget::onRemoveFavorite() {
    QString deviceId = getSelectedDeviceId();
    if (deviceId.isEmpty()) {
        return;
    }

    FavoriteDevice favorite = manager->getFavorite(deviceId);
    if (!favorite.isValid()) {
        return;
    }

    int result = QMessageBox::question(this, tr("Remove Favorite"),
                                      tr("Are you sure you want to remove '%1' from favorites?")
                                      .arg(favorite.name),
                                      QMessageBox::Yes | QMessageBox::No,
                                      QMessageBox::No);

    if (result == QMessageBox::Yes) {
        if (manager->removeFavorite(deviceId)) {
            loadFavorites();
            emit favoriteRemoved(deviceId);
            Logger::info("Removed favorite: " + favorite.name);
        } else {
            QMessageBox::warning(this, tr("Error"),
                               tr("Failed to remove favorite"));
        }
    }
}

void FavoritesWidget::onEditFavorite() {
    QString deviceId = getSelectedDeviceId();
    if (deviceId.isEmpty()) {
        return;
    }

    // TODO: Open edit favorite dialog
    QMessageBox::information(this, tr("Edit Favorite"),
                           tr("Edit favorite dialog not yet implemented."));
}

void FavoritesWidget::onQuickConnect() {
    QString deviceId = getSelectedDeviceId();
    if (deviceId.isEmpty()) {
        return;
    }

    FavoriteDevice favorite = manager->getFavorite(deviceId);
    if (favorite.isValid()) {
        emit quickConnectRequested(favorite);
        Logger::info("Quick connect requested for: " + favorite.name);
    }
}

void FavoritesWidget::onAddNote() {
    QString deviceId = getSelectedDeviceId();
    if (deviceId.isEmpty()) {
        return;
    }

    bool ok;
    QString note = QInputDialog::getMultiLineText(this,
                                                  tr("Add Note"),
                                                  tr("Note:"),
                                                  QString(),
                                                  &ok);

    if (ok && !note.isEmpty()) {
        manager->addNote(deviceId, note);
        loadFavorites();
        Logger::info("Added note to device: " + deviceId);
    }
}

void FavoritesWidget::onViewNotes() {
    QString deviceId = getSelectedDeviceId();
    if (deviceId.isEmpty()) {
        return;
    }

    QList<QString> notes = manager->getNotes(deviceId);
    FavoriteDevice favorite = manager->getFavorite(deviceId);

    if (notes.isEmpty()) {
        QMessageBox::information(this, tr("Notes"),
                               tr("No notes for %1").arg(favorite.name));
        return;
    }

    QString noteText = "Notes for " + favorite.name + ":\n\n";
    for (int i = 0; i < notes.size(); ++i) {
        noteText += QString::number(i + 1) + ". " + notes[i] + "\n\n";
    }

    QMessageBox::information(this, tr("Notes"), noteText);
}

void FavoritesWidget::onChangeIcon() {
    QString deviceId = getSelectedDeviceId();
    if (deviceId.isEmpty()) {
        return;
    }

    QString fileName = QFileDialog::getOpenFileName(this,
                                                   tr("Select Icon"),
                                                   QString(),
                                                   tr("Images (*.png *.jpg *.svg)"));

    if (!fileName.isEmpty()) {
        manager->setCustomIcon(deviceId, fileName);
        loadFavorites();
        Logger::info("Changed icon for device: " + deviceId);
    }
}

void FavoritesWidget::onGroupSelected(int index) {
    Q_UNUSED(index);
    loadFavorites();
}

void FavoritesWidget::onSearchTextChanged(const QString& text) {
    filterFavorites(text);
}

void FavoritesWidget::onFavoriteDoubleClicked(QTreeWidgetItem* item, int column) {
    Q_UNUSED(column);
    if (item && item->parent()) {  // Only device items have parents
        onQuickConnect();
    }
}

void FavoritesWidget::onContextMenu(const QPoint& pos) {
    QTreeWidgetItem* item = favoritesTree->itemAt(pos);
    if (!item) {
        return;
    }

    QMenu menu(this);

    // Check if it's a group item or device item
    if (!item->parent()) {
        // Group item
        QString groupName = item->data(0, Qt::UserRole + 1).toString();
        if (!groupName.isEmpty()) {
            menu.addAction("Delete Group", this, &FavoritesWidget::onDeleteGroup);
        }
    } else {
        // Device item
        menu.addAction("Quick Connect", this, &FavoritesWidget::onQuickConnect);
        menu.addSeparator();
        menu.addAction("Edit", this, &FavoritesWidget::onEditFavorite);
        menu.addAction("Remove", this, &FavoritesWidget::onRemoveFavorite);
        menu.addSeparator();
        menu.addAction("Add Note", this, &FavoritesWidget::onAddNote);
        menu.addAction("View Notes", this, &FavoritesWidget::onViewNotes);
        menu.addSeparator();
        menu.addAction("Change Icon", this, &FavoritesWidget::onChangeIcon);
        menu.addSeparator();

        QMenu* groupMenu = menu.addMenu("Groups");
        groupMenu->addAction("Add to Group...", this, &FavoritesWidget::onAddToGroup);

        QStringList deviceGroups = manager->getDeviceGroups(getSelectedDeviceId());
        if (!deviceGroups.isEmpty()) {
            groupMenu->addAction("Remove from Group...", this, &FavoritesWidget::onRemoveFromGroup);
        }
    }

    menu.exec(favoritesTree->mapToGlobal(pos));
}

void FavoritesWidget::onCreateGroup() {
    bool ok;
    QString groupName = QInputDialog::getText(this,
                                             tr("Create Group"),
                                             tr("Group name:"),
                                             QLineEdit::Normal,
                                             QString(),
                                             &ok);

    if (ok && !groupName.isEmpty()) {
        manager->createGroup(groupName);
        updateGroupFilter();
        loadFavorites();
        Logger::info("Created group: " + groupName);
    }
}

void FavoritesWidget::onDeleteGroup() {
    QTreeWidgetItem* item = favoritesTree->currentItem();
    if (!item || item->parent()) {
        return;
    }

    QString groupName = item->data(0, Qt::UserRole + 1).toString();
    if (groupName.isEmpty()) {
        return;
    }

    int result = QMessageBox::question(this, tr("Delete Group"),
                                      tr("Are you sure you want to delete the group '%1'?\n"
                                         "Devices will not be deleted, only the group.")
                                      .arg(groupName),
                                      QMessageBox::Yes | QMessageBox::No,
                                      QMessageBox::No);

    if (result == QMessageBox::Yes) {
        manager->deleteGroup(groupName);
        updateGroupFilter();
        loadFavorites();
        Logger::info("Deleted group: " + groupName);
    }
}

void FavoritesWidget::onAddToGroup() {
    QString deviceId = getSelectedDeviceId();
    if (deviceId.isEmpty()) {
        return;
    }

    QList<QString> groups = manager->getGroups();
    if (groups.isEmpty()) {
        QMessageBox::information(this, tr("Add to Group"),
                               tr("No groups available. Create a group first."));
        return;
    }

    bool ok;
    QString groupName = QInputDialog::getItem(this,
                                             tr("Add to Group"),
                                             tr("Select group:"),
                                             groups,
                                             0,
                                             false,
                                             &ok);

    if (ok && !groupName.isEmpty()) {
        manager->addToGroup(deviceId, groupName);
        loadFavorites();
        Logger::info("Added device to group: " + groupName);
    }
}

void FavoritesWidget::onRemoveFromGroup() {
    QString deviceId = getSelectedDeviceId();
    if (deviceId.isEmpty()) {
        return;
    }

    QStringList groups = manager->getDeviceGroups(deviceId);
    if (groups.isEmpty()) {
        return;
    }

    bool ok;
    QString groupName = QInputDialog::getItem(this,
                                             tr("Remove from Group"),
                                             tr("Select group:"),
                                             groups,
                                             0,
                                             false,
                                             &ok);

    if (ok && !groupName.isEmpty()) {
        manager->removeFromGroup(deviceId, groupName);
        loadFavorites();
        Logger::info("Removed device from group: " + groupName);
    }
}
