#ifdef QT_QML_DEBUG
#include <QtQuick>
#endif
#include <QGuiApplication>
#include <QQuickView>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlComponent>
#include <QtQml/QQmlEngine>
#include "mynotesdb.h"
#include "appsettings.h"
#include <sailfishapp.h>

int main(int argc, char *argv[])
{
    // SailfishApp::main() will display "qml/template.qml", if you need more
    // control over initialization, you can use:
    //
    //   - SailfishApp::application(int, char *[]) to get the QGuiApplication *
    //   - SailfishApp::createView() to get a new QQuickView * instance
    //   - SailfishApp::pathTo(QString) to get a QUrl to a resource file
    //
    // To display the view, call "show()" (will show fullscreen on device).
    QScopedPointer<QGuiApplication> app(
        SailfishApp::application(argc, argv));
    MyNotesDB myNotesDB;
    AppSettings myAppSettings;

    QScopedPointer<QQuickView> view(SailfishApp::createView());

    view->rootContext()->setContextProperty("mynotesdb", &myNotesDB);
    view->rootContext()->setContextProperty("myappsettings", &myAppSettings);

    view->setSource(SailfishApp::pathTo("qml/harbour-mynotes.qml"));

    view->show();

    return app->exec();
}

