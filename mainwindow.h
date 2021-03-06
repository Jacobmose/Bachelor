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
#include <QSerialPort>

#include "browsefigureswidget.h"
#include "filedialog.h"
#include "jogdialog.h"

#define COMMAND_BUFFER_LENGTH        127
#define SERIAL_PORT_NAME            "COM8"
#define SERIAL_BAUD_RATE            115200
#define FIGURE_FILE_DIRECTORY       "path"
#define GCODE_FILE_DIRECTORY        "C:/Users/jacobmosehansen/Desktop/Test/"
#define IMAGE_DIRECTORY             "C:/Users/jacobmosehansen/Pictures/testpic/"
#define PRE_HEAT_TIME_IN_MSEC       1200000
#define NEW_LINE                    "\r\n"
#define PRE_HEAT_TEMPERATURE        "60"
#define PRINT_TEMPERATURE           "30"

// G-codes. For addtional information of their function please see: http://reprap.org/wiki/G-code
#define M114                        "M114" // Get Current Position
#define M105                        "M105" // Get Extruder Temp
#define M112                        "M112" // Emergency Stop
#define G1                          "G1"   // Move Axis
#define G28                         "G28"  // Home Axis
#define M104                        "M104" // Set Extruder Temp
#define M109                        "M109" // Set Extruder Temp and Wait
#define G92                         "G92"  // Set Axis Position

namespace Ui {
    class MainWindow;
}

struct PrinterStatus {
    double MPosX;
    double MPosY;
    double MPosZ;
    double MPosE;
    double extruderTemp;
    double bedTemp;
    double axisSteps = 1;
    bool isPrinting = false;
    bool isPrintComplete = false;
    bool isPreHeating = false;
};

struct CommandQueue {
    QList<QString> commandList;
    int commandIndex;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

signals:
    void sendCommand(QString &data);

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    PrinterStatus printerStatus;

public slots:
    void onJogBtnXPlusClicked();
    void onJogBtnYPlusClicked();
    void onJogBtnZPlusClicked();
    void onJogBtnEPlusClicked();
    void onJogBtnXMinusClicked();
    void onJogBtnYMinusClicked();
    void onJogBtnZMinusClicked();
    void onJogBtnEMinusClicked();

    void onJogHalfStepClicked();
    void onJogOneStepClicked();
    void onJogTwoStepClicked();
    void onJogFiveStepClicked();

    void onHomeAxisClicked();

    void onSliceFileClicked(QString fileName);

private slots:
    void writeData(QString &data);
    void onSerialReadyRead();
    void onSerialError();
    void onListItemPressed(QModelIndex index);
    void on_btnPreHeat_clicked();
    void on_btnEmergencyStop_clicked();
    void on_btnStartPrint_clicked();
    void on_btnJog_clicked();
    void on_btnSliceAndPrint_clicked();
    void on_btnReloadChocolate_clicked();

private:
    Ui::MainWindow *ui;
    QSerialPort *m_serialPort;
    JogDialog *jogDialog;
    FileDialog *fileDialog;
    QProgressBar *progressBar;
    QWidget *imageWidget;
    QLabel *imageLabel;
    QTime *timer;
    QTime totalPreHeatingTimer;
    QStringListModel *consoleModel;
    QStringList consoleMessages;
    QString selectedPrintFileName;
    int progress;
    int timerId;
    CommandQueue commandQueue;

    QString removeComments(QString data);
    QString removeWhiteSpace(QString data);
    void openSerialPort();
    void closeSerialPort();
    void sendNextCommand(int commandIndex);
    void loadFile(QString fileName);
    void updateRemainingPrintTime(QTime timer);
    void updateProgressBar();
    void addProgressBar();
    void updateAxisPositions(QString axis, double axisPosition);
    void updateExtruderPositions(double position);
    void setAxisSteps(double steps);
    void setTemperatureReadings(QStringList list);
    void setAxisPosition(QStringList list);
    void setImage(QString &fileName);
    void addConsoleMessage(QStringList messages);
    void addConsoleMessage(QString message);
    void sliceFile(PrintObject* printObject);
    int getRemainingPreHeatTime(QTime time);
    void timerEvent(QTimerEvent *);
    int mapValueToPercent(int value, int max);
    double getAxisPositions(QString axis);
    void addDefaultImageInPreviewContainer();
};

#endif // MAINWINDOW_H
