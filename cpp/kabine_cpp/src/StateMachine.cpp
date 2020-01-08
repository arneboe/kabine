#include "StateMachine.h"
#include <iostream>
#include <QQuickWindow>
#include <QQuickItem>
#include <QQuickImageProvider>

StateMachine::StateMachine(QObject* rootGuiElement, CameraHandler& cameraHandler) : QObject(nullptr),
    rootGuiElement(rootGuiElement), cameraHandler(cameraHandler)
{
    stateHandlers.resize(State::NUM_STATES);
    
    stateHandlers[State::Startup] = std::bind(&StateMachine::startup, this);
    stateHandlers[State::Streaming] = std::bind(&StateMachine::streaming, this);
    stateHandlers[State::Taking] = std::bind(&StateMachine::taking, this);
    stateHandlers[State::Displaying] = std::bind(&StateMachine::displaying, this);
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

void StateMachine::timerExpired()
{
    currentEvent = Event::Display_Time_Expired;
    iterate();
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

void StateMachine::streaming()
{
    //entering state
    if(lastState == Startup || lastState == Deleting || lastState == Printing)
    {
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

void StateMachine::displaying()
{
    if(lastState == Taking)
    {
        lastState = Displaying;
    }
    else if(lastState == Displaying)
    {
        //got event
        if(currentEvent == Event::Display_Time_Expired)
        {
        }
        else if(currentEvent == Event::Print_Picture_Pressed)
        {
        }
        else if(currentEvent == Event::Delete_Picture_Pressed)
        {
            currentEvent = Event::Invalid_Event;
            currentState = Streaming;
            lastState = Deleting; //HACK deleting state is virtual
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
        QMetaObject::invokeMethod(takingPictureText, "show");
        cameraHandler.triggerCapture();
    }
    else if(lastState == Taking)
    {
        //received event
        if(currentEvent == Event::Picture_Taken)
        {
            currentEvent = Event::Invalid_Event;
            QMetaObject::invokeMethod(takingPictureText, "hide");
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
    
    takingPictureText = rootGuiElement->findChild<QQuickItem*>("take_pic_text");
    
    
    QQuickItem* takePicButton = rootGuiElement->findChild<QQuickItem*>("take_pic_button");
    QQuickItem* printPicButton = rootGuiElement->findChild<QQuickItem*>("print_pic_button");
    QQuickItem* deletePicButton = rootGuiElement->findChild<QQuickItem*>("delete_pic_button");
    QObject::connect(takePicButton, SIGNAL(clicked()), this, SLOT(takePicturePressed()));
    QObject::connect(printPicButton, SIGNAL(clicked()), this, SLOT(printPicturePressed()));
    QObject::connect(deletePicButton, SIGNAL(clicked()), this, SLOT(deletePicturePressed()));
    QObject::connect(&cameraHandler, SIGNAL(imageCaptureDone()), this, SLOT(imageCaptureDone()));
    
    cameraHandler.start();
    
    //execute streaming state
    currentState = Streaming;
    iterate();
}




