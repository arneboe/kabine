#pragma once
#include <QThread>

class Buttons : public QThread
{
    Q_OBJECT
public:
    
    
    
    Buttons();
    
    void enablePrintButton(const bool enabled);
    void enableTakeButton(const bool enabled);
    void enableDeleteButton(const bool enabled);
    
    void stopExecution();
    
    void run() override;
    
    
signals:
    void printPressed();
    void takePressed();
    void deletePressed();
    
private:
    
    bool printEnabled = true;
    bool takeEnabled = true;
    bool deleteEnabled = true;
    bool running = true;
    
    int deleteDebounceStartTime = 0;
    int printDebounceStartTime = 0;
    int takeDebounceStartTime = 0;
    
};
