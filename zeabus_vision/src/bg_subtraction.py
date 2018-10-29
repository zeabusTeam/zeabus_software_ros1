#!/usr/bin/python2.7
"""
    File name:bg_subtraction.py
    Author: zeabus
    Date created: 2018/10/16
    Python Version: 2.7
"""

import rospy
import cv2 as cv
import numpy as np
import matplotlib.pyplot as plt
from sensor_msgs.msg import CompressedImage

bgr = None 
sub_sampling = 0.25

def image_callback(msg):
    global bgr, sub_sampling
    arr = np.fromstring(msg.data, np.uint8)
    bgr = cv.resize(cv.imdecode(arr, 1), (0, 0),
                    fx=sub_sampling, fy=sub_sampling)

def bg_subtraction():
    global bgr
    min_gray = 120
    max_gray = 230
    ref = np.zeros((200, 200), np.uint8)
    ref[0:100, :] = min_gray
    ref[100:, :] = max_gray
    cv.imshow('ref', ref)
    while not rospy.is_shutdown():
        if bgr is None:
            continue

        gray = cv.cvtColor(bgr, cv.COLOR_BGR2GRAY)
        bg = cv.medianBlur(gray, 71)
        fg = cv.medianBlur(gray, 5)
        sub = np.uint8(abs(fg - bg))

        bg = np.uint8(bg)
        fg = np.uint8(fg)

        obj1 = np.zeros(gray.shape, np.uint8)
        obj2 = np.zeros(gray.shape, np.uint8)
        obj1[sub > min_gray] = 255
        obj2[sub < max_gray] = 255
        obj = cv.bitwise_and(obj1, obj2)

        cv.imshow('original_bgr', bgr)
        cv.imshow('bg', np.uint8(bg))
        cv.imshow('fg', fg)

        cv.imshow('sub', sub)

        cv.imshow('obj', obj)
        cv.imshow('obj1', obj1)
        cv.imshow('obj2', obj2)

        k = cv.waitKey(1) & 0xff
        if k == ord('q'):
            break
        histr = cv.calcHist([sub], [0], None, [256], [0, 256])
        plt.plot(histr, color='red')
 
        plt.pause(0.0001)
        plt.clf()
    plt.close()
    cv.destroyAllWindows()


if __name__ == '__main__':
    rospy.init_node('vision_gate', anonymous=False)
    image_topic = "/vision/front/image_raw/compressed"
    rospy.Subscriber(image_topic, CompressedImage, image_callback) 
    bg_subtraction()
