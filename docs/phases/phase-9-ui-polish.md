# Phase 9: UI Polish & Theming

**Timeline**: Week 17
**Objective**: Modern UI with dark/light themes, custom widgets, localization

---

## 9.1 Theme System

### ThemeManager.h/cpp
Theme management system

```cpp
class ThemeManager : public QObject {
    Q_OBJECT

public:
    enum Theme {
        Light,
        Dark,
        System  // Follow system theme
    };

    static ThemeManager* instance();

    void setTheme(Theme theme);
    Theme getCurrentTheme() const;

    QString getThemeStylesheet() const;
    QColor getPrimaryColor() const;
    QColor getSecondaryColor() const;
    QColor getBackgroundColor() const;
    QColor getTextColor() const;

    void applyThemeToApplication();

signals:
    void themeChanged(Theme theme);

private:
    ThemeManager(QObject* parent = nullptr);
    static ThemeManager* m_instance;

    Theme currentTheme = Light;

    QString loadStylesheet(const QString& filename);
    void detectSystemTheme();

#ifdef Q_OS_WIN
    bool isWindowsDarkTheme();
#endif
};
```

### Theme Implementation

```cpp
ThemeManager* ThemeManager::m_instance = nullptr;

ThemeManager* ThemeManager::instance() {
    if (!m_instance) {
        m_instance = new ThemeManager();
    }
    return m_instance;
}

void ThemeManager::setTheme(Theme theme) {
    if (theme == System) {
        detectSystemTheme();
    } else {
        currentTheme = theme;
    }

    applyThemeToApplication();
    emit themeChanged(currentTheme);

    Logger::info("Theme changed to: " + QString::number(static_cast<int>(currentTheme)));
}

void ThemeManager::applyThemeToApplication() {
    QString stylesheet = getThemeStylesheet();
    qApp->setStyleSheet(stylesheet);

    // Update palette
    QPalette palette;

    if (currentTheme == Dark) {
        palette.setColor(QPalette::Window, QColor(38, 50, 56));
        palette.setColor(QPalette::WindowText, Qt::white);
        palette.setColor(QPalette::Base, QColor(48, 60, 66));
        palette.setColor(QPalette::AlternateBase, QColor(58, 70, 76));
        palette.setColor(QPalette::Text, Qt::white);
        palette.setColor(QPalette::Button, QColor(48, 60, 66));
        palette.setColor(QPalette::ButtonText, Qt::white);
        palette.setColor(QPalette::Highlight, QColor(33, 150, 243));
        palette.setColor(QPalette::HighlightedText, Qt::white);
    } else {
        palette.setColor(QPalette::Window, Qt::white);
        palette.setColor(QPalette::WindowText, Qt::black);
        palette.setColor(QPalette::Base, Qt::white);
        palette.setColor(QPalette::AlternateBase, QColor(245, 245, 245));
        palette.setColor(QPalette::Text, Qt::black);
        palette.setColor(QPalette::Button, QColor(240, 240, 240));
        palette.setColor(QPalette::ButtonText, Qt::black);
        palette.setColor(QPalette::Highlight, QColor(33, 150, 243));
        palette.setColor(QPalette::HighlightedText, Qt::white);
    }

    qApp->setPalette(palette);
}

QString ThemeManager::loadStylesheet(const QString& filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        Logger::error("Failed to load stylesheet: " + filename);
        return QString();
    }

    QString stylesheet = file.readAll();
    file.close();

    return stylesheet;
}

QString ThemeManager::getThemeStylesheet() const {
    if (currentTheme == Dark) {
        return loadStylesheet(":/styles/dark.qss");
    } else {
        return loadStylesheet(":/styles/light.qss");
    }
}

#ifdef Q_OS_WIN
bool ThemeManager::isWindowsDarkTheme() {
    QSettings settings(
        "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        QSettings::NativeFormat
    );
    return settings.value("AppsUseLightTheme", 1).toInt() == 0;
}
#endif

void ThemeManager::detectSystemTheme() {
#ifdef Q_OS_WIN
    currentTheme = isWindowsDarkTheme() ? Dark : Light;
#else
    // Linux/macOS: default to light theme
    currentTheme = Light;
#endif
}
```

### dark.qss
Dark theme stylesheet

