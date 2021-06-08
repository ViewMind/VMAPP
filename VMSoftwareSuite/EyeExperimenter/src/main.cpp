#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QIcon>
#include <QApplication>

#include "../../../CommonClasses/LogInterface/loginterface.h"
#include "../../../CommonClasses/QMLQImageDisplay/qimagedisplay.h"
#include "loader.h"
#include "flowcontrol.h"

// Global Configuration
static ConfigurationManager configuration;
static CountryStruct countries;

int main(int argc, char *argv[])
{

    qmlRegisterType<QImageDisplay>("com.qml",1,0,"QImageDisplay");

    //QGuiApplication app(argc, argv);
    QApplication app(argc,argv);

    // These are used to avoid a warning.
    app.setOrganizationName("ViewMind");
    app.setOrganizationDomain("ViewMind");

    // Checking that there isn't another instance of an application running.
    LogInterface logger;
    QSystemSemaphore semaphore(Globals::Share::SEMAPHORE_NAME, 1);  // create semaphore
    semaphore.acquire();                                            // Raise the semaphore, barring other instances to work with shared memory
    QSharedMemory sharedMemory(Globals::Share::SHAREDMEMORY_NAME);  // Create a copy of the shared memory
    bool isRunning;                                                 // variable to test the already running application
    if (sharedMemory.attach()){                                     // We are trying to attach a copy of the shared memory to an existing segment
        isRunning = true;                                           // If successful, it determines that there is already a running instance
    }else{
        sharedMemory.create(1);                                     // Otherwise allocate 1 byte of memory
        isRunning = false;                                          // And determines that another instance is not running
    }
    semaphore.release();


    Loader loader(nullptr,&configuration,&countries);
    if (isRunning){
        logger.appendError("Another instance of the application was detected. Exiting");
        return 0;
    }

    // The icon
    app.setWindowIcon(QIcon(":/images/viewmind.png"));

    // The QML Engine
    QQmlApplicationEngine engine;

    // Laods all language related data
    FlowControl flowControl(nullptr,&configuration);


    engine.rootContext()->setContextProperty("loader", &loader);
    engine.rootContext()->setContextProperty("flowControl", &flowControl);

    // Rendering the QML files
    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
