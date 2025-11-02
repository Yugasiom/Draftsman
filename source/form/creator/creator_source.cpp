#include "creator_header.hpp"

#include "ui_creator_window.h"
#include "./source/custom_object/game_field_widget/game_field_widget.hpp"


Creator::Creator(QWidget *parent) :
                 QMainWindow(parent), ui(new Ui::Draftsman)
{
    ui->setupUi(this);




    ui->Field->setChecked(true);




    GameFieldWidget *f = new GameFieldWidget(this);

    auto *l = new QGridLayout(ui->GameFieldContainer);
    ui->GameFieldContainer->setLayout(l);
    l->addWidget(f);



    connect(ui->Height, QOverload<int32_t>::of(&QSpinBox::valueChanged), this, [=](int16_t h) {
        f->set_size(h, ui->Width->value());
    });

    connect(ui->Width, QOverload<int32_t>::of(&QSpinBox::valueChanged) , this, [=](int16_t w) {
        f->set_size(ui->Height->value(), w);
    });

    connect(ui->Field, &QRadioButton::toggled , this, [=](uint16_t c) {
        if(c) {
            f->set_current_tool(1);
        }
    });

    connect(ui->Flag, &QRadioButton::toggled  , this, [=](uint16_t c) {
        if(c) {
            f->set_current_tool(2);
        }
    });

    connect(ui->Player, &QRadioButton::toggled, this, [=](uint16_t c) {
        if(c) {
            f->set_current_tool(3);
        }
    });
}


Creator::~Creator()
{
    delete ui;
}