```css
/* resources/styles/dark.qss */

/* Main Window */
QMainWindow {
    background-color: #263238;
    color: #FFFFFF;
}

/* Menu Bar */
QMenuBar {
    background-color: #37474F;
    color: #FFFFFF;
    border-bottom: 1px solid #455A64;
}

QMenuBar::item:selected {
    background-color: #455A64;
}

QMenu {
    background-color: #37474F;
    color: #FFFFFF;
    border: 1px solid #455A64;
}

QMenu::item:selected {
    background-color: #455A64;
}

/* Toolbar */
QToolBar {
    background-color: #37474F;
    border: none;
    spacing: 3px;
}

QToolButton {
    background-color: transparent;
    color: #FFFFFF;
    border: none;
    border-radius: 4px;
    padding: 5px;
}

QToolButton:hover {
    background-color: #455A64;
}

QToolButton:pressed {
    background-color: #546E7A;
}

/* Status Bar */
QStatusBar {
    background-color: #37474F;
    color: #FFFFFF;
    border-top: 1px solid #455A64;
}

/* Table View */
QTableView {
    background-color: #2C393F;
    alternate-background-color: #37474F;
    color: #FFFFFF;
    gridline-color: #455A64;
    border: 1px solid #455A64;
}

QTableView::item:selected {
    background-color: #2196F3;
    color: #FFFFFF;
}

QHeaderView::section {
    background-color: #37474F;
    color: #FFFFFF;
    padding: 5px;
    border: none;
    border-right: 1px solid #455A64;
    border-bottom: 1px solid #455A64;
}

/* Scroll Bar */
QScrollBar:vertical {
    background-color: #2C393F;
    width: 12px;
    border: none;
}

QScrollBar::handle:vertical {
    background-color: #546E7A;
    border-radius: 6px;
    min-height: 20px;
}

QScrollBar::handle:vertical:hover {
    background-color: #607D8B;
}

QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
    height: 0px;
}

/* Buttons */
QPushButton {
    background-color: #2196F3;
    color: #FFFFFF;
    border: none;
    border-radius: 4px;
    padding: 8px 16px;
    font-weight: bold;
}

QPushButton:hover {
    background-color: #1976D2;
}

QPushButton:pressed {
    background-color: #0D47A1;
}

QPushButton:disabled {
    background-color: #455A64;
    color: #78909C;
}

/* Line Edit */
QLineEdit {
    background-color: #37474F;
    color: #FFFFFF;
    border: 1px solid #455A64;
    border-radius: 4px;
    padding: 5px;
}

QLineEdit:focus {
    border: 1px solid #2196F3;
}

/* Combo Box */
QComboBox {
    background-color: #37474F;
    color: #FFFFFF;
    border: 1px solid #455A64;
    border-radius: 4px;
    padding: 5px;
}

QComboBox::drop-down {
    border: none;
}

QComboBox QAbstractItemView {
    background-color: #37474F;
    color: #FFFFFF;
    selection-background-color: #2196F3;
}

/* Spin Box */
QSpinBox, QDoubleSpinBox {
    background-color: #37474F;
    color: #FFFFFF;
    border: 1px solid #455A64;
    border-radius: 4px;
    padding: 5px;
}

/* Group Box */
QGroupBox {
    color: #FFFFFF;
    border: 1px solid #455A64;
    border-radius: 4px;
    margin-top: 10px;
    padding-top: 10px;
}

QGroupBox::title {
    subcontrol-origin: margin;
    subcontrol-position: top left;
    padding: 0 5px;
}

/* Tab Widget */
QTabWidget::pane {
    border: 1px solid #455A64;
    background-color: #2C393F;
}

QTabBar::tab {
    background-color: #37474F;
    color: #FFFFFF;
    border: 1px solid #455A64;
    border-bottom: none;
    border-top-left-radius: 4px;
    border-top-right-radius: 4px;
    padding: 8px 16px;
}

QTabBar::tab:selected {
    background-color: #2C393F;
    border-bottom: 2px solid #2196F3;
}

QTabBar::tab:hover {
    background-color: #455A64;
}

/* Progress Bar */
QProgressBar {
    background-color: #37474F;
    border: 1px solid #455A64;
    border-radius: 4px;
    text-align: center;
    color: #FFFFFF;
}

QProgressBar::chunk {
    background-color: #2196F3;
    border-radius: 3px;
}

/* Check Box */
QCheckBox {
    color: #FFFFFF;
}

QCheckBox::indicator {
    width: 18px;
    height: 18px;
    border: 2px solid #455A64;
    border-radius: 3px;
    background-color: #37474F;
}

QCheckBox::indicator:checked {
    background-color: #2196F3;
    border-color: #2196F3;
}

/* Radio Button */
QRadioButton {
    color: #FFFFFF;
}

QRadioButton::indicator {
    width: 18px;
    height: 18px;
    border: 2px solid #455A64;
    border-radius: 9px;
    background-color: #37474F;
}

QRadioButton::indicator:checked {
    background-color: #2196F3;
    border-color: #2196F3;
}

/* Tool Tip */
QToolTip {
    background-color: #37474F;
    color: #FFFFFF;
    border: 1px solid #455A64;
    padding: 5px;
}
```

