#include "processwithid.h"

ProcessWithID::ProcessWithID(qint32 id)
{
    ID = id;
    connect(this,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(on_finished(int,QProcess::ExitStatus)));
}

void ProcessWithID::on_finished(int exit_code, QProcess::ExitStatus es){
    Q_UNUSED(exit_code);
    Q_UNUSED(es);
    emit(finishedWithID(ID));
}
