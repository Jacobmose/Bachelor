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
    void jogEPlusClicked();
    void jogXMinusClicked();
    void jogYMinusClicked();
    void jogZMinusClicked();
    void jogEMinusClicked();

    void jogHalfStepClicked();
    void jogOneStepClicked();
    void jogTwoStepClicked();
    void jogFiveStepClicked();

    void homeAxisClicked();

private slots:
    void on_btnClose_clicked();

    void on_btnHomeAxis_clicked();

private:
    Ui::JogDialog *ui;
    void initializeMoveButtons(bool flag);

    bool isAxisHomed = false;
};

#endif // JOGDIALOG_H
