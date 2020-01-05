#include "CameraHandler.h"
#include <QtConcurrent/QtConcurrent>

CameraHandler::CameraHandler() : QObject(nullptr)
{
    camera.open();
}

void CameraHandler::triggerCapture()
{
    QtConcurrent::run([this]()
    {
        auto image = camera.captureImage();
        emit capturedImage(image);        
    });
}
