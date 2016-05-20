#ifndef PRINTOBJECT_H
#define PRINTOBJECT_H

#include <QString>

class PrintObject
{
public:
    PrintObject();
    PrintObject(QString name, QString filePath);
    ~PrintObject();

    QString m_name;
    QString m_filePath;

};

#endif // PRINTOBJECT_H
