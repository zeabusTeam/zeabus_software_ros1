#!/usr/bin/python2.7
import rospy
import cv2 as cv
import numpy as np
from scipy import stats
from sensor_msgs.msg import CompressedImage
from dynamic_reconfigure.client import Client as Client
from vision_lib import print_result
import color_text as ctxt


class AutoExposure:
    def __init__(self, sub_topic, client_name, exposure_value_default=0, exposure_value_min=0):
        print_result("Init auto exposure topic" + sub_topic,color=ctxt.GREEN)
        self.image = None
        self.Bright = None
        self.sub_sampling = 1
        self.sub_topic = sub_topic
        self.client_name = client_name
        self.exposure_value_default = exposure_value_default
        self.exposure_value_min = exposure_value_min
        self.sub_image = rospy.Subscriber(
            sub_topic, CompressedImage, self.img_callback,  queue_size=10)
        self.client = Client(self.client_name)

    def img_callback(self, msg):
        arr = np.fromstring(msg.data, np.uint8)
        self.image = cv.resize(cv.imdecode(arr, 1), (0, 0),
                               fx=self.sub_sampling, fy=self.sub_sampling)
        b, g, r = cv.split(self.image)
        self.Bright = ((0.299*(r**2) + 0.587*(g**2) + 0.114*(b**2))**0.5)

    def set_param(self, param, value):
        value = max(value, 0)
        params = {str(param): value}
        config = self.client.update_configuration(params)

    def get_mode(self, data):
        data = data.ravel()

    def get_param(self, param):
        value = rospy.get_param("/" + str(self.client_name) + str(param), None)
        return value
