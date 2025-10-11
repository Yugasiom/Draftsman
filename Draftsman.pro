QT       += core gui


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


CONFIG += c++17


SOURCES += \
    main.cpp \
    source/form/creator/creator_source.cpp


HEADERS += \
    source/form/creator/creator_header.hpp


FORMS += \
    source/form/creator/creator_window.ui


qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
