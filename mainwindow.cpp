#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPushButton>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QApplication>
#include <QScreen>
#include <QGraphicsDropShadowEffect>
#include <QFileDialog>
#include <QSettings>
#include <QStandardPaths>
#include <QMessageBox>
#include <xlsxdocument.h>
#include <xlsxcell.h>
#include <QDir>
#include <QDebug>
#include <QDesktopServices>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    loadSettings();
    m_fontPointSize = qApp->font().pointSize();

    QSettings settings("Nanagi_Sleep", "Exel_Finder");
    bool dark = settings.value("darkTheme", true).toBool();
    applyTheme(dark);
    QString savedLang = settings.value("language", "ru").toString();
    changeLanguage(savedLang);

    setWindowFlags(Qt::FramelessWindowHint);

    ui->Settings_button->setIcon(QIcon(":/icons/settings_gear.png"));
    ui->Settings_button->setIconSize(QSize(24,24));

    setupCustomTitleBar();

    ui->centralwidget->setMouseTracking(true);
    qApp->installEventFilter(this);

    connect(ui->Substring_text, &QLineEdit::returnPressed,
            this, &MainWindow::on_Add_substring_button_2_clicked);

    ui->AddedSubstrings_list->setSelectionMode(QAbstractItemView::ExtendedSelection);

    ui->OutcomeTable_table->setColumnCount(3);
    ui->OutcomeTable_table->setHorizontalHeaderLabels({tr("File"), tr("Path"), tr("Finded")});
    ui->OutcomeTable_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->OutcomeTable_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->OutcomeTable_table->setSortingEnabled(true);

    ui->OutcomeTable_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    connect(ui->OutcomeTable_table, &QTableWidget::cellDoubleClicked,
            this, &MainWindow::openResultFile);

    ui->OutcomeTable_table->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->OutcomeTable_table, &QTableWidget::customContextMenuRequested,
            this, &MainWindow::showResultContextMenu);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    QWidget *titleBar = menuWidget();
    if (obj == titleBar)
    {
        if (event->type() == QEvent::MouseButtonPress) {
            auto *me = static_cast<QMouseEvent*>(event);
            if (me->button() == Qt::LeftButton) {
                m_dragging = true;
                m_dragPosition = me->globalPos() - frameGeometry().topLeft();
                return true;
            }
        }
        else if (event->type() == QEvent::MouseMove) {
            auto *me = static_cast<QMouseEvent*>(event);
            if (m_dragging && (me->buttons() & Qt::LeftButton) && !m_isMaximized && !m_resizing) {
                move(me->globalPos() - m_dragPosition);
                return true;
            }
        }
        else if (event->type() == QEvent::MouseButtonRelease) {
            auto *me = static_cast<QMouseEvent*>(event);
            if (me->button() == Qt::LeftButton) {
                m_dragging = false;
                return true;
            }
        }
    }
    return QMainWindow::eventFilter(obj, event);
}


