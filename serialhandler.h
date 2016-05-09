#ifndef SERIALHANDLER_H
#define SERIALHANDLER_H

#include <QtSerialPort/QSerialPort>

class SerialHandler : public QObject
{
    Q_OBJECT

signals:
    void sendData(QString &data);

public:
    SerialHandler(QSerialPort *serialPort, QObject *parent = 0);
    ~SerialHandler();

    void openSerialPort();
    void closeSerialPort();

    void readData();
    void writeData(QString &data);

private slots:


private:
    QSerialPort *_serialPort;
    QString _readData;
};

#endif // SERIALHANDLER_H
