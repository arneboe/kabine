#include "StateMachine.h"
#include <iostream>
#include <QQuickWindow>
#include <QQuickItem>
#include <QQuickImageProvider>
#include <QPixmap>
#include <QThread>
#include <QPainter>
#include <cmath>
#include <thread>
#include <chrono>

using namespace std::chrono_literals;

std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}


StateMachine::StateMachine(QObject* rootGuiElement, CameraHandler& cameraHandler, Buttons& buttons) : QObject(nullptr),
    rootGuiElement(rootGuiElement), cameraHandler(cameraHandler), buttons(buttons)
{
    stateHandlers.resize(State::NUM_STATES);
    
    stateHandlers[State::Startup] = std::bind(&StateMachine::startup, this);
    stateHandlers[State::Streaming] = std::bind(&StateMachine::streaming, this);
    stateHandlers[State::Taking] = std::bind(&StateMachine::taking, this);
    stateHandlers[State::Displaying] = std::bind(&StateMachine::displaying, this);
    stateHandlers[State::Printing] = std::bind(&StateMachine::printing, this);
    stateHandlers[State::Error] = std::bind(&StateMachine::error, this);
    stateHandlers[State::Deleting] = std::bind(&StateMachine::deleting, this);
    currentState = Startup;
    lastState = Invalid;
    
}

void StateMachine::deletePicturePressed()
{
    currentEvent = Event::Delete_Picture_Pressed;
    iterate();
}

void StateMachine::printPicturePressed()
{
    currentEvent = Event::Print_Picture_Pressed;
    iterate();
}

void StateMachine::takePicturePressed()
{
    currentEvent = Event::Take_Picture_Pressed;
    iterate();
}

void StateMachine::deleteTimerExpired()
{
    currentEvent = Event::Display_Time_Expired;
    iterate();
}

void StateMachine::highResImageCaptured(std::shared_ptr<QPixmap> pic)
{
    capturedImage = pic;
}


void StateMachine::imageCaptureDone()
{
    currentEvent = Event::Picture_Taken;
    iterate();    
}


void StateMachine::start()
{
    currentState = Startup;
    lastState = Invalid;
    currentEvent = Event::Invalid_Event;
    iterate();
}


void StateMachine::iterate()
{
    stateHandlers[currentState]();
}

int StateMachine::lpstatLineCount()
{
    const std::string lpstatResult = exec("lpstat");
    const int numLines = std::count(lpstatResult.begin(), lpstatResult.end(), '\n');
    return numLines;
}

void StateMachine::printingDone()
{
    currentEvent = Event::Printing_Done;
    iterate();
}

void StateMachine::deletingDone()
{
    currentEvent = Event::Deleting_Done;
    iterate();
}

void StateMachine::printingError()
{
    currentEvent = Event::Printing_Error;
    iterate();   
}

void StateMachine::cancelAllPrintJobs()
{
    if(lpstatLineCount() > 0)
    {
        const int limit = 10;
        for(int i = 0; i < limit; ++i)
        {
            system("cancel -a");
            QThread::msleep(700);
            if(lpstatLineCount() == 0)
                return;
        }
        throw std::runtime_error("unable to cancel print jobs!");
    }

}

bool StateMachine::printerEnabled()
{
    const std::string result = exec("lpstat -p");
    return result.find("enabled") != std::string::npos;    
}

void StateMachine::enablePrinter()
{
    if(!printerEnabled())
    {
        const int limit = 10;
        for(int i = 0; i < limit; ++i)
        {
            system("cupsenable $(lpstat -d | cut -d: -f2)");
            QThread::msleep(700);
            if(printerEnabled())
                return;
        }
        throw std::runtime_error("unable to enable printer");
    }
}