void MainWindow::applyTheme(bool dark)
{
    int fontSize = m_fontPointSize;
    QString baseStyle;

    if (dark) {
        baseStyle = QString(R"(
        QWidget {
            background-color: #2E3440;
            color: #D8DEE9;
            font-family: "Segoe UI", Tahoma, Geneva, Verdana, sans-serif;
            font-size: %1pt;
        }
        QHeaderView::section {
            background-color: #4C566A;
            color: #ECEFF4;
            padding: 6px 12px;
            border: none;
            font-weight: 600;
        }
        QPushButton {
            background-color: #434C5E;
            border: 1px solid #4C566A;
            border-radius: 6px;
            padding: 8px 14px;
            color: #D8DEE9;
            transition: background-color 0.3s ease;
        }
        QPushButton:hover {
            background-color: #5E81AC;
            border-color: #81A1C1;
            color: #ECEFF4;
        }
        QPushButton:pressed {
            background-color: #4C566A;
            border-color: #434C5E;
        }
        QTableWidget, QTableView {
            background-color: #3B4252;
            color: #D8DEE9;
            gridline-color: #4C566A;
            selection-background-color: #81A1C1;
            selection-color: #2E3440;
            alternate-background-color: #434C5E;
        }
        QTableWidget::item:selected {
            background-color: #81A1C1;
            color: #2E3440;
        }
        QScrollBar:vertical {
            background: transparent;
            width: 12px;
            margin: 16px 0 16px 0;
            border-radius: 6px;
        }
        QScrollBar::handle:vertical {
            background: #81A1C1;
            min-height: 30px;
            border-radius: 6px;
        }
        QScrollBar::handle:vertical:hover {
            background: #88C0D0;
        }
        QScrollBar:horizontal {
            background: transparent;
            height: 12px;
            margin: 0 16px 0 16px;
            border-radius: 6px;
        }
        QScrollBar::handle:horizontal {
            background: #81A1C1;
            min-width: 30px;
            border-radius: 6px;
        }
        QScrollBar::handle:horizontal:hover {
            background: #88C0D0;
        }
        QLineEdit {
            background-color: #434C5E;
            border: 1px solid #4C566A;
            border-radius: 6px;
            padding: 6px 10px;
            color: #D8DEE9;
        }
        QLineEdit:focus {
            border-color: #81A1C1;
            background-color: #4C566A;
        }
        )").arg(fontSize);
    } else {
        baseStyle = QString(R"(
        QWidget {
            background-color: #F7F9FB;
            color: #2E3440;
            font-family: "Segoe UI", Tahoma, Geneva, Verdana, sans-serif;
            font-size: %1pt;
        }
        QHeaderView::section {
            background-color: #E5E9F0;
            color: #2E3440;
            padding: 6px 12px;
            border: 1px solid #D8DEE9;
            font-weight: 600;
        }
        QPushButton {
            background-color: #D8DEE9;
            border: 1px solid #B0BEC5;
            border-radius: 6px;
            padding: 8px 14px;
            color: #2E3440;
            transition: background-color 0.3s ease;
        }
        QPushButton:hover {
            background-color: #90A4AE;
            border-color: #78909C;
            color: #ECEFF4;
        }
        QPushButton:pressed {
            background-color: #B0BEC5;
            border-color: #90A4AE;
        }
        QTableWidget, QTableView {
            background-color: #FFFFFF;
            color: #2E3440;
            gridline-color: #D8DEE9;
            selection-background-color: #90A4AE;
            selection-color: #FFFFFF;
            alternate-background-color: #ECEFF4;
        }
        QTableWidget::item:selected {
            background-color: #90A4AE;
            color: #FFFFFF;
        }
        QScrollBar:vertical {
            background: transparent;
            width: 12px;
            margin: 16px 0 16px 0;
            border-radius: 6px;
        }
        QScrollBar::handle:vertical {
            background: #90A4AE;
            min-height: 30px;
            border-radius: 6px;
        }
        QScrollBar::handle:vertical:hover {
            background: #78909C;
        }
        QScrollBar:horizontal {
            background: transparent;
            height: 12px;
            margin: 0 16px 0 16px;
            border-radius: 6px;
        }
        QScrollBar::handle:horizontal {
            background: #90A4AE;
            min-width: 30px;
            border-radius: 6px;
        }
        QScrollBar::handle:horizontal:hover {
            background: #78909C;
        }
        QLineEdit {
            background-color: #FFFFFF;
            border: 1px solid #B0BEC5;
            border-radius: 6px;
            padding: 6px 10px;
            color: #2E3440;
        }
        QLineEdit:focus {
            border-color: #90A4AE;
            background-color: #F0F4F8;
        }
        )").arg(fontSize);
    }

    qApp->setStyleSheet(baseStyle);
}



