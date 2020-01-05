#include "ImageProvider.h"
#include <iostream>
ImageProvider::ImageProvider() : QQuickImageProvider(QQuickImageProvider::Pixmap), QObject(nullptr), 
                                 pixmap(50, 50)
{
       pixmap.fill(QColor("black").rgba());
}

QPixmap ImageProvider::requestPixmap ( const QString& id, QSize* size, const QSize& requestedSize)
{
    if(size)
        *size = pixmap.size();
    return pixmap;
}

void ImageProvider::capturedImage(std::shared_ptr<QPixmap> image)
{
    //FIXME avoid copy here
    //FIXME add thread protection
    pixmap = *image;
}