### light.qss
Light theme stylesheet

```css
/* resources/styles/light.qss */

/* Main Window */
QMainWindow {
    background-color: #FFFFFF;
    color: #000000;
}

/* Menu Bar */
QMenuBar {
    background-color: #F5F5F5;
    color: #000000;
    border-bottom: 1px solid #E0E0E0;
}

QMenuBar::item:selected {
    background-color: #E0E0E0;
}

QMenu {
    background-color: #FFFFFF;
    color: #000000;
    border: 1px solid #E0E0E0;
}

QMenu::item:selected {
    background-color: #E3F2FD;
}

/* Toolbar */
QToolBar {
    background-color: #F5F5F5;
    border: none;
    spacing: 3px;
}

QToolButton {
    background-color: transparent;
    color: #000000;
    border: none;
    border-radius: 4px;
    padding: 5px;
}

QToolButton:hover {
    background-color: #E0E0E0;
}

QToolButton:pressed {
    background-color: #BDBDBD;
}

/* Status Bar */
QStatusBar {
    background-color: #F5F5F5;
    color: #000000;
    border-top: 1px solid #E0E0E0;
}

/* Table View */
QTableView {
    background-color: #FFFFFF;
    alternate-background-color: #F5F5F5;
    color: #000000;
    gridline-color: #E0E0E0;
    border: 1px solid #E0E0E0;
}

QTableView::item:selected {
    background-color: #2196F3;
    color: #FFFFFF;
}

QHeaderView::section {
    background-color: #F5F5F5;
    color: #000000;
    padding: 5px;
    border: none;
    border-right: 1px solid #E0E0E0;
    border-bottom: 1px solid #E0E0E0;
}

/* Scroll Bar */
QScrollBar:vertical {
    background-color: #F5F5F5;
    width: 12px;
    border: none;
}

QScrollBar::handle:vertical {
    background-color: #BDBDBD;
    border-radius: 6px;
    min-height: 20px;
}

QScrollBar::handle:vertical:hover {
    background-color: #9E9E9E;
}

/* Buttons */
QPushButton {
    background-color: #2196F3;
    color: #FFFFFF;
    border: none;
    border-radius: 4px;
    padding: 8px 16px;
    font-weight: bold;
}

QPushButton:hover {
    background-color: #1976D2;
}

QPushButton:pressed {
    background-color: #0D47A1;
}

QPushButton:disabled {
    background-color: #E0E0E0;
    color: #9E9E9E;
}

/* Line Edit */
QLineEdit {
    background-color: #FFFFFF;
    color: #000000;
    border: 1px solid #E0E0E0;
    border-radius: 4px;
    padding: 5px;
}

QLineEdit:focus {
    border: 1px solid #2196F3;
}

/* Combo Box */
QComboBox {
    background-color: #FFFFFF;
    color: #000000;
    border: 1px solid #E0E0E0;
    border-radius: 4px;
    padding: 5px;
}

QComboBox QAbstractItemView {
    background-color: #FFFFFF;
    color: #000000;
    selection-background-color: #E3F2FD;
}

/* Group Box */
QGroupBox {
    color: #000000;
    border: 1px solid #E0E0E0;
    border-radius: 4px;
    margin-top: 10px;
    padding-top: 10px;
}

/* Tab Widget */
QTabWidget::pane {
    border: 1px solid #E0E0E0;
    background-color: #FFFFFF;
}

QTabBar::tab {
    background-color: #F5F5F5;
    color: #000000;
    border: 1px solid #E0E0E0;
    border-bottom: none;
    border-top-left-radius: 4px;
    border-top-right-radius: 4px;
    padding: 8px 16px;
}

QTabBar::tab:selected {
    background-color: #FFFFFF;
    border-bottom: 2px solid #2196F3;
}

/* Progress Bar */
QProgressBar {
    background-color: #F5F5F5;
    border: 1px solid #E0E0E0;
    border-radius: 4px;
    text-align: center;
    color: #000000;
}

QProgressBar::chunk {
    background-color: #2196F3;
    border-radius: 3px;
}

/* Tool Tip */
QToolTip {
    background-color: #FFFFFF;
    color: #000000;
    border: 1px solid #E0E0E0;
    padding: 5px;
}
```

