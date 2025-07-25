#include "settings.h"
#include "ui_settings.h"
#include <QIcon>
#include <QDebug>
#include <QMouseEvent>
#include <QSettings>

Settings::Settings(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Settings)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);

    // сделать окно отдельным
    setWindowFlag(Qt::Window);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(tr("Settings"));

    // поставить иконки на кнопки (иконки положи в папку resources.qrc)
    ui->Theme_button->setIcon(QIcon(":/icons/theme.png"));
    ui->Languige_button->setIcon(QIcon(":/icons/lang.png"));
    ui->SizeUp_button->setIcon(QIcon(":/icons/plus.png"));
    ui->SizeDown_button->setIcon(QIcon(":/icons/minus.png"));
    ui->Exit_button->setIcon(QIcon(":/icons/exit.png"));

    // размер иконок
    ui->Theme_button->setIconSize(QSize(24,24));
    ui->Languige_button->setIconSize(QSize(24,24));
    ui->SizeUp_button->setIconSize(QSize(24,24));
    ui->SizeDown_button->setIconSize(QSize(24,24));
    ui->Exit_button->setIconSize(QSize(24,24));

    // стиль кнопок
    QString btnStyle = R"(
        QPushButton {
            background-color: rgb(58,64,76);
            border: none;
            border-radius: 6px;
            padding: 6px;
            color: white;
        }
        QPushButton:hover {
            background-color: rgb(88,88,100);
        }
    )";
    ui->Theme_button->setStyleSheet(btnStyle);
    ui->Languige_button->setStyleSheet(btnStyle);
    ui->SizeUp_button->setStyleSheet(btnStyle);
    ui->SizeDown_button->setStyleSheet(btnStyle);
    ui->Exit_button->setStyleSheet(btnStyle);
}

Settings::~Settings()
{
    delete ui;
}

void Settings::on_Theme_button_clicked()
{
    darkTheme = !darkTheme;
    emit themeChanged(darkTheme);
}


// Обработка перемещения окна
void Settings::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && !s_isMaximized) {
        s_dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
    QWidget::mousePressEvent(event);
}

void Settings::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton && !s_isMaximized) {
        move(event->globalPos() - s_dragPosition);
        event->accept();
    }
    QWidget::mouseMoveEvent(event);
}

void Settings::on_Languige_button_clicked()
{
    static bool ru = true;
    ru = !ru;

    QString lang = ru ? "ru" : "en";

    // сохраняем выбор
    QSettings settings("Nanagi_Sleep", "Exel_Finder");
    settings.setValue("language", lang);

    emit languageChanged(lang);
}




void Settings::on_SizeUp_button_clicked()
{
    emit fontSizeChanged(+1);
}

void Settings::on_SizeDown_button_clicked()
{
    emit fontSizeChanged(-1);
}

void Settings::on_Exit_button_clicked()
{
    close();
}
