#ifndef LOADERFLOWCOMM_H
#define LOADERFLOWCOMM_H

#include "../../../CommonClasses/ConfigurationManager/configurationmanager.h"
#include "../../../CommonClasses/HWRecog/hwrecognizer.h"
#include "localdb.h"

/**
 * @brief The LoaderFlowComm class - Its only purpose is to provide global access to the two main control classes, Loader and Flow Control, to the global configuration and the database.
 */

class LoaderFlowComm
{
public:
    LoaderFlowComm();

    LocalDB* db();
    ConfigurationManager* configuration();
    HWRecognizer* hw();

private:
    LocalDB localDB;
    ConfigurationManager conf;
    HWRecognizer hwRecognizer;
};

#endif // LOADERFLOWCOMM_H
