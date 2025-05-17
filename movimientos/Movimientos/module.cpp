#include "module.h"
#include "ui_module.h"

Module::Module(QWidget *parent) : QWidget(parent),ui(new Ui::Module)
{
    ui->setupUi((QWidget *)this);
}
Module::~Module()
{
    delete ui;
}