void StateMachine::printing()
{
    if(lastState == Displaying)
    {
        lastState = Printing;
        enableDeleteButton( false);
        enablePrintButton(false);
        enableTakeButton( false);
        popupText->setProperty("text", "Printing ...");
        QMetaObject::invokeMethod(image, "hide");
        QMetaObject::invokeMethod(popupText, "show");
        QMetaObject::invokeMethod(deleteTimerText, "hide");
        
        
        //paper 100x148
        //convert aspect ratio to 1.48 (padding left and right with white rectangles)
        const double targetAspect = 1.48;
        const double newWidth = targetAspect * capturedImage->height();
        const int widthDiff2 = int((newWidth - capturedImage->width()) / 2);
        
        QPixmap aspectImage(std::ceil(newWidth), capturedImage->height());
        aspectImage.fill(Qt::white);
        QPainter p(&aspectImage);      
        p.drawImage(widthDiff2, 0, capturedImage->toImage());
        p.end();
        
        aspectImage.save("/home/pi/ramdisk/image_conv.bmp");
        
        
        //if paper tray was removed the printer is disabled and does not re-enable automatically
        try
        {
            enablePrinter();
        }
        catch (std::runtime_error& error)
        {
            errorMessage = QString("Printing Error: enable fail");
            currentState = Error;
            iterate();
            return;
        }
        
        //sometimes print jobs get stuck (when we run out of paper, or other bugs happen).
        //cancel all old print jobs before printing
        try 
        {
            cancelAllPrintJobs();
        }
        catch (std::runtime_error& error)
        {
            errorMessage = QString("Printing Error: cancel fail");
            currentState = Error;
            iterate();
            return;
        }
        system("lp /home/pi/ramdisk/image_conv.bmp");
        
        //start a thread to monitor progress
        
        std::thread t([this]
        {
            auto startTime = std::chrono::steady_clock::now();
            
            //give cups time to start printing
            std::this_thread::sleep_for(5s);
            while(lpstatLineCount() > 0)
            {
                std::this_thread::sleep_for(1s);
                
                if((std::chrono::steady_clock::now() - startTime) > 180s)
                {
                    QMetaObject::invokeMethod(this, "printingError");
                    return;
                }
                
            }
            //printer reports ready approx. 15 seconds before printing is acutally done...
            std::this_thread::sleep_for(15s);
            QMetaObject::invokeMethod(this, "printingDone");
            
        });
        t.detach();
        
    }
    else if(lastState == Printing)
    {
        if(currentEvent == Event::Printing_Done)
        {
            currentEvent = Event::Invalid_Event;
            currentState = Streaming;
            iterate();
        }
        else if(currentEvent == Event::Printing_Error)
        {
            errorMessage = QString("Printing Error");
            currentState = Error;
            iterate();
        }
        else
        {
            throw std::runtime_error("illegal event in printing state");
        }
    }
    else
    {
        throw std::runtime_error("entered printing from illegal state");
    }
}

void StateMachine::deleting()
{
    if(lastState == Displaying)
    {
        lastState = Deleting;
        popupText->setProperty("text", "Deleting ...");
        QMetaObject::invokeMethod(image, "hide");
        QMetaObject::invokeMethod(deleteTimerText, "hide");
        QMetaObject::invokeMethod(popupText, "show");
        
        std::thread t([this]
        {
            std::this_thread::sleep_for(3s);
            QMetaObject::invokeMethod(this, "deletingDone");
            
        });
        t.detach();
        
    }
    else if(lastState == Deleting)
    {
        if(currentEvent == Event::Deleting_Done)
        {
            currentEvent = Event::Invalid_Event;
            currentState = Streaming;
            iterate();
        }
        else
        {
            throw std::runtime_error("illeal event in deleting state");
        }
    }
    else
    {
        throw std::runtime_error("Deleting state entered from illiegal state");
    }
    
}

void StateMachine::error()
{
    popupText->setProperty("text", errorMessage);
    enableDeleteButton( false);
    enablePrintButton( false);
    enableTakeButton( false);
    QMetaObject::invokeMethod(image, "hide");
    QMetaObject::invokeMethod(popupText, "show");
}


void StateMachine::streaming()
{
    //entering state
    if(lastState == Startup || lastState == Deleting || lastState == Printing)
    {
        QMetaObject::invokeMethod(image, "show");
        QMetaObject::invokeMethod(popupText, "hide");
        enableDeleteButton( false);
        enablePrintButton( false);
        enableTakeButton( true);
        lastState = Streaming;
        cameraHandler.triggerPreviewStreaming();
    }
    else if(lastState == Streaming)
    {// event received
       if(currentEvent == Event::Take_Picture_Pressed)
       {
           currentEvent = Event::Invalid_Event;
           currentState = Taking;
           iterate();
       }
       else
       {
           throw std::runtime_error("illegal event in state streaming");
       }
    }
    else
    {
        throw std::runtime_error("Streaming state entered from an illegal state");
    }
}

void StateMachine::startDeleteCountdown()
{
    const int secsTillDelete = 45;
        
    QMetaObject::invokeMethod(deleteTimerText, "show");    
    QMetaObject::invokeMethod(deleteTimerText, "updateTime", Q_ARG(QVariant, secsTillDelete));    
    
    stopDeleteThread = false;
    std::thread t([this, secsTillDelete]
    {
        for(int i = secsTillDelete - 1; i > 0; --i)
        {
            if(stopDeleteThread)
            {
                return;
            }
            std::this_thread::sleep_for(1s);
            QMetaObject::invokeMethod(deleteTimerText, "updateTime", Q_ARG(QVariant, i));    
        }        
        QMetaObject::invokeMethod(this, "deleteTimerExpired");
    });
    t.detach();
    
}

