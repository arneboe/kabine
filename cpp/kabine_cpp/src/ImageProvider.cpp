#include "ImageProvider.h"
#include <iostream>
ImageProvider::ImageProvider() : QQuickImageProvider(QQuickImageProvider::Pixmap)
{
}

QPixmap ImageProvider::requestPixmap ( const QString& id, QSize* size, const QSize& requestedSize )
{
    std::cout << "AAAA" << std::endl;
      int width = 100;
       int height = 50;

       if (size)
          *size = QSize(width, height);
       QPixmap pixmap(requestedSize.width() > 0 ? requestedSize.width() : width,
                      requestedSize.height() > 0 ? requestedSize.height() : height);
       pixmap.fill(QColor("red").rgba());
       return pixmap;
}
