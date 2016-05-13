#ifndef SLICEANDPRINTDIALOG_H
#define SLICEANDPRINTDIALOG_H

#include <QDialog>

namespace Ui {
class SliceAndPrintDialog;
}

class SliceAndPrintDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SliceAndPrintDialog(QWidget *parent = 0);
    ~SliceAndPrintDialog();

private:
    Ui::SliceAndPrintDialog *ui;
};

#endif // SLICEANDPRINTDIALOG_H
