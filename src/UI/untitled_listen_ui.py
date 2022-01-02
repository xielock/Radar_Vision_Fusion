 # -*- coding: utf-8 -*

'''
Description: 
Author: xielock
Date: 2021-06-17 18:54:14
LastEditors: xie
LastEditTime: 2021-07-13 09:44:03
'''

import os
import sys
import rospy
import thread
from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtGui import QPixmap
from cv_bridge import CvBridge, CvBridgeError
from sensor_msgs.msg import Image
from PyQt5.QtCore import QThread, QStringListModel, QModelIndex, pyqtSignal
from PyQt5 .QtWidgets import QApplication , QMainWindow, QGroupBox,QCheckBox,QHBoxLayout,QToolButton
from PyQt5.QtWidgets import QListView, QMenu, QAction, QFileDialog
from PyQt5.QtGui import QImage, QIcon,QDesktopServices,QFont
from PyQt5.QtCore import pyqtSlot, QUrl, QTimer, QDateTime
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from PyQt5.QtWidgets import *
from QLabel_override import BtnLabel
import cv2
import numpy as np
from untitled_listen import QNodeL
from mymsg.msg import CameraRadarTimeFusion
from mymsg.msg import CameraDataPerSec
from mymsg.msg import RadarDataPerSec
from mymsg.msg import RadarData
from line_recog.msg import origin_and_result
from xie.msg import origin_and_result_yolo
import mymsg.msg
  

