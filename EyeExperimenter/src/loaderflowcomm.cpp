#include "loaderflowcomm.h"

LoaderFlowComm::LoaderFlowComm(){
}

ConfigurationManager * LoaderFlowComm::configuration() {
    return &conf;
}

HWRecognizer * LoaderFlowComm::hw() {
    return &hwRecognizer;
}

LocalDB * LoaderFlowComm::db() {
    return &localDB;
}


