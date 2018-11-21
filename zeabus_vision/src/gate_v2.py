#!/usr/bin/env python
"""
    File name:gate.py
    Author: skconan
    Date created: 2018/10/13
    Python Version: 2.7
    Rewrite: AyumiizZ
"""

import cv2 as cv
import color_text as ct
from vision_lib import *
import constant as CONST
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


def message(state=0, pos=0, cx1=0.0, cy1=0.0, cx2=0.0, cy2=0.0, area=0.0):
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


def what_align(cnt):
    x, y, w, h = cv.boundingRect(cnt)
    # print(w > h)
    if w > h:
        return 'h'
    return 'v'


def is_pipe(cnt, percent):
    """
        Information
        Pipe    width = 150
                height = 40
    """
    (x, y), (w, h), angle = cv.minAreaRect(cnt)

    area_ratio_expected = 0.50
    wh_ratio_expected = 9  # (150 / 40.) * 2
    w, h = max(w, h), min(w, h)
    area_cnt = cv.contourArea(cnt)
    area_box = w * h
    # print('percent', percent)
    # print('area', area_cnt)
    # print('areab', area_box)
    # print('w', w, 'h', h)
    if (area_box == 0) or area_cnt <= 500:
        return False

    area_ratio = area_cnt / area_box
    wh_ratio = 1.0 * w / h
    # print('angle', angle)
    # print("area:", area_ratio, "wh", wh_ratio)
    if (not (area_ratio > area_ratio_expected
             and wh_ratio > wh_ratio_expected * percent
             and (angle >= -20 or angle <= -70))):
        return False

    # print("area:", area_ratio, "wh", wh_ratio)
    return True


def find_pipe(binary, align):
    _, contours, _ = cv.findContours(
        binary, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)
    number_of_object = 2 if align == 'v' else 1
    percent_pipe = 0.4 if align == 'h' else 0.8
    result = []
    pipe = cv.cvtColor(binary, cv.COLOR_GRAY2BGR)

    for cnt in contours:
        (x, y), (w, h), angle = rect = cv.minAreaRect(cnt)
        w, h = max(w, h), min(w, h)
        # print('na', not what_align(cnt) == align, align)
        if not is_pipe(cnt, percent_pipe) or not what_align(cnt) == align:
            continue

        box = cv.boxPoints(rect)
        box = np.int64(box)
        if align == 'v':
            cv.drawContours(pipe, [box], 0, (0, 255, 255), 2)
            result.append([int(x), int(y), int(h), int(w), angle])
        else:
            cv.drawContours(pipe, [box], 0, (255, 0, 255), 2)
            result.append([int(x), int(y), int(w), int(h), angle])
    # cv.imshow('pipe',pipe)
    if align == 'v':
        publish_result(pipe, 'bgr', public_topic + 'mask/vpipe')
    if align == 'h':
        publish_result(pipe, 'bgr', public_topic+'mask/hpipe')
    if align == 'v':
        result = sorted(result, key=itemgetter(3), reverse=True)
    else:
        result = sorted(result, key=itemgetter(2), reverse=True)

    if len(result) < number_of_object:
        return result, len(result)
    else:
        return result[:number_of_object], number_of_object


