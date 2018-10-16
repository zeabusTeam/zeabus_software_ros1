#!/usr/bin/python2.7
import rospy
import cv2 as cv
import numpy as np
from sensor_msgs.msg import CompressedImage
from zeabus_vision.msg import vision_gate
from zeabus_vision.srv import vision_srv_gate
from vision_lib import *
import color_text as ct
from time import time

bgr = None
image_result = None
public_topic = '/vision/mission/gate/'
sub_sampling = 1
debug = True


def mission_callback(msg):
    print_result('mission_callback', ct.CYAN)
    task = str(msg.task.data)
    req = str(msg.req.data)
    req = 'BLANK' if req == '' else req
    print("task is " + ct.UNDERLINE + task + ct.DEFAULT +
          " and req is " + ct.UNDERLINE + req + ct.DEFAULT)
    if task == 'gate':
        return find_gate()


def image_callback(msg):
    global bgr, image_result, sub_sampling
    arr = np.fromstring(msg.data, np.uint8)
    bgr = cv.resize(cv.imdecode(arr, 1), (0, 0),
                    fx=sub_sampling, fy=sub_sampling)
    h_img, w_img = bgr.shape[:2]
    bgr = cv.resize(bgr, (int(w_img/3), int(h_img/3)))
    image_result = bgr.copy()

def pre_process(bgr):   
    return bgr


def message(n_obj=0,pos=0, cx1=0.0, cy1=0.0, cx2=0.0, cy2=0.0, area=0.0):
    msg = vision_gate()
    msg.n_obj = n_obj
    msg.pos = pos
    msg.cx1 = cx1
    msg.cy1 = cy1
    msg.cx2 = cx2
    msg.cy2 = cy2
    msg.area = area
    if debug:
        print n_obj
    print msg
    return msg


def get_mask(img):
    gray = cv.cvtColor(img,cv.COLOR_BGR2GRAY)
    scharr_y = cv.Scharr(gray, cv.CV_8U, 0, 1, scale=0.1)
    scharr_x = cv.Scharr(gray, cv.CV_8U, 1, 0, scale=0.1)
    scharr = cv.bitwise_or(scharr_y, scharr_x)
    _, scharr_th = cv.threshold(scharr, np.percentile(scharr,99), 255, cv.THRESH_BINARY)
    return scharr_th



def find_gate():
    global bgr
    if bgr is None:
        img_is_none()
        return message(n_obj=-1)
    mask = get_mask(bgr)
    publish_result(image_result, 'bgr', public_topic + 'image_result')
    publish_result(mask, 'gray', public_topic + 'mask')
    return message()



if __name__ == '__main__':
    rospy.init_node('vision_gate', anonymous=False)

    image_topic = get_topic("front")
    rospy.Subscriber(image_topic, CompressedImage, image_callback)
    rospy.Service('vision_gate', vision_srv_gate(),
                  mission_callback)
    print_result("INIT NODE GATE", ct.GREEN)

    rospy.spin()
    print_result("END PROGRAM", ct.YELLOW_HL+ct.RED)
