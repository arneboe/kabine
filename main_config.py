import sys

from PIL import Image, ImageQt
from PyQt5.QtWidgets import QApplication, QWidget, QLabel, QPushButton, QVBoxLayout, QSpinBox, QHBoxLayout, QComboBox
from PyQt5.QtGui import QIcon, QPixmap
from PyQt5.QtCore import Qt
from Configuration import Iso, WhiteBalance, Aperature, ShutterSpeed

from Camera import Camera
from functools import partial

class App(QWidget):

    def __init__(self):
        super().__init__()
        self.camera = Camera()
        self.width = 640
        self.height = 480

        self.camera.picture_received.connect(self.picture_received)
        self.camera.preview_received.connect(self.preview_received)

        self.start_preview_button = QPushButton('Start Preview', self)
        self.stop_preview_button = QPushButton('Stop Preview', self)
        self.take_picture_button = QPushButton('Take Picture', self)
        self.config_button = QPushButton("print config", self)
        self.zoom_box = QSpinBox(parent=self)
        self.zoom_label = QLabel("Zoom")

        self.iso_box = QComboBox(parent=self)
        for s in Iso:
            self.iso_box.addItem(str(s))
        self.iso_box.currentTextChanged.connect(self.iso_changed)

        self.wb_box = QComboBox(parent=self)
        for s in WhiteBalance:
            self.wb_box.addItem(str(s))
        self.wb_box.currentTextChanged.connect(self.wb_changed)

        self.ap_box = QComboBox(parent=self)
        for s in Aperature:
            self.ap_box.addItem(str(s))
        self.ap_box.currentTextChanged.connect(self.ap_changed)

        self.ss_box = QComboBox(parent=self)
        for s in ShutterSpeed:
            self.ss_box.addItem(str(s))
        self.ss_box.currentTextChanged.connect(self.ss_changed)

        self.start_preview_button.clicked.connect(self.start_preview_clicked)
        self.stop_preview_button.clicked.connect(self.stop_preview_clicked)
        self.take_picture_button.clicked.connect(self.take_picture_clicked)
        self.config_button.clicked.connect(self.config_clicked)
        self.zoom_box.valueChanged.connect(self.zoom_value_changed)

        low_zoom, high_zoom = self.camera.config.get_zoom_range()
        self.zoom_box.setMinimum(low_zoom)
        self.zoom_box.setMaximum(high_zoom)

        self.label = QLabel(self)
        pixmap = QPixmap(800, 800)
        self.label.setPixmap(pixmap)
        layout = QVBoxLayout(self)

        layout.addWidget(self.label)
        layout.addWidget(self.start_preview_button)
        layout.addWidget(self.stop_preview_button)
        layout.addWidget(self.take_picture_button)
        layout.addWidget(self.config_button)

        config_layout = QHBoxLayout()
        config_layout.addWidget(self.zoom_label)
        config_layout.addWidget(self.zoom_box)
        config_layout.addWidget(self.iso_box)
        config_layout.addWidget(self.wb_box)
        config_layout.addWidget(self.ap_box)

        layout.addLayout(config_layout)
        self.setLayout(layout)

        self.show()

    def ss_changed(self, new_text):
        self.camera.config.set_shutter_speed(ShutterSpeed[new_text])

    def wb_changed(self, new_text):
        self.camera.config.set_white_balance(WhiteBalance[new_text])

    def ap_changed(self, new_text):
        self.camera.config.set_aperture(Aperature[new_text])

    def iso_changed(self, new_text):
        self.camera.config.set_iso(Iso[new_text])

    def config_clicked(self):
        self.camera.config.print_config()

    def zoom_value_changed(self):
        self.camera.config.set_zoom(self.zoom_box.value())

    def start_preview_clicked(self):
        self.camera.start_preview()

    def stop_preview_clicked(self):
        self.camera.stop_preview()

    def take_picture_clicked(self):
        self.camera.take_picture()

    def preview_received(self, pixmap: QPixmap):
        self.label.setPixmap(pixmap.scaledToWidth(800))

    def picture_received(self):
        pixmap = QPixmap("/home/pi/ramdisk/image.bmp")
        self.label.setPixmap(pixmap.scaledToWidth(800))


if __name__ == '__main__':
    app = QApplication(sys.argv)
    ex = App()
    app.exec()

