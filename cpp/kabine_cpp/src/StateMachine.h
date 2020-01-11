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
    Error = 7,
    NUM_STATES = 8
};

enum class Event
{
    Take_Picture_Pressed,
    Print_Picture_Pressed,
    Delete_Picture_Pressed,
    Display_Time_Expired,
    Picture_Taken,
    Printing_Done,
    Printing_Error,
    Deleting_Done,
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
    void printingDone();
    void printingError();
    void deleteTimerExpired();
    void imageCaptureDone();
    void highResImageCaptured(std::shared_ptr<QPixmap> pic);
    void start();
    void deletingDone();
    
private:
    /** run one iteration of the sm */
    void iterate();
    void displaying();
    void startup();
    void streaming();
    void taking();
    void printing();
    void deleting();
    void error();
    
    /**returns the number of lines in 'lpstat' output. This is identical to the number of print jobs running */
    int lpstatLineCount();
    
    /** cancel all queued print jobs */
    void cancelAllPrintJobs();
    
    /** check if the default printer is enabled */
    bool printerEnabled();
    
    void enablePrinter();
    
    void startDeleteCountdown();
    
    std::vector<std::function<void(void)>> stateHandlers;    
    State currentState;
    State lastState;
    Event currentEvent;
    
    QObject* rootGuiElement;
    CameraHandler& cameraHandler;
    QObject* popupText;
    QObject* image;
    QObject* takeButton;
    QObject* deleteButton;
    QObject* printButton;
    QObject* deleteTimerText;
    
    QString errorMessage;
    
    bool stopDeleteThread;
    
    /**Original image in original resolution etc. */
    std::shared_ptr<QPixmap> capturedImage;

};