void MainWindow::setupCustomTitleBar() {
    QWidget *titleBar = new QWidget(this);
    titleBar->setFixedHeight(30);
    titleBar->setStyleSheet(
        "background: #2b2b2b;"
        "border-top-left-radius: 0px;"
        "border-top-right-radius: 0px;"
        );

    QString btnStyle = R"(
        QPushButton {
            color: white;
            border: none;
            background: transparent;
            min-width: 24px;
            max-width: 24px;
            min-height: 24px;
            max-height: 24px;
            border-radius: 3px;
            font-size: 12px;
        }
        QPushButton:hover {
            background: #444;
        }
        QPushButton#closeButton:hover {
            background: #ff5555;
        }
    )";

    QPushButton *minimizeBtn = new QPushButton("–", titleBar);
    QPushButton *maximizeBtn = new QPushButton("□", titleBar);
    QPushButton *closeBtn = new QPushButton("✕", titleBar);

    maximizeBtn->setStyleSheet(btnStyle +
                               "margin-top: 0px;"
                               "margin-right: 0px;");

    minimizeBtn->setStyleSheet(btnStyle +
                               "margin-top: 0px;"
                               "margin-right: 0px;");

    closeBtn->setStyleSheet(btnStyle +
                               "margin-top: 0px;"
                               "margin-right: 0px;");

    minimizeBtn->setStyleSheet(btnStyle);
    closeBtn->setObjectName("closeButton");
    closeBtn->setStyleSheet(btnStyle);

    connect(minimizeBtn, &QPushButton::clicked, this, &QMainWindow::showMinimized);
    connect(maximizeBtn, &QPushButton::clicked, this, &MainWindow::toggleMaximize);
    connect(closeBtn, &QPushButton::clicked, this, &QMainWindow::close);

    QHBoxLayout *layout = new QHBoxLayout(titleBar);
    layout->setContentsMargins(5, 0, 5, 0);
    layout->setSpacing(3);

    layout->addStretch();
    layout->addWidget(minimizeBtn, 0, Qt::AlignVCenter);
    layout->addWidget(maximizeBtn, 0, Qt::AlignVCenter | Qt::AlignRight);
    layout->addWidget(closeBtn, 0, Qt::AlignVCenter);

    setMenuWidget(titleBar);
    titleBar->installEventFilter(this);
}

MainWindow::~MainWindow() {
    saveSettings();
    delete ui;
}

void MainWindow::toggleMaximize() {
    if (m_isMaximized) {
        showNormal();
    } else {
        showMaximized();
    }
    m_isMaximized = !m_isMaximized;
    updateMaximizeButtonIcon();
}

void MainWindow::updateMaximizeButtonIcon() {
    QHBoxLayout *layout = qobject_cast<QHBoxLayout*>(menuWidget()->layout());
    QPushButton *maximizeBtn = qobject_cast<QPushButton*>(layout->itemAt(2)->widget());
    maximizeBtn->setText(m_isMaximized ? "❐" : "□");
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        ResizeRegion region = detectResizeRegion(event->pos());
        if (region != None) {
            m_resizing = true;
            m_resizeRegion = region;
            m_dragPos = event->globalPos();
            event->accept();
            return;
        }
    }
    QMainWindow::mousePressEvent(event);
}


void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (m_resizing) {
        QPoint delta = event->globalPos() - m_dragPos;
        QRect g = geometry();

        switch (m_resizeRegion) {
        case Left:
            g.setLeft(g.left() + delta.x());
            break;
        case Right:
            g.setRight(g.right() + delta.x());
            break;
        case Top:
            g.setTop(g.top() + delta.y());
            break;
        case Bottom:
            g.setBottom(g.bottom() + delta.y());
            break;
        case TopLeft:
            g.setTop(g.top() + delta.y());
            g.setLeft(g.left() + delta.x());
            break;
        case TopRight:
            g.setTop(g.top() + delta.y());
            g.setRight(g.right() + delta.x());
            break;
        case BottomLeft:
            g.setBottom(g.bottom() + delta.y());
            g.setLeft(g.left() + delta.x());
            break;
        case BottomRight:
            g.setBottom(g.bottom() + delta.y());
            g.setRight(g.right() + delta.x());
            break;
        default:
            break;
        }
        setGeometry(g);
        m_dragPos = event->globalPos();
        event->accept();
        return;
    }

    ResizeRegion region = detectResizeRegion(event->pos());
    switch (region) {
    case Left:
    case Right:
        setCursor(Qt::SizeHorCursor);
        break;
    case Top:
    case Bottom:
        setCursor(Qt::SizeVerCursor);
        break;
    case TopLeft:
    case BottomRight:
        setCursor(Qt::SizeFDiagCursor);
        break;
    case TopRight:
    case BottomLeft:
        setCursor(Qt::SizeBDiagCursor);
        break;
    default:
        if (!m_resizing)
            setCursor(Qt::ArrowCursor);
        break;
    }

    QMainWindow::mouseMoveEvent(event);
}

