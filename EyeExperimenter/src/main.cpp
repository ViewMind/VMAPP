#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QIcon>

#include "../../CommonClasses/LogInterface/loginterface.h"
#include "loader.h"
#include "flowcontrol.h"

// Global Configuration
ConfigurationManager configuration;
CountryStruct countries;

int main(int argc, char *argv[])
{

    //QGuiApplication app(argc, argv);
    QApplication app(argc,argv);

    // Checking that there isn't another instance of an application running.
    LogInterface logger;
    QSystemSemaphore semaphore(SEMAPHORE_NAME, 1);  // create semaphore
    semaphore.acquire();                            // Raise the semaphore, barring other instances to work with shared memory
    QSharedMemory sharedMemory(SHAREDMEMORY_NAME);  // Create a copy of the shared memory
    bool isRunning;                                 // variable to test the already running application
    if (sharedMemory.attach()){                     // We are trying to attach a copy of the shared memory to an existing segment
        isRunning = true;                           // If successful, it determines that there is already a running instance
    }else{
        sharedMemory.create(1);                     // Otherwise allocate 1 byte of memory
        isRunning = false;                          // And determines that another instance is not running
    }
    semaphore.release();

    Loader loader(nullptr,&configuration,&countries);
    if (isRunning){
        logger.appendError("Another instance of the application was detected. Exiting");
        loader.clearChangeLogFile();
        return 0;
    }

    // The icon
    app.setWindowIcon(QIcon(":/images/viewmind.png"));

    // The QML Engine
    QQmlApplicationEngine engine;

    // Laods all language related data   
    FlowControl flowControl(nullptr,&configuration);

    // Doing the connections for communication between the classes
    QObject::connect(&loader,SIGNAL(fileSetReady(QStringList)),&flowControl,SLOT(onFileSetEmitted(QStringList)));
    QObject::connect(&flowControl,SIGNAL(requestFileSet(QStringList)),&loader,SLOT(onFileSetRequested(QStringList)));

    engine.rootContext()->setContextProperty("loader", &loader);
    engine.rootContext()->setContextProperty("flowControl", &flowControl);

    // Rendering the QML files
    //qWarning() << "Loading";
    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
