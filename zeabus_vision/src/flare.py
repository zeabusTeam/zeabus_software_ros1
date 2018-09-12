#!/usr/bin/python2.7
import rospy
import cv2 as cv
import numpy as np
from sensor_msgs.msg import CompressedImage
from zeabus_vision.msg import vision_flare
from zeabus_vision.srv import vision_srv_flare
from vision_lib import *
import color_text as ct

bgr = None
image_result = None
public_topic = '/vision/mission/flare'
sub_sampling = 1


def mission_callback(msg):
    print_result('mission_callback', ct.CYAN)
    task = msg.task.data

    print(msg)
    if task == 'flare':
        return find_flare()


def image_callback(msg):
    global bgr, image_result, sub_sampling
    arr = np.fromstring(msg.data, np.uint8)
    bgr = cv.resize(cv.imdecode(arr, 1), (0, 0),
                    fx=sub_sampling, fy=sub_sampling)
    h_img, w_img = bgr.shape[:2]
    bgr = cv.resize(bgr, (int(w_img/3), int(h_img/3)))
    image_result = bgr.copy()


def message(n_obj=0, cx=0.0, cy=0.0, area=0.0):
    msg = vision_flare()
    msg.n_obj = n_obj
    msg.cx = cx
    msg.cy = cy
    msg.area = area
    print n_obj
    return msg


def find_flare():
    global bgr
    if bgr is None:
        img_is_none()
        return message(n_obj=-1)
    pass


if __name__ == '__main__':
    rospy.init_node('vision_flare', anonymous=False)
    
    image_topic = get_topic("front")
    rospy.Subscriber(image_topic, CompressedImage, image_callback)
    rospy.Service('vision_flare', vision_srv_flare(),
                  mission_callback)
    print_result("INIT NODE FLARE", ct.GREEN)

    rospy.spin()
    print_result("END PROGRAM", ct.YELLOW_HL+ct.RED)
