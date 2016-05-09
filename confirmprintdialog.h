#ifndef CONFIRMPRINTDIALOG_H
#define CONFIRMPRINTDIALOG_H

#include <QDialog>

#include "mainwindow.h"

namespace Ui {
class ConfirmPrintDialog;
}

class ConfirmPrintDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfirmPrintDialog(QWidget *parent = 0);
    ~ConfirmPrintDialog();

private slots:
    void on_btnConfirmPrint_clicked();

private:
    Ui::ConfirmPrintDialog *ui;
};

#endif // CONFIRMPRINTDIALOG_H
