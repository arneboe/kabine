#pragma once

#include <QObject>
#include "DigitalCamera.h"
#include <memory>

class CameraHandler : public QObject
{
    Q_OBJECT

public:
    CameraHandler();

public slots:
    void triggerCapture();
signals:
    void capturedImage(std::shared_ptr<QPixmap> image);
    
private:
    DigitalCamera camera;
};
