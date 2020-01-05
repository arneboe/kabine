#include "DigitalCamera.h"
#include <iostream>
#include <QByteArray>
#include <QPixmap>
#include <QFile>


static void ctx_error_func (GPContext *context, const char *str, void *data)
{
    std::cout << "Context Error: \n" << str << std::endl;
}

static void ctx_status_func (GPContext *context, const char *str, void *data)
{
    std::cout << "Context Status: \n" << str << std::endl;
}

GPContext* createContext()
{
    GPContext *context;

    /* This is the mandatory part */
    context = gp_context_new();

    /* All the parts below are optional! */
        gp_context_set_error_func (context, ctx_error_func, NULL);
        gp_context_set_status_func (context, ctx_status_func, NULL);

    /* TODO maybe i should use those as well?
    gp_context_set_cancel_func    (p->context, ctx_cancel_func,  p);
        gp_context_set_message_func   (p->context, ctx_message_func, p);
        if (isatty (STDOUT_FILENO))
                gp_context_set_progress_funcs (p->context,
                        ctx_progress_start_func, ctx_progress_update_func,
                        ctx_progress_stop_func, p);
        */
    return context;
}

void DigitalCamera::open()
{
    //source https://github.com/galazzo/gphoto2-cpp/blob/master/gphoto2.cpp
    
    if(pCamera != nullptr)
    {
        throw std::runtime_error("camera already open");
    }
    
    CameraList *list;
    const char *camera_model = NULL;
    const char* camera_port = NULL;

    gp_list_new (&list);
    //gp_list_reset (list);
    int ret = gp_camera_autodetect (list, pContext);
    if (ret < GP_OK)
    { 
        throw std::runtime_error("no camera found");
    }
    else
    {
        int count = gp_list_count(list);
        std::cout << count << " Camera(s) detected" << std::endl;

        for (int i=0; i < count; i++)
        {
            gp_list_get_name (list, i, &camera_model);
            gp_list_get_value (list, i, &camera_port);
            std::cout << camera_model << " - " << camera_port << std::endl;		
            
            std::string cameraModelStr(camera_model);
            if(cameraModelStr.find("PowerShot") != std::string::npos)
            {
                std::cout << "Opening Camera ... " << std::endl;
                
                if(gp_camera_new(&pCamera) != GP_OK)
                {
                    throw std::runtime_error("cannot create camera object");
                }
                if(gp_camera_init(pCamera, pContext) != GP_OK)
                {
                    throw std::runtime_error("cannot init camera object");
                }
                std::cout << "Camera initialized" << std::endl;
                break;
            }
        }
    }
    gp_list_free (list);
}


std::shared_ptr<QPixmap> DigitalCamera::captureImage()
{
    int ret = 0;
    CameraFile *file;
    CameraFilePath camera_file_path;
    CameraStorageInformation* sifs;
    int nrofsifs = -1;

    std::cout << "Capturing" << std::endl;

    if(gp_camera_get_storageinfo(pCamera, &sifs, &nrofsifs, pContext) != GP_OK)
    if (ret != GP_OK) {
        std::cout << "error: " << gp_result_as_string(ret) << std::endl;
        throw std::runtime_error("gp_camera_get_storageinfo FAIL");
    }

    /* NOP: This gets overridden in the library to /capt0000.jpg */
    //strcpy(camera_file_path.folder, "/");
    //strcpy(camera_file_path.name, "foo.jpg");
    //strcpy(camera_file_path.name, sifs->basedir);
    //strcpy(camera_file_path.name, "foo.cr2");
    //strcat(camera_file_path.name, "/foo.cr2");

    ret = gp_camera_capture(pCamera, GP_CAPTURE_IMAGE, &camera_file_path, pContext);
    if (ret != GP_OK)
    {
        std::cout << "error: " << gp_result_as_string(ret) << std::endl;
        throw std::runtime_error("gp_camera_capture FAIL");
    }

    printf("Pathname on the camera: %s/%s", camera_file_path.folder, camera_file_path.name);
    std::cout << std::endl;
    
    ret = gp_file_new(&file);
    if (ret != GP_OK) 
    {
        std::cout << "error: " << gp_result_as_string(ret) << std::endl;
        throw std::runtime_error("gp_file_new FAIL");
    } 


    //ret = gp_camera_file_get(camera, folder, camera_file_path->name, GP_FILE_TYPE_RAW , file, context);
    ret = gp_camera_file_get(pCamera, camera_file_path.folder, camera_file_path.name, GP_FILE_TYPE_NORMAL, file, pContext);
    if (ret != GP_OK) 
    {
        std::cout << "error: " << gp_result_as_string(ret) << std::endl;
        throw std::runtime_error("gp_camera_file_get FAIL");
    }

    const char* imageData = nullptr;
    unsigned long int imageSize = 0;
    ret = gp_file_get_data_and_size (file, &imageData, &imageSize);
    if(ret != GP_OK)
    {
        std::cout << "error: " << gp_result_as_string(ret) << std::endl;
        throw std::runtime_error("gp_file_get_data_and_size FAIL");
    }

    ret = gp_camera_file_delete(pCamera, camera_file_path.folder, camera_file_path.name, pContext);
    if(ret != GP_OK)
    {
        std::cout << "error: " << gp_result_as_string(ret) << std::endl;
        throw std::runtime_error("gp_camera_file_delete FAIL");
    }
   
    std::shared_ptr<QPixmap> pixmap(new QPixmap());
    pixmap->loadFromData((const unsigned char*) imageData, imageSize);
    gp_file_free(file);
    return pixmap;    
}

std::shared_ptr<QPixmap> DigitalCamera::capturePreview()
{
    int ret = 0;
    CameraFile* cameraFile;
    
    ret = gp_file_new(&cameraFile);
    if(ret != GP_OK)
    {
        std::cout << "error: " << gp_result_as_string(ret) << std::endl;
        throw std::runtime_error("gp_file_new FAIL");
    }
    
    ret = gp_camera_capture_preview(pCamera, cameraFile, pContext);
    if(ret != GP_OK)
    {
        std::cout << "error: " << gp_result_as_string(ret) << std::endl;
        throw std::runtime_error("gp_camera_capture_preview FAIL");
    }
    
    const char* imageData = nullptr;
    unsigned long int imageSize = 0;
    ret = gp_file_get_data_and_size (cameraFile, &imageData, &imageSize);
    if(ret != GP_OK)
    {
        std::cout << "error: " << gp_result_as_string(ret) << std::endl;
        throw std::runtime_error("gp_file_get_data_and_size FAIL");
    }
    
    
    std::shared_ptr<QPixmap> pixmap(new QPixmap());
    pixmap->loadFromData((const unsigned char*) imageData, imageSize);
    gp_file_free(cameraFile);
    return pixmap;  
}


DigitalCamera::DigitalCamera()
{
    pContext = createContext();
    pCamera = nullptr;
}

DigitalCamera::~DigitalCamera()
{
    gp_camera_exit (pCamera, pContext);
    gp_camera_free (pCamera);
    gp_context_unref (pContext);

}
