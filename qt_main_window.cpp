#include "qt_main_window.hpp"
#include "ui_qt_main_window.h"


QtMainWindow::QtMainWindow(QWidget *parent) :
              QMainWindow(parent), ui(new Ui::QtMainWindow)
{
    ui->setupUi(this);
}


QtMainWindow::~QtMainWindow()
{
    delete ui;
}
