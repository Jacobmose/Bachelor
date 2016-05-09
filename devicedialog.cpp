#include "devicedialog.h"
#include "ui_devicedialog.h"

DeviceDialog::DeviceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeviceDialog)
{
    btnNumber = 0;
    ui->setupUi(this);

    //connect(ui->btnDeviceDialogSDCard, SIGNAL(clicked()), this, SLOT(DeviceDialog::on_btnDeviceDialogSDCard_clicked()) );
    //connect(ui->btnDeviceDialogUSB, SIGNAL(clicked(bool)), this, SLOT(DeviceDialog::on_btnDeviceDialogUSB_clicked()));
}

DeviceDialog::~DeviceDialog()
{
    delete ui;
}

void DeviceDialog::on_btnDeviceDialogSDCard_clicked()
{
    FigureDialog figureDialog;
    figureDialog.setModal(true);
    figureDialog.exec();
    //btnNumber = 1;
    close();
}

void DeviceDialog::on_btnDeviceDialogUSB_clicked()
{
    close();
}
