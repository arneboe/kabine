import sys

from PIL import Image, ImageQt
from PyQt5.QtWidgets import QApplication, QWidget, QLabel, QPushButton, QVBoxLayout
from PyQt5.QtGui import QIcon, QPixmap

from Camera import Camera
from functools import partial

class App(QWidget):

    def __init__(self):
        super().__init__()
        self.camera = Camera(partial(App.preview_image_updated, self), partial(App.image_updated, self))
        self.width = 640
        self.height = 480

        self.start_preview_button = QPushButton('Start Preview', self)
        self.stop_preview_button = QPushButton('Stop Preview', self)
        self.take_picture_button = QPushButton('Take Picture', self)
        self.config_button = QPushButton("config", self)

        self.start_preview_button.clicked.connect(self.start_preview_clicked)
        self.stop_preview_button.clicked.connect(self.stop_preview_clicked)
        self.take_picture_button.clicked.connect(self.take_picture_clicked)
        self.config_button.clicked.connect(self.config_clicked)

        self.label = QLabel(self)
        pixmap = QPixmap(800, 800)
        self.label.setPixmap(pixmap)
        layout = QVBoxLayout(self)

        layout.addWidget(self.label)
        layout.addWidget(self.start_preview_button)
        layout.addWidget(self.stop_preview_button)
        layout.addWidget(self.take_picture_button)
        layout.addWidget(self.config_button)
        self.setLayout(layout)

        self.show()

    def config_clicked(self):
        self.camera.print_config()

    def start_preview_clicked(self):
        self.camera.start_preview()

    def stop_preview_clicked(self):
        self.camera.stop_preview()

    def take_picture_clicked(self):
        self.camera.take_picture()

    def preview_image_updated(self, image: Image):
        image_qt = ImageQt.ImageQt(image)
        pixmap = QPixmap.fromImage(image_qt)
        self.label.setPixmap(pixmap.scaledToWidth(800))
        print("got img")

    def image_updated(self, image: Image):
        image_qt = ImageQt.ImageQt(image)
        pixmap = QPixmap.fromImage(image_qt)
        self.label.setPixmap(pixmap.scaledToWidth(800))
        print("got readl img")


if __name__ == '__main__':
    app = QApplication(sys.argv)
    ex = App()
    app.exec()

