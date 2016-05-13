#include "sliceandprintdialog.h"
#include "ui_sliceandprintdialog.h"

SliceAndPrintDialog::SliceAndPrintDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SliceAndPrintDialog)
{
    ui->setupUi(this);
}

SliceAndPrintDialog::~SliceAndPrintDialog()
{
    delete ui;
}
