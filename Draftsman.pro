QT       += core gui


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


CONFIG += c++17


SOURCES += \
    main.cpp \
    source/custom_object/resizable_plain_text_edit/resizable_plain_text_edit.cpp \
    source/custom_object/rpt_label/rpt_label.cpp \
    source/custom_object/rpte_push_button/rpte_push_button.cpp \
    source/custom_object/rpte_radio_button/rpte_radio_button.cpp \
    source/custom_object/rpte_spin_box/rpte_spin_box.cpp \
    source/form/creator/creator_source.cpp


HEADERS += \
    source/custom_object/resizable_plain_text_edit/resizable_plain_text_edit.hpp \
    source/custom_object/rpt_label/rpt_label.hpp \
    source/custom_object/rpte_push_button/rpte_push_button.hpp \
    source/custom_object/rpte_radio_button/rpte_radio_button.hpp \
    source/custom_object/rpte_spin_box/rpte_spin_box.hpp \
    source/form/creator/creator_header.hpp


FORMS += \
    source/form/creator/creator_window.ui


RESOURCE += resource.qrc


qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


