#ifndef FILEDIALOG_H
#define FILEDIALOG_H

#include <QDialog>
#include <QProcess>
#include <QDir>
#include <QDebug>
#include <QStringListModel>

#include "printobject.h"

namespace Ui {
class FileDialog;
}

class FileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FileDialog(QWidget *parent = 0);
    ~FileDialog();

    void setPrintObjects(const QList<PrintObject*> printObjects);
    void initializeFileList();

signals:
    void sliceFileClicked(QString fileName);

private slots:
    void on_btnCancel_clicked();
    void on_btnSlice_clicked();

private:
    Ui::FileDialog *ui;

    QList<PrintObject*> m_printObjects;

    QStringList getFileDirectory();
    QString getSelectedFileName();    

    QString m_selectedFileName;
    QStringListModel *listModel;
};

#endif // FILEDIALOG_H
