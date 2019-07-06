import io

import gphoto2 as gp
import threading
from PIL import Image, ImageQt
from PyQt5 import QtCore
from PyQt5.QtCore import QObject, QThread
from PyQt5.QtGui import QPixmap, QPainter, QColor

from Configuration import Configuration

last_picture = None


class PreviewThread(QThread):
    preview_received = QtCore.pyqtSignal(QPixmap)

    def __init__(self, camera):
        super().__init__()
        self.camera = camera
        self.running = False

        self.debugColor = QColor("blue")

    def start(self, priority=None):
        self.running = True
        super().start()

    def stop(self):
        self.running = False

    def run(self):
        while self.running:
            camera_file = gp.check_result(gp.gp_camera_capture_preview(self.camera))
            file_data = gp.check_result(gp.gp_file_get_data_and_size(camera_file))
            image_qt = ImageQt.ImageQt(Image.open(io.BytesIO(file_data)))
            pixmap = QPixmap.fromImage(image_qt)
            self.preview_received.emit(pixmap)

class PictureThread(QThread):
    picture_received = QtCore.pyqtSignal()

    def __init__(self, camera):
        super().__init__()
        self.camera = camera
        self.pixmap = QPixmap()

    def run(self):
        global last_picture
        file_path = gp.check_result(gp.gp_camera_capture(self.camera, gp.GP_CAPTURE_IMAGE))
        print('Camera file path: {0}/{1}'.format(file_path.folder, file_path.name))
        camera_file = gp.check_result(gp.gp_camera_file_get(self.camera, file_path.folder, file_path.name, gp.GP_FILE_TYPE_NORMAL))
        file_data = gp.check_result(gp.gp_file_get_data_and_size(camera_file))
        gp.check_result(gp.gp_camera_file_delete(self.camera, file_path.folder, file_path.name))

        image_qt = ImageQt.ImageQt(Image.open(io.BytesIO(file_data)))
        image_qt.save("/home/arne/image.png")

        self.picture_received.emit()


class Camera(QObject):
    picture_received = QtCore.pyqtSignal()
    preview_received = QtCore.pyqtSignal(QPixmap)

    def __init__(self):
        '''
        :raises: Exception if camera init fails
        '''
        super().__init__()

        gp.check_result(gp.use_python_logging())
        self.camera = gp.check_result(gp.gp_camera_new())
        gp.check_result(gp.gp_camera_init(self.camera))
        self.previewThread = PreviewThread(self.camera)
        self.previewThread.preview_received.connect(self.preview_received_slot)
        self.pictureThread = PictureThread(self.camera)
        self.pictureThread.picture_received.connect(self.picture_received_slot)

        #take initial picture to switch camera to capture mode.
        #FIXME there is proably a better way to do that
        self.take_picture(True)

        self.config = Configuration(self.camera)
        self.start_preview()

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

    def take_picture(self, wait_till_finished=False):
        self.stop_preview()
        self.pictureThread.start()
        if wait_till_finished:
            self.pictureThread.wait()


    def picture_received_slot(self):
        self.picture_received.emit()

    def preview_received_slot(self, pixmap: QPixmap):
        self.preview_received.emit(pixmap)