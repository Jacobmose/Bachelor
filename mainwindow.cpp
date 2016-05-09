#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QStringListModel>

#include <QDebug>
#include <QFile>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_serialPort = new QSerialPort(this);

    //connect(m_serialPort, &QSerialPort::readyRead, this, &MainWindow::readData);

    connect(m_serialPort, &QSerialPort::readyRead, this, &MainWindow::onSerialReadyRead);

    connect(this, &MainWindow::sendData, this, &MainWindow::writeData);
    connect(this, &MainWindow::sendCommand, this, &MainWindow::writeData);

    connect(this, &MainWindow::startPrint, this, &MainWindow::handleAndSendGCodesFromFile);

    QImage image("C:/Projects/QtCreator/Bachelor/Bachelor/images/arrow.png");
    ui->btnXMinus->setIcon(QPixmap::fromImage(image));
    ui->btnXMinus->setIconSize(ui->btnXMinus->size());
    ui->btnXMinus->resize(ui->btnXMinus->size());

    centralWidget()->setStyleSheet("background: #F2F2E4");

    openSerialPort();
}

MainWindow::~MainWindow()
{
    closeSerialPort();
    delete ui;
}

void MainWindow::openSerialPort()
{
    m_serialPort->setPortName("COM6");
    m_serialPort->setBaudRate(250000);
    m_serialPort->setDataBits(QSerialPort::Data8);
    m_serialPort->setParity(QSerialPort::NoParity);
    m_serialPort->setStopBits(QSerialPort::OneStop);
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);
    m_serialPort->open(QIODevice::ReadWrite);

    if(!m_serialPort->isOpen())
        qDebug() << "SerialPort not openend...";
}

void MainWindow::closeSerialPort()
{
    if(m_serialPort->isOpen())
        m_serialPort->close();
}

void MainWindow::loadFile(QString fileName)
{
    QString lineData;
    QFile file("C:/Users/jacobmosehansen/Desktop/Test/" + fileName);
//    if(!file.open(QIODevice::ReadOnly))
//    {
//        qDebug() << "Cannot open file: " << fileName;
//        return;
//    }

    if(file.open(QIODevice::ReadOnly))
    {
        QTextStream textStream(&file);
        while(!textStream.atEnd())
        {
            lineData = textStream.readLine();

            qDebug() << "lineData: " << lineData;

            lineData = removeComments(lineData);

            qDebug() << "lineData with removed comments: " << lineData;

            lineData = removeWhiteSpace(lineData);

            qDebug() << "lineData with removed whitspace: " << lineData;

            //lineData.append("\r\n");
            lineData = lineData.trimmed();

            qDebug() << "Final lineData to append to commandList: " << lineData;

            commandQueue.commandList.append(lineData);
        }
    }

    commandQueue.commandList.removeAll("");
    file.close();
}

void MainWindow::writeData(QString &data)
{
    m_serialPort->write(data.toLatin1());

    qDebug() << "writeData: " <<data;
}

