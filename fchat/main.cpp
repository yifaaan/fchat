#include "mainwindow.h"

#include <QApplication>
#include <qfile.h>
#include "timer_button.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFile qss{":/style/stylesheet.qss"};
    if (qss.open(QFile::ReadOnly)) {
        qDebug("Open success");
        QString style = QLatin1String(qss.readAll());
        a.setStyleSheet(style);
        qss.close();
    } else {
        qDebug("Open failed");
    }

    QString config_file_name = "config.ini";
    auto app_path = QCoreApplication::applicationDirPath();
    auto config_path = QDir::toNativeSeparators(app_path + QDir::separator() + config_file_name);
    QSettings settings{config_path, QSettings::IniFormat};
    auto host = settings.value("GateServer/host").toString();
    auto port = settings.value("GateServer/port").toString();
    GateUrlPrefix = "http://" + host + ":" + port;
    qDebug() << GateUrlPrefix;

    MainWindow w;
    w.show();
    return a.exec();
}