void MainWindow::saveSettings() {
    QSettings settings("Nanagi_Sleep", "Exel_Finder");
    settings.setValue("lastFolderPath", ui->FolderPath_text->text());

    QStringList substrings;
    for (int i = 0; i < ui->AddedSubstrings_list->count(); ++i) {
        substrings << ui->AddedSubstrings_list->item(i)->text();
    }
    settings.setValue("substringsList", substrings);

    settings.setValue("fontSize", m_fontPointSize);
}


void MainWindow::loadSettings() {
    QSettings settings("Nanagi_Sleep", "Exel_Finder");

    QString lastPath = settings.value("lastFolderPath", QDir::homePath()).toString();
    ui->FolderPath_text->setText(lastPath);

    QStringList substrings = settings.value("substringsList").toStringList();
    ui->AddedSubstrings_list->addItems(substrings);

    m_fontPointSize = settings.value("fontSize", qApp->font().pointSize()).toInt();
    QFont f = qApp->font();
    f.setPointSize(m_fontPointSize);
    qApp->setFont(f);
}


void MainWindow::initSettings()
{
    try {
        QSettings settings("Nanagi_Sleep", "Exel_Finder");

        QString lastPath = settings.value("lastFolderPath").toString();
        QDir lastDir(lastPath);

        if (lastPath.isEmpty() || !lastDir.exists()) {
            lastPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
        }

        ui->FolderPath_text->setText(QDir::toNativeSeparators(lastPath));

    } catch (...) {
        qWarning() << tr("Failed to load settings");
        ui->FolderPath_text->setText(QDir::homePath());
    }
}


MainWindow::ResizeRegion MainWindow::detectResizeRegion(const QPoint &pos)
{
    const int MARGIN = 6;
    int x = pos.x();
    int y = pos.y();
    int w = width();
    int h = height();

    bool left   = x <= MARGIN;
    bool right  = x >= w - MARGIN;
    bool top    = y <= MARGIN;
    bool bottom = y >= h - MARGIN;

    if (top && left)    return TopLeft;
    if (top && right)   return TopRight;
    if (bottom && left) return BottomLeft;
    if (bottom && right)return BottomRight;
    if (left)           return Left;
    if (right)          return Right;
    if (top)            return Top;
    if (bottom)         return Bottom;

    return None;
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_resizing) {
        m_resizing = false;
        m_resizeRegion = None;
        event->accept();
        return;
    }
    QMainWindow::mouseReleaseEvent(event);
}

void MainWindow::leaveEvent(QEvent *event)
{
    if (!m_resizing) {
        setCursor(Qt::ArrowCursor);
    }
    QMainWindow::leaveEvent(event);
}

bool MainWindow::searchSubstringsInExcel(const QString &filePath, const QStringList &substrings, QList<QPair<QString, QStringList>>& results) {
    QXlsx::Document xlsx(filePath);
    if (!xlsx.load()) {
        qWarning() << tr("Failed to load file:") << filePath;
        return false;
    }

    bool foundAny = false;
    QStringList foundSubstrings;

    for (const QString &sheetName : xlsx.sheetNames()) {
        QXlsx::Worksheet *sheet = dynamic_cast<QXlsx::Worksheet*>(xlsx.sheet(sheetName));
        if (!sheet) continue;

        const int TARGET_COLUMN = 7;

        for (int row = 1; row <= sheet->dimension().lastRow(); ++row) {
            auto cell = sheet->cellAt(row, TARGET_COLUMN);
            if (!cell) continue;

            QString cellText = cell->value().toString();
            if (cellText.isEmpty()) continue;

            for (const QString &substring : substrings) {
                if (cellText.contains(substring, Qt::CaseInsensitive)) {
                    if (!foundSubstrings.contains(substring)) {
                        foundSubstrings.append(substring);
                    }
                    foundAny = true;
                }
            }
        }
    }

    if (foundAny) {
        results.append(qMakePair(filePath, foundSubstrings));
    }

    return foundAny;
}

