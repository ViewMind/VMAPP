#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "../../../CommonClasses/QMLQImageDisplay/qimagedisplay.h"
#include "control.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    // Registering the QImage Display Which allows to displays images generated in the C++ part of the application in the QML front end.
    qmlRegisterType<QImageDisplay>("com.qml",1,0,"QImageDisplay");

    QGuiApplication app(argc, argv);

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

    return app.exec();
}
