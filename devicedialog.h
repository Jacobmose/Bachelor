#ifndef DEVICEDIALOG_H
#define DEVICEDIALOG_H

#include <QDialog>

#include "figuredialog.h"

namespace Ui {
class DeviceDialog;
}

class DeviceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DeviceDialog(QWidget *parent = 0);
    ~DeviceDialog();

    int btnNumber;

private slots:
    void on_btnDeviceDialogSDCard_clicked();

    void on_btnDeviceDialogUSB_clicked();

private:
    Ui::DeviceDialog *ui;
};

#endif // DEVICEDIALOG_H
