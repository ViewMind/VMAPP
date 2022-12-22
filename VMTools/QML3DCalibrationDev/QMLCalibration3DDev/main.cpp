#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QWindow>

#include "control.h"
#include "../../../CommonClasses/debug.h"
#include "../../../CommonClasses/LogInterface/staticthreadlogger.h"
#include "../../../CommonClasses/LinearLeastSquares/ordinaryleastsquares.h"

// Debug Structure
namespace Debug {
   ConfigurationManager DEBUG_OPTIONS;
}

int main(int argc, char *argv[])
{

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);

    StaticThreadLogger::StartLogger("3DAppDev","logfile.log");

    Control control;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("control", &control);

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    if (QWindow *window = qobject_cast<QWindow*>(engine.rootObjects().at(0))){
        control.setWindowID(window->winId());
    }

    return app.exec();
}
