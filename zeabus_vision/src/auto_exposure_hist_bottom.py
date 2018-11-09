#!/usr/bin/python2.7
"""
    File name: auto_exposure_hist_bottom.py
    Author: skconan
    Date created: 2018/11/04
    Python Version: 2.7
"""


import os
import rospy
from auto_exposure_hist import AutoExposure


def auto_exposure_bottom():
    rospy.init_node('AutoExposureBottom')
    AE = AutoExposure("/auto_exposure_hist_bottom")
    AE.run()

if __name__=='__main__':
    auto_exposure_bottom()