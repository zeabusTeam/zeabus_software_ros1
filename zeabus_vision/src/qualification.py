#!/usr/bin/env python
"""
    File name: qualification
    Author: AyumiizZ
    Date created: 2019/2/16
    Python Version: 2.7
"""

import rospy
import numpy as np
import cv2 as cv
from sensor_msgs.msg import CompressedImage
from zeabus_vision.msg import vision_gate
from zeabus_vision.srv import vision_srv_gate
import color_text as ct
import vision_lib as lib
from operator import itemgetter

IMAGE = None
PROCESS_DATA = {}
PUBLIC_TOPIC = '/vision/mission/qualification/'
SUB_SAMPLING = 0.3
DEBUG = {
    'time': False,
    'console': True,
    'rqt-grid': False,
    'detail': False,
    'not-only-res': True
}


def mission_callback(msg):
    task = str(msg.task.data)
    req = str(msg.req.data)
    if DEBUG['console']:
        lib.print_mission(task, req)
    if task == 'qualify' or task == 'qualification':
        return find_qualify_pole()


def image_callback(msg):
    global IMAGE
    arr = np.fromstring(msg.data, np.uint8)
    bgr = cv.resize(cv.imdecode(arr, 1), (0, 0),
                    fx=SUB_SAMPLING, fy=SUB_SAMPLING)
    IMAGE = bgr.copy()


def message(state=0, pos=0, cx=0.0, cy=0.0, area=0.0):
    msg = vision_gate()
    msg.state = state
    msg.pos = pos
    msg.cx1 = cx
    msg.cy1 = cy
    msg.area = area
    if DEBUG['console'] or DEBUG['detail']:
        print msg
    return msg


def is_verticle_pipe(cnt, percent, rect):
    """
        Information
        Pipe    width = 100
                height = 40
    """

    x, y, w, h = cv.boundingRect(cnt)
    if h <= w:
        return False

    (x, y), (w, h), angle = rect
    if not (angle >= -25 or angle <= -65):
        return False

    area_cnt = cv.contourArea(cnt)
    area_box = w * h

    if area_box <= 50 or area_cnt <= 500:
        return False

    area_ratio_expected = 0.5
    area_ratio = area_cnt / area_box
    if area_ratio < area_ratio_expected:
        return False

    wh_ratio_expected = (100/40.)/2
    w, h = max(w, h), min(w, h)
    wh_ratio = 1.0 * w / h
    if wh_ratio < wh_ratio_expected * percent:
        return False

    return True


def find_pipe(binary):
    _, contours, _ = cv.findContours(
        binary, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)
    number_of_object = 2
    percent_pipe = 0.7
    result = []
    pipe = cv.cvtColor(binary, cv.COLOR_GRAY2BGR)

    for cnt in contours:
        (x, y), (w, h), angle = rect = cv.minAreaRect(cnt)
        w, h = max(w, h), min(w, h)
        if not is_verticle_pipe(cnt, percent_pipe, rect):
            continue

        print('c1')

        box = cv.boxPoints(rect)
        box = np.int64(box)
        if DEBUG['not-only-res']:
            cv.drawContours(pipe, [box], 0, (0, 255, 255), 2)
        result.append([int(x), int(y), int(h), int(w), angle])

    if DEBUG['not-only-res']:
        lib.publish_result(pipe, 'bgr', PUBLIC_TOPIC + 'mask/pipe')
    result = sorted(result, key=itemgetter(3), reverse=True)

    if len(result) < number_of_object:
        return result, len(result)
    else:
        return result[:number_of_object], number_of_object


