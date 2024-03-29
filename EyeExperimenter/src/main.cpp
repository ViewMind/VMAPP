#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QIcon>
#include <QApplication>
#include <QSslSocket>

#include "../../CommonClasses/LogInterface/staticthreadlogger.h"
#include "loader.h"
#include "flowcontrol.h"

// Global Configuration
static ConfigurationManager configuration;

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
      QString PROCESSING_PARAMETER_KEY = "";
   }

}

// Debug Structure
namespace Debug {
   ConfigurationManager DEBUG_OPTIONS;
}

int main(int argc, char *argv[])
{

    // The first thing we do is start up the logger.
    StaticThreadLogger::StartLogger("EyeExplorer","logfile.log");

    // We need to load the defines to configure the rest of hte application.
    ConfigurationManager defines;
    if (!defines.loadConfiguration(Globals::Paths::APPSPEC)){
        StaticThreadLogger::error("main","Could not load configuration file due to " + defines.getError());
    }
    else{

        if (!defines.containsKeyword(Globals::VMAppSpec::ET)){
             StaticThreadLogger::error("main","EyeTracker Specification is missing from app spec");
            return 0;
        }

        if (!defines.containsKeyword(Globals::VMAppSpec::Region)){
            // In this case the region now defaults to Global
            defines.addKeyValuePair(Globals::VMAppSpec::Region,"GLOBAL");
            StaticThreadLogger::warning("main","No region defined. Defaulting to global");
            return 0;
        }

        bool OK = false;

        OK = Globals::SetUpEyeTrackerNameSpace(defines.getString(Globals::VMAppSpec::ET));
        if (!OK){
             StaticThreadLogger::error("main","Could not set up ET Configuration for " + defines.getString(Globals::VMAppSpec::ET));
            return 0;
        }

        OK = Globals::SetUpRegion(defines.getString(Globals::VMAppSpec::Region));
        if (!OK){
             StaticThreadLogger::error("main","Could not set up Region Configuration for " + defines.getString(Globals::VMAppSpec::Region));
            return 0;
        }

        // Loading the DEBUG Options. This needs to happen BEFORE Setup Experimenter Version so as to ensure that the options oppear in the title bar.
        QString error = Debug::LoadOptions(Globals::Paths::DEBUG_OPTIONS_FILE);
        if (error != ""){
             StaticThreadLogger::error("main","Loading Debug Options File: " + error);
        }

    }

//    qDebug() << "Active SSL BackEnd: " << QSslSocket::activeBackend();
//    qDebug() << "Available SSL BackEnds " << QSslSocket::availableBackends();
//    qDebug() << "SSL Lib Build version Number" <<  QSslSocket::sslLibraryBuildVersionNumber();
//    qDebug() << "SSL Lib Build version string" <<  QSslSocket::sslLibraryBuildVersionString();
//    qDebug() << "SSL Library Version Number" <<  QSslSocket::sslLibraryVersionNumber();
//    qDebug() << "SSL Library Version String" <<  QSslSocket::sslLibraryVersionString();

    if (!QSslSocket::supportsSsl()){
         StaticThreadLogger::error("main","SSL NOT Supported. Will not be able to connect to the API. Was compiled against version: "
                                   + QSslSocket::sslLibraryBuildVersionString()
                                   + QString::number(QSslSocket::sslLibraryBuildVersionNumber())
                                   + QString::number(QSslSocket::sslLibraryVersionNumber())
                                   + QSslSocket::sslLibraryVersionString() );
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


    Loader loader(nullptr,&configuration);
    if (isRunning){
        StaticThreadLogger::error("main","Another instance of the application was detected. Exiting");
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

    // We need to set the identification window which starts the server render process.
    if (QWindow *window = qobject_cast<QWindow*>(engine.rootObjects().at(0))){
        flowControl.startRenderServerAndSetWindowID(window->winId());
        //control.runUnityRenderServer();
    }

    return app.exec();
}
