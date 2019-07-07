import os
import sys
from enum import Enum

from PyQt5.QtWidgets import QApplication, QWidget, QLabel, QPushButton, QVBoxLayout, QSpinBox, QHBoxLayout, QComboBox, \
    QGraphicsView
from PyQt5.QtGui import QIcon, QPixmap, QFont, QPainter, QColor, QPalette, QImage
from PyQt5.QtCore import Qt, QTimer

from Camera import Camera
from gpio import Buttons

class AppState(Enum):
    PRVIEW_SHOWING = 0
    PICTURE_SHOWING = 1
    WAITING_FOR_PICTURE = 2
    PRINTING = 3
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

        palette = QPalette()
        palette.setColor(QPalette.Background, QColor("black"))
        self.setAutoFillBackground(True)
        self.setPalette(palette)

        self.label = QLabel(self)
        self.label.setAlignment(Qt.AlignHCenter)
        layout = QVBoxLayout(self)
        #layout.setAlignment(Qt.AlignHCenter)
        layout.addWidget(self.label)

        buttonFont = QFont( "Arial")
        buttonFont.setBold(True)
        buttonFont.setPointSize(30)

        buttonLayout = QHBoxLayout()
        buttonLayout.setAlignment(Qt.AlignHCenter)
        buttonLayout.setSpacing(200)


        self.takePicButton = QLabel("Take Picture")
        self.takePicButton.setFont(buttonFont)
        self.takePicButton.setStyleSheet("QLabel { background-color : black; color : white; }")
        buttonLayout.addWidget(self.takePicButton)

        self.printAndDelButton = QLabel("Print & Delete Picture")
        self.printAndDelButton.setFont(buttonFont)
        self.printAndDelButton.setStyleSheet("QLabel { background-color : black; color : white; }")


        buttonLayout.addWidget(self.printAndDelButton)

        self.deleteButton = QLabel("Delete Picture")
        self.deleteButton.setFont(buttonFont)
        self.deleteButton.setStyleSheet("QLabel { background-color : black; color : white; }")
        buttonLayout.addWidget(self.deleteButton)

        layout.addLayout(buttonLayout)
        self.setLayout(layout)
        self.state = AppState.INIT
        self.showFullScreen()
        self.imageHeight = 900

        self.buttons = Buttons()
        self.buttons.button_pressed.connect(self.button_pressed)
        self.buttons.start()

        #the pi is slow, wait for ui to show before initing the camera
        QTimer.singleShot(2000, self.delayed_init) 

        
    def delayed_init(self):
        #self.imageHeight = QApplication.desktop().screenGeometry().height() - self.deleteButton.height() - 70
        print(self.imageHeight)

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
        print("start preview")
        if self.state == AppState.INIT or self.state == AppState.PICTURE_SHOWING or AppState.PRINTING:
            self.camera.start_preview()
            self.deleteTimer.stop()
            self.takePicButton.setStyleSheet("QLabel { background-color : black; color : white; }")
            self.printAndDelButton.setStyleSheet("QLabel { background-color : black; color : black; }")
            self.deleteButton.setStyleSheet("QLabel { background-color : black; color : black; }")
            self.state = AppState.PRVIEW_SHOWING
        else:
            print("start Preview, illegal state", self.state)

    def print_pressed(self):
        if self.state == AppState.PICTURE_SHOWING:
            self.deleteTimer.stop()
            self.show_text("Printing... (~60 Seconds)")
            self.state = AppState.PRINTING
            QTimer.singleShot(100, self.start_printing) 
            QTimer.singleShot(60000, self.start_preview)
        else:
            print("print_pressed illegal state")

    def start_printing(self):
        os.system("/home/pi/kabine/aspectpad -a 1.48 -p white /home/pi/ramdisk/image.bmp /home/pi/ramdisk/image_conv.bmp")
        os.system("lp /home/pi/ramdisk/image_conv.bmp")        


    def delete_pressed(self):
        if self.state == AppState.PICTURE_SHOWING:
            self.deleteTimer.stop()
            self.start_preview()

    def show_text(self, text):
        pixmap = QPixmap(self.label.pixmap().width(), self.imageHeight)
        scaled = pixmap.scaledToHeight(self.imageHeight)
        painter = QPainter(scaled)
        painter.fillRect(0, 0, self.label.pixmap().width(), self.imageHeight, QColor("black"))
        painter.setPen(QColor("white"))
        font = QFont("Arial")
        font.setPointSize(50)
        painter.setFont(font)
        painter.drawText(200, 400, text)
        painter.end()
        self.label.setPixmap(scaled)
        self.takePicButton.setStyleSheet("QLabel { background-color : black; color : black; }")
        self.printAndDelButton.setStyleSheet("QLabel { background-color : black; color : black; }")
        self.deleteButton.setStyleSheet("QLabel { background-color : black; color : black; }")


    def clear_picture(self):
        self.show_text("Taking Picture...")


    def take_picture_pressed(self):
        self.state = AppState.WAITING_FOR_PICTURE
        self.deleteTimer.stop()
        self.camera.stop_preview()
        self.clear_picture()
        self.camera.take_picture()

    def picture_received(self):
        if self.state == AppState.WAITING_FOR_PICTURE:
            pixmap = QPixmap("/home/pi/ramdisk/image.bmp")
            print(pixmap.size())
            print(self.imageHeight)
            scaled = pixmap.scaledToHeight(self.imageHeight)
            self.label.setPixmap(scaled)
            self.received_picture = QPixmap(pixmap)
            self.printAndDelButton.setStyleSheet("QLabel { background-color : black; color : white; }")
            self.deleteButton.setStyleSheet("QLabel { background-color : black; color : white; }")
            self.start_delete_timer()
            self.state = AppState.PICTURE_SHOWING

    def preview_received(self, pixmap: QPixmap):
        if self.state == AppState.PRVIEW_SHOWING:
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

    def button_pressed(self, button_number):
        print("button: " + str(button_number))
        if button_number == 8:
            self.print_pressed()
        elif button_number == 12:
            self.delete_pressed()
        elif button_number == 16:
            self.take_picture_pressed()
        else:
            print("unknown button number")
        
if __name__ == '__main__':
    app = QApplication(sys.argv)
    ex = App()
    app.exec()          
