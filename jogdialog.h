#ifndef JOGDIALOG_H
#define JOGDIALOG_H

#include <QDialog>

namespace Ui {
class JogDialog;
}

class JogDialog : public QDialog
{
    Q_OBJECT

public:
    explicit JogDialog(QWidget *parent = 0);
    ~JogDialog();

signals:
    void jogXPlusClicked();
    void jogYPlusClicked();
    void jogZPlusClicked();
    void jogXMinusClicked();
    void jogYMinusClicked();
    void jogZMinusClicked();

    void jogHalfStepClicked();
    void jogOneStepClicked();
    void jogTwoStepClicked();
    void jogFiveStepClicked();

private:
    Ui::JogDialog *ui;
};

#endif // JOGDIALOG_H
