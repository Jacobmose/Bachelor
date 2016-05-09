#ifndef FIGUREDIALOG_H
#define FIGUREDIALOG_H

#include <QDialog>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QStringListModel>

#include "gcodehandler.h"

#include "confirmprintdialog.h"

namespace Ui {
class FigureDialog;
}

class FigureDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FigureDialog(QWidget *parent = 0);
    ~FigureDialog();

private slots:
    void on_btnFigureDialogCancel_clicked();

    void on_btnFigureDialogNext_clicked();

private:
    Ui::FigureDialog *ui;
    QStringListModel figureListModel;
    GCodeHandler *gcodeHandler;
    void showFigureFileDirectory();
};

#endif // FIGUREDIALOG_H
