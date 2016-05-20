#ifndef JOGWINDOW_H
#define JOGWINDOW_H

#include <QWidget>

namespace Ui {
class JogWindow;
}

class JogWindow : public QWidget
{
    Q_OBJECT

public:
    explicit JogWindow(QWidget *parent = 0);
    ~JogWindow();

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

    void homeAxisClicked();

private slots:
    void on_btnClose_clicked();

private:
    Ui::JogWindow *ui;
};

#endif // JOGWINDOW_H
