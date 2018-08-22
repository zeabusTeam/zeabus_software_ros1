#!/usr/bin/python2.7
import rospy
import cv2 as cv
import numpy as np
from sensor_msgs.msg import CompressedImage
from dynamic_reconfigure.client import Client as Client
from vision_lib import *

class AutoExposure:
    def __init__(self,subTopic,clientName):
        print_result("init auto exposure topic" + subTopic)
        self.image = None
        self.gray = None
        self.clientName = clientName
        self.subImage = rospy.Subscriber(subTopic, CompressedImage, self.img_callback,  queue_size=10)
        self.client = Client(self.clientName)
        self.sub_sampling = 1
    
    def img_callback(self, msg):
        arr = np.fromstring(msg.data, np.uint8)
        self.image = cv.resize(cv.imdecode(arr, 1), (0,0),fx=self.sub_sampling,fy=self.sub_sampling)
        self.gray = cv.cvtColor(self.image, cv.COLOR_BGR2GRAY)

    def set_param(self, param, value):
        params = {str(param): value}
        config = self.client.update_configuration(params)


    def get_param(self, param):
        value = rospy.get_param("/" + str(self.clientName) + str(param), None)
        return value