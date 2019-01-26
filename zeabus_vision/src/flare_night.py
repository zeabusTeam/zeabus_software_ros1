#!/usr/bin/python2.7
"""
    File name: flare_night.py
    Author: AyumiizZ
    Python Version: 2.7
    About: code for finding flare at night time
"""

import rospy
import cv2 as cv
import numpy as np
from sensor_msgs.msg import CompressedImage
from zeabus_vision.msg import vision_flare
from zeabus_vision.srv import vision_srv_flare
from vision_lib import *
import color_text as ct
import os

bgr = None
image_result = None
public_topic = '/vision/mission/flare/'
sub_sampling = 1


def mission_callback(msg):
    print_result('mission_callback', ct.CYAN)
    task = str(msg.task.data)
    req = str(msg.req.data)

    print("task",task + " " + req)
    if task == 'flare' and req in ['near','far']:
        return find_flare(req)


def image_callback(msg):
    global bgr, image_result, sub_sampling
    arr = np.fromstring(msg.data, np.uint8)
    bgr = cv.resize(cv.imdecode(arr, 1), (0, 0),
                    fx=sub_sampling, fy=sub_sampling)
    h_img, w_img = bgr.shape[:2]
    bgr = cv.resize(bgr, (int(w_img/3), int(h_img/3)))
    image_result = bgr.copy()


def message(state=0, cx=0.0, cy=0.0, area=0.0):
    msg = vision_flare()
    msg.state = state
    msg.cx = cx
    msg.cy = cy
    msg.area = area
    print msg
    return msg


def get_mask(img):
    blur = cv.medianBlur(img,5)
    hsv = cv.cvtColor(blur, cv.COLOR_BGR2HSV)
    # upper, lower = get_color_range('yellow', 'front', '1', 'flare')
    h,s,v = cv.split(hsv)
    s = cv.equalizeHist(s)
    v = cv.equalizeHist(v)
    hsv = cv.merge((h,s,v))
    upper = np.array([43, 255, 255], dtype=np.uint8)
    lower = np.array([9, 0, 0], dtype=np.uint8)
    mask = cv.inRange(hsv, lower, upper)
    return mask


def get_ROI(mask, case):
    himg, wimg = mask.shape[:2]
    ROI = []
    contours = cv.findContours(
        mask, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)[1]
    for cnt in contours:
        check_area = 10000 if case == 'near' else 100
        area = cv.contourArea(cnt)
        if area < check_area:
            continue
        rect = cv.minAreaRect(cnt)
        (w,h) = rect[1]
        angle = rect[2]
        print angle
        box = cv.boxPoints(rect)
        box = np.int64(box)
        BR = box[0]
        BL = box[1]
        TL = box[2]
        TR = box[3]
        cv.drawContours(image_result,[box],0,(0,0,255),2)
        top_excess = (TL[1] <0.05*himg or TR[1] < 0.05*himg)
        right_excess = (TR[0] > 0.95*wimg or BR[0] > 0.95*wimg)
        left_excess = (TL[0] < (0.05*wimg) or BL[0] < 0.05*wimg)
        bottom_excess = (BL[1] > 0.95*himg or BR[1] > 0.95*himg)
        if case == 'near':
            percent_area = (float(area)/(himg*wimg))
            print TL[1] , TR[1] , 0.05*wimg
            print percent_area , bottom_excess , top_excess
            if ((percent_area > 0.25) and bottom_excess and top_excess):
                ROI.append(cnt)
        elif case == 'far':
            h_w_ratio = 1.0*h/w
            print h_w_ratio ,left_excess ,right_excess
            if not(left_excess or right_excess) and h_w_ratio >= 0.035 and (angle <= -80 or angle == 0) :
                ROI.append(cnt)
    return ROI


def get_cx(cnt):
    global image_result
    himg, wimg = image_result.shape[:2]
    rect = cv.minAreaRect(cnt)
    box = cv.boxPoints(rect)
    box = np.int64(box)
    BR = box[0]
    BL = box[1]
    TL = box[2]
    TR = box[3]
    cv.drawContours(image_result,[box],0,(0,255,0),2)
    cx = ((TL[0]+TR[0])/2 + (BL[0]+BR[0])/2)/2
    cy = ((TR[1]+BR[1])/2 + (TL[1]+BL[1])/2)/2
    cv.circle(image_result, (cx, cy), 5, (0, 0, 255), -1)
    cx = Aconvert(cx, wimg)
    cy = -1.0*Aconvert(cy, himg)
    area = -1
    return cx, cy, area


def find_flare(req):
    global bgr
    if bgr is None:
        img_is_none()
        return message(state=-1)
    mask = get_mask(bgr)
    ROI = get_ROI(mask, case=req)
    mode = len(ROI)
    if mode == 0:
        print_result("NOT FOUND", ct.RED)
        publish_result(image_result, 'bgr', public_topic + 'image_result')
        publish_result(mask, 'gray', public_topic + 'mask')
        return message()
    elif mode >= 1:
        if mode == 1:
            print_result("FOUND A FLARE", ct.GREEN)
        elif mode > 1:
            print_result("FOUND BUT HAVE SOME NOISE (" +
                         str(mode) + ")", ct.YELLOW)
        cx, cy, area = get_cx(cnt=max(ROI, key=cv.contourArea))
        publish_result(image_result, 'bgr', public_topic + 'image_result')
        publish_result(mask, 'gray', public_topic + 'mask')
        if cx < -1 and cx > 1 and cy > 1 and cx < 1 : 
            return message()
        return message(cx=cx, cy=cy, area=area, state=len(ROI))

# def find_far_flare():
#     global bgr
#     if bgr is None:
#         img_is_none()
#         return message(state=-1)
#     mask = get_mask(image_result)
#     ROI = get_ROI(mask,case = 'far')
#     if len(ROI) == 0 :
#         mode = 1
#         print_result("NOT FOUND", ct.RED)
#     elif len(ROI) == 1:
#         mode = 2
#         print_result("FOUND A FLARE", ct.GREEN)
#     elif len(ROI) > 1:
#         mode = 2
#         print_result("FOUND BUT HAVE SOME NOISE", ct.YELLOW)
#     if mode == 1:
#         publish_result(image_result, 'bgr', public_topic + 'image_result')
#         publish_result(mask, 'gray', public_topic + 'mask')
#         return message()
#     elif mode == 2:
#         cx, cy, area = get_cx(cnt=max(ROI, key=cv.contourArea))
#         publish_result(image_result, 'bgr', public_topic + 'image_result')
#         publish_result(mask, 'gray', public_topic + 'mask')
#         return message(cx=cx, cy=cy, area=area, state=len(ROI))


if __name__ == '__main__':
    rospy.init_node('vision_flare', anonymous=False)

    image_topic = get_topic("front")
    rospy.Subscriber(image_topic, CompressedImage, image_callback)
    rospy.Service('vision_flare', vision_srv_flare(),
                  mission_callback)
    print_result("INIT NODE FLARE", ct.GREEN)

    rospy.spin()
    print_result("END PROGRAM", ct.YELLOW_HL+ct.RED)
