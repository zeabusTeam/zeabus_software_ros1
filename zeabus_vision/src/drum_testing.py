#!/usr/bin/env python
"""
    File name:gate.py
    Author: skconan
    Date created: 2018/10/10
    Python Version: 3.5
"""


import cv2 as cv
import color_text as ct
from vision_lib import *
import constant as CONST
import math
import matplotlib.pyplot as plt
from statistics import Statistics
from zeabus_vision.msg import vision_gate
from sensor_msgs.msg import CompressedImage
from zeabus_vision.srv import vision_srv_gate
from fourier_transform import FourierTransform
from operator import itemgetter

image_input = None
public_topic = '/vision/mission/gate/'
sub_sampling = 0.3
debug = False
debug_time = False
stat = Statistics()
fft = FourierTransform()
bg = None
previous_bg = None
first_object = True
tracker = None
tracker_status = False
object_box = None
untrack_frame = 0


def mission_callback(msg):
    print_result('mission_callback', ct.CYAN)
    task = str(msg.task.data)
    req = str(msg.req.data)
    req = 'BLANK' if req == '' else req
    print("task is " + ct.UNDERLINE + task + ct.DEFAULT +
          " and req is " + ct.UNDERLINE + req + ct.DEFAULT)
    if task == 'gate':
        return find_golf()


def image_callback(msg):
    global image_input, sub_sampling
    arr = np.fromstring(msg.data, np.uint8)
    bgr = cv.resize(cv.imdecode(arr, 1), (0, 0),
                    fx=sub_sampling, fy=sub_sampling)
    image_input = bgr.copy()


def message(n_obj=0, pos=0, cx1=0.0, cy1=0.0, cx2=0.0, cy2=0.0, area=0.0):
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


def what_align(cnt):
    x, y, w, h = cv.boundingRect(cnt)
    if w > h:
        return 'h'
    return 'v'


def is_golf(cnt):
    (x, y), radius = cv.minEnclosingCircle(cnt)
    area_cnt = cv.contourArea(cnt)
    area_cir = math.pi * radius * radius

    if not 7 <= radius <= 30:
        return False

    area_ratio = area_cnt / area_cir

    if not (area_ratio > area_ratio_expected):
        return False
    print("area_ratio, radius")
    print(area_ratio, radius)
    return True




def find_circle(binary):
    _, contours, _ = cv.findContours(
        binary, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)

    result = []
    golf = cv.cvtColor(binary, cv.COLOR_GRAY2BGR)

    for cnt in contours:
        (x, y), radius = rect = cv.minEnclosingCircle(cnt)

        if not is_golf(cnt): #or not what_align(cnt) == align:
            continue
        
        x,y,radius = int(x),int(y), int(radius)
        cv.circle(golf,(x,y),radius,(0,255,255),-1)
        cv.putText(golf, str(round(radius)), (x, y),
                   cv.FONT_HERSHEY_SIMPLEX, 0.75, (255, 255, 0), 2)
        result.append([x,y,radius])

    cv.imshow('golf',golf)
    cv.waitKey(1)
    return result


def tracking_golf():
    global tracker, image_input, tracker_status, object_box, untrack_frame

    frame = image_input.copy()
    tracker_status, object_box = tracker.update(frame)

    if tracker_status:
        p1 = (int(object_box[0]), int(object_box[1]))
        p2 = (int(object_box[0] + object_box[2]),
              int(object_box[1] + object_box[3]))
        cv.rectangle(frame, p1, p2, (255, 0, 0), 2, 1)
    else:
        untrack_frame += 1
        cv.putText(frame, "Tracking failure detected", (100, 80),
                   cv.FONT_HERSHEY_SIMPLEX, 0.75, (0, 0, 255), 2)

    cv.putText(frame, "medianflow Tracker", (100, 20),
               cv.FONT_HERSHEY_SIMPLEX, 0.75, (50, 170, 50), 2)
    cv.imshow("Tracking", frame)
    cv.waitKey(1)


def color_mask(bgr):
    hsv = cv.cvtColor(bgr, cv.COLOR_BGR2HSV)
    h,s,v = cv.split(hsv)
    mask = np.zeros(h.shape,np.uint8)
    mask[h<38] = 255

    roi1 = np.zeros(h.shape,np.uint8)
    roi2 = np.zeros(h.shape,np.uint8)
    roi1[h>75] = 255
    roi2[h<130] = 255
    roi = roi1 & roi2
    return mask, roi

def find_drum():
    pass

def find_golf():
    global image_input, stat, previous_bg, first_object, tracker, tracker_status, object_box, untrack_frame
    min_gray = 20
    max_gray = 250
    gray = cv.cvtColor(image_input.copy(), cv.COLOR_BGR2GRAY)

    high_accuracy_gate = False
    obj = bg_subtraction(gray)
    
    mask, roi = color_mask(image_input) 
    roi = 255 - roi.copy()

    kernel = get_kernel(ksize=(3, 3))
    circle = cv.erode(obj.copy(),kernel) & mask & roi
    result = find_circle(circle)
    print(result)
  
    if len(result) > 0 :
        x,y,r = result[0]
        golf = np.zeros(gray.shape,np.uint8)
        cv.circle(golf,(x,y),r,(255,255,255),-1)
        points = cv.findNonZero(golf)
        x, y, w, h = np.int64(cv.boundingRect(points))
        object_box = (x, y, w, h)

    # if (tracker is None or (untrack_frame >= 2 and len(result) > 0)):# or len(result) > 0):
    #     print("======================================")

        # tracker = cv.TrackerGOTURN_create()
        # tracker = cv.TrackerMedianFlow_create()
    #     tracker = cv.TrackerMIL_create()
    #     first_object = False
    #     ok = tracker.init(image_input, object_box)
    #     if ok:
    #         tracker_status = True
    #         high_accuracy_gate = False
    #     first_object = False
    #     untrack_frame = 0
    # elif tracker:
    #     tracking_golf()

    # cv.imshow('display', display)
    # cv.imshow('obj', obj)
    # cv.imshow('roi', roi)
    # cv.imshow('mask', mask)
    # cv.imshow('circle', circle)
if __name__ == '__main__':
    rospy.init_node('vision_drum_testing', anonymous=False)
    image_topic = get_topic("bottom")
    rospy.Subscriber(image_topic, CompressedImage, image_callback)
    while not rospy.is_shutdown():
        if image_input is None:
            continue
        find_golf()
        k = cv.waitKey(1) & 0xff
        if k == ord('q'):
            break
    cv.destroyAllWindows()
    # rospy.Service('vision_gate', vision_srv_gate(),
    #               mission_callback)
    # print_result("INIT NODE GATE", ct.GREEN)
    # rospy.spin()
    # print_result("END PROGRAM", ct.YELLOW_HL+ct.RED)
