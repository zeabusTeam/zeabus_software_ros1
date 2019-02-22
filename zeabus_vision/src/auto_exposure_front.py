#!/usr/bin/python2.7
"""
    File name: auto_exposure_front.py
    Author: skconan
    Date created: 2018/11/03
    Python Version: 2.7
"""


import os
import rospy
from auto_exposure_hist import AutoExposure


def auto_exposure_front():
    rospy.init_node('AutoExposureFront')
    AE = AutoExposure("/auto_exposure_hist_front")
    AE.run()


if __name__ == '__main__':
    auto_exposure_front()
