#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "devicedialog.h"
#include "serialhandler.h"

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



    bool isOkReceived();

private slots:
    void writeData(QString &data);

    void handleAndSendGCodesFromFile(QString &fileName);

    void readData();

    void onSerialReadyRead();
    void onSerialError();




    void on_btnHome_clicked();
    void on_btnXPlus_clicked();
    void on_btnYMinus_clicked();
    void on_btnXMinus_clicked();
    void on_btnYPlus_clicked();
    void on_btnZPlus_clicked();
    void on_btnZMinus_clicked();
    void on_btnNewPrint_clicked();

    void on_btnEmergencyStop_clicked();

    void on_btnStartPrint_clicked();

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QSerialPort *m_serialPort;

    QString removeComments(QString data);
    QString removeWhiteSpace(QString data);


    CommandQueue commandQueue;
    PrinterStatus printerStatus;

    void sendNextCommand(int commandIndex);

    void openSerialPort();
    void closeSerialPort();


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

};

#endif // MAINWINDOW_H
