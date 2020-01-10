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
    
    /** Is emitted for both previews and pictures. */
    void capturedImage(std::shared_ptr<QPixmap> image);
    
    /** Is only emitted for high res pictures, not for previews */
    void highResImageCaptured(std::shared_ptr<QPixmap> image);
    
    /** Signals that a real picture was taken */
    void imageCaptureDone();
    
private:
    DigitalCamera camera;
    std::mutex cameraMutex;
    bool stopPreviewStream; //signal for the stream thread to stop
};
