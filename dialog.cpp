#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) : QDialog(parent), ui(new Ui::Dialog)
{
    ui->setupUi(this);
}

Dialog::~Dialog()
{
    delete ui;
}

int Dialog::on_buttonBox_accepted()
{
    return 1;
}

int Dialog::on_buttonBox_rejected()
{
    return 1;
}
