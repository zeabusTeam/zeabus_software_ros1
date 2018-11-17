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
debug = False
debug_time = False


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
    b,g,r = cv.split(bgr)
    r.fill(255)
    bgr = cv.merge((b,g,r))
    blur = cv.medianBlur(bgr,5)
    equ = equalize_bgr(blur)
    processed = equ
    return processed


def message(state=0,pos=0, cx1=0.0, cy1=0.0, cx2=0.0, cy2=0.0, area=0.0):
    msg = vision_gate()
    msg.state = state
    msg.pos = pos
    msg.cx1 = cx1
    msg.cy1 = cy1
    msg.cx2 = cx2
    msg.cy2 = cy2
    msg.area = area
    if debug:
        print state
    print msg
    return msg


def get_mask(img):
    hsv = cv.cvtColor(img, cv.COLOR_BGR2HSV)
    upper = np.array([2, 255, 255], dtype=np.uint8)
    lower = np.array([0, 247, 0], dtype=np.uint8)
    mask = cv.inRange(hsv,lower,upper)
    return mask


def get_ROI(mask):
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
        (xm,ym),(wm,hm),angle = cv.minAreaRect(cnt)
        w_h_bound = (float(wm)/wimg < 0.008 or float(hm)/himg < 0.01)
        if debug:
            print('w_h_bound', w_h_bound)
        if w_h_bound:
            continue
        cv.rectangle(image_result, (x, y), (x+w, y+h), (0, 255, 255), 2)
        square_area = wm*hm
        percent = area/square_area
        print percent
        if percent > 0.22 or area/percent < 0.5:
            continue
        
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
    cx1 = Aconvert(x, wimg)
    cy1 = -1.0*Aconvert(y, himg)
    cx2 = Aconvert(x+w, wimg)
    cy2 = -1.0*Aconvert(y+h, himg)
    area = (1.0*w*h)/(wimg*himg)
    top_excess = (y < 0.05*himg)
    right_excess = ((x+w) > 0.95*wimg)
    left_excess = (x < (0.05*wimg))
    bottom_excess = ((y+h) > 0.95*himg)
    if right_excess and not left_excess:
        pos = 1
    elif left_excess and not right_excess:
        pos = -1
    else:
        pos = 0
    return cx1, cy1, cx2, cy2, area, pos


def find_gate():
    global bgr
    if bgr is None:
        img_is_none()
        return message(state=-1)
    if debug_time:
        a = []
        checkpoint = time()
    bgr = pre_process(bgr)
    if debug_time:
        a.append(('c1',time()-checkpoint))
        checkpoint = time()
    mask = get_mask(bgr)
    if debug_time:
        a.append(('c2',time()-checkpoint))
        checkpoint = time()
    ROI = get_ROI(mask)
    if debug_time:
        a.append(('c3',time()-checkpoint))
        checkpoint = time()
    mode = len(ROI)
    if mode == 0:
        print_result("NOT FOUND", ct.RED)
        publish_result(image_result, 'bgr', public_topic + 'image_result')
        publish_result(mask, 'gray', public_topic + 'mask')
        if debug_time:
            a.append(('c4',time()-checkpoint))
        return message()
    elif mode >= 1:
        if mode == 1:
            print_result("FOUND GATE", ct.GREEN)
        elif mode > 1:
            print_result("FOUND BUT HAVE SOME NOISE (" +
                         str(mode) + ")", ct.YELLOW)
        select_cnt = max(ROI, key=cv.contourArea)
        if debug_time:
            a.append(('c5',time()-checkpoint))
            checkpoint = time()
        cx1, cy1, cx2, cy2, area, pos = get_rect(select_cnt)
        if debug_time:
            a.append(('c6',time()-checkpoint))
        # checkpoint = time()
        publish_result(image_result, 'bgr', public_topic + 'image_result')
        publish_result(mask, 'gray', public_topic + 'mask')
        # print time()-checkpoint
        if debug_time:
            print a
            print max(a,key=lambda x: a[1])
            print ('sum',sum([i[1] for i in a]))
        return message(state=mode, cx1=cx1, cy1=cy1, cx2=cx2, cy2=cy2, area=area, pos=pos)


if __name__ == '__main__':
    rospy.init_node('vision_gate', anonymous=False)

    image_topic = get_topic("front")
    rospy.Subscriber(image_topic, CompressedImage, image_callback)
    rospy.Service('vision_gate', vision_srv_gate(),
                  mission_callback)
    print_result("INIT NODE GATE", ct.GREEN)

    rospy.spin()
    print_result("END PROGRAM", ct.YELLOW_HL+ct.RED)
