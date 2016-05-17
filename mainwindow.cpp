#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    imageLabel(new QLabel),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    jogDialog = new JogDialog();
    fileDialog = new FileDialog();
    m_serialPort = new QSerialPort(this);
    timer = new QTime();

    connect(m_serialPort, &QSerialPort::readyRead, this, &MainWindow::onSerialReadyRead);
    connect(this, &MainWindow::sendCommand, this, &MainWindow::writeData);

    connect(fileDialog, SIGNAL(startPrintFromFile(QString)), this, SLOT(onStartPrintFromFileClicked(QString)));

    // Jog controls
    connect(jogDialog, SIGNAL(jogXPlusClicked()), this, SLOT(onJogBtnXPlusClicked()));
    connect(jogDialog, SIGNAL(jogYPlusClicked()), this, SLOT(onJogBtnYPlusClicked()));
    connect(jogDialog, SIGNAL(jogZPlusClicked()), this, SLOT(onJogBtnZPlusClicked()));
    connect(jogDialog, SIGNAL(jogXMinusClicked()), this, SLOT(onJogBtnXMinusClicked()));
    connect(jogDialog, SIGNAL(jogYMinusClicked()), this, SLOT(onJogBtnYMinusClicked()));
    connect(jogDialog, SIGNAL(jogZMinusClicked()), this, SLOT(onJogBtnZMinusClicked()));
    connect(jogDialog, SIGNAL(jogHalfStepClicked()), this, SLOT(onJogHalfStepClicked()));
    connect(jogDialog, SIGNAL(jogOneStepClicked()), this, SLOT(onJogOneStepClicked()));
    connect(jogDialog, SIGNAL(jogTwoStepClicked()), this, SLOT(onJogTwoStepClicked()));
    connect(jogDialog, SIGNAL(jogFiveStepClicked()), this, SLOT(onJogFiveStepClicked()));

    progressBar = new QProgressBar();
    progressBar->setMinimum(0);
    progressBar->setMaximum(100);

    ui->vlPrograssBar->addWidget(progressBar);

    connect(ui->lwBrowseFigures, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onListItemClicked(QListWidgetItem*)));

    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setScaledContents(true);

    ui->btnEmergencyStop->hide();


    ui->showFigureLayout->addWidget(imageLabel);

    openSerialPort();
}

MainWindow::~MainWindow()
{
    closeSerialPort();
    delete jogDialog;
    delete ui;
}

void MainWindow::onListItemClicked(QListWidgetItem* item)
{
      QPixmap image("C:/Users/jacobmosehansen/Pictures/testpic/" + item->text());
      imageLabel->setPixmap(image);

      selectedImageName = item->text();
}


void MainWindow::openSerialPort()
{
    m_serialPort->setPortName("COM8");
    m_serialPort->setBaudRate(115200);
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

void MainWindow::getFigureFileDirectory()
{
    QDir figureDir("C:/Users/jacobmosehansen/Pictures/testpic");
    figureDir.setNameFilters(QStringList("*.png"));
    figureDir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);

    QFileInfoList files = figureDir.entryInfoList();

    QListWidgetItem *fileItem = 0;

    foreach(QFileInfo file, files)
    {
        fileItem = new QListWidgetItem(file.fileName());
        fileItem->setIcon(QIcon("C:/Users/jacobmosehansen/Pictures/testpic/" + file.fileName()));
    }
}

void MainWindow::loadFile(QString fileName)
{
    QString lineData;
    QFile file("C:/Users/jacobmosehansen/Desktop/Test/" + fileName);



    if(file.open(QIODevice::ReadOnly))
    {
        if(file.isOpen())
        {
            QTextStream textStream(&file);
            while(!textStream.atEnd())
            {
                lineData = textStream.readLine();
                lineData = removeComments(lineData);
                lineData = lineData.trimmed();

                commandQueue.commandList.append(lineData);
            }
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
    QStringList list;

    QRegExp coordRegExp("X:(\\d+.\\d+)\\sY:(\\d+.\\d+)\\sZ:(\\d+.\\d+)");
    QRegExp tempRegExp("T:(\\d+.\\d+)......B:(\\d+.\\d+)");

    while(m_serialPort->canReadLine())
    {
        QString data = m_serialPort->readLine().trimmed();

        qDebug() << data;

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

                    int progress = mapValueToPercent(commandQueue.commandIndex, commandQueue.commandList.count());
                    progressBar->setValue(progress);

                    updateRemainingTime(progress);

                    if(commandQueue.commandIndex > commandQueue.commandList.count())
                    {
                        printerStatus.isPrintComplete = true;
                        printerStatus.isPrinting = false;

                        ui->btnStartPrint->setEnabled(true);

                        progressBar->setValue(0);
                        ui->lTime->setText("Done!");

                        timer->restart();

                        qDebug() << "No more commands from command queue.. Print is done";

                        return;
                    }

                    list.append("Command sent: " + commandQueue.commandList.at(commandQueue.commandIndex - 1));

                    sendNextCommand(commandQueue.commandIndex);

                    qDebug() << "Command at index: " << commandQueue.commandIndex << " is sent";
                }
                else if(data.contains("resend"))
                {
                    sendNextCommand(commandQueue.commandIndex);

                    qDebug() << "Command resent...";
                }
                else if(data.contains("error"))
                {
                    qDebug() << "Error on sending command!";

                }
            }
        }

        list.append("Reply received: " + data);
    }

