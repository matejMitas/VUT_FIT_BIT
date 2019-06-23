#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QPoint>
#include <QDebug>
#include <QQmlComponent>

// vlastni moduly programu
#include "qwrap.h"
#include "Doc.h"

// Jirkuv backend
#include "backend/model/Model.h"
#include "backend/file_manager/Manager.h"

// main
int main(int argc, char *argv[]) {
    // nastaveni pro HDPI
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    // inicializace QT aplikace
    QGuiApplication app(argc, argv);
    // nacteni vykreslovaciho jadra
    QQmlApplicationEngine engine;

    // aplikacni rozhrani pro
    qmlRegisterType<QWrap>("QWrap", 1, 0, "QWrap");

    // nacteni hlavniho layoutoveho souboru
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;


    /*QQmlComponent component(&engine, "qrc:/assets/comp/test.qml");
    QObject *object = component.create();

    QVariant msg = "Hello from C++";
    QMetaObject::invokeMethod(object, "myQmlFunction",
            Q_ARG(QVariant, msg));*/



    //delete object;

    // spusteni aplikace
    return app.exec();
}
