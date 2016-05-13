#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QProgressBar>

#include <QImageReader>

#include <QLabel>

#include <QDir>
#include "serialhandler.h"

#include "browsefigureswidget.h"

#include "jogdialog.h"

#include <QStringListModel>

#include <QDebug>
#include <QFile>

#include "filedialog.h"

#define COMMAND_BUFFER_LENGTH 127

namespace Ui {
class MainWindow;
}

struct PrinterStatus {
    double MPosX;
    double MPosY;
    double MPosZ;
    double extruderTemp;
    double bedTemp;
    double axisSteps = 1;
    bool isPrinting = false;
    bool isPrintComplete = false;
};

struct CommandQueue {
    QList<QString> commandList;
    int commandIndex;
};

struct CommandControl {
    bool isPendingOk = false;
    bool isPrinting = false;
    bool isPrintComplete = false;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

signals:
    void sendData(QString &data);
    void sendCommand(QString &data);
    void startPrint(QString &fileName);

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


    PrinterStatus printerStatus;

public slots:
    void onJogBtnXPlusClicked();
    void onJogBtnYPlusClicked();
    void onJogBtnZPlusClicked();
    void onJogBtnXMinusClicked();
    void onJogBtnYMinusClicked();
    void onJogBtnZMinusClicked();

    void onJogHalfStepClicked();
    void onJogOneStepClicked();
    void onJogTwoStepClicked();
    void onJogFiveStepClicked();

private slots:

    void writeData(QString &data);
    void onSerialReadyRead();
    void onSerialError();

    void onListItemClicked(QListWidgetItem* item);

    void on_btnHome_clicked();    
    void on_btnEmergencyStop_clicked();
    void on_btnStartPrint_clicked();
    void on_btnJog_clicked();

    void on_btnSliceAndPrint_clicked();

private:
    Ui::MainWindow *ui;
    QSerialPort *m_serialPort;
    JogDialog *jogDialog;
    FileDialog *fileDialog;

    QProgressBar *progressBar;

    QString removeComments(QString data);
    QString removeWhiteSpace(QString data);

    void getFigureFileDirectory();

    CommandQueue commandQueue;

    void sendNextCommand(int commandIndex);

    void openSerialPort();
    void closeSerialPort();

    int mapValueToPercent(int value, int max);


    QString dataRead;
    QString axisPositions;

    double getAxisPositions(QString axis);

    double newPos;

    int commandBufferLength();

    double m_xPos = 0;
    double m_yPos = 0;
    double m_zPos = 0;


    int m_fileCommandIndex;
    int m_fileProcessedIndex;

    bool m_isPrinting;
    bool m_printComplete;
    bool m_lastFileCommandSent;

    QString m_fileName;

    void loadFile(QString fileName);

    //SerialHandler *serialHandler;


    QString selectedImageName;

    QWidget *imageWidget;
    QLabel *imageLabel;


    //for image display
    void setImage(QString &fileName);

};

#endif // MAINWINDOW_H
