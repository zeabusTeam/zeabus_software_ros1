#!/usr/bin/python2.7

import numpy as np
import cv2 as cv
from vision_lib import get_color


class Image:
    def __init__(self, sub_sampling=0.3):
        self.bgr = None
        self.display = None
        self.sub_sampling = sub_sampling

    def get_hsv(self):
        try:
            self.hsv
        except:
            self.hsv = cv.cvtColor(self.bgr, cv.COLOR_BGR2HSV)

    def get_gray(self):
        try:
            self.gray
        except:
            self.gray = cv.cvtColor(self.bgr, cv.COLOR_BGR2GRAY)

    def callback(self, msg):
        arr = np.fromstring(msg.data, np.uint8)
        self.image = cv.resize(cv.imdecode(arr, 1), (0, 0),
                               fx=self.sub_sampling, fy=self.sub_sampling)
        self.display = self.image.copy()

    def topic(self, camera, mode='new'):
        if camera == 'front':
            if mode == 'new':
                return '/vision/front/image_rect_color/compressed'
            if mode == 'old':
               return '/vision/front/image_rect_color/compressed'
        elif camera == 'bottom':
            return '/vision/bottom/image_raw/compressed'
