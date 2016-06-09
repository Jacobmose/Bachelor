#include "printobject.h"

/**
 * @brief PrintObject that hold file name and path for figure to be printed
 * @param name
 * @param filePath
 */
PrintObject::PrintObject(QString name, QString filePath)
{
    m_name = name;
    m_filePath = filePath;
}
