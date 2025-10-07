QT       += core gui


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


CONFIG += c++17


SOURCES += \
    main.cpp \
    qt_main_window.cpp


HEADERS += \
    qt_main_window.hpp


FORMS += \
    qt_main_window.ui


qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
