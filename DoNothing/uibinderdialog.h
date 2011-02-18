#ifndef UIBINDERDIALOG_H
#define UIBINDERDIALOG_H

#include <QDialog>
#include <QMap>

namespace Ui {
    class uiBinderDialog;
}

class uiBinderDialog : public QDialog
{
    Q_OBJECT

public:
    explicit uiBinderDialog(QWidget *parent = 0);
    ~uiBinderDialog();

    QPair<QString, QPair<QString, QStringList> > getProperties();
    void setButtonNames(const QStringList & buttonNames) const;
    void setClassNames(const QStringList & classNames) const;
    void setFunctionNames(const QStringList & functionNames) const;
    void setFunctionParams(const QStringList & paramNames) const;
    void setClassMap(QMap<QString, QStringList> & map);
    void clearClassMap();

signals:
    void applyClicked();
    void okClicked();
    void cancelClicked();

private slots:
    void on_pushOK_clicked();
    void on_pushCancel_clicked();
    void updateFunctionNames(const QString & className);
    void saveChanges(int index);
    void loadChanges(int index);
    void init();

private:
    Ui::uiBinderDialog *ui;
    QStringList parameterList;
    QMap<QString, QStringList> classMap;
    QHash<QString, QHash<QString, QString> > mappings;

    bool validate() const;
    void disableSignals(bool);
};

#endif // UIBINDERDIALOG_H
