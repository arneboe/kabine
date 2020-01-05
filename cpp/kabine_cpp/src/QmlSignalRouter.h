#pragma once

#include <QObject>


//This is a helper to connect signals from qml to lambdas...
class QmlSignalRouter : public QObject
{
    Q_OBJECT
    
public slots:
    void imageClicked();
    
signals:
    void imageClickedSignal();

};


