#coding:utf-8
from PyQt5.QtWidgets import QApplication, QLabel, QSlider  
from PyQt5 import QtWidgets  
import PyQt5.QtCore 
from PyQt5.QtCore import Qt, pyqtSignal
from PyQt5.QtGui import QPixmap,  QImage
from PyQt5.QtWidgets import *

from rospy.core import xmlrpcapi

class BtnLabel(QLabel):
    x = 100
    y = 200
    mouse = pyqtSignal()
    def __init__(self,parent=None):  
        super(BtnLabel,self).__init__(parent)  
        self.if_mouse_press = False

    def mouseMoveEvent(self,e):  
        #print ('mouse move:(%d,%d)\n'%(e.pos().x(),e.pos().y()))
        if self.if_mouse_press:
            print("hhhh")
    def mousePressEvent(self,e):  
        #print ('mousePressEvent(%d,%d)\n'%(e.pos().x(),e.pos().y()))
        self.if_mouse_press = True
        global x
        global y
        x = e.pos().x()
        y = e.pos().y()
        print(x, y)
        print(self.x, self.y)
        self.mouse.emit()
      #  print(self.location_return())
       
       
    def location_return(self):
        return x,y

    def mouseReleaseEvent(self,e):  
        #print ('mouseReleaseEvent(%d,%d)\n'%(e.pos().x(),e.pos().y()))
        self.if_mouse_press = False