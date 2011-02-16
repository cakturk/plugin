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

private:
    Ui::uiBinderDialog *ui;
    QPair<QString, QPair<QString, QStringList> > properties;
    QMap<QString, QStringList> classMap;

    bool validate() const;
};

#endif // UIBINDERDIALOG_H