class Ui_MainWindow1(object):
    #signal must be the varible of the class , not the funciton
    #updateLog = pyqtSignal()
    def setupUi(self, MainWindow):
        MainWindow.setObjectName("MainWindow1")
        MainWindow.resize(1880, 1000)
        self.centralwidget = QtWidgets.QWidget(MainWindow)
        self.centralwidget.setObjectName("centralwidget")
      
       ###groupBox 组件
        self.groupBox1 = QtWidgets.QGroupBox(self.centralwidget)
        self.groupBox1.setTitle(" ")
        self.groupBox1.setObjectName("groupBox1")
        self.groupBox1.setGeometry(QtCore.QRect(1710,255, 110, 50))
        # self.groupBox1.setStyleSheet('QGroupBox:title {color: rgb(120, 230, 153);}')
        self.groupBox1.setStyleSheet("border:1px solid gray") 
        self.groupBox1.setAlignment(QtCore.Qt.AlignLeft)
        self.groupBox1.setAlignment(QtCore.Qt.AlignVCenter)
        self.checkBox1 = QCheckBox("&天气")
        self.checkBox1.setChecked(False)
        #self.checkBox2 = QCheckBox("&背景2")
        #self.checkBox2.setChecked(False)
        # self.checkBox3 = QCheckBox("&背景3")
        # self.checkBox3.setChecked(False)
        layout = QHBoxLayout()
        #控件添加到水平布局中
        layout.addWidget(self.checkBox1)
        # layout.addWidget(self.checkBox2)
        # layout.addWidget(self.checkBox3)
        self.groupBox1.setLayout(layout)
        #添加动作链接
        self.checkBox1.stateChanged.connect(lambda: self.btnstate(self.checkBox1))
        # self.checkBox2.stateChanged.connect(lambda: self.btnstate(self.checkBox2))
        # self.checkBox3.stateChanged.connect(lambda: self.btnstate(self.checkBox3))

        self.label_c = QtWidgets.QLabel(self.centralwidget)
        self.label_c.setGeometry(QtCore.QRect(50, 105, 170, 50))
        self.label_c.setObjectName("label")
        #self.label_c.setFrameShape(QtWidgets.QFrame.Box)
        self.label_c.setAlignment(Qt.AlignCenter)
	self.label_c.setFont(QFont("Roman times",20))
	self.label_c.setText(" 当前模式：" )

        self.comboBox = QtWidgets.QComboBox(self.centralwidget)
        self.comboBox.setGeometry(QtCore.QRect(240, 100, 190, 60))
        self.comboBox.setObjectName("comboBox")
        self.comboBox.setStyleSheet('''QComboBox::down-arrow {image: url(./2.png);padding-left:15px;}
                QComboBox QAbstractItemView::item { min-height: 50px; }
                QComboBox::drop-down {border:0px solid gray;width: 40px;}
                QComboBox{border:2px solid white;font-size:30px;}
                ''')
        self.comboBox.addItem("")
        self.comboBox.addItem("")
        self.comboBox.addItem("")
        self.comboBox.setItemText(0, "作物行检测")
        self.comboBox.setItemText(1, "田埂检测")
        self.comboBox.setItemText(2, "障碍物检测")
        self.comboBox.currentIndexChanged.connect(self.selectionchange)
        #以下几个flag控制软件在不同的工作模式 flag1---camera   flag2---line   flag3---ridge  flag4---obstacle
        self.flag_1 = True
        self.flag_2 = False
        self.flag_3 = False



        ###QToolButton组件
    #     self.tb = QToolButton(self)
    #     self.tb.setGeometry(QtCore.QRect(0, 85, 100,30))
    #     self.tb.setToolButtonStyle(QtCore.Qt.ToolButtonTextBesideIcon)
    #     self.tb.setToolTip('测试')
    #     self.tb.setPopupMode(QToolButton.InstantPopup)
    #     self.tb.setText('测试按钮')
    #     self.tb.setStyleSheet( "QToolButton{background-color:rgb(78,255,255)}")
    #     #tb.setIcon(QIcon('icon/bank.ico'))
    #     self.tb.setAutoRaise(True)
    #     menu = QMenu(self)
    #     self.alipayAct = QAction(QIcon('icon/alipay.ico'),'支付宝支付', self)
    #     self.wechatAct = QAction(QIcon('icon/wechat.ico'),'微信支付', self)
    #     self.visaAct = QAction(QIcon('icon/visa.ico'),'Visa卡支付', self)
    #     self.master_cardAct = QAction(QIcon('icon/master_card.ico'),'作者主页', self)
    #     menu.addAction(self.alipayAct)
    #     menu.addAction(self.wechatAct)
    #     menu.addSeparator()
    #     menu.addAction(self.visaAct)
    #     menu.addAction(self.master_cardAct)
    #     self.tb.setMenu(menu)
    #     self.alipayAct.triggered.connect(self.on_click)
    #     self.wechatAct.triggered.connect(self.on_click)
    #     self.visaAct.triggered.connect(self.on_click)
    #     self.master_cardAct.triggered.connect(self.on_click)

    #     self.tb2 = QToolButton(self)
    #     self.tb2.setToolButtonStyle(QtCore.Qt.ToolButtonTextBesideIcon)
    #     self.tb2.setGeometry(QtCore.QRect(100,85, 100, 30))
    #     self.tb2.setStyleSheet( "QToolButton{background-color:rgb(78,255,255)}")
    #     # self.tb2.setToolTip('选择适合你的支付方式')
    #     self.tb2.setText('帮助')
    #     self.tb2.setAutoRaise(True)
    #     self.tb2.setPopupMode(QToolButton.InstantPopup)
    #     self.help1 = QAction(QIcon('icon/alipay.ico'),'帮助1', self)
    #     self.help2 = QAction(QIcon('icon/wechat.ico'),'帮助2', self)
    #     menu2 = QMenu(self)
    #     menu2.addAction(self.help1)
    #     menu2.addAction(self.help2)
    #     self.tb2.setMenu(menu2)

    #     self.tb3 = QToolButton(self)
    #     self.tb3.setToolButtonStyle(QtCore.Qt.ToolButtonTextBesideIcon)
    #     self.tb3.setGeometry(QtCore.QRect(200,85, 100, 30))
    #     self.tb3.setStyleSheet( "QToolButton{background-color:rgb(78,255,255)}")
    #     # self.tb2.setToolTip('选择适合你的支付方式')
    #     self.tb3.setText('文件')
    #     self.tb3.setAutoRaise(True)
    #     self.tb3.setPopupMode(QToolButton.InstantPopup)
    #     self.file1 = QAction(QIcon('icon/alipay.ico'),'导入文件', self)
    #     self.file2 = QAction(QIcon('icon/wechat.ico'),'显示文件内容', self)
    #     menu3 = QMenu(self)
    #     menu3.addAction(self.file1)
    #     menu3.addAction(self.file2)
    #     self.tb3.setMenu(menu3)
    #     self.file1.triggered.connect(self.file_click)
    #     self.file2.triggered.connect(self.file_click2)
    # '''
    # @description:   下面是一些点击和接受的反应函数
    # @param {*} self
    # @return {*}
    # '''
    # def file_click(self):
    #     #self.txt = QFileDialog.getOpenFileName(self,'打开文件',"~","All Files (*);;Text Files (*.txt)")
    #     self.file_path = QFileDialog.getOpenFileName(self,'打开文件',"~","Text Files (*.txt)")
    #     print(self.file_path)
    #     if self.file_path[0] =='':
    #         return 

    #     self.f = open(self.file_path[0], "r")
    #     self.txt = self.f.read()
    #     self.f.close()
    #     self.label.setText(self.txt)
                
    # def file_click2(self):
    #     self.label.setText(self.txt)   
    # '''
    # @description: toolbutton1   的checkbutton的action反应函数
    # @param {*} self
    # @return {*}
    # '''        
    # def on_click(self):
    #     if self.sender() == self.alipayAct:
    #         QDesktopServices.openUrl(QUrl('https://www.alipay.com/'))
    #     elif self.sender() == self.wechatAct:
    #         QDesktopServices.openUrl(QUrl('https://pay.weixin.qq.com/index.php'))
    #     elif self.sender() == self.visaAct:
    #         QDesktopServices.openUrl(QUrl('https://www.visa.com.cn/'))
    #     else:
    #         QDesktopServices.openUrl(QUrl('http://xielock.com'))




        ###Qlabel组件       label3重载了Qlabel 能完成鼠标回调函数事件
        self.label = QtWidgets.QLabel(self.centralwidget)
        self.label.setGeometry(QtCore.QRect(50, 210, 800, 600))
        self.label.setObjectName("label")
        self.label.setFrameShape(QtWidgets.QFrame.Box)
        self.label.setAlignment(Qt.AlignCenter)

        #self.label.setFrameShadow(QtWidgets.QFrame.Raised)
        #self.label.setAlignment(QtCore.Qt.AlignVCenter)
        self.label2 = QtWidgets.QLabel(self.centralwidget)
        self.label2.setGeometry(QtCore.QRect(870, 210, 800, 600))
        self.label2.setObjectName("label2")
        self.label2.setAlignment(Qt.AlignCenter)
        self.label2.setFrameShape(QtWidgets.QFrame.Box)
        # self.label3 = BtnLabel(self.centralwidget)
        # self.label3.setGeometry(QtCore.QRect(930, 50, 800, 600))
        # self.label3.setFrameShape(QtWidgets.QFrame.Box)
        # self.label3.setObjectName("label3")
        self.label4 = QtWidgets.QLabel(self.centralwidget)
        self.label4.setGeometry(QtCore.QRect(50, 180, 100, 24))
        self.label4.setObjectName("label4")
        self.label4.setFont(QFont("Arial ",14))
        self.label4_1 = QtWidgets.QLabel(self.centralwidget)
        self.label4_1.setGeometry(QtCore.QRect(870, 180, 100, 24))
        self.label4_1.setObjectName("label4_1")
        self.label4_1.setFont(QFont("Arial ",14))
        # self.label5 = QtWidgets.QLabel(self.centralwidget)
        # self.label5.setGeometry(QtCore.QRect(380, 730, 200,70))
        # self.label5.setObjectName("label5")
        self.label6 = QtWidgets.QLabel(self.centralwidget)
        self.label6.setGeometry(QtCore.QRect(0, 0, 320,80))
        self.label6.setObjectName("label6")


        #background
        self.label8 = QtWidgets.QLabel(self.centralwidget)
        self.label8.setGeometry(QtCore.QRect(0, 860, 1920,120))
        self.label8.setObjectName("label8")
        self.label8.setFrameShape(QtWidgets.QFrame.Box)
        self.label8.setFrameShadow(QtWidgets.QFrame.Plain)
        self.label8.setStyleSheet('border-width: 1px;border-style: solid;border-color: rgb(255, 170, 0);background-color: rgb(33, 153, 255);')
        self.label8.setText("  信\n  息\n  栏")
       
        self.label9 = QtWidgets.QLabel(self.centralwidget)
        self.label9.setGeometry(QtCore.QRect(1680, 210, 147,600))
        self.label9.setObjectName("label9")
        self.label9.setFrameShape(QtWidgets.QFrame.Box)
        self.label9.setFrameShadow(QtWidgets.QFrame.Plain)


        self.timelabel = QtWidgets.QLabel(self.centralwidget)
        self.timelabel.setGeometry(QtCore.QRect(1600, 880, 200, 70))
        self.timelabel.setObjectName("timelabel")
        self.label7 = QtWidgets.QLabel(self.centralwidget)
        self.label7.setGeometry(QtCore.QRect(1250, 904, 250,40))
        self.label7.setObjectName("label7")
        #self.label7.lower()
        self.groupBox1.raise_()


        self.label_info1 = QtWidgets.QLabel(self.centralwidget)
        self.label_info1.setGeometry(QtCore.QRect(1480, 120, 120,40))
        self.label_info1.setObjectName("label_info1")
        self.label_info2 = QtWidgets.QLabel(self.centralwidget)
        self.label_info2.setGeometry(QtCore.QRect(1480, 190, 120,40))
        self.label_info2.setObjectName("label_info2")
        self.label_info3 = QtWidgets.QLabel(self.centralwidget)
        self.label_info3.setGeometry(QtCore.QRect(1480, 260, 120,40))
        self.label_info3.setObjectName("label_info3")
        self.label_info4 = QtWidgets.QLabel(self.centralwidget)
        self.label_info4.setGeometry(QtCore.QRect(1480, 330, 120,40))
        self.label_info4.setObjectName("label_info4")


        self.label_info5 = QtWidgets.QLabel(self.centralwidget)
        self.label_info5.setGeometry(QtCore.QRect(1480, 600, 120,40))
        self.label_info5.setObjectName("label_info5")
        self.label_info6 = QtWidgets.QLabel(self.centralwidget)
        self.label_info6.setGeometry(QtCore.QRect(1480, 670, 120,40))
        self.label_info6.setObjectName("label_info6")
        self.label_info7 = QtWidgets.QLabel(self.centralwidget)
        self.label_info7.setGeometry(QtCore.QRect(1480, 740, 120,40))
        self.label_info7.setObjectName("label_info7")
        self.label_info8 = QtWidgets.QLabel(self.centralwidget)
        self.label_info8.setGeometry(QtCore.QRect(1480, 810, 120,40))
        self.label_info8.setObjectName("label_info8")

        self.label_title = QtWidgets.QLabel(self.centralwidget)
        self.label_title.setGeometry(QtCore.QRect(550, 10, 1000,80))
        self.label_title.setObjectName("label_title")
        self.label_title.setFont(QFont("NSimSun",42,QFont.Bold))
        self.label_title.setText("农业机器人农田环境感知软件-v1.0")

        self.timer = QTimer(self)
        self.timer.timeout.connect(self.showtime)
        self.timer.start()

        ###logview组件
        self.logView = QtWidgets.QListView(self.centralwidget)
        self.logView.setGeometry(QtCore.QRect(50, 861, 800,109))
        self.logView.setObjectName("logView")
        self.logView.setStyleSheet('''QListView::down-arrow {image: url(./1.png);padding-left:15px;}
                QListView QAbstractItemView::item { min-height: 34px; }
                QListView::drop-down {border:0px solid blue;width: 40px;}
                QListView{  background-color:   rgb(23, 145, 255);}
                QListView{border:2px solid white;font-size:18px;}
                ''')

        ###pushbutton组件
        self.pushButton = QtWidgets.QPushButton(self.centralwidget)
        self.pushButton.setGeometry(QtCore.QRect(1700, 450, 120, 40))
        self.pushButton.setObjectName("pushButton")
        self.pushButton.setStyleSheet("QPushButton{font-size:20px}")
        self.setsheet(self.pushButton)

	#close
        self.pushButton_2 = QtWidgets.QPushButton(self.centralwidget)
        self.pushButton_2.setGeometry(QtCore.QRect(1700, 20, 120, 40))
        self.pushButton_2.setObjectName("pushButton2")
        self.setsheet(self.pushButton_2)

        # self.pushButton_3 = QtWidgets.QPushButton(self.centralwidget)
        # self.pushButton_3.setGeometry(QtCore.QRect(1600, 670, 120, 40))
        # self.pushButton_3.setObjectName("pushButton3")
        # self.setsheet(self.pushButton_3)
        
        # self.pushButton_8 = QtWidgets.QPushButton(self.centralwidget)
        # self.pushButton_8.setGeometry(QtCore.QRect(1600, 740, 120, 40))
        # self.pushButton_8.setObjectName("pushButton8")
        # self.setsheet(self.pushButton_8)

        # self.pushButton_9 = QtWidgets.QPushButton(self.centralwidget)
        # self.pushButton_9.setGeometry(QtCore.QRect(1600, 810, 120, 40))
        # self.pushButton_9.setObjectName("pushButton9")
        # self.setsheet(self.pushButton_9)
        
        self.pushButton_4 = QtWidgets.QPushButton(self.centralwidget)
        self.pushButton_4.setGeometry(QtCore.QRect(1700, 350, 120, 40))
        self.pushButton_4.setObjectName("pushButton4")
        self.pushButton_4.setStyleSheet("QPushButton{font-size:20px}")
        self.setsheet(self.pushButton_4)

        # self.pushButton_5 = QtWidgets.QPushButton(self.centralwidget)
        # self.pushButton_5.setGeometry(QtCore.QRect(1600, 190, 120, 40))
        # self.pushButton_5.setStyleSheet("QPushButton{font-size:18px}")
        # self.pushButton_5.setObjectName("pushButton5")
        # self.pushButton_6 = QtWidgets.QPushButton(self.centralwidget)
        # self.pushButton_6.setGeometry(QtCore.QRect(1600, 260, 120, 40))
        # self.pushButton_6.setStyleSheet("QPushButton{font-size:18px}")
        # self.pushButton_6.setObjectName("pushButton6")
        # self.pushButton_7 = QtWidgets.QPushButton(self.centralwidget)
        # self.pushButton_7.setGeometry(QtCore.QRect(1600, 330, 120,40))
        # self.pushButton_7.setStyleSheet("QPushButton{font-size:18px}")
        # self.pushButton_7.setObjectName("pushButton7")
        #stop button
        self.pushButton_10 = QtWidgets.QPushButton(self.centralwidget)
        self.pushButton_10.setGeometry(QtCore.QRect(1700, 600, 100, 100))
        self.pushButton_10.setObjectName("pushButton10")
        self.pushButton_10.setStyleSheet("QPushButton{color:black}"
                                  "QPushButton:hover{color:white}"
                                  "QPushButton{background-color:rgb(235,25,25)}"
                                  "QPushButton{border:2px}"
                                  "QPushButton{border-radius:30px}"
				  "QPushButton{font-size:24px}"
                                  "QPushButton{padding:2px 4px}")
    

        MainWindow.setCentralWidget(self.centralwidget)
        self.menubar = QtWidgets.QMenuBar(MainWindow)
        self.menubar.setGeometry(QtCore.QRect(0, 0, 800, 28))
        self.menubar.setObjectName("menubar")
        MainWindow.setMenuBar(self.menubar)
        self.statusbar = QtWidgets.QStatusBar(MainWindow)
        self.statusbar.setObjectName("statusbar")
        MainWindow.setStatusBar(self.statusbar)


        ###connect
        self.retranslateUi(MainWindow)
        self.pushButton.clicked.connect(self.sub)
        self.pushButton_2.clicked.connect(self.close)
        # self.pushButton_3.clicked.connect(self.sub2)
        self.pushButton_4.clicked.connect(self.pub1)
        # self.pushButton_5.clicked.connect(self.pub2)
        # self.pushButton_6.clicked.connect(self.pub3)
        # self.pushButton_7.clicked.connect(self.pub4)
        # self.pushButton_8.clicked.connect(self.sub3)
        # self.pushButton_9.clicked.connect(self.sub4)
        self.pushButton_10.clicked.connect(self.stop)
        

        QtCore.QMetaObject.connectSlotsByName(MainWindow)        



        self.flag1 = False
        self.flag2 = False
        self.pic = QPixmap("1.png")
        self.label6.setPixmap(self.pic)
        self.label6.setScaledContents (True)  # 让图片自适应label大小
        self.node = QNodeL(self.logView)
        # bind the logview with the model
        self.model = QStringListModel(self)
        self.list = []
        self.model.setStringList(self.list)
        self.logView.setModel(self.model)
        self.node.updateLog.connect(self.updateLogView)
        self.node.updateLog2.connect(self.updateLogView2)
        #self.label3.mouse.connect(self.mouse_action)


    '''
    @description:  将相机图像显示在label中
    @param {*} self
    @param {*} data
    @return {*}
    '''
    # def action(self, data):
        # if data is not None:
        #     #cv2.imshow("ok", data)
        #     height, width, depth =data.shape
        #     q_img = cv2.cvtColor(data, cv2.COLOR_BGR2RGB)
        #     q_img = QImage(q_img.data, width, height, width*depth, QImage.Format_RGB888)
        #     qpix_img = QPixmap.fromImage(q_img)
        #     #label.setPixmap(qpix_img)
        #     print("ok")
    '''
    @description: camera订阅节点开关
    @param {*} self
    @return {*}
    '''    
    #########################################################################
    def sub(self):
            self.pushButton.setEnabled(False)
            self.pushButton.setStyleSheet("QPushButton{color:gray}"
                                                "QPushButton{border:2px}"
                                    "QPushButton{padding:2px 4px}")

            if self.flag_1:
                self.node_init()
                # 订阅 相机图像
                #thread.start_new_thread(self.run1, ())
                thread.start_new_thread(self.run1, ())
                # 订阅处理完成的图像

            if self.flag_2:
                self.node_init()
                # 订阅相机
                self.run2_()
                # 订阅田埂处理完的结果
                #self.run2_1()
            if self.flag_3:
                self.node_init()
                # 订阅原始障碍物图
                self.run3()
                # 订阅结果
                #self.run3_1()

    def sub2(self):
            #self.pushButton_2.setDisabled(True)
            self.pushButton_3.setStyleSheet("QPushButton{color:gray}"
                                              "QPushButton{border:2px}"
                                  "QPushButton{padding:2px 4px}")
            self.node_init()
            #self.run2()

    '''
    @description: 更新log信息窗口
    @param {*}
    @return {*}
    '''    
    @pyqtSlot()
    def updateLogView(self):
        #print(self.frame_id)
        #self.list.append(str(self.frame_id)+"  "+ str(self.time_secs)+"." +str(self.time_nsecs))
        #self.list.append("ffuck")
        self.model.setStringList(self.list2)
        self.logView.setModel(self.model)
        self.logView.scrollToBottom()


    @pyqtSlot()
    def updateLogView2(self):
        #print(self.frame_id)
        self.list.append(str(self.id)+"  （"+ str(self.loc_center_x)+"," +str(self.loc_center_y) +")")
        #self.list.append("ffuck")
        self.model.setStringList(self.list)
        self.logView.setModel(self.model)
        self.logView.scrollToBottom()
    '''
    @description: 鼠标点击的槽函数
    @param {*}
    @return {*}
    '''    
    @pyqtSlot()
    def mouse_action(self):
        print ("xielock")
        if self.flag1:
            self.cv_img_copy = self.cv_img
            cv2.circle(self.cv_img_copy,self.label3.location_return(), 2, (255,255,0),2)
            height, width, depth =self.cv_img.shape
            q_img = cv2.cvtColor(self.cv_img_copy, cv2.COLOR_BGR2RGB)
            q_img = QImage(q_img.data, width, height, width*depth, QImage.Format_RGB888)
            qpix_img = QPixmap.fromImage(q_img)
            self.label3.setPixmap(qpix_img)
            print("assert here" ,  self.label3.location_return())



    '''
    @description: 给pyqt窗口加上描述
    @param {*} self
    @param {*} MainWindow
    @return {*}
    '''    
    def retranslateUi(self, MainWindow):
        _translate = QtCore.QCoreApplication.translate
        MainWindow.setWindowTitle(_translate("MainWindow", " "))
        #MainWindow.setWindowOpacity(0.5)
        #MainWindow.setAttribute(QtCore.Qt.WA_TranslucentBackground)
        #self.label.setText(_translate("MainWindow", "TextLabel"))
        # self.label3.setText(_translate("MainWindow", " test_mouse"))
        self.label4.setText(_translate("MainWindow","原始图像"))
        self.label4_1.setText(_translate("MainWindow","检测结果"))
        #self.label5.setText(_translate("MainWindow", " log 信息栏"))
        # self.label_info1.setText(_translate("MainWindow", " 相机发布"))
        # self.label_info2.setText(_translate("MainWindow", " 作物行检测"))
        # self.label_info3.setText(_translate("MainWindow", " 田埂检测"))
        # self.label_info4.setText(_translate("MainWindow", " 障碍物检测"))
        # self.label_info5.setText(_translate("MainWindow", " 相机订阅"))
        # self.label_info6.setText(_translate("MainWindow", " 作物行订阅"))
        # self.label_info7.setText(_translate("MainWindow", " 田埂订阅"))
        # self.label_info8.setText(_translate("MainWindow", " 障碍物订阅"))
        self.pushButton.setText(_translate("MainWindow", "确认"))
        self.pushButton_2.setText(_translate("MainWindow", "退出"))
        # self.pushButton_3.setText(_translate("MainWindow", "subscribe2"))
        self.pushButton_4.setText(_translate("MainWindow", "开始"))
        # self.pushButton_5.setText(_translate("MainWindow", "publish2"))
        # self.pushButton_6.setText(_translate("MainWindow", "publish3"))
        # self.pushButton_7.setText(_translate("MainWindow", "publish4"))
        # self.pushButton_8.setText(_translate("MainWindow", "subscribe3"))
        # self.pushButton_9.setText(_translate("MainWindow", "subscribe4"))
        self.pushButton_10.setText(_translate("MainWindow", "停止"))

    
    '''
    @description: 初始化节点
    @param {*} self
    @return {*}
    '''    
    def node_init(self):
        rospy.init_node('listen', anonymous=True)
        print("here we go")
        #self.bridge = CvBridge()


    def run1(self):
        rospy.Subscriber('/detect_img', origin_and_result, self.callback_camera1)

    def callback_camera1(self, data):
        #print("result")
        self.cv_img1 = np.frombuffer(data.origin_img.data, dtype=np.uint8).reshape(data.origin_img.height, data.origin_img.width, -1)
        self.cv_img2 = np.frombuffer(data.result_img.data, dtype=np.uint8).reshape(data.result_img.height, data.result_img.width, -1)
        if self.cv_img1.shape[0] == 0:
            print("img is empty")
            return
        #self.time_nsecs = data.header.stamp.nsecs
        #self.time_secs = data.header.stamp.secs
        #self.frame_id = data.header.frame_id
        # cv2.imshow("frame",cv_img)
        height, width, depth = self.cv_img1.shape
        q_img = cv2.cvtColor(self.cv_img1, cv2.COLOR_BGR2RGB)
        q_img2 = cv2.cvtColor(self.cv_img2, cv2.COLOR_BGR2RGB)
        q_img = QImage(q_img.data, width, height, width * depth, QImage.Format_RGB888)
        q_img2 = QImage(q_img2.data, width, height, width * depth, QImage.Format_RGB888)
        qpix_img = QPixmap.fromImage(q_img)
        qpix_img2 = QPixmap.fromImage(q_img2)
        self.label.setPixmap(qpix_img)
        self.label2.setPixmap(qpix_img2)

    def run2_(self):
        rospy.Subscriber('/ridge_detection__origin_result', mymsg.msg.origin_and_result, self.callback_camera2_)

    def callback_camera2_(self, data):
        self.cv_img1 = np.frombuffer(data.origin_img.data, dtype=np.uint8).reshape(data.origin_img.height, data.origin_img.width, -1)
        self.cv_img2 = np.frombuffer(data.result_img.data, dtype=np.uint8).reshape(data.result_img.height, data.result_img.width, -1)
        height, width, depth = self.cv_img1.shape
        q_img = cv2.cvtColor(self.cv_img1, cv2.COLOR_BGR2RGB)
        q_img2 = cv2.cvtColor(self.cv_img2, cv2.COLOR_BGR2RGB)
        q_img = QImage(q_img.data, width, height, width * depth, QImage.Format_RGB888)
        q_img2 = QImage(q_img2.data, width, height, width * depth, QImage.Format_RGB888)
        qpix_img = QPixmap.fromImage(q_img)
        qpix_img2 = QPixmap.fromImage(q_img2)
        self.label.setPixmap(qpix_img)
        self.label2.setPixmap(qpix_img2)


    '''
    @description: Space_fusion_radar的订阅函数
    @param {*} self
    @return {*} 返回结果
    '''    
    def run3(self):
        rospy.Subscriber('/yolo_result', origin_and_result_yolo,self.fusionCallback3 )
    def fusionCallback3(self,data):
        self.cv_img1 = np.frombuffer(data.origin_img.data, dtype=np.uint8).reshape(data.origin_img.height, data.origin_img.width, -1)
        self.cv_img2 = np.frombuffer(data.result_img.data, dtype=np.uint8).reshape(data.result_img.height, data.result_img.width, -1)
        self.list2 = []
        for msg in data.name_and_dis:
            temp = ''
            if(msg.type == 0):
                temp = str(msg.id)+ " "+"farm_machine  "
            else:
                temp = str(msg.id)+ " "+"person  "
            temp += "x:" +str( msg.dislong) +" m"
            temp += "   y:" + str(msg.dislat) + " m"
            self.list2.append(temp)
        height, width, depth =self.cv_img1.shape
        q_img1 = cv2.cvtColor(self.cv_img1, cv2.COLOR_BGR2RGB)
        q_img2 = cv2.cvtColor(self.cv_img2, cv2.COLOR_BGR2RGB)
        q_img1 = QImage(q_img1.data, width, height, width*depth, QImage.Format_RGB888)
        q_img2 = QImage(q_img2.data, width, height, width*depth, QImage.Format_RGB888)
        qpix_img1 = QPixmap.fromImage(q_img1)
        qpix_img2 = QPixmap.fromImage(q_img2)
        self.label.setPixmap(qpix_img1)
        self.label2.setPixmap(qpix_img2)
        self.node.updateLog.emit()



    '''
    @description: 时间显示函数
    @param {*} self
    @return {*}
    '''
    def showtime(self):
        datetime = QDateTime.currentDateTime()
        text = datetime.toString()
        self.timelabel.setText("     "+ text)

    '''
    @description:   下面是一些点击和接受的反应函数
    @param {*} self
    @return {*}
    '''
    def file_click(self):
        #self.txt = QFileDialog.getOpenFileName(self,'打开文件',"~","All Files (*);;Text Files (*.txt)")
        self.file_path = QFileDialog.getOpenFileName(self,'打开文件',"~","Text Files (*.txt)")
        print(self.file_path)
        if self.file_path[0] =='':
            return 

        self.f = open(self.file_path[0], "r")
        self.txt = self.f.read()
        self.f.close()
        self.label.setText(self.txt)
                
    def file_click2(self):
        self.label.setText(self.txt)   
    #################################################################################
    def pub1(self):
        self.pushButton_4.setDisabled(True)
        self.pushButton_4.setStyleSheet("QPushButton{color:gray}"
                                              "QPushButton{border:2px}"
                                  "QPushButton{padding:2px 4px}")
        if self.flag_1:
            os.system('../../start.sh')
            print("Line script 调用")
        if self.flag_2:
            os.system('../../ridge_start.sh')
            print("Ridge script 调用")

        if self.flag_3:
            os.system('../../start_1radar_1camera.sh')
            print("Obstacle script 调用")

    # def pub2(self):
    #     #os.system('python  test.py')
    #     os.system('~/yolo_ws/test.sh')
    #     self.pushButton_5.setDisabled(True)
    #
    # def pub3(self):
    #     #os.system('python  test.py')
    #     os.system('~/yolo_ws/test.sh')
    #     self.pushButton_6.setDisabled(True)
    # def pub4(self):
    #     #os.system('python  test.py')
    #     os.system('~/yolo_ws/test.sh')
    #     self.pushButton_7.setDisabled(True)

    def stop(self):
        print("Stop")
        self.pushButton.setDisabled(False)
        self.pushButton_4.setDisabled(False)
        self.setsheet(self.pushButton_4)
        # self.pushButton_3.setDisabled(False)
        # self.pushButton_8.setDisabled(False)
        # self.pushButton_9.setDisabled(False)
        self.setsheet(self.pushButton)
        # self.setsheet(self.pushButton_3)
        # self.setsheet(self.pushButton_8)
        # self.setsheet(self.pushButton_9)
        rospy.signal_shutdown("hh")

    '''
    @description: 这里是qgroupbox的 反应函数 ，对checkbox的状态变化做出反应
    @param {*} self
    @param {*} btn
    @return {*}
    '''        
    def btnstate(self, btn):
        if self.checkBox1.isChecked():
            os.system( "curl wttr.in/Beijing?format=4  > ./data/filename.txt ")
            file = open('./data/filename.txt')
            for each_line in file:
                self.label7.setText(each_line)
        if self.checkBox1.isChecked() == False:
            self.label7.clear()
    def setsheet(self, bnt):
        bnt.setStyleSheet("QPushButton{color:black}"
                                  "QPushButton:hover{color:red}"
                                  "QPushButton{font-size:18px}"
                                  #原来rgb78 255 255
                                  "QPushButton{background-color:rgb(33,153,255)}"
                                  "QPushButton{border:2px}"
                                  #"QPushButton{border-radius:10px}"
                                  "QPushButton{padding:2px 4px}")
    def selectionchange(self):

        text = self.comboBox.currentText()
	a = u"作物行检测"
	b = u'田埂检测'
	c = u'障碍物检测'
	print(text)
        if text.encode("utf8") == a.encode("utf8"):
            self.flag_1 = True
            self.flag_2= False
            self.flag_3 = False
        if text.encode("utf8") == b.encode("utf8"):
            self.flag_1 = False
            self.flag_2= True
            self.flag_3 = False         
        if text.encode("utf8") == c.encode("utf8"):
            self.flag_1 = False
            self.flag_2= False
            self.flag_3 = True                 







