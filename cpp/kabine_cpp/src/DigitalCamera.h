#pragma once
#include <gphoto2/gphoto2-camera.h>
#include <QPixmap>
#include <memory>

/**
 * @todo write docs
 */
class DigitalCamera
{
public:
    /**
     * Default constructor
     */
    DigitalCamera();

    /**
     * Destructor
     */
    ~DigitalCamera();
    void open();
    
    std::shared_ptr<QPixmap> captureImage();
        
    
private:

    GPContext* pContext;
    Camera* pCamera;
    
};