void StateMachine::displaying()
{
    if(lastState == Taking)
    {
        lastState = Displaying;
        startDeleteCountdown();
        
    }
    else if(lastState == Displaying)
    {
        //got event
        if(currentEvent == Event::Print_Picture_Pressed)
        {
            stopDeleteThread = true;
            currentEvent = Event::Invalid_Event;
            currentState = Printing;
            iterate();
        }
        else if(currentEvent == Event::Delete_Picture_Pressed || currentEvent == Event::Display_Time_Expired)
        {
            stopDeleteThread = true;
            currentEvent = Event::Invalid_Event;
            currentState = Deleting;
            iterate();
        }
        else
        {
            throw std::runtime_error("Displaying state illegal event");
        }
    }
    else
    {
        throw std::runtime_error("Displaying state entered from illegal state");
    }
}


void StateMachine::taking()
{
    if(lastState == Streaming)
    {
        lastState = Taking;
         cameraHandler.stopPreviewStreaming();
        
        //Show text while we wait for the picture
        popupText->setProperty("text", "Taking Picture");
        QMetaObject::invokeMethod(popupText, "show");
        QMetaObject::invokeMethod(image, "hide");
        enableTakeButton( false);
        cameraHandler.triggerCapture();
    }
    else if(lastState == Taking)
    {
        //received event
        if(currentEvent == Event::Picture_Taken)
        {
            currentEvent = Event::Invalid_Event;
            QMetaObject::invokeMethod(popupText, "hide");
            QMetaObject::invokeMethod(image, "show");
            enableDeleteButton( true);
            enablePrintButton( true);
            //switch to displaying
            currentState = Displaying;
            iterate();
        }
       
    }
    else
    {
        throw std::runtime_error("Taking state entered from illegal state");
    }
}

void StateMachine::startup()
{
    if(lastState != Invalid)
        throw std::runtime_error("Startup state entered from an illegal state");
    
    lastState = Startup;
    
    popupText = rootGuiElement->findChild<QQuickItem*>("take_pic_text");
    image = rootGuiElement->findChild<QQuickItem*>("image_viewer");
    takeButton = rootGuiElement->findChild<QQuickItem*>("take_pic_button");
    deleteButton = rootGuiElement->findChild<QQuickItem*>("print_pic_button");
    printButton = rootGuiElement->findChild<QQuickItem*>("delete_pic_button");
    deleteTimerText = rootGuiElement->findChild<QQuickItem*>("timer_text");
        
    QQuickItem* takePicButton = rootGuiElement->findChild<QQuickItem*>("take_pic_button");
    QQuickItem* printPicButton = rootGuiElement->findChild<QQuickItem*>("print_pic_button");
    QQuickItem* deletePicButton = rootGuiElement->findChild<QQuickItem*>("delete_pic_button");
    QObject::connect(takePicButton, SIGNAL(clicked()), this, SLOT(takePicturePressed()));
    QObject::connect(printPicButton, SIGNAL(clicked()), this, SLOT(printPicturePressed()));
    QObject::connect(deletePicButton, SIGNAL(clicked()), this, SLOT(deletePicturePressed()));
    QObject::connect(&buttons, SIGNAL(takePressed()), this, SLOT(takePicturePressed()));
    QObject::connect(&buttons, SIGNAL(printPressed()), this, SLOT(printPicturePressed()));
    QObject::connect(&buttons, SIGNAL(deletePressed()), this, SLOT(deletePicturePressed()));
    
    
    QObject::connect(&cameraHandler, SIGNAL(imageCaptureDone()), this, SLOT(imageCaptureDone()));
    QObject::connect(&cameraHandler, SIGNAL(highResImageCaptured(std::shared_ptr<QPixmap>)), this, SLOT(highResImageCaptured(std::shared_ptr<QPixmap>)));
    
    cameraHandler.start();
    
    //execute streaming state
    currentState = Streaming;
    iterate();
}


void StateMachine::enablePrintButton(bool enabled)
{
    printButton->setProperty("enabled", enabled);
    buttons.enablePrintButton(enabled);
}

void StateMachine::enableDeleteButton(bool enabled)
{
    deleteButton->setProperty("enabled", enabled);
    buttons.enableDeleteButton(enabled);
}

void StateMachine::enableTakeButton(bool enabled)
{
    takeButton->setProperty("enabled", enabled);
    buttons.enableTakeButton(enabled);
}

