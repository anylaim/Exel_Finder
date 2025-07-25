#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QSettings>

QTranslator appTranslator;
QTranslator qtTranslator;

void loadLanguage(QApplication &app, const QString &langCode) {
    // Сначала удаляем старые переводчики
    app.removeTranslator(&appTranslator);
    app.removeTranslator(&qtTranslator);

    // Загружаем новые
    QString appTransPath = QString(":/translations/exelfinder_%1.qm").arg(langCode);
    QString qtTransPath = QString(":/translations/qt_%1.qm").arg(langCode);

    if (appTranslator.load(appTransPath)) {
        app.installTranslator(&appTranslator);
    }
    if (qtTranslator.load(qtTransPath)) {
        app.installTranslator(&qtTranslator);
    }
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Читаем сохранённый язык (по умолчанию "ru")
    QSettings settings("Nanagi_Sleep", "Exel_Finder");
    QString langCode = settings.value("language", "ru").toString();

    loadLanguage(app, langCode);

    MainWindow w;
    w.show();

    return app.exec();
}
