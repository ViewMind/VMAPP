#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QIcon>

#include "../../CommonClasses/LogInterface/loginterface.h"
#include "loader.h"
#include "flowcontrol.h"

// The global logging object.
LogInterface globalLog;

// Global Configuration
ConfigurationManager configuration;

int main(int argc, char *argv[])
{
    //QGuiApplication app(argc, argv);
    QApplication app(argc,argv);

    // The icon
    app.setWindowIcon(QIcon(":/images/viewmind.png"));

    // The QML Engine
    QQmlApplicationEngine engine;

    // Laods all language related data
    Loader loader(nullptr,&globalLog,&configuration);
    //FlowControl flowControl(nullptr,&globalLog,&configuration);
    FlowControl flowControl(nullptr,&globalLog,&configuration);
    engine.rootContext()->setContextProperty("loader", &loader);
    engine.rootContext()->setContextProperty("flowControl", &flowControl);

    // Rendering the QML files
    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
