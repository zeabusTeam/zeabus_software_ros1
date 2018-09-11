#!/usr/bin/python2.7
import rospy
import cv2 as cv
import numpy as np
from scipy import stats
from sensor_msgs.msg import CompressedImage
from dynamic_reconfigure.client import Client as Client
from vision_lib import *
import color_text as ctxt


class AutoExposure:
    def __init__(self, sub_topic, client_name, default_exposure_value=0.1, min_exposure_value=0.1):
        print_result("Init auto exposure topic " + sub_topic, color=ctxt.GREEN)
        self.image = None
        self.Brightness = None
        self.sub_sampling = 1
        self.sub_topic = sub_topic
        self.client_name = client_name
        self.default_exposure_value = default_exposure_value
        self.min_exposure_value = min_exposure_value
        rospy.Subscriber(sub_topic, CompressedImage, self.img_callback,  queue_size=10)
        print_result("checkpoint",ctxt.CYAN)
        self.client = Client(self.client_name)
        self.set_param(value=default_exposure_value)
        # self.set_param(value=0.1)
        print_result("checkpoint",ctxt.PURPLE)
        print_result("checkpoint",ctxt.BLUE)

    def img_callback(self, msg):
        arr = np.fromstring(msg.data, np.uint8)
        self.image = cv.resize(cv.imdecode(arr, 1), (0, 0),
                               fx=self.sub_sampling, fy=self.sub_sampling)
        b, g, r = cv.split(self.image)
        b = b.astype(np.float)
        g = g.astype(np.float)
        r = r.astype(np.float)
        # self.Brightness = cv.cvtColor()
        self.Brightness = ((0.299*(r**2) + 0.587*(g**2) +
                            0.114*(b**2))**0.5)
                            # .astype(np.uint8)

    def set_param(self, value=0.1, param='exposure'):
        value = max(value, 0.1)
        params = {str(param): float(value)}
        self.client.update_configuration(params)

    def get_param(self, param='exposure'):
        value = rospy.get_param("/" + str(self.client_name) + str(param), None)
        return value

    def get_mode(self, data):
        if data is None:
            return None
        
        data = data.ravel()
        mode = np.mean(data)
        # mode = stats.(data).mode
        return mode

    def adjust_exposure_value(self):
        while not rospy.is_shutdown():
            if self.image is None:
                img_is_none()
                continue
            mode = self.get_mode(self.Brightness)
            ev = self.get_param()
            print_result('Exposure')
            print_result(ev)
            print_result('mode')
            print_result(mode)
            if ev is None:
                continue
            if mode >= 180:
                ev -= 0.12
                print "minusex"
            elif 180 >= mode >= 140:
                ev -= 0.08
                print "minus"
            elif 140 >= mode >= 110:
                ev -= 0.04
                print "minusnorm"
            elif 40 <= mode <= 50:
                ev += 0.04
                print "plusnorm"
            elif 40 >= mode >= 0:
                ev += 0.08
                print "plus"
            self.set_param(value=ev)
            rospy.sleep(0.01)