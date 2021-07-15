#include "updater.h"

Updater::Updater()
{
}

void Updater::run(){
   QDir dir(".");
   dir.cdUp();
   QString cmd = "cd " + dir.path() + "&& " + Globals::Paths::UPDATE_SCRIPT;
   system(cmd.toStdString().c_str());
}
