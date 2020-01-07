#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlComponent>
#include <memory>
#include <iostream>
#include "QmlSignalRouter.h"
#include "StateMachine.h"
#include "ImageProvider.h"
#include "CameraHandler.h"

Q_DECLARE_METATYPE(std::shared_ptr<QPixmap>);

int main(int argc, char *argv[])
{
    qRegisterMetaType<std::shared_ptr<QPixmap>>();
    QGuiApplication app(argc, argv);
    
    QQmlEngine engine;
    ImageProvider* imageProvder = new ImageProvider();
    engine.addImageProvider(QLatin1String("capture_provider"), imageProvder);
    QQmlContext *objectContext = new QQmlContext(engine.rootContext());
    QQmlComponent component(&engine, QUrl(QStringLiteral("qrc:/qml/main.qml")));
    QObject *object = component.create(objectContext);
    
    std::cout << component.errorString().toStdString() << std::endl;
    
    CameraHandler cameraHandler;
    //forward image to display
    //this will constantly display the current image (either preview or picture)
    QObject::connect(&cameraHandler, &CameraHandler::capturedImage,
                     imageProvder, &ImageProvider::capturedImage);
    
    StateMachine sm(object, cameraHandler);
    sm.start();
    
    app.exec();
    
    //if streaming is running while we shutdown we need to stop it
    //otherwise the camera will crash
    cameraHandler.stopPreviewStreaming();

    return 0;
    
}
