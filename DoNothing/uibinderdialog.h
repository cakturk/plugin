#ifndef UIBINDERDIALOG_H
#define UIBINDERDIALOG_H

#include <QWidget>

namespace Ui {
    class uiBinderDialog;
}

class uiBinderDialog : public QWidget
{
    Q_OBJECT

public:
    explicit uiBinderDialog(QWidget *parent = 0);
    ~uiBinderDialog();

private:
    Ui::uiBinderDialog *ui;
};

#endif // UIBINDERDIALOG_H
