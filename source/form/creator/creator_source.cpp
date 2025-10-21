#include "creator_header.hpp"

#include "ui_creator_window.h"


Creator::Creator(QWidget *parent) :
                 QMainWindow(parent), ui(new Ui::Draftsman)
{
    ui->setupUi(this);
}


Creator::~Creator()
{
    delete ui;
}
