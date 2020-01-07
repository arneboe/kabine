#pragma once

#include <QObject>
#include "DigitalCamera.h"
#include <memory>
#include <mutex>

class CameraHandler : public QObject
{
    Q_OBJECT

public:
    CameraHandler();

public slots:
    void triggerCapture();
    void triggerPreviewStreaming();
    void stopPreviewStreaming();
    void start();
signals:
    void capturedImage(std::shared_ptr<QPixmap> image);
    
private:
    DigitalCamera camera;
    std::mutex cameraMutex;
    bool stopPreviewStream; //signal for the stream thread to stop
};
