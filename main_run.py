import sys
import threading
from enum import Enum

from PIL import Image, ImageQt
from PyQt5.QtWidgets import QApplication, QWidget, QLabel, QPushButton, QVBoxLayout, QSpinBox, QHBoxLayout, QComboBox, \
    QGraphicsView
from PyQt5.QtGui import QIcon, QPixmap, QFont, QPainter, QColor
from PyQt5.QtCore import Qt
from Configuration import Iso, WhiteBalance, Aperature, ShutterSpeed

from Camera import Camera
from functools import partial


class AppState(Enum):
    PRVIEW_SHOWING = 0
    PICTURE_SHOWING = 1
    WAITING_FOR_PICTURE = 2
    INIT = 99
    pass

class App(QWidget):

    def __init__(self):
        super().__init__()
        self.camera = Camera(partial(App.preview_image_updated, self), partial(App.image_updated, self))


        self.graphic = QGraphicsView()

        self.label = QLabel(self)
        layout = QVBoxLayout(self)
        layout.setAlignment(Qt.AlignHCenter)
        layout.addWidget(self.label)

        buttonFont = QFont( "Arial")
        buttonFont.setBold(True)
        buttonFont.setPointSize(30)

        buttonLayout = QHBoxLayout()


        printButton = QLabel("Print")
        printButton.setFont(buttonFont)
        buttonLayout.addWidget(printButton)

        printAndDel = QLabel("Print & Delete")
        printAndDel.setFont(buttonFont)

        buttonLayout.addWidget(printAndDel)

        delete = QLabel("Delete")
        delete.setFont(buttonFont)
        buttonLayout.addWidget(delete)

        layout.addLayout(buttonLayout)
        self.setLayout(layout)

        self.showFullScreen()

        self.imageHeight = QApplication.desktop().screenGeometry().height() - delete.height() - 30
        print(self.imageHeight)
        self.state = AppState.INIT
        self.start_preview()

    def keyPressEvent(self, event):
        super().keyPressEvent(event)
      #  print(event.key())
        #77 is m
        #68 is d
        #80 is p
        #32 is space

        if event.key() == 77:
            if self.isFullScreen():
                self.showMaximized()
            else:
                self.showFullScreen()
        elif event.key() == 80:
            self.print_pressed()
        elif event.key() == 68:
            self.delete_pressed()
        elif event.key() == 32:
            self.take_picture_pressed()

    def start_preview(self):
        if self.state == AppState.INIT or self.state == AppState.PICTURE_SHOWING:
            self.camera.start_preview()
            self.state = AppState.PRVIEW_SHOWING
        else:
            print("start Preview, illegal state", self.state)

    def print_pressed(self):
        if self.state == AppState.PICTURE_SHOWING:
            self.start_preview()


    def delete_pressed(self):
        if self.state == AppState.PICTURE_SHOWING:
            self.start_preview()


    def clear_picture(self):
        pixmap = QPixmap(self.label.pixmap().width(), self.imageHeight)
        painter = QPainter(pixmap)
        painter.fillRect(0, 0, self.label.pixmap().width(), self.imageHeight, QColor("black"))
        painter.end()
        self.label.setPixmap(pixmap.scaledToHeight(self.imageHeight))

    def take_picture_pressed(self):
        self.state = AppState.WAITING_FOR_PICTURE
        self.camera.stop_preview()
        self.clear_picture()

        #do in background to not block gui
        t = threading.Thread(target=self.take_picture_thread)
        t.start()

    def take_picture_thread(self):
        self.camera.take_picture()

    def preview_image_updated(self, image: Image):
        image_qt = ImageQt.ImageQt(image)
        pixmap = QPixmap.fromImage(image_qt)
        self.label.setPixmap(pixmap.scaledToHeight(self.imageHeight))

    def image_updated(self, image: Image):
        if self.state == AppState.WAITING_FOR_PICTURE:
            image_qt = ImageQt.ImageQt(image)
            pixmap = QPixmap.fromImage(image_qt)
            self.label.setPixmap(pixmap.scaledToHeight(self.imageHeight))
            self.state = AppState.PICTURE_SHOWING


if __name__ == '__main__':
    app = QApplication(sys.argv)
    ex = App()
    app.exec()

