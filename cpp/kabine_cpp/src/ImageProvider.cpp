#include "ImageProvider.h"
#include <iostream>
ImageProvider::ImageProvider() : QQuickImageProvider(QQuickImageProvider::Pixmap), QObject(nullptr), 
                                 pixmap(3264, 2448)
{
       pixmap.fill(QColor("black").rgba());
}

QPixmap ImageProvider::requestPixmap ( const QString& id, QSize* size, const QSize& requestedSize)
{
    {
        const std::lock_guard<std::mutex> lock(pixmapMutex);
        pixmap.scaledToHeight(requestedSize.height());
        if(size)
            *size = pixmap.size();
        return pixmap;
    }
}

void ImageProvider::capturedImage(std::shared_ptr<QPixmap> image)
{
    //FIXME avoid copy here
    const std::lock_guard<std::mutex> lock(pixmapMutex);
    pixmap = *image;
}

QPixmap ImageProvider::getPixmap()
{
    const std::lock_guard<std::mutex> lock(pixmapMutex);
    return pixmap;
}
