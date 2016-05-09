#include "figuredialog.h"
#include "ui_figuredialog.h"

FigureDialog::FigureDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FigureDialog)
{
    ui->setupUi(this);

    //connect(ui->btnFigureDialogNext, SIGNAL(clicked(bool)), this, SLOT(on_btnFigureDialogNext_clicked()));
}

FigureDialog::~FigureDialog()
{
    delete ui;
}

void FigureDialog::showFigureFileDirectory()
{
    QStringList fileDirectoryList;

}

void FigureDialog::on_btnFigureDialogCancel_clicked()
{

}

void FigureDialog::on_btnFigureDialogNext_clicked()
{
    ConfirmPrintDialog confirmPrintDialog;
    confirmPrintDialog.setModal(true);
    confirmPrintDialog.exec();
}
