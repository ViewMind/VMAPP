#include "maingui.h"
#include <QApplication>

#ifdef SHOW_CONSOLE
#include <windows.h>
#include <stdio.h>
#endif

int main(int argc, char *argv[])
{

#ifdef SHOW_CONSOLE
    // detach from the current console window
    // if launched from a console window, that will still run waiting for the new console (below) to close
    // it is useful to detach from Qt Creator's <Application output> panel
    FreeConsole();

    // create a separate new console window
    AllocConsole();

    // attach the new console to this application's process
    AttachConsole(GetCurrentProcessId());

    // reopen the std I/O streams to redirect I/O to the new console
    freopen("CON", "w", stdout);
    freopen("CON", "w", stderr);
    freopen("CON", "r", stdin);
#endif

    QApplication a(argc, argv);
    MainGUI w;
    w.show();

    return a.exec();
}