void MainWindow::onSerialReadyRead()
{
    while(m_serialPort->canReadLine())
    {
        QString data = m_serialPort->readLine().trimmed();

        qDebug() << "Entire serial data: " << data;

        QRegExp coordRegExp("X:(\\d+.\\d+)\\sY:(\\d+.\\d+)\\sZ:(\\d+.\\d+)");
        QRegExp tempRegExp("T:(\\d+.\\d+)......B:(\\d+.\\d+)");

        if(data.contains(coordRegExp))
        {
            if(coordRegExp.indexIn(data) != -1)
            {
                printerStatus.MPosX = coordRegExp.cap(1).toDouble();
                printerStatus.MPosY = coordRegExp.cap(2).toDouble();
                printerStatus.MPosZ = coordRegExp.cap(3).toDouble();

                qDebug() << "MPosX = " << printerStatus.MPosX << ", MPosY = " << printerStatus.MPosY << ", MPosZ = " << printerStatus.MPosZ;
            }
        }
        else if(data.contains(tempRegExp))
        {
            if(tempRegExp.indexIn(data) != -1)
            {
                printerStatus.extruderTemp = tempRegExp.cap(1).toDouble();
                printerStatus.bedTemp = tempRegExp.cap(2).toDouble();
            }
        }
        else if(data.length() > 0)
        {
            if(printerStatus.isPrinting)
            {
                if(data.contains("ok"))
                {
                    commandQueue.commandIndex++;

                    if(commandQueue.commandIndex > commandQueue.commandList.count())
                    {
                        printerStatus.isPrintComplete = true;
                        printerStatus.isPrinting = false;

                        qDebug() << "commandIndex is > than commandList.count";

                        return;
                    }

                    sendNextCommand(commandQueue.commandIndex);

                    qDebug() << "ok received... Command at index: " << commandQueue.commandIndex << " is sent";
                }
            }
        }
    }

    QStringList list;
    list.append("Axis positions:");
    list.append("X: " + QString::number(printerStatus.MPosX));
    list.append("Y: " + QString::number(printerStatus.MPosZ));
    list.append("Z: " + QString::number(printerStatus.MPosY));

    list.append("Temperatures:");
    list.append("Extruder: " + QString::number(printerStatus.extruderTemp));
    list.append("Room: " + QString::number(printerStatus.bedTemp));

    QStringListModel *model = new QStringListModel(this);
    model->setStringList(list);
    ui->testListView->setModel(model);
}

void MainWindow::sendNextCommand(int commandIndex)
{
    QString data = commandQueue.commandList.at(commandIndex -1);

    data.append("\r\n");

    qDebug() << "sendNextCommand data: " << data;

    emit sendCommand(data);
}

void MainWindow::onSerialError()
{

}

void MainWindow::readData()
{
    QString serialRead;
    serialRead.append(m_serialPort->readAll());

    dataRead.append(serialRead);

    qDebug() << "dataRead :" << dataRead;

    if(isOkReceived())
    {
        if(dataRead.contains("X:"))
        {
            axisPositions = dataRead;
            qDebug() << "axisPositions: " << axisPositions;

            m_xPos = getAxisPositions("X");
            m_yPos = getAxisPositions("Y");
            m_zPos = getAxisPositions("Z");
        }

        dataRead.clear();
        qDebug() << "ok received!!";
    }

    QStringList list;
    list.append(serialRead);

    QStringListModel *model = new QStringListModel(this);
    model->setStringList(list);
    ui->testListView->setModel(model);
}

bool MainWindow::isOkReceived()
{
    QStringList data = dataRead.split("\n");

    qDebug() << "data in ok func: " << data;

    //if(data.takeLast() == "ok")
    if(data.contains("ok"))
        return true;

    return false;
}

double MainWindow::getAxisPositions(QString axis)
{
    QRegExp rx("(\\d*.\\d+)");
    QString axisPos;
    QString position;
    QStringList axisPosList;
    int pos = 0;

    axisPos = axisPositions;

    while((pos = rx.indexIn(axisPos, pos)) != -1)
    {
        axisPosList << rx.cap(1);
        pos += rx.matchedLength();
    }

    if(!axisPosList.empty())
    {
        if(axis == "X")
            position = axisPosList[0];
        if(axis == "Y")
            position = axisPosList[1];
        if(axis == "Z")
            position = axisPosList[2];
    }
    else
        qDebug() << "Error: Axis position list is empty!";

    return position.toDouble();
}

void MainWindow::handleAndSendGCodesFromFile(QString &fileName)
{
    QString name = fileName;
    QString lineData;
    QString lineRead;

    QFile file("C:/Users/jacobmosehansen/Desktop/Test/" + name);

    if(m_serialPort->isOpen())
    {
        if(file.open(QIODevice::ReadOnly))
        {
            QTextStream textStream(&file);

            while(!textStream.atEnd())
            {
                lineData = textStream.readLine();
                lineRead = lineData.toLatin1();
                lineRead.append("\r\n");

                qDebug () << "Before OK check...." << lineRead;

                //if(isOkReceived())

                    //qDebug() << "lineRead of GCode file: " << lineData;
                    emit sendData(lineRead);
            }

            file.close();

            qDebug() << "End of file... All GCodes should have been sent.";
        }
    }
}

