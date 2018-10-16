#!/usr/bin/python2.7
import rospy
import cv2 as cv
import numpy as np
from sensor_msgs.msg import CompressedImage
from zeabus_vision.msg import vision_gate
from zeabus_vision.srv import vision_srv_gate
from vision_lib import *
import color_text as ct

bgr = None
image_result = None
public_topic = '/vision/mission/gate/'
sub_sampling = 1
debug = True


def mission_callback(msg):
    print_result('mission_callback', ct.CYAN)
    task = str(msg.task.data)
    req = str(msg.req.data)

    print("task is " + ct.UNDERLINE + task + ct.DEFAULT +
          " and req is " + ct.UNDERLINE + req + ct.DEFAULT)
    color = ['red', 'green', 'black']
    if task == 'gate' and req in color:
        return find_gate(req)


def image_callback(msg):
    global bgr, image_result, sub_sampling
    arr = np.fromstring(msg.data, np.uint8)
    bgr = cv.resize(cv.imdecode(arr, 1), (0, 0),
                    fx=sub_sampling, fy=sub_sampling)
    h_img, w_img = bgr.shape[:2]
    bgr = cv.resize(bgr, (int(w_img/3), int(h_img/3)))
    image_result = bgr.copy()


def message(n_obj=0, cx1=0.0, cy1=0.0, cx2=0.0, cy2=0.0, area=0.0):
    msg = vision_gate()
    msg.n_obj = n_obj
    msg.cx1 = cx1
    msg.cy1 = cy1
    msg.cx2 = cx2
    msg.cy2 = cy2
    msg.area = area
    if debug:
        print n_obj
    return msg


def get_mask(img, color):
    hsv = cv.cvtColor(img, cv.COLOR_BGR2HSV)
    if color == 'red':
        upper = np.array([45, 255, 255], dtype=np.uint8)
        lower = np.array([7, 160, 0], dtype=np.uint8)
    elif color == 'green':
        upper = np.array([45, 255, 255], dtype=np.uint8)
        lower = np.array([7, 160, 0], dtype=np.uint8)
    elif color == 'black':
        upper = np.array([45, 255, 255], dtype=np.uint8)
        lower = np.array([7, 160, 0], dtype=np.uint8)
    mask = cv.inRange(hsv, lower, upper)
    return mask


def get_ROI(mask, direction):
    himg, wimg = mask.shape[:2]
    ROI = []
    contours = cv.findContours(
        mask, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)[1]
    for cnt in contours:
        area = cv.contourArea(cnt)
        if debug:
            print area
        if area < 1500:
            continue
        x, y, w, h = cv.boundingRect(cnt)
        horizontal_bound = (direction == "horizontal" and float(h)/himg < 0.01)
        vertical_bound = (direction == "vertical" and float(w)/wimg < 0.008)
        if debug:
            print(direction, eval(direction+"_bound"))
        if horizontal_bound or vertical_bound:
            continue
        top_excess = (y < 0.05*himg)
        right_excess = ((x+w) > 0.95*wimg)
        left_excess = (x < (0.05*wimg))
        bottom_excess = ((y+h) > 0.95*himg)
        horizontal = direction == "horizontal" and not top_excess and not bottom_excess and w > 3*h
        vertical = direction == "vertical" and not right_excess and not left_excess and h > 3*w
        if horizontal or vertical:
            ROI.append(cnt)
    return ROI


def get_rect(cnt):
    global image_result
    himg, wimg = image_result.shape[:2]
    x, y, w, h = cv.boundingRect(cnt)
    cv.rectangle(image_result, (x, y), (x+w, y+h), (0, 255, 0), 2)
    cx = x + (w/2)
    cy = y + (h/2)
    cv.circle(image_result, (cx, cy), 5, (0, 0, 255), -1)
    cx = Aconvert(cx, wimg)
    cy = -1.0*Aconvert(cy, himg)
    area = (1.0*w*h)/(wimg*himg)
    return x, y, x+w, y+h, area


def find_gate(color):
    global bgr
    if bgr is None:
        img_is_none()
        return message(n_obj=-1)
    mask = get_mask(bgr, color)
    direction = "horizontal" if color == "black" else "vertical"
    ROI = get_ROI(mask, direction)
    mode = len(ROI)
    if mode == 0:
        print_result("NOT FOUND", ct.RED)
        publish_result(image_result, 'bgr', public_topic + 'image_result')
        publish_result(mask, 'gray', public_topic + 'mask')
        return message()
    elif mode >= 1:
        if mode == 1:
            print_result("FOUND THIS COLOR", ct.GREEN)
        elif mode > 1:
            print_result("FOUND BUT HAVE SOME NOISE (" +
                         str(mode) + ")", ct.YELLOW)
        select_cnt = max(ROI, key=cv.contourArea)
        cx1, cy1, cx2, cy2, area = get_rect(select_cnt)
        publish_result(image_result, 'bgr', public_topic + 'image_result')
        publish_result(mask, 'gray', public_topic + 'mask')
        return message(n_obj=mode, cx1=cx1, cy1=cy1, cx2=cx2, cy2=cy2, area=area)


if __name__ == '__main__':
    rospy.init_node('vision_gate', anonymous=False)

    image_topic = get_topic("front")
    rospy.Subscriber(image_topic, CompressedImage, image_callback)
    rospy.Service('vision_gate', vision_srv_gate(),
                  mission_callback)
    print_result("INIT NODE GATE", ct.GREEN)

    rospy.spin()
    print_result("END PROGRAM", ct.YELLOW_HL+ct.RED)
