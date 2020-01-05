#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <memory>
#include "DigitalCamera.h"

#include <iostream>


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    DigitalCamera c;
    c.open();
    
    const char* image = nullptr;
    size_t imageSize = 0;
    
    c.captureImage();

    
    return app.exec();

    
}
