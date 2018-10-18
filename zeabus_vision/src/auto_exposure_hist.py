#!/usr/bin/python2.7
"""
    File name: auto_exposure_hist.py
    Author: skconan
    Date created: 2018/10/16
    Python Version: 2.7
"""


import os
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

p_min_default = None
p_max_default = None
th_p_min_default = None
th_p_max_default = None
# get_param_default()

PATH_PKG = os.path.dirname(os.path.abspath(__file__))

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

def pre_processing(bgr):
    blur = cv.blur(bgr.copy(), (11, 11))
    return blur

def get_param_default():
    global p_min_default, p_max_default, th_p_min_default, th_p_max_default, PATH_PKG
    img_ref = cv.imread(PATH_PKG+'/ref.png',0)

    p_min_default = 5
    p_max_default = 75
    
    th_p_min_default = stat.get_percentile(img_ref, p_min_default)
    th_p_max_default = stat.get_percentile(img_ref, p_max_default)

    th_p_max_default = int(th_p_max_default)
    th_p_min_default = int(th_p_min_default)

def auto_exposure_hist():
    global bgr, stat, p_min_default, p_max_default, th_p_min_default, th_p_max_default
    get_param_default()
    cv.namedWindow('image')

    cv.createTrackbar('p_min', 'image', 0, 50, nothing)
    cv.createTrackbar('p_max', 'image', 50, 100, nothing)

    cv.createTrackbar('th_p_min', 'image', 0, 255, nothing)
    cv.createTrackbar('th_p_max', 'image', 0, 255, nothing)

    cv.moveWindow('image', 100, 100)

    cv.setTrackbarPos('p_min', 'image', p_min_default)
    cv.setTrackbarPos('p_max', 'image', p_max_default)
    cv.setTrackbarPos('th_p_min', 'image', th_p_min_default)
    cv.setTrackbarPos('th_p_max', 'image', th_p_max_default)

    while not rospy.is_shutdown():
        if bgr is None:
            continue
        pre_process_bgr = pre_processing(bgr.copy())
        gray = cv.cvtColor(pre_process_bgr, cv.COLOR_BGR2GRAY)
        hsv = cv.cvtColor(pre_process_bgr, cv.COLOR_BGR2HSV)
        h, s, v = cv.split(hsv)
        p_min = cv.getTrackbarPos('p_min', 'image')
        p_max = cv.getTrackbarPos('p_max', 'image')

        th_p_min = cv.getTrackbarPos('th_p_min', 'image')
        th_p_max = cv.getTrackbarPos('th_p_max', 'image')

        ev = get_exposure()

        stat_p_min = stat.get_percentile(gray, p_min)
        stat_p_max = stat.get_percentile(gray, p_max)

        print('=='*20)
        print(stat_p_min, stat_p_max, ev)

        if stat_p_min < th_p_min:
            set_param(ev + 0.1)
        if stat_p_max > th_p_max:
            set_param(ev - 0.1)

        stretch = stretching(gray)
        stretch_v = stretching(v)

        hist_matching = histogram_matching(stretch_v, gray)

        cv.imshow('gray', gray)
        cv.imshow('image', bgr)
        cv.imshow('pre_process_bgr', pre_process_bgr)
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