### Theme Switcher in Settings

```cpp
void SettingsDialog::onThemeChanged(int index) {
    QString themeData = ui->themeCombo->itemData(index).toString();

    ThemeManager::Theme theme;
    if (themeData == "dark") {
        theme = ThemeManager::Dark;
    } else if (themeData == "light") {
        theme = ThemeManager::Light;
    } else {
        theme = ThemeManager::System;
    }

    ThemeManager::instance()->setTheme(theme);
}
```

---

## 9.2 Custom Widgets

### QualityGauge.h/cpp
Circular gauge widget for quality score

```cpp
class QualityGauge : public QWidget {
    Q_OBJECT

public:
    QualityGauge(QWidget* parent = nullptr);

    void setValue(int value); // 0-100
    int getValue() const;

    void setQualityLevel(const QString& quality); // Excellent/Good/Fair/Poor

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    int value = 0;
    QString qualityLevel = "Unknown";

    void drawGauge(QPainter* painter);
    void drawNeedle(QPainter* painter);
    void drawValue(QPainter* painter);
    QColor getColorForValue(int value);
};
```

### QualityGauge Implementation

```cpp
void QualityGauge::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    drawGauge(&painter);
    drawNeedle(&painter);
    drawValue(&painter);
}

void QualityGauge::drawGauge(QPainter* painter) {
    int side = qMin(width(), height());
    QRect rect((width() - side) / 2, (height() - side) / 2, side, side);

    // Draw background arc
    painter->setPen(QPen(Qt::lightGray, 10));
    painter->drawArc(rect, 30 * 16, 120 * 16); // 30° to 150°

    // Draw colored arc based on value
    QColor color = getColorForValue(value);
    painter->setPen(QPen(color, 10));
    int spanAngle = (value * 120) / 100; // Map 0-100 to 0-120°
    painter->drawArc(rect, 30 * 16, spanAngle * 16);
}

void QualityGauge::drawNeedle(QPainter* painter) {
    int centerX = width() / 2;
    int centerY = height() / 2;
    int radius = qMin(width(), height()) / 2 - 20;

    // Calculate needle angle (30° to 150°)
    double angle = 30 + (value * 120.0 / 100.0);
    double radians = angle * M_PI / 180.0;

    int needleX = centerX + radius * cos(radians);
    int needleY = centerY + radius * sin(radians);

    painter->setPen(QPen(Qt::black, 3));
    painter->drawLine(centerX, centerY, needleX, needleY);

    // Draw center circle
    painter->setBrush(Qt::black);
    painter->drawEllipse(QPoint(centerX, centerY), 5, 5);
}

void QualityGauge::drawValue(QPainter* painter) {
    painter->setPen(Qt::black);
    QFont font = painter->font();
    font.setPointSize(16);
    font.setBold(true);
    painter->setFont(font);

    QString text = QString("%1%").arg(value);
    QRect textRect(0, height() / 2 + 20, width(), 30);
    painter->drawText(textRect, Qt::AlignCenter, text);

    // Draw quality level
    font.setPointSize(12);
    font.setBold(false);
    painter->setFont(font);
    QRect qualityRect(0, height() / 2 + 50, width(), 20);
    painter->drawText(qualityRect, Qt::AlignCenter, qualityLevel);
}

QColor QualityGauge::getColorForValue(int value) {
    if (value >= 90) return QColor("#4CAF50"); // Green
    if (value >= 70) return QColor("#8BC34A"); // Light green
    if (value >= 50) return QColor("#FF9800"); // Orange
    return QColor("#F44336"); // Red
}
```

### StatusLed.h/cpp
Animated LED status indicator

