#include "filedialog.h"
#include "ui_filedialog.h"

#include <QModelIndex>
#include <QStringListModel>

/**
 * @brief FileDialog constructor
 * @param parent
 */
FileDialog::FileDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileDialog)
{
    ui->setupUi(this);    
}

/**
 * @brief FileDialog destructor
 */
FileDialog::~FileDialog()
{
    delete ui;
}

/**
 * @brief Slot for cancel clicked signal
 */
void FileDialog::on_btnCancel_clicked()
{
    close();
}

/**
 * @brief This is done because we have to wait for the PrintObject list before loading files into the file dialog
 *
 */
void FileDialog::initializeFileList()
{
    QStringList fileDirectoryList;
    fileDirectoryList = getFileDirectory();

    listModel = new QStringListModel(this);
    listModel->setStringList(fileDirectoryList);
    ui->lvFiles->setModel(listModel);
}

void FileDialog::setPrintObjects(const QList<PrintObject*> printObjects)
{
    m_printObjects = printObjects;
}

/**
 * @brief Gets selected file name
 * @return
 */
QString FileDialog::getSelectedFileName()
{
    QString fileName;

    ui->lvFiles->setSelectionMode(QAbstractItemView::SingleSelection);

    foreach(const QModelIndex &index,
            ui->lvFiles->selectionModel()->selectedIndexes())
    {
        fileName = index.data(Qt::DisplayRole).toString();
    }

    return fileName.section(".", 0, 0);
}

/**
 * @brief Gets the file directory for .stl files
 * @return
 */
QStringList FileDialog::getFileDirectory()
{
    QDir figureDir("C:/Users/jacobmosehansen/Desktop/Test");
    figureDir.setNameFilters(QStringList("*.stl"));
    figureDir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);

    QStringList figureList = figureDir.entryList();

    foreach(QString file, figureList)
    {
        QString correctedFileName = file;

        foreach(PrintObject *printObject, m_printObjects)
        {
            if(printObject->m_name + ".stl" != correctedFileName && !figureList.contains(correctedFileName))
            {
                figureList.append(correctedFileName);
            }
            else
            {
                figureList.removeOne(correctedFileName);
            }
        }
    }

    return figureList;
}

/**
 * @brief Slot for slice button clicked signal
 */
void FileDialog::on_btnSlice_clicked()
{
    m_selectedFileName = getSelectedFileName();    

    emit sliceFileClicked(m_selectedFileName);

    close();
}
