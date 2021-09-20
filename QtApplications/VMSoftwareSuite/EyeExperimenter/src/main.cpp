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

// Defining externs. When an extern is defined, it is declared in a SINGLE cpp file. The functions below will setup the values according to the actual conf file and they
// will remaing constant and globally accessible for the rest fo the application's lifetime.
// This allows control of the API Region and configured EyeTracker in compile time by manipulating the text file config.cnf.
namespace Globals {
   namespace Share {
      QString EXPERIMENTER_VERSION = "";
   }
   namespace EyeTracker{
      QString NAME = "";
      bool ENABLE_GAZE_FOLLOW = false;
      bool IS_VR = false;
      QString PROCESSING_PARAMETER_KEY = "";
      qreal VRSCALING = 1.0;
   }
}

int main(int argc, char *argv[])
{

    // We need to load the defines to configure the rest of hte application.
    ConfigurationManager defines;
    if (!defines.loadConfiguration(":/configs/config.cnf",Globals::Share::TEXT_CODEC)){
        qDebug() << "Could not load configuration file due to " << defines.getError();
    }
    else{
        Globals::SetUpEyeTrackerNameSpace(defines.getString("et"));
        Globals::SetUpRegion(defines.getString("region"));
        //qDebug() << Globals::EyeTracker::NAME << Globals::REGION;
        Globals::SetExperimenterVersion(); // This basically will show the correct information in the title bar.
        //qDebug() << Globals::Share::EXPERIMENTER_VERSION;
    }

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
