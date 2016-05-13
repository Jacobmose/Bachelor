#include "filedialog.h"
#include "ui_filedialog.h"

#include <QModelIndex>
#include <QStringListModel>

FileDialog::FileDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileDialog)
{
    ui->setupUi(this);

    connect(ui->btnNext, SIGNAL(clicked(bool)), this, SIGNAL(onBtnNextClicked()));
}

FileDialog::~FileDialog()
{
    delete ui;
}

void FileDialog::on_btnCancel_clicked()
{
    close();
}

void FileDialog::on_btnNext_clicked()
{
    QString fileName;

    fileName = getSelectedFileName();

    sliceFile(fileName);

    if(isGCodeFileCreated(fileName))
        //start print

    qDebug() << fileName;

}

bool FileDialog::isGCodeFileCreated(QString fileName)
{
    QDir fileDir("C:/Users/jacobmosehansen/Pictures/testpic");
    fileDir.setNameFilters(QStringList("*.gcode"));
    fileDir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);

    QStringList fileList = fileDir.entryList();

    for(int i=0; i<fileList.count(); i++)
        fileList[i];

    if(fileList.contains(fileName))
        return true;

    return false;
}

QString FileDialog::getSelectedFileName()
{
    QModelIndexList list = ui->lvFiles->selectionModel()->selectedIndexes();

    QStringList fileList;
    foreach(QModelIndex index, list){
        fileList = list.append(index.data().toString());
    }

    fileName = fileList.first();

    return fileName;
}

void FileDialog::sliceFile(QString fileName)
{
    QProcess process;
    QString configPath = "";
    QString outputPath = "/path/to/" + fileName;
    QString sliceCommand = "slic3r " + fileName + " --output " + outputPath;

    // load config file
    process.execute("--load " + configPath);
    process.waitForFinished();

    qDebug() << process.readAll();

    // slice file
    process.execute(sliceCommand);
    process.waitForFinished();

    qDebug() << process.readAll();

    process.close();
}
