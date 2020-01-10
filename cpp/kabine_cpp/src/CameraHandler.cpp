#include "CameraHandler.h"
#include <QtConcurrent/QtConcurrent>
#include <iostream>

CameraHandler::CameraHandler() : QObject(nullptr), stopPreviewStream(false)
{
}

void CameraHandler::start()
{
    camera.open();
}


void CameraHandler::triggerCapture()
{   
    QtConcurrent::run([this]()
    {
        //stop preview if running
        stopPreviewStream = true;
    
        //acquire access to the camera (wait for preview to stop)
        const std::lock_guard<std::mutex> lock(cameraMutex);
        
        auto image = camera.captureImage();
        emit highResImageCaptured(image);
        emit capturedImage(image);      
        emit imageCaptureDone();
        stopPreviewStream = false;
    });
}

void CameraHandler::triggerPreviewStreaming()
{
    QtConcurrent::run([this]()
    {   
        //acquire access to the camera (wait for capture to complete)
        const std::lock_guard<std::mutex> lock(cameraMutex);
        
        //stream preview images until we are signaled to stop
        while(!stopPreviewStream)
        {
            auto preview = camera.capturePreview();
            emit capturedImage(preview);    
        }
    });    
}

void CameraHandler::stopPreviewStreaming()
{
    //stop preview if running
    stopPreviewStream = true;
    //wait for preview to stop
    const std::lock_guard<std::mutex> lock(cameraMutex);    
}