void MainWindow::searchExcelFiles(const QString &folderPath, const QStringList &substrings, QList<QPair<QString, QStringList>>& results) {
    QDir dir(folderPath);
    if (!dir.exists()) {
        qWarning() << tr("Folder does not exist:") << folderPath;
        return;
    }

    QStringList excelFiles = dir.entryList(QStringList() << "*.xlsx", QDir::Files);
    for (const QString &file : excelFiles) {
        searchSubstringsInExcel(dir.absoluteFilePath(file), substrings, results);
    }

    QStringList subDirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QString &subDir : subDirs) {
        searchExcelFiles(dir.absoluteFilePath(subDir), substrings, results);
    }
}

void MainWindow::showSearchResults(const QList<QPair<QString, QStringList>>& results) {
    ui->OutcomeTable_table->setRowCount(0);

    for (const auto& result : results) {
        int row = ui->OutcomeTable_table->rowCount();
        ui->OutcomeTable_table->insertRow(row);

        QFileInfo fileInfo(result.first);
        QString foundStrings = result.second.join(", ");

        auto *fileItem = new QTableWidgetItem(fileInfo.fileName());
        fileItem->setData(Qt::UserRole, result.first);

        ui->OutcomeTable_table->setItem(row, 0, fileItem);
        ui->OutcomeTable_table->setItem(row, 1, new QTableWidgetItem(fileInfo.path()));
        ui->OutcomeTable_table->setItem(row, 2, new QTableWidgetItem(foundStrings));
    }

    ui->OutcomeTable_table->resizeColumnsToContents();
    ui->OutcomeTable_table->horizontalHeader()->setStretchLastSection(true);
}

void MainWindow::openResultFile(int row, int column) {
    Q_UNUSED(column);
    QTableWidgetItem* item = ui->OutcomeTable_table->item(row, 0);
    if (item) {
        QString filePath = item->data(Qt::UserRole).toString();
        QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
    }
}

void MainWindow::showResultContextMenu(const QPoint &pos) {
    QTableWidgetItem* item = ui->OutcomeTable_table->itemAt(pos);
    if (!item) return;

    QString filePath = ui->OutcomeTable_table->item(item->row(), 0)->data(Qt::UserRole).toString();

    QMenu menu;
    QAction* openAction = menu.addAction(tr("Open file"));
    QAction* showInFolder = menu.addAction(tr("Show in folder"));

    QAction* selected = menu.exec(ui->OutcomeTable_table->viewport()->mapToGlobal(pos));
    if (!selected) return;

    if (selected == openAction) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
    }
    else if (selected == showInFolder) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(filePath).path()));
    }
}

void MainWindow::adjustFontSize(int delta)
{
    m_fontPointSize = std::max(6, m_fontPointSize + delta);

    QFont f = qApp->font();
    f.setPointSize(m_fontPointSize);
    qApp->setFont(f);

    ui->OutcomeTable_table->setFont(f);
    ui->AddedSubstrings_list->setFont(f);
    ui->Substring_text->setFont(f);
    ui->FolderPath_text->setFont(f);

    QSettings settings("Nanagi_Sleep", "Exel_Finder");
    bool dark = settings.value("darkTheme", true).toBool();
    applyTheme(dark);
}

