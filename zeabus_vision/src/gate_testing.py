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
import constants as CONST
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
        return find_gate()


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
    x,y,w,h = cv.boundingRect(cnt)
    if w > h:
        return 'h'
    return 'v'

def is_pipe(cnt):
    """
        Information
        Pipe    width = 150
                height = 40
    """
    (x, y), (w, h), angle = rect = cv.minAreaRect(cnt)

    area_ratio_expected = 0.65
    wh_ratio_expected = 9 #(150 / 40.) * 2
    # angle_expected = # 0 - -90
    w, h = max(w, h), min(w, h)
    area_cnt = cv.contourArea(cnt)
    area_box = w * h

    if (area_box == 0 or h == 0) or area_cnt <= 1000:
        return False

    area_ratio = area_cnt / area_box
    wh_ratio = 1.* w / h
    
    if not (area_ratio > area_ratio_expected and wh_ratio > wh_ratio_expected and (angle >= -10 or angle <= -80)) :
        return False
    
    print("area:",area_ratio,"wh",wh_ratio)
    return True

def find_pipe(binary, number_of_object, align):
    _, contours, _ = cv.findContours(
        binary, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)
    
    result = []
    pipe = cv.cvtColor(binary, cv.COLOR_GRAY2BGR)

    for cnt in contours:
        (x, y), (w, h), angle = rect = cv.minAreaRect(cnt)
        w, h = max(w, h), min(w, h)
        if not is_pipe(cnt) or not what_align(cnt) == align:
            continue
        
        box = cv.boxPoints(rect)
        box = np.int0(box)
        if align == 'v':
            cv.drawContours(pipe,[box],0,(0,255,255),2)
            result.append([int(x),int(y),int(h),int(w),angle])
        else:
            cv.drawContours(pipe,[box],0,(255,0,255),2)
            result.append([int(x),int(y),int(w),int(h),angle])

    
    cv.imshow('pipe', pipe)
    cv.waitKey(1)
    if align == 'v':
        result = sorted(result, key = itemgetter(3), reverse=True)
    else:
        result = sorted(result, key = itemgetter(2), reverse=True)
    
    if len(result) < number_of_object:
        return result, len(result), False
    else:
        return result[:number_of_object], number_of_object, True

def tracking_gate():
    global tracker, image_input, tracker_status, object_box, untrack_frame
   
    frame = image_input.copy()
    tracker_status, object_box = tracker.update(frame)
 
    if tracker_status:
        p1 = (int(object_box[0]), int(object_box[1]))
        p2 = (int(object_box[0] + object_box[2]), int(object_box[1] + object_box[3]))
        cv.rectangle(frame, p1, p2, (255,0,0), 2, 1)
    else :
        # Tracking failure
        untrack_frame += 1
        cv.putText(frame, "Tracking failure detected", (100,80), cv.FONT_HERSHEY_SIMPLEX, 0.75,(0,0,255),2)
        

    cv.putText(frame, "medianflow Tracker", (100,20), cv.FONT_HERSHEY_SIMPLEX, 0.75, (50,170,50),2)
    cv.imshow("Tracking", frame)
    cv.waitKey(1)

def find_gate():
    global image_input, stat, previous_bg, first_object, tracker, tracker_status, object_box, untrack_frame
    min_gray = 20
    max_gray = 245
    gray = cv.cvtColor(image_input.copy(), cv.COLOR_BGR2GRAY)

    high_accuracy_gate = False

    bg = cv.medianBlur(gray, 61)
    # if not previous_bg is None:
    #     bg_tmp = np.int16(bg.copy())
    #     bg_tmp += np.int16(previous_bg.copy())
    #     bg = np.uint8(bg_tmp/2.)

    fg = cv.medianBlur(gray, 7)
    sub = np.uint8(abs(fg - bg))

    obj1 = np.zeros(gray.shape, np.uint8)
    obj2 = np.zeros(gray.shape, np.uint8)
    obj1[sub > min_gray] = 255
    obj2[sub < max_gray] = 255
    obj = cv.bitwise_and(obj1, obj2)

    kernel_vertical = get_kernel(ksize=(1, 21))
    kernel_horizontal = get_kernel(ksize=(21, 1))
    kernel_box = get_kernel(ksize=(7,7))

    vertical = cv.erode(obj.copy(), kernel_vertical)
    vertical = cv.dilate(vertical.copy(), kernel_box)
    kernel_erode = get_kernel(ksize=(3,11))
    vertical = cv.erode(vertical.copy(), kernel_erode)

    horizontal = cv.erode(obj.copy(), kernel_horizontal)
    horizontal = cv.dilate(horizontal.copy(), kernel_box)
    kernel_erode = get_kernel(ksize=(11,3))
    horizontal = cv.erode(horizontal.copy(), kernel_erode)

    vertical_pipe, no_pipe_v, _ = find_pipe(vertical, 2, 'v') 
    horizontal_pipe, no_pipe_h, _ = find_pipe(horizontal, 1, 'h')


    display = image_input.copy()

    # for res in vertical_pipe:
    #     x, y, h, w, angle = res
    #     cv.rectangle(display, (int(x-w/2.),int(y-h/2.)), (int(x+w/2.),int(y+h/2.)), (0,255,255), 2)
    
    for res in horizontal_pipe + vertical_pipe:
        # yellow
        x, y, w, h, angle = res
        cv.rectangle(display, (int(x-w/2.),int(y-h/2.)), (int(x+w/2.),int(y+h/2.)), (255,0,255), 2)
    
    if no_pipe_v + no_pipe_h >= 2:
        high_accuracy_gate = True
        gate = np.zeros(gray.shape, np.uint8)
        for res in horizontal_pipe + vertical_pipe:
            x, y, w, h, angle = res
            cv.rectangle(gate, (int(x-w/2.),int(y-h/2.)), (int(x+w/2.),int(y+h/2.)), (255,255,255), -1)
        
        points = cv.findNonZero(gate)
        x, y, w, h = np.int0(cv.boundingRect(points))
    
        object_box = (x,y,w,h)

    if (tracker is None or untrack_frame >= 15) and no_pipe_v + no_pipe_h >= 2:
        print("======================================")
        
        # tracker = cv.TrackerGOTURN_create()   
        # tracker = cv.TrackerMedianFlow_create()   
        tracker = cv.TrackerMIL_create()   
        first_object = False
        ok = tracker.init(image_input,object_box)
        if ok:
            tracker_status = True
            high_accuracy_gate = False
        first_object = False
        untrack_frame = 0
    elif tracker:
        tracking_gate()


    previous_bg = bg
    # cv.imshow('original_bgr', image_input)
    # cv.imshow('gray', gray)
    cv.imshow('display', display)
    # cv.imshow('bg', bg)
    cv.imshow('fg', fg)
    # cv.imshow('sub', sub)
    cv.imshow('obj', obj)
    cv.imshow('vertical', vertical)
    cv.imshow('horizoncal', horizontal)


if __name__ == '__main__':
    rospy.init_node('vision_gate_testing', anonymous=False)
    image_topic = get_topic("front")
    rospy.Subscriber(image_topic, CompressedImage, image_callback)
    while not rospy.is_shutdown():
        if image_input is None:
            continue
        find_gate()
        k = cv.waitKey(1) & 0xff
        if k == ord('q'):
            break
    cv.destroyAllWindows()
    # rospy.Service('vision_gate', vision_srv_gate(),
    #               mission_callback)
    # print_result("INIT NODE GATE", ct.GREEN)
    # rospy.spin()
    # print_result("END PROGRAM", ct.YELLOW_HL+ct.RED)