QString MainWindow::removeComments(QString data)
{
    data.replace(QRegExp(";.*"), "");

    return data.trimmed();
}

QString MainWindow::removeWhiteSpace(QString data)
{
    return data.replace(QRegExp("\\s"), "");
}

void MainWindow::on_btnNewPrint_clicked()
{
    DeviceDialog deviceDialog;

    deviceDialog.setModal(true);
    deviceDialog.exec();


    //QString file = "kodetest.gcode";
    //handleAndSendGCodesFromFile(file);
}

void MainWindow::on_btnHome_clicked()
{
    QString homeCode = "G28\r\n";
    QString axisPosCode = "M114\r\n";


    emit sendCommand(homeCode);

    emit sendCommand(axisPosCode);

        //qDebug() << "ok received after home. sending m114 to get axis pos";
}

void MainWindow::on_btnXPlus_clicked()
{
    //m_xPos += 5;
    printerStatus.MPosX += 5;

    QString positionString = "G1 X";
    positionString.append(QString::number(printerStatus.MPosX));
    positionString.append("\r\n");

    emit sendCommand(positionString);
}

void MainWindow::on_btnXMinus_clicked()
{
    //m_xPos -= 5;
    printerStatus.MPosX -= 5;

    QString positionString = "G1 X";
    positionString.append(QString::number(printerStatus.MPosX));
    positionString.append("\r\n");

    emit sendCommand(positionString);
}

void MainWindow::on_btnYPlus_clicked()
{
    //m_yPos += 5;
    printerStatus.MPosY += 5;

    QString positionString = "G1 Y";
    positionString.append(QString::number(printerStatus.MPosY));
    positionString.append("\r\n");

    emit sendCommand(positionString);
}

void MainWindow::on_btnYMinus_clicked()
{
    //m_yPos -= 5;
    printerStatus.MPosY -= 5;

    QString positionString = "G1 Y";
    positionString.append(QString::number(printerStatus.MPosY));
    positionString.append("\r\n");

    emit sendCommand(positionString);
}

void MainWindow::on_btnZPlus_clicked()
{
    //m_zPos += 0.5;
    printerStatus.MPosZ += 0.5;

    QString positionString = "G1 Z";
    positionString.append(QString::number(printerStatus.MPosZ));
    positionString.append("\r\n");

    emit sendCommand(positionString);
}

void MainWindow::on_btnZMinus_clicked()
{
    //m_zPos -= 0.5;
    printerStatus.MPosZ -= 0.5;

    QString positionString = "G1 Z";
    positionString.append(QString::number(printerStatus.MPosZ));
    positionString.append("\r\n");

    emit sendCommand(positionString);
}

void MainWindow::on_btnEmergencyStop_clicked()
{
    QString stopCommand = "M112\r\n";

    emit sendCommand(stopCommand);
}

void MainWindow::on_btnStartPrint_clicked()
{
    commandQueue.commandIndex = 0;
    commandQueue.commandList.clear();

    QString a = "yay.gcode";
    loadFile(a);

    qDebug() << "Entire loaded file: " << commandQueue.commandList;

    printerStatus.isPrinting = true;

    if(commandQueue.commandList.isEmpty())
    {
        qDebug() << "commandList is empty!";
        return;
    }

    QString firstCommand = commandQueue.commandList.at(commandQueue.commandIndex);

    firstCommand.append("\r\n");

    qDebug() << "Print started with first command: " << firstCommand;

    emit sendCommand(firstCommand);
}

void MainWindow::on_pushButton_clicked()
{
    QString statusMessage = "M114\r\n";

    emit sendCommand(statusMessage);

    QString a = "M105\r\n";

    emit sendCommand(a);
}
