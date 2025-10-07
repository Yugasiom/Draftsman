#include <cstdint>
#include <QApplication>

#include "qt_main_window.hpp"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QtMainWindow w;
    w.show();


    return a.exec();
}
