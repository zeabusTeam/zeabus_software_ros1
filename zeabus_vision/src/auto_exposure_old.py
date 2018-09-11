#!/usr/bin/python2.7
import rospy
import cv2 as cv
import numpy as np
from sensor_msgs.msg import CompressedImage
from dynamic_reconfigure.client import Client as Client
from vision_lib import *
import color_text as ctxt


class AutoExposure:
    def __init__(self, sub_topic, client_name, default_exposure_value=0.1, min_exposure_value=0.1, sub_sampling=1):
        print_result("Init auto exposure topic " + sub_topic, color=ctxt.GREEN)
        self.image = None
        self.hsv = None
        self.sub_sampling = 1
        self.client_name = client_name
        self.min_exposure_value = min_exposure_value
        rospy.Subscriber(sub_topic, CompressedImage,
                         self.img_callback,  queue_size=10)
        self.client = Client(self.client_name)
        self.set_param(value=default_exposure_value)

    def img_callback(self, msg):
        arr = np.fromstring(msg.data, np.uint8)
        self.image = cv.resize(cv.imdecode(arr, 1), (0, 0),
                               fx=self.sub_sampling, fy=self.sub_sampling)
        self.hsv = cv.cvtColor(self.image, cv.COLOR_BGR2HSV)

    def set_param(self, value=0.1, param='exposure'):
        value = max(value, self.min_exposure_value)
        params = {str(param): float(value)}
        self.client.update_configuration(params)

    def get_param(self, param='exposure'):
        value = rospy.get_param("/" + str(self.client_name) + str(param), None)
        return value

    def get_mode(self, data):
        if data is None:
            return None
        values, counts = np.unique(data, return_counts=True)
        mode_index = counts.argmax()
        mode = values[mode_index]
        return mode

    def adjust_exposure_value(self):
        while not rospy.is_shutdown():
            if self.hsv is None:
                img_is_none()
                continue
            h, s, v = cv.split(self.hsv)
            mode = self.get_mode(v.ravel())
            ev = self.get_param()
            print_result('Exposure')
            print_result(ev)
            print_result('mode')
            print_result(mode)
            if ev is None:
                continue
            if mode >= 200:
                ev -= 0.04
            elif mode <= 50:
                ev += 0.04
            self.set_param(value=ev)
            rospy.sleep(0.01)
