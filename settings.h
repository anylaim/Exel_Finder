#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>

namespace Ui {
class Settings;
}

class Settings : public QWidget
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = nullptr);
    ~Settings();

signals:
    void themeChanged(bool darkMode);
    void languageChanged(const QString &langCode);
    void fontSizeChanged(int delta);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private slots:
    void on_Theme_button_clicked();
    void on_Languige_button_clicked();
    void on_SizeUp_button_clicked();
    void on_SizeDown_button_clicked();
    void on_Exit_button_clicked();

private:
    Ui::Settings *ui;
    QPoint s_dragPosition;
    bool s_isMaximized = false;
    bool darkTheme = true;
};

#endif // SETTINGS_H
