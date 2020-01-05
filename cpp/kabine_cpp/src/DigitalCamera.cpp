#include "DigitalCamera.h"
#include <iostream>
#include <QByteArray>
#include <QPixmap>
#include <QFile>
#include "ConfigValues.h"


static void ctx_error_func (GPContext *context, const char *str, void *data)
{
    std::cout << "Context Error: \n" << str << std::endl;
}

static void ctx_status_func (GPContext *context, const char *str, void *data)
{
    std::cout << "Context Status: \n" << str << std::endl;
}

/*
 * This function looks up a label or key entry of
 * a configuration widget.
 * The functions descend recursively, so you can just
 * specify the last component.
 */
int _lookup_widget(CameraWidget*widget, const char *key, CameraWidget **child)
{
    //NOTE from https://github.com/galazzo/gphoto2-cpp/blob/master/gphoto2.cpp
    int ret;
    ret = gp_widget_get_child_by_name (widget, key, child);
    if (ret < GP_OK)
        ret = gp_widget_get_child_by_label (widget, key, child);
    return ret;
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

/* Sets a string configuration value.
 * This can set for:
 *  - A Text widget
 *  - The current selection of a Radio Button choice
 *  - The current selection of a Menu choice
 *
 * Sample (for Canons eg):
 *   get_config_value_string (camera, "owner", &ownerstr, context);
 */
int DigitalCamera::SetConfigValueString ( const char* key, const char* val )
{

    CameraWidget		*widget = NULL, *child = NULL;
    CameraWidgetType	type;
    int			ret;

    ret = gp_camera_get_config (pCamera, &widget, pContext);
    if (ret < GP_OK) {
        fprintf (stderr, "camera_get_config failed: %s\n", gp_result_as_string(ret));
        return ret;
    }

    ret = _lookup_widget (widget, key, &child);
    if (ret < GP_OK) {
        fprintf (stderr, "lookup widget failed: %s\n", gp_result_as_string(ret));
        goto out;
    }

    /* This type check is optional, if you know what type the label
     * has already. If you are not sure, better check. */
    ret = gp_widget_get_type (child, &type);
    if (ret < GP_OK) {
        std::cout <<  "widget get type failed: " <<  gp_result_as_string(ret) << std::endl;
        goto out;
    }

    switch (type)
    {
    case GP_WIDGET_MENU:
    case GP_WIDGET_RADIO:
    case GP_WIDGET_TEXT:
    {
        /* This is the actual set call. Note that we keep
            * ownership of the string and have to free it if necessary.
            */
        ret = gp_widget_set_value (child, val);
        if (ret < GP_OK) {
                fprintf (stderr, "could not set widget value: %s\n", gp_result_as_string(ret));
                goto out;
        }
        break;
    }
    case GP_WIDGET_TOGGLE: // int
    case GP_WIDGET_RANGE:
    {
        int t = atoi((const char*) val);
        ret = gp_widget_set_value (child, &t);
        if (ret < GP_OK) {
                fprintf (stderr, "could not set widget value: %s\n", gp_result_as_string(ret));
                goto out;
        } else {
                fprintf (stdout, "set widget value for key '%s' to %d\n", key, t);
        }

        break;
    }
    default: 
    {
        fprintf (stderr, "widget has bad type %d\n", type);
        ret = GP_ERROR_BAD_PARAMETERS;
        goto out;
    }
            
    }

    ret = gp_camera_set_single_config (pCamera, key, child, pContext);
    if (ret != GP_OK)
    {
        // This stores it on the camera again
        ret = gp_camera_set_config (pCamera, widget, pContext);
        if (ret < GP_OK) {
                fprintf (stderr, "camera_set_config failed: %s\n", gp_result_as_string(ret));
                return ret;
        }
    }

out:
    gp_widget_free (widget);
    return ret;
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
    
    configure();
}


void DigitalCamera::configure()
{
    //take initial picture to switch camera to capture mode.
    //FIXME there is proably a better way to do that
    captureImage();
    
    //note in av mode shutter speed is ignored!
    SetConfigValueString("shootingmode", SHOOTING_MODE_AV); //needs to be done first, otherwise some settings are read only
    SetConfigValueString("zoom", "0"); //FIXME does not do anything?
    SetConfigValueString("iso", ISO_100);
    SetConfigValueString("d013", WB_AUTO);
    SetConfigValueString("aperture", AP_AUTO);
    SetConfigValueString("shutterspeed", SS_auto);
    SetConfigValueString("afdistance", "Zone Focus (Close-up)");
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
