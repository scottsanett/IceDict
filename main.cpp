#include "MainWindow.hpp"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    MainWindow w;
    w.show();
    return a.exec();
}
