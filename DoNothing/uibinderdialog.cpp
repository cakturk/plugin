#include "uibinderdialog.h"
#include "ui_uibinderdialog.h"
#include <QDebug>
#include <QTimer>

uiBinderDialog::uiBinderDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::uiBinderDialog)
{
    ui->setupUi(this);
    QTimer::singleShot(0, this, SLOT(init()));
    //QMetaObject::invokeMethod(this, "init", Qt::QueuedConnection);
//    connect(ui->pushApply, SIGNAL(clicked()), this, SIGNAL(applyClicked()));
//    connect(ui->pushOK, SIGNAL(clicked()), this, SIGNAL(okClicked()));
//    connect(ui->pushCancel, SIGNAL(clicked()), this, SIGNAL(cancelClicked()));

    qDebug() << "In da constructor";

}

uiBinderDialog::~uiBinderDialog()
{
    delete ui;
}

const QHash<QString, QHash<QString, QString> > uiBinderDialog::getMappings() const
{
    return mappings;
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

void uiBinderDialog::setFunctionParams(const QStringList & paramNames)
{
    if (paramNames.isEmpty())
        return;

    parameterList = paramNames;

    if (paramNames.first() != "None")
        parameterList.prepend("None");

    ui->comboParam_1->clear();
    ui->comboParam_2->clear();
    ui->comboParam_3->clear();
    ui->comboParam_4->clear();
    ui->comboParam_5->clear();

    ui->comboParam_1->addItems(parameterList);
    ui->comboParam_2->addItems(parameterList);
    ui->comboParam_3->addItems(parameterList);
    ui->comboParam_4->addItems(parameterList);
    ui->comboParam_5->addItems(parameterList);

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

void uiBinderDialog::saveChanges(int index)
{
    (void)index;

    int buttonIndex;
    if ((buttonIndex = ui->comboButton->currentIndex() < 0))
        return;

    QHash<QString, QString> map;

    map["class"] = ui->comboClass->currentText();
    map["function"] = ui->comboFunction->currentText();
    map["param_1"] = ui->comboParam_1->currentText();
    map["param_2"] = ui->comboParam_2->currentText();
    map["param_3"] = ui->comboParam_3->currentText();
    map["param_4"] = ui->comboParam_4->currentText();
    map["param_5"] = ui->comboParam_5->currentText();

    mappings[ui->comboButton->currentText()] = map;
}

void uiBinderDialog::loadChanges(int)
{
    disableSignals(true);

    QString index = ui->comboButton->currentText();
    QStringList classNames = classMap.keys();
    QStringList functions;
    int j;

    if (mappings.contains(index)) {
        QHash<QString, QString> map = mappings[index];
        QString className;
        if (map.contains("class")) {
            className = map["class"];
            if ((j = classNames.indexOf(className)) != -1)
                ui->comboClass->setCurrentIndex(j);
            else
                ui->comboClass->setCurrentIndex(0);
        }

        if (map.contains("function")) {
            QString key = map["function"];
            functions = classMap[className];
            ui->comboFunction->clear();
            ui->comboFunction->addItems(functions);
            if ((j = functions.indexOf(key)) != -1)
                ui->comboFunction->setCurrentIndex(j);
            else
                ui->comboFunction->setCurrentIndex(0);
        }

        if (map.contains("param_1")) {
            QString key = map["param_1"];
            if ((j = parameterList.indexOf(key)) != -1)
                ui->comboParam_1->setCurrentIndex(j);
            else
                ui->comboParam_1->setCurrentIndex(0);
        }

        if (map.contains("param_2")) {
            QString key = map["param_2"];
            if ((j = parameterList.indexOf(key)) != -1)
                ui->comboParam_2->setCurrentIndex(j);
            else
                ui->comboParam_2->setCurrentIndex(0);
        }

        if (map.contains("param_3")) {
            QString key = map["param_3"];
            if ((j = parameterList.indexOf(key)) != -1)
                ui->comboParam_3->setCurrentIndex(j);
            else
                ui->comboParam_3->setCurrentIndex(0);
        }

        if (map.contains("param_4")) {
            QString key = map["param_4"];
            qDebug() << "param_4 ????????" << key;
            if ((j = parameterList.indexOf(key)) != -1)
                ui->comboParam_4->setCurrentIndex(j);
            else
                ui->comboParam_4->setCurrentIndex(0);
        }

        if (map.contains("param_5")) {
            QString key = map["param_5"];
            if ((j = parameterList.indexOf(key)) != -1)
                ui->comboParam_5->setCurrentIndex(j);
            else
                ui->comboParam_5->setCurrentIndex(0);
        }
    } else {
        ui->comboClass->setCurrentIndex(0);
        updateFunctionNames(ui->comboClass->currentText());
        ui->comboParam_1->setCurrentIndex(0);
        ui->comboParam_2->setCurrentIndex(0);
        ui->comboParam_3->setCurrentIndex(0);
        ui->comboParam_4->setCurrentIndex(0);
        ui->comboParam_5->setCurrentIndex(0);
    }

    disableSignals(false);
}

bool uiBinderDialog::validate() const
{
    if (ui->comboButton->currentText().isEmpty() ||
        ui->comboClass->currentText().isEmpty() ||
        ui->comboFunction->currentText().isEmpty())
        return false;
    return true;
}

void uiBinderDialog::disableSignals(bool b)
{
    ui->comboButton->blockSignals(b);
    ui->comboClass->blockSignals(b);
    ui->comboFunction->blockSignals(b);
    ui->comboParam_1->blockSignals(b);
    ui->comboParam_2->blockSignals(b);
    ui->comboParam_3->blockSignals(b);
    ui->comboParam_4->blockSignals(b);
    ui->comboParam_5->blockSignals(b);
}

void uiBinderDialog::init()
{
    connect(ui->comboButton, SIGNAL(currentIndexChanged(int)), this, SLOT(loadChanges(int)));
    connect(ui->comboClass, SIGNAL(currentIndexChanged(int)), this, SLOT(saveChanges(int)));
    connect(ui->comboFunction, SIGNAL(currentIndexChanged(int)), this, SLOT(saveChanges(int)));
    connect(ui->comboParam_1, SIGNAL(currentIndexChanged(int)), this, SLOT(saveChanges(int)));
    connect(ui->comboParam_2, SIGNAL(currentIndexChanged(int)), this, SLOT(saveChanges(int)));
    connect(ui->comboParam_3, SIGNAL(currentIndexChanged(int)), this, SLOT(saveChanges(int)));
    connect(ui->comboParam_4, SIGNAL(currentIndexChanged(int)), this, SLOT(saveChanges(int)));
    connect(ui->comboParam_5, SIGNAL(currentIndexChanged(int)), this, SLOT(saveChanges(int)));
    connect(ui->comboClass, SIGNAL(currentIndexChanged(QString)),this, SLOT(updateFunctionNames(QString)));
}

void uiBinderDialog::on_pushCancel_clicked()
{
    this->done(0);
}

void uiBinderDialog::on_pushOK_clicked()
{
    this->done(1);
}
