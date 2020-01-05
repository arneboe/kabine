#pragma once
#include <gphoto2/gphoto2-camera.h>


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
    
    void captureImage();
    
private:

    GPContext* pContext;
    Camera* pCamera;
    
};

