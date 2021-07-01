#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QIcon>
#include "loader.h"

int main(int argc, char *argv[])
{
#if defined(Q_OS_WIN)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);

    // The icon
    app.setWindowIcon(QIcon(":/EyeExperimenter/src/images/viewmind.png"));

    // Laods all language related data
    Loader loader(nullptr);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("loader", &loader);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
