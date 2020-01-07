#include "QmlSignalRouter.h"

void QmlSignalRouter::imageClicked()
{
    emit imageClickedSignal();
}

void QmlSignalRouter::deletePicClicked()
{
    emit deletePicClickedSignal();
}

void QmlSignalRouter::printPicClicked()
{
    emit printPicClickedSignal();
}

void QmlSignalRouter::takePicClicked()
{
    emit takePicClickedSignal();
}



