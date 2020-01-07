#pragma once
#include <QQuickImageProvider>
#include <QPixmap>
#include <memory>
#include <mutex>

//ATTENTION QObject needs to be inherited from first
class ImageProvider : public QObject, public QQuickImageProvider
{
    Q_OBJECT
public:
    ImageProvider();
    virtual QPixmap requestPixmap ( const QString& id, QSize* size, const QSize& requestedSize );

public slots:
    void capturedImage(std::shared_ptr<QPixmap> image);
    
private:
    QPixmap pixmap;
    std::mutex pixmapMutex;
};
