#include "jogdialog.h"
#include "ui_jogdialog.h"

/**
 * @brief JogDialog constructor
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
    connect(ui->btnEPlus, SIGNAL(clicked(bool)), this, SIGNAL(jogEPlusClicked()));
    connect(ui->btnXMinus, SIGNAL(clicked(bool)), this, SIGNAL(jogXMinusClicked()));
    connect(ui->btnYMinus, SIGNAL(clicked(bool)), this, SIGNAL(jogYMinusClicked()));
    connect(ui->btnZMinus, SIGNAL(clicked(bool)), this, SIGNAL(jogZMinusClicked()));
    connect(ui->btnEMinus, SIGNAL(clicked(bool)), this, SIGNAL(jogEMinusClicked()));

    // Jog steps
    connect(ui->btnHalfStep, SIGNAL(clicked(bool)), this, SIGNAL(jogHalfStepClicked()));
    connect(ui->btnOneStep, SIGNAL(clicked(bool)), this, SIGNAL(jogOneStepClicked()));
    connect(ui->btnTwoStep, SIGNAL(clicked(bool)), this, SIGNAL(jogTwoStepClicked()));
    connect(ui->btnFiveStep, SIGNAL(clicked(bool)), this, SIGNAL(jogFiveStepClicked()));

    // Home
    connect(ui->btnHomeAxis, SIGNAL(clicked(bool)), this, SIGNAL(homeAxisClicked()));

    if(!isAxisHomed)
        initializeMoveButtons(false);
    else
        initializeMoveButtons(true);
}

/**
 * @brief JogDialog destructor
 */
JogDialog::~JogDialog()
{
    delete ui;
}

/**
 * @brief Slot to close JogDialog on close clicked
 */
void JogDialog::on_btnClose_clicked()
{
    close();
}

void JogDialog::on_btnHomeAxis_clicked()
{
    initializeMoveButtons(true);
}

void JogDialog::initializeMoveButtons(bool flag)
{
    ui->btnXPlus->setEnabled(flag);
    ui->btnYPlus->setEnabled(flag);
    ui->btnZPlus->setEnabled(flag);
    ui->btnEPlus->setEnabled(flag);
    ui->btnXMinus->setEnabled(flag);
    ui->btnYMinus->setEnabled(flag);
    ui->btnZMinus->setEnabled(flag);
    ui->btnEMinus->setEnabled(flag);
}
