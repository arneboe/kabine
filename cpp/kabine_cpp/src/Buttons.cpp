#include "Buttons.h"
#include <wiringPi.h>


Buttons::Buttons()
{
    wiringPiSetup() ;
    pinMode(8, INPUT);
    pinMode(12, INPUT);
    pinMode(16, INPUT);
    pullUpDnControl(8, PUD_DOWN);
    pullUpDnControl(12, PUD_DOWN);
    pullUpDnControl(16, PUD_DOWN);
}

void Buttons::enableDeleteButton(const bool enabled)
{
    deleteEnabled = enabled;
}

void Buttons::enablePrintButton(const bool enabled)
{
    printEnabled = enabled;
}

void Buttons::enableTakeButton(const bool enabled)
{
    takeEnabled = enabled;
}

void Buttons::run()
{
    while(running)
    {
        msleep(50);
        if(deleteEnabled && digitalRead(8) == LOW)
        {
            emit deletePressed();
            msleep(5000); //a button can only be pressed once every 5 seconds, this is a shitty HACK of course
        }
        if(printEnabled &&digitalRead(12) == LOW)
        {
            emit printPressed();
            msleep(5000); //a button can only be pressed once every 5 seconds
        }
        if(takeEnabled && digitalRead(16) == LOW)
        {
            emit takePressed();
            msleep(5000); //a button can only be pressed once every 5 seconds
        }
    }
}


void Buttons::stopExecution()
{
    running = false;
}




