import io

import gphoto2 as gp
import threading
from PIL import Image, ImageQt
from PyQt5 import QtCore
from PyQt5.QtCore import QObject, QThread
from PyQt5.QtGui import QPixmap, QPainter, QColor

from Configuration import Configuration


class PreviewThread(QThread):
    preview_received = QtCore.pyqtSignal(QPixmap)

    def __init__(self):
        super().__init__()
        self.running = False

        self.debugColor = QColor("blue")

    def start(self, priority=None):
        self.running = True
        super().start()

    def stop(self):
        self.running = False

    def run(self):
        while self.running:
            #camera_file = gp.check_result(gp.gp_camera_capture_preview(self.camera))
            #file_data = gp.check_result(gp.gp_file_get_data_and_size(camera_file))
            #self.user_preview_callback(Image.open(io.BytesIO(file_data)))
            self.debugColor.setRed((self.debugColor.red() + 1) % 255)

            pixmap = QPixmap(300, 300)
            painter = QPainter(pixmap)
            painter.fillRect(0, 0, pixmap.width(), pixmap.height(), self.debugColor)
            painter.end()
            self.preview_received.emit(pixmap)
            self.msleep(10)

class PictureThread(QThread):
    picture_received = QtCore.pyqtSignal(QPixmap)

    def __init__(self):
        super().__init__()

    def run(self):
        #file_path = gp.check_result(gp.gp_camera_capture(self.camera, gp.GP_CAPTURE_IMAGE))
        #print('Camera file path: {0}/{1}'.format(file_path.folder, file_path.name))
        #camera_file = gp.check_result(gp.gp_camera_file_get(self.camera, file_path.folder, file_path.name, gp.GP_FILE_TYPE_NORMAL))
        #file_data = gp.check_result(gp.gp_file_get_data_and_size(camera_file))
        #gp.check_result(gp.gp_camera_file_delete(self.camera, file_path.folder, file_path.name))

        #image_qt = ImageQt.ImageQt(Image.open(io.BytesIO(file_data)))
        #pixmap = QPixmap.fromImage(image_qt)
        self.sleep(2)
        pixmap = QPixmap(300, 300)
        painter = QPainter(pixmap)
        painter.fillRect(0, 0, pixmap.width(), pixmap.height(), QColor("green"))
        painter.end()
        self.picture_received.emit(pixmap)


class Camera(QObject):
    picture_received = QtCore.pyqtSignal(QPixmap)
    preview_received = QtCore.pyqtSignal(QPixmap)

    def __init__(self):
        '''
        :raises: Exception if camera init fails
        '''
        super().__init__()

        self.previewThread = PreviewThread()
        self.previewThread.preview_received.connect(self.preview_received_slot)

        #gp.check_result(gp.use_python_logging())
        #self.camera = gp.check_result(gp.gp_camera_new())
        #self.run_thread = False
        #self.user_preview_callback = preview_callback
        #self.user_picture_callback = picture_callback
        #gp.check_result(gp.gp_camera_init(self.camera))

        #take initial picture to switch camera to capture mode.
        #FIXME there is proably a better way to do that
        #self.take_picture(False)

        #self.config = Configuration(self.camera)

    def __del__(self):
        gp.check_result(gp.gp_camera_exit(self.camera))

    def start_preview(self):
        '''
        Start preview streaming
        :return:
        '''
        self.previewThread.start()

    def stop_preview(self):
        '''
        Stop the preview streaming
        :return:
        '''
        if(self.previewThread.isRunning()):
            self.previewThread.stop()
            self.previewThread.wait()

    def take_picture(self, use_callback=True):
        self.stop_preview()
        self.t = PictureThread()
        self.t.picture_received.connect(self.picture_received_slot)
        self.t.start()

    def picture_received_slot(self, pixmap: QPixmap):
        self.picture_received.emit(pixmap)

    def preview_received_slot(self, pixmap: QPixmap):
        self.preview_received.emit(pixmap)