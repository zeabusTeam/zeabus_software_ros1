#!/usr/bin/python2.7
import rospy
import cv2 as cv
import numpy as np
from sensor_msgs.msg import CompressedImage
from dynamic_reconfigure.client import Client as Client
from vision_lib import *
import color_text as ctxt