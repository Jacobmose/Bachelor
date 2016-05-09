#ifndef GCODEHANDLER_H
#define GCODEHANDLER_H

#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QDir>

class GCodeHandler
{
public:
    GCodeHandler();
    ~GCodeHandler();

    QStringList getGCodesFromFile(QString fileName);
    QStringList getFigureFileDirectory();

    QString removeComments(QString data);

    QString removeWhiteSpace(QString data);

    QString figureFileName;
};

#endif // GCODEHANDLER_H