```cpp
class StatusLed : public QWidget {
    Q_OBJECT

public:
    enum State {
        Off,
        On,
        Blinking
    };

    StatusLed(QWidget* parent = nullptr);

    void setState(State state);
    void setColor(const QColor& color);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    State currentState = Off;
    QColor color = Qt::green;
    bool blinkState = false;

    QTimer* blinkTimer;

    void drawLed(QPainter* painter);

private slots:
    void onBlinkTimer();
};
```

### StatusLed Implementation

```cpp
StatusLed::StatusLed(QWidget* parent)
    : QWidget(parent)
{
    setFixedSize(20, 20);

    blinkTimer = new QTimer(this);
    blinkTimer->setInterval(500); // Blink every 500ms
    connect(blinkTimer, &QTimer::timeout, this, &StatusLed::onBlinkTimer);
}

void StatusLed::setState(State state) {
    currentState = state;

    if (state == Blinking) {
        blinkTimer->start();
    } else {
        blinkTimer->stop();
        blinkState = (state == On);
    }

    update();
}

void StatusLed::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    drawLed(&painter);
}

void StatusLed::drawLed(QPainter* painter) {
    QRect rect = this->rect().adjusted(2, 2, -2, -2);

    // Draw LED
    if (currentState == On || (currentState == Blinking && blinkState)) {
        // LED is on
        QRadialGradient gradient(rect.center(), rect.width() / 2);
        gradient.setColorAt(0, color.lighter(150));
        gradient.setColorAt(1, color);

        painter->setBrush(gradient);
        painter->setPen(color.darker());
    } else {
        // LED is off
        painter->setBrush(color.darker(300));
        painter->setPen(Qt::gray);
    }

    painter->drawEllipse(rect);
}

void StatusLed::onBlinkTimer() {
    blinkState = !blinkState;
    update();
}
```

### MiniSparkline.h/cpp
Mini inline chart for quick metrics

```cpp
class MiniSparkline : public QWidget {
    Q_OBJECT

public:
    MiniSparkline(QWidget* parent = nullptr);

    void addDataPoint(double value);
    void setData(const QVector<double>& data);
    void clear();

    void setColor(const QColor& color);
    void setMaxDataPoints(int max);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QVector<double> dataPoints;
    QColor lineColor = QColor("#2196F3");
    int maxDataPoints = 20;

    void drawSparkline(QPainter* painter);
    double getMinValue() const;
    double getMaxValue() const;
};
```

### IpRangePicker.h/cpp
Custom IP range selector widget

```cpp
class IpRangePicker : public QWidget {
    Q_OBJECT

public:
    IpRangePicker(QWidget* parent = nullptr);

    QString getStartIp() const;
    QString getEndIp() const;
    QString getCidr() const;

    void setStartIp(const QString& ip);
    void setEndIp(const QString& ip);
    void setCidr(const QString& cidr);

signals:
    void rangeChanged(const QString& startIp, const QString& endIp);
    void cidrChanged(const QString& cidr);

private slots:
    void onStartIpChanged();
    void onEndIpChanged();
    void onCidrChanged();

private:
    QLineEdit* startIpEdit;
    QLineEdit* endIpEdit;
    QLineEdit* cidrEdit;
    QComboBox* modeCombo; // "Range" or "CIDR"

    void setupUI();
    void validateInputs();
};
```

---

## 9.3 UI Enhancements

### SVG Icons (Material Design)

```cpp
// Icon resources structure
resources/icons/
├── scan.svg
├── stop.svg
├── export.svg
├── settings.svg
├── ping.svg
├── details.svg
├── favorite.svg
├── copy.svg
├── delete.svg
├── power.svg
└── ...

// Load SVG icons
QIcon loadSvgIcon(const QString& filename, const QSize& size = QSize(24, 24)) {
    QPixmap pixmap(size);
    pixmap.fill(Qt::transparent);

    QSvgRenderer renderer(filename);
    QPainter painter(&pixmap);
    renderer.render(&painter);

    return QIcon(pixmap);
}

// Usage
QAction* scanAction = toolbar->addAction(
    loadSvgIcon(":/icons/scan.svg"),
    tr("Scan")
);
```

### Smooth Animations (QPropertyAnimation)

