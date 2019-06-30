import sys
import threading
from enum import Enum

from PIL import Image, ImageQt
from PyQt5.QtWidgets import QApplication, QWidget, QLabel, QPushButton, QVBoxLayout, QSpinBox, QHBoxLayout, QComboBox, \
    QGraphicsView
from PyQt5.QtGui import QIcon, QPixmap, QFont, QPainter, QColor
from PyQt5.QtCore import Qt, QTimer
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
        self.camera = Camera()
        self.camera.picture_received.connect(self.picture_received)
        self.camera.preview_received.connect(self.preview_received)

        self.deleteTimer = QTimer()
        self.deleteTimer.setInterval(1000)
        self.deleteTimer.timeout.connect(self.delete_timer_tick)
        self.deleteTimer.setSingleShot(False)

        self.deleteCounter = 0

        self.label = QLabel(self)
        layout = QVBoxLayout(self)
        layout.setAlignment(Qt.AlignHCenter)
        layout.addWidget(self.label)

        buttonFont = QFont( "Arial")
        buttonFont.setBold(True)
        buttonFont.setPointSize(30)

        buttonLayout = QHBoxLayout()
        buttonLayout.setAlignment(Qt.AlignHCenter)
        buttonLayout.setSpacing(200)


        self.takePicButton = QLabel("Take Picture")
        self.takePicButton.setFont(buttonFont)
        buttonLayout.addWidget(self.takePicButton)

        self.printAndDelButton = QLabel("Print & Delete Picture")
        self.printAndDelButton.setFont(buttonFont)
        self.printAndDelButton.setEnabled(False)


        buttonLayout.addWidget(self.printAndDelButton)

        self.deleteButton = QLabel("Delete Picture")
        self.deleteButton.setFont(buttonFont)
        self.deleteButton.setEnabled(False)
        buttonLayout.addWidget(self.deleteButton)

        layout.addLayout(buttonLayout)
        self.setLayout(layout)

        self.showFullScreen()

        self.imageHeight = QApplication.desktop().screenGeometry().height() - self.deleteButton.height() - 70
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
            self.deleteTimer.stop()
            self.deleteButton.setEnabled(False)
            self.printAndDelButton.setEnabled(False)
            self.takePicButton.setEnabled(True)
            self.state = AppState.PRVIEW_SHOWING
        else:
            print("start Preview, illegal state", self.state)

    def print_pressed(self):
        if self.state == AppState.PICTURE_SHOWING:
            self.deleteTimer.stop()
            self.start_preview()


    def delete_pressed(self):
        if self.state == AppState.PICTURE_SHOWING:
            self.deleteTimer.stop()
            self.start_preview()

    def clear_picture(self):
        pixmap = QPixmap(self.label.pixmap().width(), self.imageHeight)
        painter = QPainter(pixmap)
        painter.fillRect(0, 0, self.label.pixmap().width(), self.imageHeight, QColor("black"))
        painter.end()
        self.label.setPixmap(pixmap.scaledToHeight(self.imageHeight))

    def take_picture_pressed(self):
        self.state = AppState.WAITING_FOR_PICTURE
        self.deleteTimer.stop()
        self.clear_picture()
        self.camera.take_picture()

    def picture_received(self, pixmap: QPixmap):
        if self.state == AppState.WAITING_FOR_PICTURE:
            self.label.setPixmap(pixmap.scaledToHeight(self.imageHeight))
            self.received_picture = QPixmap(pixmap)
            self.start_delete_timer()
            self.deleteButton.setEnabled(True)
            self.printAndDelButton.setEnabled(True)
            self.state = AppState.PICTURE_SHOWING

    def preview_received(self, pixmap: QPixmap):
        self.label.setPixmap(pixmap.scaledToHeight(self.imageHeight))

    def start_delete_timer(self):
        self.deleteCounter = 60
        self.deleteTimer.start()

    def delete_timer_tick(self):
        self.deleteCounter -= 1
        if self.deleteCounter <= 0:
            self.deleteTimer.stop()
            self.start_preview()

        pixmap = QPixmap(self.received_picture)
        pixmap = pixmap.scaledToHeight(self.imageHeight)
        painter = QPainter(pixmap)
        painter.setPen(QColor("white"))
        font = QFont("Arial")
        font.setBold(True)
        font.setPointSize(30)
        painter.setFont(font)
        painter.drawText(50, 50, "Picture will be deleted in " + str(self.deleteCounter) + " seconds")
        painter.end()
        self.label.setPixmap(pixmap)


if __name__ == '__main__':
    app = QApplication(sys.argv)
    ex = App()
    app.exec()

