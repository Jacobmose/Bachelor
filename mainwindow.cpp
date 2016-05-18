#include "mainwindow.h"
#include "ui_mainwindow.h"

/**
 * @brief MainWindow::MainWindow
 * @param parent
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    imageLabel(new QLabel),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Initialize the dialogs
    jogDialog = new JogDialog();
    fileDialog = new FileDialog();
    m_serialPort = new QSerialPort(this);

    // connects signal ReadyRead from QSerialPort to SLOT onSerialReadtRead
    connect(m_serialPort, &QSerialPort::readyRead, this, &MainWindow::onSerialReadyRead);

    // Connects signal clicked() on btnPrint from fileDialog to SLOT onStartPrintFromFileClicked
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

    connect(ui->lwBrowseFigures, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onListItemClicked(QListWidgetItem*)));

    addProgressBar();

    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setScaledContents(true);

    QDir figureDir(IMAGE_DIRECTORY);
    figureDir.setNameFilters(QStringList("*.png"));
    figureDir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);

    QFileInfoList files = figureDir.entryInfoList();

    QListWidgetItem *fileItem = 0;

    fileItem = new QListWidgetItem(files[0].fileName());
    fileItem->setIcon(QIcon(IMAGE_DIRECTORY + files[0].fileName()));

    QString defaultImage = fileItem->text();
    QPixmap image(IMAGE_DIRECTORY + defaultImage);
    imageLabel->setPixmap(image);

    ui->btnEmergencyStop->hide();


    ui->showFigureLayout->addWidget(imageLabel);

    openSerialPort();
}

/**
 * @brief MainWindow::~MainWindow
 */
MainWindow::~MainWindow()
{
    delete jogDialog;
    delete fileDialog;
    delete m_serialPort;
    delete ui;
}

/**
 * @brief MainWindow::addProgressBar
 */
void MainWindow::addProgressBar()
{
    progressBar = new QProgressBar();
    progressBar->setMinimum(0);
    progressBar->setMaximum(100);

    ui->vlPrograssBar->addWidget(progressBar);
}

/**
 * @brief MainWindow::onListItemClicked
 * @param item
 */
void MainWindow::onListItemClicked(QListWidgetItem* item)
{
      QPixmap image(IMAGE_DIRECTORY + item->text());
      imageLabel->setPixmap(image);

      selectedImageName = item->text();
}

/**
 * @brief MainWindow::openSerialPort
 *
 */
void MainWindow::openSerialPort()
{
    m_serialPort->setPortName(SERIAL_PORT_NAME);
    m_serialPort->setBaudRate(SERIAL_BAUD_RATE);
    m_serialPort->setDataBits(QSerialPort::Data8);
    m_serialPort->setParity(QSerialPort::NoParity);
    m_serialPort->setStopBits(QSerialPort::OneStop);
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);
    m_serialPort->open(QIODevice::ReadWrite);

    if(!m_serialPort->isOpen()) // TODO For debugging purposes
        qDebug() << "SerialPort not openend...";
}

/**
 * @brief MainWindow::getFigureFileDirectory
 */
void MainWindow::getFigureFileDirectory()
{
    QDir figureDir(IMAGE_DIRECTORY);
    figureDir.setNameFilters(QStringList() << "*.png" << "*.jpg" << "*.jpeg");
    figureDir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);

    QFileInfoList files = figureDir.entryInfoList();

    QListWidgetItem *fileItem = 0;

    foreach(QFileInfo file, files)
    {
        fileItem = new QListWidgetItem(file.fileName());
        fileItem->setIcon(QIcon(IMAGE_DIRECTORY + file.fileName()));
    }

    QString defaultImage = fileItem->text();

    qDebug() << defaultImage;

    QPixmap image(IMAGE_DIRECTORY + defaultImage);
    imageLabel->setPixmap(image);

}

/**
 * @brief MainWindow::loadFile
 * @param fileName
 */
