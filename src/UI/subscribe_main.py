 # -*- coding: utf-8 -*

'''
Description: 
Author: xielock
Date: 2021-06-17 18:54:22
LastEditors: xie
LastEditTime: 2021-07-12 12:08:44
'''
import sys
#import untitled
import subscribe_ui
from subscribe_ui import Ui_MainWindow1
from PyQt5.QtWidgets import QApplication , QMainWindow, QWidget
from PyQt5.QtGui import QColor, QPixmap, QIcon, QPalette,QBrush
from PyQt5 import QtCore, QtWidgets
#control two




'''
@description: main window
@param {*}
@return {*}
'''
class MainWindow(QMainWindow, Ui_MainWindow1):
    def __init__(self):
        super(MainWindow, self).__init__()
        self.setupUi(self)


# def main():
#     app = QApplication(sys.argv)
#     main_window = MainWindow()
#     #set the background
#     main_window.setObjectName("MainWindow")
#     main_window.setStyleSheet("#MainWindow{border-image:url(./3.png);}")
#     main_window.show()
#     sys.exit(app.exec_())

'''
@description:  start window
@param {*}
@return {*}
'''
class FirstUi(QWidget):
    def __init__(self):
        super(FirstUi, self).__init__()
        self.resize(1920,1000)
        self.setWindowTitle("xielock")
        self.changeP = QtWidgets.QPushButton(self)
        self.changeP.setGeometry(QtCore.QRect(800,300,300,200))
        self.changeP.setText("开始")
        self.changeP.setStyleSheet("QPushButton{color:black}"
                                  "QPushButton:hover{color:red}"
                                  "QPushButton{font-size:28px}"
                                  "QPushButton{background-color:rgb(78,255,255)}"
                                  "QPushButton{border:2px}"
                                  "QPushButton{border-radius:10px}"
                                  "QPushButton{padding:2px 4px}")

        self.changeP.clicked.connect(self.close)
        self.changeP.clicked.connect(self.open_second_ui)

    def open_second_ui(self):
        self.second_ui = MainWindow()
        self.second_ui.setObjectName("MainWindow")
        self.second_ui.setStyleSheet("#MainWindow{border-image:url(./9.jpg);}")
        self.second_ui.show()
        #sys.exit(app.exec_())
        print("ok")



if __name__ == '__main__':
    app1 = QApplication(sys.argv)
    widget = FirstUi()
    palette = QPalette()
    palette.setBrush(QPalette.Background, QBrush(QPixmap("./9.jpg")))  
    widget.setPalette(palette)

    widget.show()
    sys.exit(app1.exec_())
    


