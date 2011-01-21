#include "settingsdialog.h"
#include "ui_settingsdialog.h"

settingsDialog::settingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::settingsDialog)
{
    ui->setupUi(this);
}

settingsDialog::~settingsDialog()
{
    delete ui;
}

QString settingsDialog::ipAddress() const
{
    return ui->lineEditIP->text();
}

QString settingsDialog::portNumber() const
{
    return ui->lineEditPort->text();
}

void settingsDialog::setIpAddress(const QString ipAddress)
{
    ui->lineEditIP->setText(ipAddress);
}

void settingsDialog::setPortNumber(const QString portNumber)
{
    ui->lineEditPort->setText(portNumber);
}
