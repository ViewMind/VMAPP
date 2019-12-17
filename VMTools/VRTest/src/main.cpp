#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#ifdef USE_NON_UI_SOLUTION
#include "openvrcontrolobject.h"
#else
#include "openvrcontrolwindow.h"
#endif

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);

#ifdef USE_NON_UI_SOLUTION
    OpenVRControlObject control(nullptr);
#else
    OpenVRControlWindow control;
#endif

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    engine.rootContext()->setContextProperty("control",&control);

    engine.load(url);

    return app.exec();
}
