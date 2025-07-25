#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPoint>
#include <QStringList>
#include <QTranslator>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;

private slots:
    void changeLanguage(const QString &langCode);
    void toggleMaximize();

    void on_FindFolder_button_clicked();
    void on_Add_substring_button_2_clicked();
    void on_GetStart_button_clicked();
    void on_RemoveSubstring_button_clicked();
    void on_RemoveAll_button_clicked();
    void on_Settings_button_clicked();

    void applyTheme(bool dark);

private:
    Ui::MainWindow *ui;
    bool m_dragging = false;
    QPoint m_dragPosition;
    bool m_isMaximized = false;
    QTranslator m_appTranslator;
    QTranslator m_qtTranslator;
    int m_fontPointSize = 11;


    void adjustFontSize(int delta);
    void setupCustomTitleBar();
    void updateMaximizeButtonIcon();
    void saveSettings();
    void loadSettings();
    void initSettings();

    enum ResizeRegion {
        None,
        Left, Right, Top, Bottom,
        TopLeft, TopRight, BottomLeft, BottomRight
    };

    ResizeRegion m_resizeRegion = None;
    bool m_resizing = false;
    QPoint m_dragPos;

    ResizeRegion detectResizeRegion(const QPoint &pos);

    bool searchSubstringsInExcel(const QString &filePath, const QStringList &substrings,
                                 QList<QPair<QString, QStringList>> &results);
    void searchExcelFiles(const QString &folderPath, const QStringList &substrings,
                          QList<QPair<QString, QStringList>> &results);
    void showSearchResults(const QList<QPair<QString, QStringList>> &results);

    void openResultFile(int row, int column);
    void showResultContextMenu(const QPoint &pos);
};

#endif // MAINWINDOW_H