void MainWindow::on_FindFolder_button_clicked()
{

    QString folderPath = QFileDialog::getExistingDirectory(
        this,
        tr("Choose folder"),
        QDir::homePath(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
        );

    if (!folderPath.isEmpty()) {
        ui->FolderPath_text->setText(folderPath);
    }
}

void MainWindow::on_Add_substring_button_2_clicked()
{
    QString substring = ui->Substring_text->text().trimmed();

    if (!substring.isEmpty()) {
        bool alreadyExists = false;
        for (int i = 0; i < ui->AddedSubstrings_list->count(); ++i) {
            if (ui->AddedSubstrings_list->item(i)->text() == substring) {
                alreadyExists = true;
                break;
            }
        }

        if (!alreadyExists) {
            QListWidgetItem* item = new QListWidgetItem(substring);
            ui->AddedSubstrings_list->addItem(item);

            ui->AddedSubstrings_list->setCurrentItem(item);
            ui->AddedSubstrings_list->scrollToItem(item, QAbstractItemView::PositionAtBottom);

            ui->Substring_text->clear();
            ui->Substring_text->setFocus();
        } else {
            QMessageBox::information(this, tr("Ooops..."), tr("Already exist!"));
        }

        ui->Substring_text->clear();
        ui->Substring_text->setFocus();

    }
}

void MainWindow::changeLanguage(const QString &langCode)
{
    qApp->removeTranslator(&m_appTranslator);
    qApp->removeTranslator(&m_qtTranslator);

    if (m_appTranslator.load(":/translations/exelfinder_" + langCode + ".qm")) {
        qApp->installTranslator(&m_appTranslator);
    }
    if (m_qtTranslator.load(":/translations/qt_" + langCode + ".qm")) {
        qApp->installTranslator(&m_qtTranslator);
    }

    ui->retranslateUi(this);

    ui->OutcomeTable_table->setHorizontalHeaderLabels({tr("File"), tr("Path"), tr("Finded")});
    setWindowTitle(tr("Excel Finder"));
}

void MainWindow::on_GetStart_button_clicked() {
    QString folderPath = ui->FolderPath_text->text().trimmed();
    if (folderPath.isEmpty()) {
        QMessageBox::warning(this, tr("Error!"), tr("Choose folder!!!"));
        return;
    }

    QStringList substrings;
    for (int i = 0; i < ui->AddedSubstrings_list->count(); ++i) {
        substrings << ui->AddedSubstrings_list->item(i)->text();
    }

    if (substrings.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("Add parameters"));
        return;
    }

    QList<QPair<QString, QStringList>> results;
    searchExcelFiles(folderPath, substrings, results);

    ui->OutcomeTable_table->setRowCount(0);
    for (const auto& result : results) {
        int row = ui->OutcomeTable_table->rowCount();
        ui->OutcomeTable_table->insertRow(row);

        QFileInfo fileInfo(result.first);
        QString foundStrings = result.second.join(", ");

        auto *fileItem = new QTableWidgetItem(fileInfo.fileName());
        fileItem->setData(Qt::UserRole, result.first);
        ui->OutcomeTable_table->setItem(row, 0, fileItem);

        ui->OutcomeTable_table->setItem(row, 1, new QTableWidgetItem(fileInfo.path()));
        ui->OutcomeTable_table->setItem(row, 2, new QTableWidgetItem(foundStrings));
    }

    ui->OutcomeTable_table->resizeColumnsToContents();
    ui->OutcomeTable_table->horizontalHeader()->setStretchLastSection(true);

    ui->OutcomeTable_table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);


    if (results.isEmpty()) {
        QMessageBox::information(this, tr("Result"), tr("Parameters not found..."));
    }
}

void MainWindow::on_RemoveSubstring_button_clicked()
{
    qDeleteAll(ui->AddedSubstrings_list->selectedItems());
}


void MainWindow::on_RemoveAll_button_clicked()
{
    qDeleteAll(ui->AddedSubstrings_list->findItems("*", Qt::MatchWildcard));
}

#include "settings.h"

void MainWindow::on_Settings_button_clicked()
{
    auto *settingsWindow = new Settings(this);
    settingsWindow->setAttribute(Qt::WA_DeleteOnClose);

    connect(settingsWindow, &Settings::fontSizeChanged, this, [this](int delta){
        adjustFontSize(delta);
    });

    connect(settingsWindow, &Settings::themeChanged, this, [this](bool dark){
        QSettings settings("Nanagi_Sleep", "Exel_Finder");
        settings.setValue("darkTheme", dark);
        applyTheme(dark);
    });

    connect(settingsWindow, &Settings::languageChanged, this, [this](const QString &langCode){
        QSettings settings("Nanagi_Sleep", "Exel_Finder");
        settings.setValue("language", langCode);
        changeLanguage(langCode);
    });

    settingsWindow->show();
}
