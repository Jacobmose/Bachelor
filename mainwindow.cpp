#include "mainwindow.h"
#include "ui_mainwindow.h"

/**
 * @brief MainWindow constructor
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
    consoleModel = new QStringListModel(this);
    ui->commandListView->setModel(consoleModel);

    // connects signal ReadyRead from QSerialPort to SLOT onSerialReadyRead
    connect(m_serialPort, &QSerialPort::readyRead, this, &MainWindow::onSerialReadyRead);

    connect(fileDialog, SIGNAL(sliceFileClicked(QString)), this, SLOT(onSliceFileClicked(QString)));

    // Jog controls
    connect(jogDialog, SIGNAL(jogXPlusClicked()), this, SLOT(onJogBtnXPlusClicked()));
    connect(jogDialog, SIGNAL(jogYPlusClicked()), this, SLOT(onJogBtnYPlusClicked()));
    connect(jogDialog, SIGNAL(jogZPlusClicked()), this, SLOT(onJogBtnZPlusClicked()));
    connect(jogDialog, SIGNAL(jogEPlusClicked()), this, SLOT(onJogBtnEPlusClicked()));
    connect(jogDialog, SIGNAL(jogEMinusClicked()), this, SLOT(onJogBtnEMinusClicked()));
    connect(jogDialog, SIGNAL(jogXMinusClicked()), this, SLOT(onJogBtnXMinusClicked()));
    connect(jogDialog, SIGNAL(jogYMinusClicked()), this, SLOT(onJogBtnYMinusClicked()));
    connect(jogDialog, SIGNAL(jogZMinusClicked()), this, SLOT(onJogBtnZMinusClicked()));
    connect(jogDialog, SIGNAL(jogHalfStepClicked()), this, SLOT(onJogHalfStepClicked()));
    connect(jogDialog, SIGNAL(jogOneStepClicked()), this, SLOT(onJogOneStepClicked()));
    connect(jogDialog, SIGNAL(jogTwoStepClicked()), this, SLOT(onJogTwoStepClicked()));
    connect(jogDialog, SIGNAL(jogFiveStepClicked()), this, SLOT(onJogFiveStepClicked()));
    connect(jogDialog, SIGNAL(homeAxisClicked()), this, SLOT(onHomeAxisClicked()));

    connect(ui->lvBrowseFigureWidget, SIGNAL(pressed(QModelIndex)), this, SLOT(onListItemPressed(QModelIndex)));

    connect(this, SIGNAL(sendCommand(QString&)), this, SLOT(writeData(QString&)));

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
 * @brief MainWindow destructor
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
 * @brief Displays the selected figure in big format and gets the name
 * @param index
 */
void MainWindow::onListItemPressed(QModelIndex index)
{
    PrintObject *printObject = ui->lvBrowseFigureWidget->printObjects.at(index.row());

    QPixmap image(printObject->m_filePath);
    imageLabel->setPixmap(image);

    selectedPrintFileName = printObject->m_name;
}

/**
 * @brief Opens the serial port with the specified settings.
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
        qDebug() << "SerialPort not open...";
}

/**
 * @brief Adds a message to the console
 * @param message
 */
void MainWindow::addConsoleMessage(QString message)
{
    addConsoleMessage(QStringList() << message);
}

/**
 * @brief Adds message(s) to the console
 * @param messages
 */
void MainWindow::addConsoleMessage(QStringList messages)
{
    consoleMessages.append(messages);
    consoleModel->setStringList(consoleMessages);
    ui->commandListView->scrollToBottom();
}

/**
 * @brief Loads the selected file and stores the g code commands in the command queue.
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
 * @brief Function that get called everytime the serial port reads data.
 *
 *
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

                QString extruderTemperature = QString::number(printerStatus.extruderTemp);
                QString roomTemperature = QString::number(printerStatus.bedTemp);

                addConsoleMessage("Current temperature is: " + extruderTemperature);
                addConsoleMessage("Room temperature is: " + roomTemperature);
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

                    updateRemainingPrintTime(timer);

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

        list.clear();
        list.append("Reply received: " + data);        
    }

    addConsoleMessage(list);
}

/**
 * @brief sets the temperature readings in the command console to check temperatures.
 *
 * For debugging purposes
 *
 * @param list
 *
 */
void MainWindow::setTemperatureReadings(QStringList list)
{
    list.append("Temperatures:");
    list.append("Extruder: " + QString::number(printerStatus.extruderTemp));
    list.append("Room: " + QString::number(printerStatus.bedTemp));
}

/**
 * @brief sets the axis positions in the command console to check the actual positions.
 *
 * For debugging purposes
 *
 * @param list
 *
 */
void MainWindow::setAxisPosition(QStringList list)
{
    list.append("Axis positions:");
    list.append("X: " + QString::number(printerStatus.MPosX));
    list.append("Y: " + QString::number(printerStatus.MPosZ));
    list.append("Z: " + QString::number(printerStatus.MPosY));
}