void MainWindow::loadFile(QString fileName)
{
    QString lineData;
    QFile file(GCODE_FILE_DIRECTORY + fileName);

    if(file.open(QIODevice::ReadOnly))
    {
        if(file.isOpen())
        {
            QTextStream textStream(&file);
            while(!textStream.atEnd())
            {
                lineData = textStream.readLine();
                lineData.replace(QRegExp(";.*"), "");
                lineData = lineData.trimmed();

                commandQueue.commandList.append(lineData);
            }
        }
    }

    commandQueue.commandList.removeAll("");
    file.close();
}

/**
 * @brief MainWindow::writeData
 * @param data
 */
void MainWindow::writeData(QString &data)
{
    m_serialPort->write(data.toLatin1());

    qDebug() << "writeData: " <<data;
}

/**
 * @brief MainWindow::onSerialReadyRead
 */
void MainWindow::onSerialReadyRead()
{
    QStringList list;
    QTime timer;

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
                timer.restart();
                timer.start();

                if(data.contains("ok"))
                {
                    commandQueue.commandIndex++;

                    updateProgressBar();

                    updateRemainingTime(timer);

                    if(commandQueue.commandIndex > commandQueue.commandList.count())
                    {
                        printerStatus.isPrintComplete = true;
                        printerStatus.isPrinting = false;

                        ui->btnStartPrint->setEnabled(true);

                        progressBar->setValue(0);
                        ui->lTime->setText("Done!");

                        timer.restart();

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

    QStringListModel *model = new QStringListModel(this);
    model->setStringList(list);
    ui->commandListView->setModel(model);
}

/**
 * @brief MainWindow::setTemperatureReadings
 * @param list
 *
 * For debugging purposes
 */
void MainWindow::setTemperatureReadings(QStringList list)
{
    list.append("Temperatures:");
    list.append("Extruder: " + QString::number(printerStatus.extruderTemp));
    list.append("Room: " + QString::number(printerStatus.bedTemp));
}

/**
 * @brief MainWindow::setAxisPosition
 * @param list
 *
 * For debugging purposes
 */
void MainWindow::setAxisPosition(QStringList list)
{
    list.append("Axis positions:");
    list.append("X: " + QString::number(printerStatus.MPosX));
    list.append("Y: " + QString::number(printerStatus.MPosZ));
    list.append("Z: " + QString::number(printerStatus.MPosY));
}

/**
 * @brief MainWindow::sendNextCommand
 * @param commandIndex
 */
void MainWindow::sendNextCommand(int commandIndex)
{
    QString data = commandQueue.commandList.at(commandIndex -1);

    data.append(NEW_LINE);

    qDebug() << "sendNextCommand data: " << data;

    emit sendCommand(data);
}

/**
 * @brief MainWindow::onSerialError
 *
 */
void MainWindow::onSerialError()
{
    qDebug() << m_serialPort->errorString();
}

/**
 * @brief MainWindow::mapValueToPercent
 * @param value
 * @param max
 * @return
 */
int MainWindow::mapValueToPercent(int value, int max)
{
    int oldRange = 0 - max;
    int percentRange = 0 - 99;
    int newValue = (((value - 0) * percentRange) / oldRange) + 0;

    return newValue;
}

/**
 * @brief MainWindow::updateRemainingTime
 * @param timer
 */
void MainWindow::updateRemainingTime(QTime timer)
{
    double timeTaken = timer.elapsed();
    double timeLeft = timeTaken * (1 / progress - 1);

    ui->lTime->setText(QString::number(timeLeft));
}

/**
 * @brief MainWindow::updateProgressBar
 */
void MainWindow::updateProgressBar()
{
    progress = mapValueToPercent(commandQueue.commandIndex, commandQueue.commandList.count());

    progressBar->setValue(progress);
}

/**
 * @brief MainWindow::on_btnHome_clicked
 */
void MainWindow::on_btnHome_clicked()
{
    QString homeCode = G28;
    homeCode.append(NEW_LINE);

    QString axisPosCode = M114;
    axisPosCode.append(NEW_LINE);

    QString tempCode = M105;
    tempCode.append(NEW_LINE);

    emit sendCommand(homeCode);
    emit sendCommand(axisPosCode);
    emit sendCommand(tempCode);
}

/**
 * @brief MainWindow::on_btnEmergencyStop_clicked
 */
void MainWindow::on_btnEmergencyStop_clicked()
{
    QString stopCommand = M112;
    stopCommand.append(NEW_LINE);

    ui->btnStartPrint->setEnabled(true);

    emit sendCommand(stopCommand);
}

/**
 * @brief MainWindow::onStartPrintFromFileClicked
 * @param fileName
 */
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

    firstCommand.append(NEW_LINE);

    qDebug() << "Print started with first command: " << firstCommand;

    emit sendCommand(firstCommand);
}

/**
 * @brief MainWindow::on_btnStartPrint_clicked
 */
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

    firstCommand.append(NEW_LINE);

    qDebug() << "Print started with first command: " << firstCommand;

    timer->start();

    emit sendCommand(firstCommand);
}

