#ifndef FILEDIALOG_H
#define FILEDIALOG_H

#include <QDialog>

#include <QProcess>

#include <QDir>

#include <QDebug>

#include <QStringListModel>

namespace Ui {
class FileDialog;
}

class FileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FileDialog(QWidget *parent = 0);
    ~FileDialog();

signals:
    void onBtnNextClicked();

private slots:
    void on_btnCancel_clicked();

    void on_btnPrint_clicked();

    void on_btnSlice_clicked();

private:
    Ui::FileDialog *ui;

    QStringList getFileDirectory();

    void sliceFile(QString fileName);
    QString getSelectedFileName();
    bool isGCodeFileCreated(QString fileName);

    QStringListModel *listModel;
};

#endif // FILEDIALOG_H