/**
 * @brief Send the next command in the command queue.
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
    addConsoleMessage("Error: " + m_serialPort->errorString());
}

/**
 * @brief Maps the the command queue index value to the percent of the total commands in command queue.
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
 * @brief Updates the remaining time until the current print is complete.
 * @param timer
 */
void MainWindow::updateRemainingPrintTime(QTime timer)
{
    double timeTaken = timer.elapsed() / 1000;
    double timeLeft = (timeTaken / commandQueue.commandIndex) * commandQueue.commandList.count();

    ui->lTime->setText(QString::number(timeLeft));
}

/**
 * @brief get remaining pre heating time
 * @param time
 * @return
 */
int MainWindow::getRemainingPreHeatTime(QTime time)
{
    int timeRemainingInMSec =  PRE_HEAT_TIME_IN_MSEC-time.elapsed();

    int timeRemainingInMin = (timeRemainingInMSec / (1000*60)) % 60;

    return timeRemainingInMin;
}

/**
 * @brief Updates the progress bar with the current percentage done of the current print.
 */
void MainWindow::updateProgressBar()
{
    progress = mapValueToPercent(commandQueue.commandIndex, commandQueue.commandList.count());

    progressBar->setValue(progress);
}

/**
 * @brief Preheats the chocolate extruder to make sure all chocolate is melted for printing
 */
void MainWindow::on_btnPreHeat_clicked()
{
    QString setTemperatureCode = M104;
    setTemperatureCode.append(" ");
    setTemperatureCode.append(PRE_HEAT_TEMPERATURE);
    setTemperatureCode.append(NEW_LINE);

    totalPreHeatingTimer.start();

    emit sendCommand(setTemperatureCode);

    ui->btnStartPrint->setEnabled(false);

    timerId = startTimer(1000);
}

/**
 * @brief timer event to continually poll Marlin for temperatures
 */
void MainWindow::timerEvent(QTimerEvent *)
{
    QString getTemperatureCode = M105;
    getTemperatureCode.append(NEW_LINE);

    emit sendCommand(getTemperatureCode);

    int timeRemaining = getRemainingPreHeatTime(totalPreHeatingTimer);

    addConsoleMessage("Pre heat time remaining: " + QString::number(timeRemaining));

    if(totalPreHeatingTimer.elapsed() >= PRE_HEAT_TIME_IN_MSEC)
    {
        addConsoleMessage("Pre heat done!");

        QString setTemperetureCode = M104;
        setTemperetureCode.append(" ");
        setTemperetureCode.append(PRINT_TEMPERATURE);
        setTemperetureCode.append(NEW_LINE);

        emit sendCommand(setTemperetureCode);

        killTimer(timerId);
        ui->btnStartPrint->setEnabled(true);
    }
}

/**
 * @brief Sends an emergency stop code "M112"
 */
void MainWindow::on_btnEmergencyStop_clicked()
{
    QString stopCommand = M112;
    stopCommand.append(NEW_LINE);

    ui->btnStartPrint->setEnabled(true);

    emit sendCommand(stopCommand);
}

/**
 * @brief slot to be executed when slice is clicked in fileDialog
 * @param fileName
 */
void MainWindow::onSliceFileClicked(QString fileName)
{
    QString filePathDir = "C:/Users/jacobmosehansen/Pictures/testpic/";
    QString filePath = filePathDir + "defaultPrintImage.png";

    PrintObject *printObject = new PrintObject(fileName, filePath);

    ui->lvBrowseFigureWidget->printObjects.append(printObject);

    QPixmap pixmap(filePath);
    QPixmap scaledPixmap = pixmap.scaled(QSize(100,100));
    QIcon icon;
    icon.addPixmap(scaledPixmap);
    QListWidgetItem *fileItem = new QListWidgetItem(printObject->m_name);
    fileItem->setIcon(icon);

    ui->lvBrowseFigureWidget->addItem(fileItem);

    sliceFile(printObject);
}

/**
 * @brief Slices a file
 * @param printObject
 */
void MainWindow::sliceFile(PrintObject* printObject)
{
    QString fileName = printObject->m_name + ".stl";

    QProcess process;
    QString configPath = "/usr/local/slicerconfig/config.ini";
    QString outputPath = "/usr/local/figures/files/" + fileName;
    QString sliceCommand = "slic3r " + fileName + " --output " + outputPath;

    // load config file
    process.execute("--load " + configPath);
    process.waitForFinished();

    addConsoleMessage(process.readAll());

    // slice file
    process.execute(sliceCommand);
    process.waitForFinished();

    addConsoleMessage(process.readAll());

    process.close();
}

/**
 * @brief Starts a print with the current selected figure
 */
void MainWindow::on_btnStartPrint_clicked()
{
    commandQueue.commandIndex = 0;
    commandQueue.commandList.clear();
    QString addGcodeExtensionToSelectedFileName = selectedPrintFileName + ".gcode";

    loadFile(addGcodeExtensionToSelectedFileName); 

    if(commandQueue.commandList.isEmpty())
    {
        qDebug() << "Command list is empty... file did not load correctly";
        return;
    }

    printerStatus.isPrinting = true;

    ui->btnEmergencyStop->show();
    ui->btnStartPrint->setEnabled(false);

    QString firstCommand = commandQueue.commandList.at(commandQueue.commandIndex);
    firstCommand.append(NEW_LINE);

    timer->start();

    emit sendCommand(firstCommand);
}

