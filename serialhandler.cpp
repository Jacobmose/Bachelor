#include "serialhandler.h"

SerialHandler::SerialHandler(QSerialPort *serialPort, QObject *parent)
    : QObject(parent),
      _serialPort(serialPort)
{
    //connect(serialPort, &QSerialPort::readyRead, this, &SerialHandler::readData);

}

SerialHandler::~SerialHandler()
{

}

void SerialHandler::openSerialPort()
{
    _serialPort->setPortName("COM6");
    _serialPort->setBaudRate(250000);
    _serialPort->setDataBits(QSerialPort::Data8);
    _serialPort->setParity(QSerialPort::NoParity);
    _serialPort->setStopBits(QSerialPort::OneStop);
    _serialPort->setFlowControl(QSerialPort::NoFlowControl);
    _serialPort->open(QIODevice::ReadWrite);
}

void SerialHandler::closeSerialPort()
{
    _serialPort->close();
}

void SerialHandler::writeData(QString &data)
{
    _serialPort->write(data.toLatin1());
}

void SerialHandler::readData()
{
    _readData = _serialPort->readAll();
}
