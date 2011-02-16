#include "uibinderdialog.h"
#include "ui_uibinderdialog.h"
#include <QDebug>

uiBinderDialog::uiBinderDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::uiBinderDialog)
{
    ui->setupUi(this);

    connect(ui->pushApply, SIGNAL(clicked()), this, SIGNAL(applyClicked()));
    connect(ui->pushOK, SIGNAL(clicked()), this, SIGNAL(okClicked()));
    connect(ui->pushCancel, SIGNAL(clicked()), this, SIGNAL(cancelClicked()));
    connect(ui->comboClass, SIGNAL(currentIndexChanged(QString)), this, SLOT(updateFunctionNames(QString)));
}

uiBinderDialog::~uiBinderDialog()
{
    delete ui;
}

QPair<QString, QPair<QString, QStringList> > uiBinderDialog::getProperties()
{
    QPair<QString, QPair<QString, QStringList> > retVal;

    retVal.first = ui->comboButton->currentText();
    retVal.second.first = ui->comboClass->currentText();
    retVal.second.second.append(ui->comboParam_1->currentText());
    retVal.second.second.append(ui->comboParam_2->currentText());
    retVal.second.second.append(ui->comboParam_3->currentText());
    retVal.second.second.append(ui->comboParam_4->currentText());
    retVal.second.second.append(ui->comboParam_5->currentText());

    return retVal;
}

void uiBinderDialog::setButtonNames(const QStringList &buttonNames) const
{
    ui->comboButton->clear();
    ui->comboButton->addItems(buttonNames);
}

void uiBinderDialog::setClassNames(const QStringList &classNames) const
{
    ui->comboClass->clear();
    ui->comboClass->addItems(classNames);
}

void uiBinderDialog::setFunctionNames(const QStringList &functionNames) const
{
    ui->comboFunction->clear();
    ui->comboFunction->addItems(functionNames);
}

void uiBinderDialog::setFunctionParams(const QStringList & paramNames) const
{
    ui->comboParam_1->clear();
    ui->comboParam_2->clear();
    ui->comboParam_3->clear();
    ui->comboParam_4->clear();
    ui->comboParam_5->clear();

    ui->comboParam_1->addItems(paramNames);
    ui->comboParam_2->addItems(paramNames);
    ui->comboParam_3->addItems(paramNames);
    ui->comboParam_4->addItems(paramNames);
    ui->comboParam_5->addItems(paramNames);
}

void uiBinderDialog::setClassMap(QMap<QString, QStringList> &map)
{
    this->classMap.clear();
    this->classMap = map;
    this->setClassNames(classMap.keys());
}

void uiBinderDialog::clearClassMap()
{
    this->classMap.clear();
}

void uiBinderDialog::updateFunctionNames(const QString &className)
{
    if (ui->comboClass->currentText().isEmpty())
        return;
    this->setFunctionNames(classMap[className]);
}

bool uiBinderDialog::validate() const
{
    if (ui->comboButton->currentText().isEmpty() ||
        ui->comboClass->currentText().isEmpty() ||
        ui->comboFunction->currentText().isEmpty())
        return false;
    return true;
}

void uiBinderDialog::on_pushCancel_clicked()
{
    this->done(0);
}

void uiBinderDialog::on_pushOK_clicked()
{
    this->done(1);
}
