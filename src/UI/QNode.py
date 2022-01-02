#!/usr/bin/python

'''
Description: 
Author: xielock
Date: 2021-06-17 18:54:06
LastEditors: xie
LastEditTime: 2022-01-02 20:10:46
'''

import sys
import cv2
import numpy as np
from PyQt5.QtCore import QThread, QStringListModel, QModelIndex, pyqtSignal
from PyQt5.QtWidgets import QListView

'''
@description: emit signal 
@param {*}
@return {*}
'''
class QNodeL(QThread): 
    updateLog = pyqtSignal() ## create a signal to emit when the listview updates
    updateLog2 = pyqtSignal()
    def __init__(self, text):
        QThread.__init__(self)
        self.text_window = text
        self.model = QStringListModel(self)
        self.list = []
        self.model.setStringList(self.list)
        self.text_window.setModel(self.model)


