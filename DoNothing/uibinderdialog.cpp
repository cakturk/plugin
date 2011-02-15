#include "uibinderdialog.h"
#include "ui_uibinderdialog.h"

uiBinderDialog::uiBinderDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::uiBinderDialog)
{
    ui->setupUi(this);
}

uiBinderDialog::~uiBinderDialog()
{
    delete ui;
}
