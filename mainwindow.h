#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProgressBar>
#include <QImageReader>
#include <QLabel>
#include <QDir>
#include <QStringListModel>
#include <QDebug>
#include <QFile>

#include "serialhandler.h"
#include "browsefigureswidget.h"
#include "jogdialog.h"
#include "filedialog.h"

#define COMMAND_BUFFER_LENGTH        127
#define SERIAL_PORT_NAME            "COM8"
#define SERIAL_BAUD_RATE            115200
#define FIGURE_FILE_DIRECTORY       "path"
#define GCODE_FILE_DIRECTORY        "C:/Users/jacobmosehansen/Desktop/Test/"
#define IMAGE_DIRECTORY             "C:/Users/jacobmosehansen/Pictures/testpic/"
#define M114                        "M114"
#define M105                        "M105"
#define M112                        "M112"
#define G1                          "G1"
#define G28                         "G28"
#define NEW_LINE                    "\r\n"

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

    void onStartPrintFromFileClicked(QString fileName);

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

    QWidget *imageWidget;
    QLabel *imageLabel;
    QTime *timer;

    QString selectedImageName;
    int progress;

    CommandQueue commandQueue;

    QString removeComments(QString data);
    QString removeWhiteSpace(QString data);

    void openSerialPort();
    void closeSerialPort();
    void getFigureFileDirectory();
    void sendNextCommand(int commandIndex);
    void loadFile(QString fileName);
    void updateRemainingTime(QTime timer);
    void updateProgressBar();
    void addProgressBar();
    void updateAxisPositions(QString axis, double axisPosition);
    void setAxisSteps(double steps);
    void setTemperatureReadings(QStringList list);
    void setAxisPosition(QStringList list);
    void setImage(QString &fileName);

    int mapValueToPercent(int value, int max);
    double getAxisPositions(QString axis);
};

#endif // MAINWINDOW_H
