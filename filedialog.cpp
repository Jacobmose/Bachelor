#include "filedialog.h"
#include "ui_filedialog.h"

#include <QModelIndex>
#include <QStringListModel>

FileDialog::FileDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileDialog)
{
    ui->setupUi(this);

    //connect(ui->btnNext, SIGNAL(clicked(bool)), this, SIGNAL(onBtnNextClicked()));

    QStringList fileDirectoryList;
    fileDirectoryList = getFileDirectory();

    listModel = new QStringListModel(this);
    listModel->setStringList(fileDirectoryList);
    ui->lvFiles->setModel(listModel);
}

FileDialog::~FileDialog()
{
    delete ui;
}

void FileDialog::on_btnCancel_clicked()
{
    close();
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
    QString fileName;

    ui->lvFiles->setSelectionMode(QAbstractItemView::SingleSelection);

    foreach(const QModelIndex &index,
            ui->lvFiles->selectionModel()->selectedIndexes())
    {
        fileName = index.data(Qt::DisplayRole).toString();
    }

    return fileName;
}

QStringList FileDialog::getFileDirectory()
{
    QDir figureDir("C:/Users/jacobmosehansen/Desktop/Test");
    figureDir.setNameFilters(QStringList("*.stl"));
    figureDir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);

    QStringList figureList = figureDir.entryList();

    for(int i=0; i<figureList.count(); i++)
    {
        figureList[i];
    }

    return figureList;
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

void FileDialog::on_btnPrint_clicked()
{
    QString fileName;

    fileName = getSelectedFileName();

    sliceFile(fileName);

    if(isGCodeFileCreated(fileName))
        //start print

    qDebug() << fileName;
}

void FileDialog::on_btnSlice_clicked()
{
    QString fileName;

    fileName = getSelectedFileName();

    sliceFile(fileName);
}
