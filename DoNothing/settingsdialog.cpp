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

void settingsDialog::setIpAddress(const QString ipAddress)
{
    ui->lineEditIP->setText(ipAddress);
}

void settingsDialog::setStatus(const QString status)
{
    if (status == "Connected")
        ui->labelStatus->setStyleSheet("color: green;");
    if (status == "Disconnected")
        ui->labelStatus->setStyleSheet("color: red");

    ui->labelStatus->setText(status);
}