def find_gate():
    global image_input, stat, previous_bg, first_object, tracker, tracker_status, object_box, untrack_frame
    if image_input is None:
        img_is_none()
        return message(state=-1)
    gray = cv.cvtColor(image_input.copy(), cv.COLOR_BGR2GRAY)

    obj = bg_subtraction(gray)

    kernel_vertical = get_kernel(ksize=(1, 21))
    kernel_horizontal = get_kernel(ksize=(21, 1))
    kernel_box = get_kernel(ksize=(7, 7))

    vertical = cv.erode(obj.copy(), kernel_vertical)
    vertical = cv.dilate(vertical.copy(), kernel_box)
    kernel_erode = get_kernel(ksize=(3, 11))
    vertical = cv.erode(vertical.copy(), kernel_erode)

    horizontal = cv.erode(obj.copy(), kernel_horizontal)
    horizontal = cv.dilate(horizontal.copy(), kernel_box)
    kernel_erode = get_kernel(ksize=(11, 3))
    horizontal = cv.erode(horizontal.copy(), kernel_erode)

    vertical_pipe, no_pipe_v = find_pipe(vertical, 'v')
    horizontal_pipe, no_pipe_h = find_pipe(horizontal, 'h')
    # horizontal_pipe, no_pipe_h = [],0

    display = image_input.copy()

    horizontal_cx = []
    horizontal_cy = []
    vertical_cx = []
    vertical_cy1 = []
    vertical_cy2 = []

    for res in horizontal_pipe:
        # green
        x, y, w, h, angle = res
        # print('hor', res)
        cv.rectangle(display, (int(x - w / 2.), int(y - h / 2.)),
                     (int(x + w / 2.), int(y + h / 2.)), (0, 255, 0), 2)
        horizontal_cx.append([(x - w / 2.), (x + w / 2.)])
        horizontal_cy.append(y)
    # print(image_input.shape)
    for res in vertical_pipe:
        # pink
        x, y, w, h, angle = res
        # print('ver', res)
        cv.rectangle(display, (int(x - w / 2.), int(y - h / 2.)),
                     (int(x + w / 2.), int(y + h / 2.)), (108, 105, 255), 2)
        vertical_cx.append(x)
        vertical_cy1.append((y - h / 2.))
        vertical_cy2.append((y + h / 2.))
    # print(vertical_cx)
    # print(horizontal_cx)
    himg, wimg = obj.shape[:2]
    mode = 0
    if no_pipe_h == 1:
        if no_pipe_v == 2:
            mode = 1
        elif no_pipe_v == 1:
            mode = 2
        elif no_pipe_v == 0:
            mode = 3
    elif no_pipe_h == 0 and no_pipe_v == 2:
        mode = 4
    else:
        mode = 0
    # print("no_pipe_v", no_pipe_v)
    # print("no_pipe_h", no_pipe_h)
    if mode == 0:
        print_result("NOT FOUND", ct.RED)
        publish_result(display, 'bgr', public_topic + 'image_result')
        publish_result(vertical, 'gray', public_topic + 'mask/vertical')
        publish_result(horizontal, 'gray', public_topic + 'mask/horizontal')
        publish_result(obj, 'gray', public_topic + 'mask')
        return message()
    elif mode == 1:
        print_result("FOUND GATE", ct.GREEN)
        cx1 = (horizontal_cx[0][0] + min(vertical_cx)) / 2.
        cx2 = (horizontal_cx[0][1] + max(vertical_cx)) / 2.
        cy1 = horizontal_cy[0]
    elif mode == 2:
        print_result("FOUND ONE V AND ONE H", ct.YELLOW)
        cx1 = horizontal_cx[0][0]
        cx2 = horizontal_cx[0][1]
        cy1 = horizontal_cy[0]
    elif mode == 3:
        print_result("FOUND ONE H", ct.YELLOW)
        cx1 = horizontal_cx[0][0]
        cx2 = horizontal_cx[0][1]
        cy1 = horizontal_cy[0]
    elif mode == 4:
        print_result("FOUND TWO V", ct.YELLOW)
        cx1 = min(vertical_cx)
        cx2 = max(vertical_cx)
        cy1 = (sum(vertical_cy1)+min(vertical_cy1))/len(vertical_cy1)
    cy2 = (sum(vertical_cy2)+max(vertical_cy2))/(len(vertical_cy2)+1) if no_pipe_v != 0 else himg
# horizontal_cy[0] + cx2-cx1
    right_excess = (cx2 > 0.95*wimg)
    left_excess = (cx1 < (0.05*wimg))
    if (right_excess and not left_excess):
        pos = 1
    elif (not right_excess and left_excess):
        pos = -1
    else:
        pos = 0
    # cv.line(display, (0, int(cy1)), (wimg, int(cy1)), (255, 255, 255), 5)
    # cv.line(display, (int(cx1), 0), (int(cx1), himg), (255, 0, 0), 5)
    # cv.line(display, (int(cx2), 0), (int(cx2), himg), (0, 255, 0), 5)
    cv.rectangle(display, (int(cx1), int(cy1)), (int(cx2), int(cy2)), (0, 255, 0), 3)
    area = 1.0*abs(cx2-cx1)*abs(cy1-cy2)/(himg*wimg)
    publish_result(display, 'bgr', public_topic + 'image_result')
    publish_result(vertical, 'gray', public_topic + 'mask/vertical')
    publish_result(horizontal, 'gray', public_topic + 'mask/horizontal')
    publish_result(obj, 'gray', public_topic + 'mask')
    cx1 = Aconvert(cx1, wimg)
    cx2 = Aconvert(cx2, wimg)
    cy1 = -1.0*Aconvert(cy1, himg)
    cy2 = -1.0*Aconvert(cy2, himg)
    return message(state=mode, cx1=cx1, cx2=cx2, cy1=cy1, cy2=cy2, pos=pos, area=area)


if __name__ == '__main__':
    rospy.init_node('vision_gate', anonymous=False)
    image_topic = get_topic("front")
    rospy.Subscriber(image_topic, CompressedImage, image_callback)
    rospy.Service('vision/gate', vision_srv_gate(),
                  mission_callback)
    print_result("INIT NODE GATE", ct.GREEN)
    rospy.spin()
    print_result("END PROGRAM", ct.YELLOW_HL+ct.RED)
