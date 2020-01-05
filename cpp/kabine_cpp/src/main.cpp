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
#include "QmlSignalRouter.h"

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
    
    std::cout << component.errorString().toStdString() << std::endl;
    
    std::cout << "import paths\n";
    const QStringList importPaths = engine.importPathList();
    for(const QString& p : importPaths)
    {
        std::cout << p.toStdString() << std::endl;
    }
    
    
    QObject *object = component.create(objectContext);
    QQuickItem* image = object->findChild<QQuickItem*>("image_viewer");
        
    QmlSignalRouter signalRouter;
    

    
    
//     CameraHandler h;
//     QObject::connect(&h, &CameraHandler::capturedImage,
//                      imageProvder, &ImageProvider::capturedImage);
//     
//     
//     QObject::connect(image, SIGNAL(imageClicked()), &signalRouter, SLOT(imageClicked()));
//     QObject::connect(&signalRouter, &QmlSignalRouter::imageClickedSignal, 
//                      [&h]()
//                      {
//                          h.triggerCapture();
//                     });
//     
//      h.triggerPreviewStreaming();

    
    app.exec();
//     h.stopPreviewStreaming();

    return 0;
    
}