//    list.append("Axis positions:");
//    list.append("X: " + QString::number(printerStatus.MPosX));
//    list.append("Y: " + QString::number(printerStatus.MPosZ));
//    list.append("Z: " + QString::number(printerStatus.MPosY));

//    list.append("Temperatures:");
//    list.append("Extruder: " + QString::number(printerStatus.extruderTemp));
//    list.append("Room: " + QString::number(printerStatus.bedTemp));

    //list.append(data);

    QStringListModel *model = new QStringListModel(this);
    model->setStringList(list);
    ui->commandListView->setModel(model);
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

int MainWindow::mapValueToPercent(int value, int max)
{
    int oldRange = (0-max);
    int percentRange = (0-99);

    int newValue = (((value - 0) * percentRange) / oldRange) + 0;

    return newValue;
}

void MainWindow::updateRemainingTime(int percent)
{
    double timeTaken = timer->elapsed();

    double timeLeft = timeTaken * (1/percent -1);

    ui->lTime->setText(QString::number(timeLeft));
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

void MainWindow::on_btnHome_clicked()
{
    QString homeCode = "G28\r\n";
    QString axisPosCode = "M114\r\n";
    QString tempCode = "M105\r\n";

    emit sendCommand(homeCode);
    emit sendCommand(axisPosCode);
    emit sendCommand(tempCode);
}

void MainWindow::on_btnEmergencyStop_clicked()
{
    QString stopCommand = "M112\r\n";

    ui->btnStartPrint->setEnabled(true);

    emit sendCommand(stopCommand);
}

void MainWindow::onStartPrintFromFileClicked(QString fileName)
{
    commandQueue.commandIndex = 0;
    commandQueue.commandList.clear();

    loadFile(fileName);

    printerStatus.isPrinting = true;

    if(commandQueue.commandList.isEmpty())
    {
        qDebug() << "commandList is empty!";
        return;
    }

    QString firstCommand = commandQueue.commandList.at(commandQueue.commandIndex);

    firstCommand.append("\r\n");

    qDebug() << "Print started with first command: " << firstCommand;

    timer->start();

    emit sendCommand(firstCommand);
}

void MainWindow::on_btnStartPrint_clicked()
{
    commandQueue.commandIndex = 0;
    commandQueue.commandList.clear();

    QString croppedImageName = selectedImageName.section(".", 0, 0) + ".gcode";

    loadFile(croppedImageName);

    printerStatus.isPrinting = true;

    if(commandQueue.commandList.isEmpty())
    {
        qDebug() << "commandList is empty!";
        return;
    }

    ui->btnEmergencyStop->show();
    ui->btnStartPrint->setEnabled(false);

    QString firstCommand = commandQueue.commandList.at(commandQueue.commandIndex);

    firstCommand.append("\r\n");

    qDebug() << "Print started with first command: " << firstCommand;

    timer->start();

    emit sendCommand(firstCommand);
}

void MainWindow::on_btnJog_clicked()
{
    jogDialog->setModal(true);
    jogDialog->exec();
}

void MainWindow::onJogBtnXPlusClicked()
{
    printerStatus.MPosX += printerStatus.axisSteps;

    QString positionString = "G1 X";
    positionString.append(QString::number(printerStatus.MPosX));
    positionString.append("\r\n");

    emit sendCommand(positionString);
}

void MainWindow::onJogBtnXMinusClicked()
{
    printerStatus.MPosX -= printerStatus.axisSteps;

    QString positionString = "G1 X";
    positionString.append(QString::number(printerStatus.MPosX));
    positionString.append("\r\n");

    emit sendCommand(positionString);
}

void MainWindow::onJogBtnYPlusClicked()
{
    printerStatus.MPosY += printerStatus.axisSteps;

    QString positionString = "G1 Y";
    positionString.append(QString::number(printerStatus.MPosY));
    positionString.append("\r\n");

    emit sendCommand(positionString);
}

void MainWindow::onJogBtnYMinusClicked()
{
    printerStatus.MPosY -= printerStatus.axisSteps;

    QString positionString = "G1 Y";
    positionString.append(QString::number(printerStatus.MPosY));
    positionString.append("\r\n");

    emit sendCommand(positionString);
}

void MainWindow::onJogBtnZPlusClicked()
{
    printerStatus.MPosZ += printerStatus.axisSteps;

    QString positionString = "G1 Z";
    positionString.append(QString::number(printerStatus.MPosZ));
    positionString.append("\r\n");

    emit sendCommand(positionString);
}

void MainWindow::onJogBtnZMinusClicked()
{
    printerStatus.MPosZ -= printerStatus.axisSteps;

    QString positionString = "G1 Z";
    positionString.append(QString::number(printerStatus.MPosZ));
    positionString.append("\r\n");

    emit sendCommand(positionString);
}

void MainWindow::onJogHalfStepClicked()
{
    printerStatus.axisSteps = 0.5;
}

void MainWindow::onJogOneStepClicked()
{
    printerStatus.axisSteps = 1; // default
}

void MainWindow::onJogTwoStepClicked()
{
    printerStatus.axisSteps = 2;
}

void MainWindow::onJogFiveStepClicked()
{
    printerStatus.axisSteps = 5;
}

void MainWindow::on_btnSliceAndPrint_clicked()
{
    fileDialog->setModal(true);
    fileDialog->exec();
}