/**
 * @brief MainWindow::on_btnJog_clicked
 */
void MainWindow::on_btnJog_clicked()
{
    jogDialog->setModal(true);
    jogDialog->exec();
}

/**
 * @brief MainWindow::updateAxisPositions
 * @param axis
 * @param axisPosition
 */
void MainWindow::updateAxisPositions(QString axis, double axisPosition)
{
    QString positionString = G1;
    positionString.append(" ");
    positionString.append(axis);
    positionString.append(QString::number(axisPosition));
    positionString.append(NEW_LINE);

    emit sendCommand(positionString);
}

/**
 * @brief MainWindow::onJogBtnXPlusClicked
 */
void MainWindow::onJogBtnXPlusClicked()
{
    printerStatus.MPosX += printerStatus.axisSteps;

    updateAxisPositions("X", printerStatus.MPosX);
}

/**
 * @brief MainWindow::onJogBtnXMinusClicked
 */
void MainWindow::onJogBtnXMinusClicked()
{
    printerStatus.MPosX -= printerStatus.axisSteps;

    updateAxisPositions("X", printerStatus.MPosX);
}

void MainWindow::onJogBtnYPlusClicked()
{
    printerStatus.MPosY += printerStatus.axisSteps;

    updateAxisPositions("Y", printerStatus.MPosY);
}

/**
 * @brief MainWindow::onJogBtnYMinusClicked
 */
void MainWindow::onJogBtnYMinusClicked()
{
    printerStatus.MPosY -= printerStatus.axisSteps;

    updateAxisPositions("Y", printerStatus.MPosY);
}

/**
 * @brief MainWindow::onJogBtnZPlusClicked
 */
void MainWindow::onJogBtnZPlusClicked()
{
    printerStatus.MPosZ += printerStatus.axisSteps;

    updateAxisPositions("Z", printerStatus.MPosZ);
}

/**
 * @brief MainWindow::onJogBtnZMinusClicked
 */
void MainWindow::onJogBtnZMinusClicked()
{
    printerStatus.MPosZ -= printerStatus.axisSteps;

    updateAxisPositions("Z", printerStatus.MPosZ);
}

/**
 * @brief MainWindow::setAxisSteps
 * @param steps
 */
void MainWindow::setAxisSteps(double steps)
{
    printerStatus.axisSteps = steps;
}

/**
 * @brief MainWindow::onJogHalfStepClicked
 */
void MainWindow::onJogHalfStepClicked()
{
    setAxisSteps(0.5);
}

/**
 * @brief MainWindow::onJogOneStepClicked
 */
void MainWindow::onJogOneStepClicked()
{
    setAxisSteps(1);
}

/**
 * @brief MainWindow::onJogTwoStepClicked
 */
void MainWindow::onJogTwoStepClicked()
{
     setAxisSteps(2);
}

/**
 * @brief MainWindow::onJogFiveStepClicked
 */
void MainWindow::onJogFiveStepClicked()
{
     setAxisSteps(5);
}

/**
 * @brief MainWindow::on_btnSliceAndPrint_clicked
 */
void MainWindow::on_btnSliceAndPrint_clicked()
{
    fileDialog->setModal(true);
    fileDialog->exec();
}
