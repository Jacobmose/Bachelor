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
    void onStartPrintClicked();
    void startPrintFromFile(QString fileName);

private slots:
    void on_btnCancel_clicked();
    void on_btnPrint_clicked();
    void on_btnSlice_clicked();

private:
    Ui::FileDialog *ui;

    QStringList getFileDirectory();
    bool isGCodeFilePresent(QString fileName);
    void sliceFile(QString fileName);
    QString getSelectedFileName();

    QString m_selectedFileName;

    QStringListModel *listModel;
};

#endif // FILEDIALOG_H
