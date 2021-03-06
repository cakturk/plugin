#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
    class settingsDialog;
}

class settingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit settingsDialog(QWidget *parent = 0);
    ~settingsDialog();

    QString ipAddress() const;

    void setIpAddress(const QString);
    void setStatus(const QString);

private:
    Ui::settingsDialog *ui;
};

#endif // SETTINGSDIALOG_H
