#pragma once

#include <QObject>


//This is a helper to connect signals from qml to lambdas...
class QmlSignalRouter : public QObject
{
    Q_OBJECT
    
public slots:
    void imageClicked();
    void printPicClicked();
    void takePicClicked();
    void deletePicClicked();
    
signals:
    void imageClickedSignal();
    void printPicClickedSignal();
    void takePicClickedSignal();
    void deletePicClickedSignal();
    

};


