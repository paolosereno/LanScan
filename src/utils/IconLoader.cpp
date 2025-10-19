#include "utils/IconLoader.h"
#include <QSvgRenderer>
#include <QPainter>
#include <QFile>
#include "managers/ThemeManager.h"

QIcon IconLoader::loadIcon(const QString& iconName, const QSize& size) {
    // Use colored version based on current theme
    return loadIconWithColor(iconName, getIconColor(), size);
}

QIcon IconLoader::loadIconWithColor(const QString& iconName, const QColor& color, const QSize& size) {
    QString iconPath = getIconPath(iconName);

    // Load SVG content
    QFile file(iconPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QIcon();
    }

    QString svgContent = file.readAll();
    file.close();

    // Replace currentColor with actual color
    svgContent.replace("currentColor", color.name());

    QPixmap pixmap(size);
    pixmap.fill(Qt::transparent);

    QSvgRenderer renderer(svgContent.toUtf8());
    if (renderer.isValid()) {
        QPainter painter(&pixmap);
        renderer.render(&painter);
    }

    return QIcon(pixmap);
}

QPixmap IconLoader::loadPixmap(const QString& iconName, const QSize& size) {
    QString iconPath = getIconPath(iconName);

    QPixmap pixmap(size);
    pixmap.fill(Qt::transparent);

    QSvgRenderer renderer(iconPath);
    if (renderer.isValid()) {
        QPainter painter(&pixmap);
        renderer.render(&painter);
    }

    return pixmap;
}

QString IconLoader::getIconPath(const QString& iconName) {
    // Add .svg extension if not present
    QString fileName = iconName;
    if (!fileName.endsWith(".svg")) {
        fileName += ".svg";
    }

    return QString(":/icons/%1").arg(fileName);
}

QColor IconLoader::getIconColor() {
    // Get current theme from ThemeManager
    ThemeManager::Theme theme = ThemeManager::instance().effectiveTheme();

    // Return appropriate color based on theme
    if (theme == ThemeManager::Theme::Dark) {
        return QColor("#E0E0E0");  // Light gray for dark theme
    } else {
        return QColor("#424242");  // Dark gray for light theme
    }
}