```cpp
class AnimatedButton : public QPushButton {
public:
    AnimatedButton(const QString& text, QWidget* parent = nullptr)
        : QPushButton(text, parent)
    {
        animation = new QPropertyAnimation(this, "geometry");
        animation->setDuration(200);
        animation->setEasingCurve(QEasingCurve::OutCubic);
    }

protected:
    void enterEvent(QEnterEvent* event) override {
        // Grow slightly on hover
        QRect current = geometry();
        QRect target = current.adjusted(-2, -2, 2, 2);

        animation->setStartValue(current);
        animation->setEndValue(target);
        animation->start();

        QPushButton::enterEvent(event);
    }

    void leaveEvent(QEvent* event) override {
        // Return to original size
        animation->stop();
        // Reset geometry
        QPushButton::leaveEvent(event);
    }

private:
    QPropertyAnimation* animation;
};

// Fade-in animation for widgets
void fadeInWidget(QWidget* widget, int duration = 300) {
    QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect(widget);
    widget->setGraphicsEffect(effect);

    QPropertyAnimation* animation = new QPropertyAnimation(effect, "opacity");
    animation->setDuration(duration);
    animation->setStartValue(0.0);
    animation->setEndValue(1.0);
    animation->setEasingCurve(QEasingCurve::InOutQuad);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

// Slide-in animation
void slideInWidget(QWidget* widget, QAbstractAnimation::Direction direction, int duration = 300) {
    QRect startRect = widget->geometry();
    QRect endRect = startRect;

    if (direction == QAbstractAnimation::Forward) {
        startRect.moveLeft(-startRect.width());
    }

    widget->setGeometry(startRect);

    QPropertyAnimation* animation = new QPropertyAnimation(widget, "geometry");
    animation->setDuration(duration);
    animation->setStartValue(startRect);
    animation->setEndValue(endRect);
    animation->setEasingCurve(QEasingCurve::OutCubic);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}
```

### Detailed Tooltips

```cpp
void setupDetailedTooltips() {
    // Device table tooltips
    deviceTableView->setToolTip(
        "<b>Device Table</b><br>"
        "Double-click a device to view details<br>"
        "Right-click for context menu<br>"
        "Use search box to filter devices"
    );

    // Scan button tooltip
    scanButton->setToolTip(
        "<b>Start Scan</b><br>"
        "Scan the network for active devices<br>"
        "Shortcut: Ctrl+N"
    );

    // Metrics chart tooltip
    latencyChart->setToolTip(
        "<b>Latency Chart</b><br>"
        "Real-time network latency monitoring<br>"
        "Green: Min latency<br>"
        "Blue: Average latency<br>"
        "Red: Max latency"
    );
}

// Rich text tooltips with formatting
QString createDeviceTooltip(const Device& device) {
    return QString(
        "<table cellspacing='5'>"
        "<tr><td><b>IP Address:</b></td><td>%1</td></tr>"
        "<tr><td><b>Hostname:</b></td><td>%2</td></tr>"
        "<tr><td><b>MAC Address:</b></td><td>%3</td></tr>"
        "<tr><td><b>Vendor:</b></td><td>%4</td></tr>"
        "<tr><td><b>Status:</b></td><td>%5</td></tr>"
        "<tr><td><b>Last Seen:</b></td><td>%6</td></tr>"
        "</table>"
    ).arg(device.ipAddress)
     .arg(device.hostname)
     .arg(device.macAddress)
     .arg(device.vendor)
     .arg(device.isOnline ? "Online" : "Offline")
     .arg(device.lastSeen.toString("yyyy-MM-dd hh:mm:ss"));
}
```

### Responsive Layout

```cpp
void MainWindow::setupResponsiveLayout() {
    // Use QSplitter for resizable panels
    QSplitter* splitter = new QSplitter(Qt::Horizontal, this);

    // Left panel: Device table
    devicePanel = new QWidget();
    QVBoxLayout* leftLayout = new QVBoxLayout(devicePanel);
    leftLayout->addWidget(deviceTable);
    splitter->addWidget(devicePanel);

    // Right panel: Metrics and details
    metricsPanel = new QWidget();
    QVBoxLayout* rightLayout = new QVBoxLayout(metricsPanel);
    rightLayout->addWidget(metricsWidget);
    splitter->addWidget(metricsPanel);

    // Set initial sizes (60% left, 40% right)
    splitter->setStretchFactor(0, 6);
    splitter->setStretchFactor(1, 4);

    setCentralWidget(splitter);

    // Adjust layout on window resize
    connect(this, &MainWindow::resizeEvent, [this]() {
        if (width() < 800) {
            // Small window: stack vertically
            metricsPanel->setVisible(false);
        } else {
            metricsPanel->setVisible(true);
        }
    });
}
```