def find_qualify_pole():
    global IMAGE
    if IMAGE is None:
        lib.img_is_none()
        return message(state=-1)

    display = IMAGE.copy()
    gray = cv.cvtColor(IMAGE.copy(), cv.COLOR_BGR2GRAY)
    obj = lib.bg_subtraction(gray)

    kernel_box = lib.get_kernel(ksize=(7, 7))

    kernel_vertical = lib.get_kernel(ksize=(1, 25))
    vertical = cv.erode(obj.copy(), kernel_vertical)
    vertical = cv.dilate(vertical.copy(), kernel_box)
    kernel_erode = lib.get_kernel(ksize=(3, 11))
    vertical = cv.erode(vertical.copy(), kernel_erode)

    vertical_pipe, no_pipe_v = find_pipe(vertical)

    vertical_cx1 = []
    vertical_cx2 = []
    vertical_cy1 = []
    vertical_cy2 = []
    for res in vertical_pipe:
        x, y, w, h, angle = res
        cv.rectangle(display, (int(x - w / 2.), int(y - h / 2.)),
                     (int(x + w / 2.), int(y + h / 2.)), (108, 105, 255), 2)
        vertical_cx1.append((x - w / 2.))
        vertical_cx2.append((x + w / 2.))
        vertical_cy1.append((y - h / 2.))
        vertical_cy2.append((y + h / 2.))

    himg, wimg = obj.shape[:2]
    mode = no_pipe_v
    if mode == 0:
        lib.print_result("NOT FOUND", ct.RED)
        lib.publish_result(display, 'bgr', PUBLIC_TOPIC + 'image_result')
        lib.publish_result(vertical, 'gray', PUBLIC_TOPIC + 'mask/vertical')
        lib.publish_result(obj, 'gray', PUBLIC_TOPIC + 'mask')
        return message()
    elif mode == 1:
        lib.print_result("FOUNG ONE POLE", ct.YELLOW)
        cx1 = min(vertical_cx1)
        cx2 = max(vertical_cx2)
        cy1 = min(vertical_cy1)
        cy2 = max(vertical_cy2)
    elif mode == 2:
        lib.print_result("FOUND", ct.GREEN)
        cx1 = min(max(vertical_cx1),max(vertical_cx2))
        cx2 = max(min(vertical_cx1),min(vertical_cx2))
        cy1 = min(max(vertical_cy1),max(vertical_cy2))
        cy2 = max(min(vertical_cy1),min(vertical_cy2))
    else:
        lib.print_result("FOUND POLE WITH NOISE (" +
                         str(no_pipe_v) + ")", ct.YELLOW)
        cx1 = min(max(vertical_cx1),max(vertical_cx2))
        cx2 = max(min(vertical_cx1),min(vertical_cx2))
        cy1 = min(max(vertical_cy1),max(vertical_cy2))
        cy2 = max(min(vertical_cy1),min(vertical_cy2))

    cx1, cx2 = min(cx1, cx2), max(cx1, cx2)
    cy1, cy2 = min(cy1, cy2), max(cy1, cy2)

    cx1, cx2 = max(cx1, 0), min(cx2, wimg)
    cy1, cy2 = max(cy1, 0), min(cy2, himg)

    right_excess = (cx2 > 0.95*wimg)
    left_excess = (cx1 < (0.05*wimg))
    if (right_excess and not left_excess):
        pos = 1
    elif (not right_excess and left_excess):
        pos = -1
    else:
        pos = 0

    cv.rectangle(display, (int(cx1), int(cy1)),
         (int(cx2), int(cy2)), (0, 255, 0), 3)

    cv.circle(display, (int((cx1+cx2)/2), int((cy1+cy2)/2)),
              3, (0, 255, 255), -1)
    area = 1.0*abs(cx2-cx1)*abs(cy2-cy1)/(himg*wimg)
    lib.publish_result(display, 'bgr', PUBLIC_TOPIC + 'image_result')
    lib.publish_result(vertical, 'gray', PUBLIC_TOPIC + 'mask/vertical')
    lib.publish_result(obj, 'gray', PUBLIC_TOPIC + 'mask')
    cx = lib.Aconvert((cx1+cx2)/2, wimg)
    cy = -1.0*lib.Aconvert((cy1+cy2)/2, himg)
    return message(state=mode, cx=cx, cy=cy, pos=pos, area=area)


if __name__ == '__main__':
    rospy.init_node('vision_qualification', anonymous=False)
    rospy.Subscriber(lib.get_topic("front"), CompressedImage, image_callback)
    rospy.Service('vision/qualification', vision_srv_gate(),
                  mission_callback)
    lib.print_result("INIT NODE QUALIFICATION", ct.GREEN)
    rospy.spin()
    lib.print_result("END PROGRAM", ct.YELLOW_HL+ct.RED)
