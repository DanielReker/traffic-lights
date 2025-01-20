#include <QApplication>

#include "MyWindow.hpp"


using namespace traffic_lights::qt;

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    MyWindow w;

    w.show();

    return a.exec();
}