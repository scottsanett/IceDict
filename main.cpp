#include "MainWindow.hpp"


int main(int argc, char *argv[])
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);
    a.setApplicationVersion(VERSION_STRING);
    MainWindow w;
    w.show();
    return a.exec();
}
