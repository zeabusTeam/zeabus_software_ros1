#!/usr/bin/python2.7
"""
    File name: histogram_matching.py
    Author: zeabus
    Date created: 2018/10/16
    Python Version: 2.7
"""


import cv2 as cv
import numpy as np


def histogram_matching(ref, input):
    hist_ref, _ = np.histogram(ref.flatten(), 256, [0, 256])
    hist_input, _ = np.histogram(input.flatten(), 256, [0, 256])

    cdf_ref = hist_ref.cumsum()
    cdf_input = hist_input.cumsum()

    xp = range(0, 256)
    lut = np.interp(cdf_input, cdf_ref, xp)
    lut = np.uint8(lut)

    res = np.uint8(np.interp(input, xp, lut))
    return res


if __name__ == '__main__':
    pass
