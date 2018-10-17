#!/usr/bin/python2.7
"""
    File name: auto_exposure_hist.py
    Author: zeabus
    Date created: 2018/10/16
    Python Version: 2.7
"""


import rospy
import cv2 as cv
import numpy as np
import matplotlib.pyplot as plt
from statistic import Statistic
from sensor_msgs.msg import CompressedImage
from dynamic_reconfigure.client import Client as Client
from histogram_matching import *

bgr = None
sub_sampling = 0.25
stat = Statistic()
client_name = "ueye_cam_nodelet_front"
client = Client(client_name)


def image_callback(msg):
    global bgr, sub_sampling
    arr = np.fromstring(msg.data, np.uint8)
    bgr = cv.resize(cv.imdecode(arr, 1), (0, 0),
                    fx=sub_sampling, fy=sub_sampling)


def stretching(gray):
    global stat
    min = stat.get_min(gray)
    max = stat.get_max(gray)
    print(stat.get_skewness(gray))
    gray = (gray - min) * ((200. - 20) / (max - min))
    gray += 20
    gray = np.uint8(gray)
    return gray


def RGB_to_I(bgr):
    b, g, r = cv.split(bgr)
    b = np.float16(b)
    g = np.float16(g)
    r = np.float16(r)
    i = (r + g + b) / 3.
    return np.uint8(i)


def nothing(x):
    pass


def set_param(value=0.1):
    global client, client_name
    params = {"exposure": float(value)}
    client.update_configuration(params)


def get_exposure():
    global client_name
    value = rospy.get_param("/" + client_name + "/exposure", None)
    return value


def auto_exposure_hist():
    global bgr, stat

    cv.namedWindow('image')

    cv.createTrackbar('p_min', 'image', 0, 50, nothing)
    cv.createTrackbar('p_max', 'image', 50, 100, nothing)

    cv.createTrackbar('th_p_min', 'image', 0, 255, nothing)
    cv.createTrackbar('th_p_max', 'image', 0, 255, nothing)

    cv.moveWindow('image',100,100)
    while not rospy.is_shutdown():
        if bgr is None:
            continue
        blur = cv.blur(bgr.copy(), (11,11))
        gray = cv.cvtColor(blur, cv.COLOR_BGR2GRAY)
        hsv = cv.cvtColor(bgr, cv.COLOR_BGR2HSV)
        h, s, v = cv.split(hsv)
        p_min = cv.getTrackbarPos('p_min', 'image')
        p_max = cv.getTrackbarPos('p_max', 'image')
        
        th_p_min = cv.getTrackbarPos('th_p_min', 'image')
        th_p_max = cv.getTrackbarPos('th_p_max', 'image')
        

        ev = get_exposure()

        stat_p_min = stat.get_percentile(gray, p_min)
        stat_p_max = stat.get_percentile(gray, p_max)

        print('===========percentile==============')
        print(stat_p_min,stat_p_max,ev)

        print('===========ev==============')
        print(ev)

        if stat_p_min < th_p_min:
            set_param(ev + 0.1)
        if stat_p_max > th_p_max:
            set_param(ev - 0.1)


        stretch = stretching(gray)
        stretch_v = stretching(v)


        hist_matching = histogram_matching(stretch_v, gray)

        cv.imshow('gray', gray)
        cv.imshow('image', bgr)
        cv.imshow('blur', blur)
        cv.imshow('strect', stretch)
        cv.imshow('hist_matching', hist_matching)

        k = cv.waitKey(1) & 0xff
        if k == ord('q'):
            break
        histr = cv.calcHist([gray], [0], None, [256], [0, 256])
        plt.plot(histr, color='blue')

        plt.pause(0.00001)
        plt.clf()
    plt.close()
    cv.destroyAllWindows()


if __name__ == '__main__':
    rospy.init_node('auto_exposure_hist')
    image_topic = "/vision/front/image_raw/compressed"
    rospy.Subscriber(image_topic, CompressedImage, image_callback)
    auto_exposure_hist()
