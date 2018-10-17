#!/usr/bin/python2.7
"""
    File name: histogram_matching.py
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

bgr = None
sub_sampling = 0.4
stat = Statistic()


def histogram_matching(ref, input):
    hist_ref, _ = np.histogram(ref.flatten(), 256, [0, 256])
    hist_input, _ = np.histogram(input.flatten(), 256, [0, 256])

    cdf_ref = hist_ref.cumsum()
    cdf_input = hist_input.cumsum()

    # cdf_input = [0.19,0.44,0.65,0.81,0.89,0.95,0.98,1]
    # cdf_ref = [0,0,0,0.15,0.35,0.65,0.85,1]
    # xp = range(0,8)
    xp = range(0, 256)

    # xp 0 - 255
    # fp cdf ref

    lut = np.interp(cdf_input, cdf_ref, xp)
    # round_lamda = lambda a : round(a)
    # lut = round_lamda(lut)
    lut = np.uint8(lut)
    # print(lut)
    res = np.uint8(np.interp(input, xp, lut))
    # cv.imshow('res',res)
    # cv.imshow('ref',ref)
    # cv.imshow('input',input)
    # cv.waitKey(-1)
    return res


if __name__ == '__main__':
    # rospy.init_node('histogram_matching', anonymous=False)
    # histogram_matching()
    pass