/**
 * @brief Shows the jogDialog
 */
void MainWindow::on_btnJog_clicked()
{    
    jogDialog->show();
}

/**
 * @brief Home X, Y Aand Z axis and gets their current position
 */
void MainWindow::onHomeAxisClicked()
{
    QString homeCode = G28;
    homeCode.append(NEW_LINE);

    QString axisPosCode = M114;
    axisPosCode.append(NEW_LINE);

    QString setExtruderPositionCode = G92;
    setExtruderPositionCode.append(" E0");
    setExtruderPositionCode.append(NEW_LINE);

    emit sendCommand(homeCode);
    emit sendCommand(setExtruderPositionCode);
    emit sendCommand(axisPosCode);
}

/**
 * @brief Updates the current axis position after a jog in jogDialog
 * @param axis
 * @param axisPosition
 */
void MainWindow::updateAxisPositions(QString axis, double axisPosition)
{
    QString positionString = G1;
    positionString.append(" ");

    if(axis == "E")
    {
        positionString.append(axis);
        positionString.append(QString::number(axisPosition));
        positionString.append(NEW_LINE);

        emit sendCommand(positionString);
        return;
    }

    positionString.append(axis);
    positionString.append(QString::number(axisPosition));
    positionString.append(NEW_LINE);

    emit sendCommand(positionString);
}

void MainWindow::updateExtruderPositions(double position)
{
    QString positionString = G1;
    positionString.append(" E");
    positionString.append(QString::number(position));
    positionString.append(" F1800");
    positionString.append(NEW_LINE);

    emit sendCommand(positionString);
}

/**
 * @brief Jogs the X axisin the positive direction with the selected axis steps
 */
void MainWindow::onJogBtnXPlusClicked()
{
    printerStatus.MPosX += printerStatus.axisSteps;

    updateAxisPositions("X", printerStatus.MPosX);
}

/**
 * @brief Jogs the X axisin the negative direction with the selected axis steps
 */
void MainWindow::onJogBtnXMinusClicked()
{
    printerStatus.MPosX -= printerStatus.axisSteps;

    updateAxisPositions("X", printerStatus.MPosX);
}

/**
 * @brief Jogs the Y axisin the positive direction with the selected axis steps
 */
void MainWindow::onJogBtnYPlusClicked()
{
    printerStatus.MPosY += printerStatus.axisSteps;

    updateAxisPositions("Y", printerStatus.MPosY);
}

/**
 * @brief Jogs the Y axisin the negative direction with the selected axis steps
 */
void MainWindow::onJogBtnYMinusClicked()
{
    printerStatus.MPosY -= printerStatus.axisSteps;

    updateAxisPositions("Y", printerStatus.MPosY);
}

/**
 * @brief Jogs the Z axisin the positive direction with the selected axis steps
 */
void MainWindow::onJogBtnZPlusClicked()
{
    printerStatus.MPosZ += printerStatus.axisSteps;

    updateAxisPositions("Z", printerStatus.MPosZ);
}

/**
 * @brief Jogs the Z axisin the negative direction with the selected axis steps
 */
void MainWindow::onJogBtnZMinusClicked()
{
    printerStatus.MPosZ -= printerStatus.axisSteps;

    updateAxisPositions("Z", printerStatus.MPosZ);
}

void MainWindow::onJogBtnEPlusClicked()
{
    printerStatus.MPosE += printerStatus.axisSteps * 100;

    updateAxisPositions("E", printerStatus.MPosE);
}

void MainWindow::onJogBtnEMinusClicked()
{
    printerStatus.MPosE -= printerStatus.axisSteps * 100;

    updateAxisPositions("E", printerStatus.MPosE);
}

/**
 * @brief Sets the axis steps
 * @param steps
 */
void MainWindow::setAxisSteps(double steps)
{
    printerStatus.axisSteps = steps;
}

/**
 * @brief Sets axis steps to 0.5
 */
void MainWindow::onJogHalfStepClicked()
{
    setAxisSteps(0.5);
}

/**
 * @brief Sets axis steps to 1
 */
void MainWindow::onJogOneStepClicked()
{
    setAxisSteps(1);
}

/**
 * @brief Sets axis steps to 2
 */
void MainWindow::onJogTwoStepClicked()
{
     setAxisSteps(2);
}

/**
 * @brief Sets axis steps to 5
 */
void MainWindow::onJogFiveStepClicked()
{
     setAxisSteps(5);
}

/**
 * @brief Shows the slice and print dialog
 */
void MainWindow::on_btnSliceAndPrint_clicked()
{
    fileDialog->setPrintObjects(ui->lvBrowseFigureWidget->printObjects);
    fileDialog->initializeFileList();
    fileDialog->setModal(true);
    fileDialog->exec();
}

void MainWindow::on_btnReloadChocolate_clicked()
{

}
