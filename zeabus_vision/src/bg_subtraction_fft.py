#!/usr/bin/python2.7
"""
    File name:bg_subtraction_fft.py
    Author: zeabus
    Date created: 2018/10/16
    Python Version: 2.7
"""

import rospy
import cv2 as cv
import numpy as np
import matplotlib.pyplot as plt
from sensor_msgs.msg import CompressedImage
from fourier_transform import FourierTransform

bgr = None 
sub_sampling = 0.25
fft = FourierTransform()

def image_callback(msg):
    global bgr, sub_sampling
    arr = np.fromstring(msg.data, np.uint8)
    bgr = cv.resize(cv.imdecode(arr, 1), (0, 0),
                    fx=sub_sampling, fy=sub_sampling)

def bg_subtraction_fft():
    global bgr, fft
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
        bg = cv.medianBlur(gray, 61)
        fg = cv.medianBlur(gray, 5) 

      

        fft_bg = fft.image_to_fft(bg)
        fft_fg = fft.image_to_fft(fg)

        fft_bg = fft.shift_to_center(fft_bg)
        fft_fg = fft.shift_to_center(fft_fg)

        fft_bg_log = fft.get_log_scale(fft_bg)
        fft_fg_log = fft.get_log_scale(fft_fg)

        fft_sub = fft_fg - fft_bg
        cv.imshow('log_sub', np.uint8(fft_sub.copy()))

        fft_sub = fft.fft_to_image(fft_sub)
        fft_sub = (fft_sub - fft_sub.min()) / (fft_sub.max() - fft_sub.min())
        fft_sub = fft_sub * 255
        fft_sub = np.uint8(fft_sub)
        
        obj = np.zeros(gray.shape, np.uint8)
        obj[fft_sub < 100] = 255



        cv.imshow('original_bgr', bgr)
        cv.imshow('bg', np.uint8(bg))
        cv.imshow('fg', fg)
        cv.imshow('obj', obj)
        cv.imshow('log_bg', fft_bg_log)
        cv.imshow('log_fg', fft_fg_log)
        cv.imshow('fft_sub', fft_sub)


        k = cv.waitKey(1) & 0xff
        if k == ord('q'):
            break

    cv.destroyAllWindows()


if __name__ == '__main__':
    rospy.init_node('vision_gate', anonymous=False)
    image_topic = "/vision/front/image_raw/compressed"
    rospy.Subscriber(image_topic, CompressedImage, image_callback) 
    bg_subtraction_fft()
