import RPi.GPIO as GPIO
from PyQt5 import QtCore
from PyQt5.QtCore import QThread
from PyQt5.QtWidgets import QApplication
import sys

class Buttons(QThread):
    button_pressed = QtCore.pyqtSignal(int)
    def __init__(self):
        super().__init__()

        self.buttons = [8, 12, 16]

        GPIO.setmode(GPIO.BOARD)
        for i in self.buttons:
            GPIO.setup(i, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
        self.running = True

    def stop(self):
        self.running = False

    def run(self):
        while self.running:
            self.msleep(30)
            for i in self.buttons:
                if (GPIO.input(i) == GPIO.HIGH):
                    print("button: " + str(i))
                    self.button_pressed.emit(i)


if __name__ == '__main__':
    app = QApplication(sys.argv)
    es = Buttons()
    es.start()
    
    app.exec()    