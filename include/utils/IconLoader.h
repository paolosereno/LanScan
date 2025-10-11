#ifndef ICONLOADER_H
#define ICONLOADER_H

#include <QIcon>
#include <QPixmap>
#include <QString>
#include <QSize>
#include <QColor>

/**
 * @brief Utility class for loading SVG icons from Qt resources
 *
 * IconLoader provides static methods to load SVG icons with customizable
 * sizes and colors. All icons are loaded from the Qt resource system
 * with the ":/icons/" prefix.
 */
class IconLoader {
public:
    /**
     * @brief Loads an SVG icon from resources
     * @param iconName Name of the icon file (without path or extension)
     * @param size Desired icon size (default: 24x24)
     * @return QIcon containing the rendered SVG
     */
    static QIcon loadIcon(const QString& iconName, const QSize& size = QSize(24, 24));

    /**
     * @brief Loads an SVG icon with a specific color
     * @param iconName Name of the icon file (without path or extension)
     * @param color Color to apply to the icon
     * @param size Desired icon size (default: 24x24)
     * @return QIcon containing the rendered and colored SVG
     * @note This works best with SVG icons that use "currentColor"
     */
    static QIcon loadIconWithColor(const QString& iconName, const QColor& color, const QSize& size = QSize(24, 24));

    /**
     * @brief Loads an SVG icon as a pixmap
     * @param iconName Name of the icon file (without path or extension)
     * @param size Desired pixmap size (default: 24x24)
     * @return QPixmap containing the rendered SVG
     */
    static QPixmap loadPixmap(const QString& iconName, const QSize& size = QSize(24, 24));

    /**
     * @brief Gets the full resource path for an icon
     * @param iconName Name of the icon file (without path or extension)
     * @return Full Qt resource path (e.g., ":/icons/scan.svg")
     */
    static QString getIconPath(const QString& iconName);

    /**
     * @brief Common icon sizes
     */
    enum IconSize {
        Small = 16,    ///< 16x16 pixels (toolbar small)
        Medium = 24,   ///< 24x24 pixels (default)
        Large = 32,    ///< 32x32 pixels (toolbar large)
        ExtraLarge = 48 ///< 48x48 pixels (dialogs)
    };

private:
    IconLoader() = delete; // Utility class, no instantiation
};

#endif // ICONLOADER_H
