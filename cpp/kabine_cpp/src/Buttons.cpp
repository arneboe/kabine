#include "Buttons.h"
#include <wiringPi.h>
#include <iostream>

Buttons::Buttons()
{
    wiringPiSetup() ;
    pinMode(15, INPUT);
    pinMode(1, INPUT);
    pinMode(4, INPUT);
    pullUpDnControl(15, PUD_DOWN);
    pullUpDnControl(1, PUD_DOWN);
    pullUpDnControl(4, PUD_DOWN);
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
        if(deleteEnabled && digitalRead(15) == HIGH)
        {
            bool stillDown = true;
            for(int i = 0; i < 10; ++i)
            {
                msleep(10);
                if(digitalRead(15) == LOW)
                {
                    stillDown = false;
                    break;
                }
            }
            if(stillDown)
            {
                emit deletePressed();
                msleep(5000); //a button can only be pressed once every 5 seconds, this is a shitty HACK of course
            }
        }
        if(printEnabled &&digitalRead(1) == HIGH)
        {
            bool stillDown = true;
            for(int i = 0; i < 10; ++i)
            {
                msleep(10);
                if(digitalRead(1) == LOW)
                {
                    stillDown = false;
                    break;
                }
            }
            if(stillDown)
            {
                emit printPressed();
                msleep(5000); //a button can only be pressed once every 5 seconds, this is a shitty HACK of course
            }
        }
        if(takeEnabled && digitalRead(4) == HIGH)
        {
            bool stillDown = true;
            for(int i = 0; i < 10; ++i)
            {
                msleep(10);
                if(digitalRead(4) == LOW)
                {
                    stillDown = false;
                    break;
                }
            }
            if(stillDown)
            {
                emit takePressed();
                msleep(5000); //a button can only be pressed once every 5 seconds, this is a shitty HACK of course
            }
        }
 
    }
}


void Buttons::stopExecution()
{
    running = false;
}




