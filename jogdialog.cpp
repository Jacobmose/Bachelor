#include "jogdialog.h"
#include "ui_jogdialog.h"

/**
 * @brief JogDialog::JogDialog
 * @param parent
 */
JogDialog::JogDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::JogDialog)
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

    //this->setObjectName("jogDialogObj");
}

/**
 * @brief JogDialog::~JogDialog
 */
JogDialog::~JogDialog()
{
    delete ui;
}

/**
 * @brief JogDialog::on_btnClose_clicked
 */
void JogDialog::on_btnClose_clicked()
{
    close();
}
