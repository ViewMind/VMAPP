#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QIcon>
#include <QApplication>
#include <QSslSocket>

#include "../../CommonClasses/LogInterface/staticthreadlogger.h"
#include "loader.h"
#include "flowcontrol.h"
#include "vmrunningloader.h"

// Global Configuration
static ConfigurationManager configuration;

// Defining externs. When an extern is defined, it is declared in a SINGLE cpp file. The functions below will setup the values according to the actual conf file and they
// will remaing constant and globally accessible for the rest fo the application's lifetime.
// This allows control of the API Region and configured EyeTracker in compile time by manipulating the text file config.cnf.
namespace Globals {

   QString API_URL;
   QString REGION;

}

// Debug Structure
namespace Debug {
   ConfigurationManager DEBUG_OPTIONS;
}

int main(int argc, char *argv[])
{

    // The first thing we do is start up the logger.
    StaticThreadLogger::StartLogger("EyeExplorer","logfile.log");
    StaticThreadLogger::log("main","Started ViewMind Atlas Version " + Globals::Share::EXPERIMENTER_VERSION_NUMBER);

    // We need to load the defines to configure the rest of hte application.
    ConfigurationManager defines;
    defines.addKeyValuePair(Globals::VMAppSpec::Region,"GLOBAL");
    if (QFile::exists(Globals::Paths::APPSPEC)){
        // If APP Spec Exists we try to load it.
        if (!defines.loadConfiguration(Globals::Paths::APPSPEC)){
            StaticThreadLogger::warning("main","Could not load existing APP SPEC  file due to " + defines.getError() + ". Will pretend the file does not exist");
        }
        else {
            if (defines.containsKeyword(Globals::VMAppSpec::ET)){
                StaticThreadLogger::warning("main","EyeTracker Specification contains app spec. It is ignored as it is assumed to be provided by the RRS");
            }

            if (!defines.containsKeyword(Globals::VMAppSpec::Region)){
                // In this case the region now defaults to Global
                defines.addKeyValuePair(Globals::VMAppSpec::Region,"GLOBAL");
                StaticThreadLogger::warning("main","No region defined. Defaulting to global");
                return 0;
            }
        }
    }

    // At this point we know the region, so we set it up.
    bool OK = false;
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
    }
    else{
        sharedMemory.create(1);                                     // Otherwise allocate 1 byte of memory
        isRunning = false;                                          // And determines that another instance is not running
    }
    semaphore.release();

    if (isRunning){
        app.setWindowIcon(QIcon(":/images/info_blue.png"));
        StaticThreadLogger::error("main","Another instance of the application was detected. Exiting");
        QQmlApplicationEngine engine2;
        VMRunningLoader vmrunningloader;
        engine2.rootContext()->setContextProperty("loader",&vmrunningloader);
        engine2.load(QUrl(QStringLiteral("qrc:/qml/VMAlreadyRunningDialog.qml")));
        if (engine2.rootObjects().isEmpty()){
            StaticThreadLogger::error("main","Failed to open VM Already Running Window");
        }
        StaticThreadLogger::kill();
        return app.exec();
    }

    // The icon
    app.setWindowIcon(QIcon(":/images/icon.png"));

    Loader loader(nullptr,&configuration);

    // The QML Engine
    QQmlApplicationEngine engine;

    // Laods all language related data
    FlowControl flowControl(nullptr,&configuration);

    // Now that the flow control and loader are created we need to connect them.
    QObject::connect(&flowControl,&FlowControl::notifyLoader,&loader,&Loader::onNotificationFromFlowControl);

    engine.rootContext()->setContextProperty("loader", &loader);
    engine.rootContext()->setContextProperty("flowControl", &flowControl);

    // Rendering the QML files
    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    // We need to set the identification window which starts the server render process.
    if (QWindow *window = qobject_cast<QWindow*>(engine.rootObjects().at(0))){
        flowControl.startRenderServerAndSetWindowID(window->winId());
    }

    return app.exec();
}
