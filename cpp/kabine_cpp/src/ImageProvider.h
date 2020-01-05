#pragma once
#include <QQuickImageProvider>


class ImageProvider : public QQuickImageProvider
{

public:
    ImageProvider();
    virtual QPixmap requestPixmap ( const QString& id, QSize* size, const QSize& requestedSize );

};
