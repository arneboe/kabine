#pragma once
#include <QObject>
#include <functional>
#include "CameraHandler.h"
#include "ImageProvider.h"

enum State
{
    Startup = 0,
    Streaming = 1,
    Taking = 2,
    Displaying = 3,
    Deleting = 4,
    Printing = 5,
    Invalid = 6,
    NUM_STATES = 7
};

enum class Event
{
    Take_Picture_Pressed,
    Print_Picture_Pressed,
    Delete_Picture_Pressed,
    Display_Time_Expired,
    Picture_Taken,
    Invalid_Event
};


class StateMachine : QObject
{
    Q_OBJECT;
    
public:
    StateMachine(QObject* rootGuiElement, CameraHandler& cameraHandler);


public slots:
    void takePicturePressed();
    void deletePicturePressed();
    void printPicturePressed();
    void timerExpired();
    void imageCaptureDone();
    void highResImageCaptured(std::shared_ptr<QPixmap> pic);
    void start();
    
private:
    /** run one iteration of the sm */
    void iterate();
    void displaying();
    void startup();
    void streaming();
    void taking();
    void printing();
    
    /**returns the number of lines in 'lpstat' output. This is identical to the number of print jobs running */
    int lpstatLineCount();
    
    /** cancel all queued print jobs */
    void cancelAllPrintJobs();
    
    std::vector<std::function<void(void)>> stateHandlers;    
    State currentState;
    State lastState;
    Event currentEvent;
    
    QObject* rootGuiElement;
    CameraHandler& cameraHandler;
    QObject* takingPictureText;
    QObject* image;
    QObject* takeButton;
    QObject* deleteButton;
    QObject* printButton;
    
    /**Original image in original resolution etc. */
    std::shared_ptr<QPixmap> capturedImage;

};


