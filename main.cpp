#include <cstdint>

#include <QApplication>

#include "source/form/creator/creator_header.hpp"


int main(int32_t argc, char *argv[])
{
    QApplication app(argc, argv);
    Creator window;


    window.show();


    return app.exec();
}
