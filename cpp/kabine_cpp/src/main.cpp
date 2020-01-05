#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <memory>
#include "CameraHandler.h"
#include "ImageProvider.h"
#include <QQuickWindow>
#include <QQuickItem>
#include <QQmlContext>
#include <QQuickImageProvider>
#include <iostream>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    
    QQmlEngine engine;
    engine.addImageProvider(QLatin1String("capture_provider"), new ImageProvider);
    QQmlContext *objectContext = new QQmlContext(engine.rootContext());
    QQmlComponent component(&engine, QUrl(QStringLiteral("qrc:/qml/main.qml")));
    QObject *object = component.create(objectContext);
//     QQuickWindow *item = qobject_cast<QQuickWindow*>(object);
//     QQuickItem* image = item->findChild<QQuickItem*>("image_viewer");
//     
//     QMetaObject::invokeMethod(image, "sourceChanged", Q_ARG(QUrl,  QUrl(QStringLiteral("image://capture_provider/image5"))));

    
    
//     view.show();
    
//     QQmlApplicationEngine engine;

//     engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));
   


    
    
//     QObject *qmlObject = rootObject->findChild<QObject*>("main_window");
//     QObject* image = qmlObject->findChild<QObject*>("image_viewer");
//     image->setProperty("cache", false);
    



//     CameraHandler h;
//     
//     QObject::connect(&h, &CameraHandler::capturedImage, 
//     [&engine](std::shared_ptr<QPixmap> image)
//     {   
//     
//     QString returnedValue;
// QString msg = "Hello from C++";
//     
//     std::cout << "engine.rootObjects(): " << engine.rootObjects().size() << std::endl;
// 
//     QObject *rootObject = engine.rootObjects().first();
//     QMetaObject::invokeMethod(rootObject, "myQmlFunction",
//         Q_RETURN_ARG(QString, returnedValue),
//         Q_ARG(QString, msg));
//     });
//     
//      h.triggerCapture();

    
    return app.exec();

    
}
