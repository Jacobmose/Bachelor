#include "confirmprintdialog.h"
#include "ui_confirmprintdialog.h"

ConfirmPrintDialog::ConfirmPrintDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfirmPrintDialog)
{
    ui->setupUi(this);

    connect(ui->btnConfirmPrint, SIGNAL(clicked(bool)), this, SLOT(on_btnConfirmPrint_clicked()));
}

ConfirmPrintDialog::~ConfirmPrintDialog()
{
    delete ui;
}

void ConfirmPrintDialog::on_btnConfirmPrint_clicked()
{
    //MainWindow::handleAndSendGCodesFromFile("kodetest.gcode");

    MainWindow w;

    QString fileName = "kodetest.gcode";

    emit w.startPrint(fileName);
}
