#include "gcodehandler.h"

#include <QDebug>

GCodeHandler::GCodeHandler()
{

}

GCodeHandler::~GCodeHandler()
{

}

QStringList GCodeHandler::getGCodesFromFile(QString fileName)
{
    QString name = fileName;
    QFile file("C:/Users/jacobmosehansen/Desktop/Test/" + name);

    QStringList fileData;
    QString lineData;

    if(file.open(QIODevice::ReadOnly))
    {
        QTextStream i(&file);
        while(!i.atEnd())
        {
            lineData = i.readLine();
            fileData.append(lineData);
        }

        file.close();
    }

    return fileData;
}

QStringList GCodeHandler::getFigureFileDirectory()
{
    QDir figureDir("C:/Users/jacobmosehansen/Desktop/Test");
    figureDir.setNameFilters(QStringList("*.gcode"));
    figureDir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);

    QStringList figureList = figureDir.entryList();

    for(int i = 0; i<figureList.count(); i++)
    {
        figureList[i];
    }

    return figureList;
}

QString GCodeHandler::removeComments(QString data)
{    
    data.replace(QRegExp(";.*"), "");

    return data.trimmed();
}

QString GCodeHandler::removeWhiteSpace(QString data)
{
    return data.replace(QRegExp("\\s"), "");
}
