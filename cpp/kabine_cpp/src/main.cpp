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

Q_DECLARE_METATYPE(std::shared_ptr<QPixmap>);

int main(int argc, char *argv[])
{
    qRegisterMetaType<std::shared_ptr<QPixmap>>();
    QGuiApplication app(argc, argv);
    
    QQmlEngine engine;
    ImageProvider imageProvder;
    engine.addImageProvider(QLatin1String("capture_provider"), &imageProvder);
    QQmlContext *objectContext = new QQmlContext(engine.rootContext());
    QQmlComponent component(&engine, QUrl(QStringLiteral("qrc:/qml/main.qml")));
    QObject *object = component.create(objectContext);


    CameraHandler h;

    QObject::connect(&h, &CameraHandler::capturedImage,
                     &imageProvder, &ImageProvider::capturedImage);
    
     h.triggerCapture();

    
    return app.exec();

    
}
