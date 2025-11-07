#include <cstdint>

#include <QFont>
#include <QApplication>
#include <QFontDatabase>

#include "source/form/creator/creator_header.hpp"


int main(int32_t argc, char *argv[])
{
    QApplication a(argc, argv);



    int32_t id = QFontDatabase::addApplicationFont(":/font/source/font/MesloLGSNerdFont-Regular.ttf");
    if(id != -1) {
        QString fa = QFontDatabase::applicationFontFamilies(id).at(0);
        QFont f(fa, 12, QFont::DemiBold);
        f.setHintingPreference(QFont::PreferFullHinting);
        f.setStyleStrategy(QFont::PreferAntialias);
        a.setFont(f);
    }



    Creator w;


    w.show();


    return a.exec();
}
