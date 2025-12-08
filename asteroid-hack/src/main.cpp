#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickView>
#include "CommandRunner.h"
#include <asteroidapp.h>

int main(int argc, char *argv[])
{
    // Create AsteroidApp application
    QScopedPointer<QGuiApplication> app(AsteroidApp::application(argc, argv));

    // Create AsteroidApp view
    QScopedPointer<QQuickView> view(AsteroidApp::createView());

    // Create CommandRunner instance with proper parent for memory management
    CommandRunner *commandRunner = new CommandRunner(view.data());

    // Expose to QML
    view->rootContext()->setContextProperty("commandRunner", commandRunner);

    // Load QML file - AsteroidApp uses qrc:/main.qml by default
    view->setSource(QUrl("qrc:/main.qml"));
    view->showFullScreen();

    return app->exec();
}
