#ifndef LANGS_H
#define LANGS_H

#include "../../CommonClasses/ConfigurationManager/configurationmanager.h"
#include <QDebug>

/**
 * @brief The Langs class - Will returnt the proper language string for a key.
 * @details - As of creating this class the only language will be english. So it's pointles. This is just future proofing.
 */

class Langs
{
public:
    Langs();

    static bool LoadLanguageFile(const QString &langcode);
    static QString getString(const QString &key);
    static QStringList getStringList(const QString &key);

private:
    static ConfigurationManager language;
};

#endif // LANGS_H
