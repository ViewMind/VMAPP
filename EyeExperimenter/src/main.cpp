#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "loader.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // The QML Engine
    QQmlApplicationEngine engine;

    // Laods all language related data
    Loader loader;
    engine.rootContext()->setContextProperty("loader",&loader);

    // Rendering the QML files
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