---

## 9.4 Localization

### Qt Linguist Setup

```cmake
# CMakeLists.txt
find_package(Qt6 REQUIRED COMPONENTS LinguistTools)

set(TS_FILES
    translations/lanscan_en.ts
    translations/lanscan_it.ts
)

qt6_add_translation(QM_FILES ${TS_FILES})

add_executable(LanScan
    ${PROJECT_SOURCES}
    ${QM_FILES}
)

# Create translations
add_custom_target(translations DEPENDS ${QM_FILES})
```

### Translation Files

```cpp
// In code, use tr() for all user-facing strings
QPushButton* button = new QPushButton(tr("Start Scan"));
QLabel* label = new QLabel(tr("Device Count: %1").arg(count));

// Context-specific translations
QString status = QObject::tr("Online", "Device status");
QString error = QObject::tr("Connection failed", "Network error");
```

### lanscan_it.ts (Italian translations)

```xml
<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="it_IT">
<context>
    <name>MainWindow</name>
    <message>
        <source>Start Scan</source>
        <translation>Avvia Scansione</translation>
    </message>
    <message>
        <source>Stop Scan</source>
        <translation>Interrompi Scansione</translation>
    </message>
    <message>
        <source>Export</source>
        <translation>Esporta</translation>
    </message>
    <message>
        <source>Settings</source>
        <translation>Impostazioni</translation>
    </message>
    <message>
        <source>Device Count: %1</source>
        <translation>Dispositivi: %1</translation>
    </message>
</context>
</TS>
```

### Runtime Language Switching

```cpp
class LanguageManager : public QObject {
    Q_OBJECT

public:
    static LanguageManager* instance();

    void loadLanguage(const QString& language);
    QString getCurrentLanguage() const;
    QStringList getAvailableLanguages() const;

signals:
    void languageChanged(const QString& language);

private:
    LanguageManager();
    static LanguageManager* m_instance;

    QTranslator* translator;
    QString currentLanguage;
};

void LanguageManager::loadLanguage(const QString& language) {
    if (translator) {
        qApp->removeTranslator(translator);
        delete translator;
    }

    translator = new QTranslator();
    QString filename = QString(":/translations/lanscan_%1.qm").arg(language);

    if (translator->load(filename)) {
        qApp->installTranslator(translator);
        currentLanguage = language;
        emit languageChanged(language);
        Logger::info("Language changed to: " + language);
    } else {
        Logger::error("Failed to load translation: " + filename);
    }
}

// In SettingsDialog
void SettingsDialog::onLanguageChanged(int index) {
    QString language = ui->languageCombo->itemData(index).toString();
    LanguageManager::instance()->loadLanguage(language);

    // Prompt to restart application
    QMessageBox::information(this, tr("Language Changed"),
        tr("Please restart the application for the language change to take full effect."));
}
```

---

## Deliverable

**Phase 9 Completion Criteria**:
- ✅ Theme system with dark/light themes working
- ✅ QSS stylesheets fully implemented
- ✅ System theme detection working (Windows)
- ✅ Custom widgets implemented (QualityGauge, StatusLed, MiniSparkline, IpRangePicker)
- ✅ SVG icons integrated (Material Design)
- ✅ Smooth animations implemented
- ✅ Detailed tooltips for all UI elements
- ✅ Responsive layout working
- ✅ Qt Linguist setup complete
- ✅ English and Italian translations complete
- ✅ Runtime language switching functional
- ✅ Professional, modern UI appearance

---

## Integration Points

### With All Previous Phases
- Theme applies to all widgets and dialogs
- Custom widgets replace standard widgets where appropriate
- Translations cover all user-facing strings

### For Phase 10
- UI polish makes testing more pleasant
- Themes will be tested on different platforms
- Accessibility will be evaluated

---

## Best Practices

### Theme Design
- Consistent color palette
- Sufficient contrast for readability
- Test on different screen sizes
- Follow platform guidelines

### Custom Widgets
- Inherit from appropriate Qt base classes
- Implement proper sizing hints
- Support theme changes
- Add tooltips and accessibility

### Localization
- Use tr() for all user-facing strings
- Provide context where needed
- Test with longer translations (German, etc.)
- Support RTL languages if needed

---

## Next Phase

After completing Phase 9, proceed to:
→ **Phase 10: Testing & Quality Assurance** (Week 18-19)
