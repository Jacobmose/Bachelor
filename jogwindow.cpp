#include "jogwindow.h"
#include "ui_jogwindow.h"

JogWindow::JogWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::JogWindow)
{
    ui->setupUi(this);

    // Jog controls
    connect(ui->btnXPlus, SIGNAL(clicked(bool)), this, SIGNAL(jogXPlusClicked()));
    connect(ui->btnYPlus, SIGNAL(clicked(bool)), this, SIGNAL(jogYPlusClicked()));
    connect(ui->btnZPlus, SIGNAL(clicked(bool)), this, SIGNAL(jogZPlusClicked()));
    connect(ui->btnXMinus, SIGNAL(clicked(bool)), this, SIGNAL(jogXMinusClicked()));
    connect(ui->btnYMinus, SIGNAL(clicked(bool)), this, SIGNAL(jogYMinusClicked()));
    connect(ui->btnZMinus, SIGNAL(clicked(bool)), this, SIGNAL(jogZMinusClicked()));

    // Jog steps
    connect(ui->btnHalfStep, SIGNAL(clicked(bool)), this, SIGNAL(jogHalfStepClicked()));
    connect(ui->btnOneStep, SIGNAL(clicked(bool)), this, SIGNAL(jogOneStepClicked()));
    connect(ui->btnTwoStep, SIGNAL(clicked(bool)), this, SIGNAL(jogTwoStepClicked()));
    connect(ui->btnFiveStep, SIGNAL(clicked(bool)), this, SIGNAL(jogFiveStepClicked()));

    // Home
    connect(ui->btnHomeAxis, SIGNAL(clicked(bool)), this, SIGNAL(homeAxisClicked()));
}

JogWindow::~JogWindow()
{
    delete ui;
}

void JogWindow::on_btnClose_clicked()
{
    close();
}
