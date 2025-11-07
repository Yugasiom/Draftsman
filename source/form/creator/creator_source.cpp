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



    ui->Height->setRange(1, 9);
    ui->Width->setRange(1, 9);



    connect(ui->Secured, &QCheckBox::toggled                               , this, [=](uint16_t lo)
    {
        ui->Height->setEnabled(!lo);
        ui->Width->setEnabled(!lo);
    });

    connect(ui->Height, QOverload<int32_t>::of(&RPTESpinBox::valueChanged) , this, [=](uint16_t h)
    {
        if(h == 1 && ui->Width->value() == 1) {
            ui->Width->setValue(2);
        }

        f->set_size(h, ui->Width->value());
    });

    connect(ui->Width , QOverload<int32_t>::of(&RPTESpinBox::valueChanged) , this, [=](uint16_t w)
    {
        if(ui->Height->value() == 1 && w == 1) {
            ui->Height->setValue(2);
        }

        f->set_size(ui->Height->value(), w);
    });

    connect(ui->Field , &RPTERadioButton::toggled  , this, [=](uint16_t c)
    {
        if(c) {
            f->set_current_tool(1);
        }
    });

    connect(ui->Flag  , &RPTERadioButton::toggled  , this, [=](uint16_t c)
    {
        if(c) {
            f->set_current_tool(2);
        }
    });

    connect(ui->Player, &RPTERadioButton::toggled  , this, [=](uint16_t c)
    {
        if(c) {
            f->set_current_tool(3);
        }
    });
}


Creator::~Creator()
{
    delete ui;
}
