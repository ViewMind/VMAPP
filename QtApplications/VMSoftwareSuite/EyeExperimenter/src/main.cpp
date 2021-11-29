#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QIcon>
#include <QApplication>
#include <QSslSocket>

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

   QString API_URL;
   QString REGION;

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

// Debug Structure
namespace Debug {
   ConfigurationManager DEBUG_OPTIONS;
}

int main(int argc, char *argv[])
{

    LogInterface logger;

    // We need to load the defines to configure the rest of hte application.
    ConfigurationManager defines;
    if (!defines.loadConfiguration(Globals::Paths::APPSPEC)){
        logger.appendError("Could not load configuration file due to " + defines.getError());
    }
    else{

        if (!defines.containsKeyword(Globals::VMAppSpec::ET)){
            logger.appendError("EyeTracker Specification is missing from app spec");
            return 0;
        }

        if (!defines.containsKeyword(Globals::VMAppSpec::Region)){
            logger.appendError("Application region is missing from app spec");
            return 0;
        }

        bool OK = false;

        OK = Globals::SetUpEyeTrackerNameSpace(defines.getString(Globals::VMAppSpec::ET));
        if (!OK){
            logger.appendError("Could not set up ET Configuration for " + defines.getString(Globals::VMAppSpec::ET));
            return 0;
        }

        OK = Globals::SetUpRegion(defines.getString(Globals::VMAppSpec::Region));
        if (!OK){
            logger.appendError("Could not set up Region Configuration for " + defines.getString(Globals::VMAppSpec::Region));
            return 0;
        }

        // Loading the DEBUG Options. This needs to happen BEFORE Setup Experimenter Version so as to ensure that the options oppear in the title bar.
        QString error = Debug::LoadOptions(Globals::Paths::DEBUG_OPTIONS_FILE);
        if (error != ""){
            logger.appendError("Loading Debug Options File: " + error);
        }

    }

    if (!QSslSocket::supportsSsl()){
        logger.appendError("SSL NOT Supported. Will not be able to connect to the API");
        return 0;
    }

    //QGuiApplication app(argc, argv);
    QApplication app(argc,argv);

    // These are used to avoid a warning.
    app.setOrganizationName("ViewMind");
    app.setOrganizationDomain("ViewMind");

    // Checking that there isn't another instance of an application running.
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
    app.setWindowIcon(QIcon(":/images/icon.png"));

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
