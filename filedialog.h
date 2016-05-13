#ifndef FILEDIALOG_H
#define FILEDIALOG_H

#include <QDialog>

#include <QProcess>

#include <QDir>

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

private:
    Ui::FileDialog *ui;

    void sliceFile(QString fileName);
    QString getSelectedFileName();
    bool isGCodeFileCreated(QString fileName);
};

#endif // FILEDIALOG_H
