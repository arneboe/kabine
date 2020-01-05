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
    DigitalCamera();
    ~DigitalCamera();
    
    void open();
    
    std::shared_ptr<QPixmap> captureImage();
    std::shared_ptr<QPixmap> capturePreview();   
    
private:
    
    /**applys our configuration */
    void configure();
    
    int SetConfigValueString (const char *key, const char *val);
    int GetConfigValueString (const char *key, std::string& result);

    GPContext* pContext;
    Camera* pCamera;
    
};

