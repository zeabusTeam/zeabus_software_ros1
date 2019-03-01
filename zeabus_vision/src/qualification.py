#!/usr/bin/python2.7
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
from time import time


class Log:
    def __init__(self):
        self.time = None
        self.cx1 = self.cx2 = None
        self.cy1 = self.cy2 = None
        self.state = None
        self.assume = None

    def update_time(self):
        if self.time is not None and self.time - time() > 5:
            self.cx1 = self.cx2 = None
            self.cy1 = self.cy2 = None
            self.state = self.assume = None
        self.time = time()

    def save_data(self, cx1=None, cx2=None, cy1=None, cy2=None, state=None):
        self.cx1 = cx1
        self.cx2 = cx2
        self.cy1 = cy1
        self.cy2 = cy2
        self.state = state

    def assume_pole(self, mode, x, y):
        if self.state == 2 and self.assume is None and mode == 1:
            line1 = abs(self.cx1 - x)
            line2 = abs(self.cx2 - x)
            if line1 > line2:
                self.assume = 'Right'
            else:
                self.assume = 'Left'
        elif mode == 2:
            self.assume = None
                        


    def assume_to_pos(self):
        if self.assume == 'Right':
            return 1
        if self.assume == 'Left':
            return -1
        return 0


IMAGE = None
PUBLIC_TOPIC = '/vision/mission/qualification/'
SUB_SAMPLING = 0.3
DEBUG = {
    'print': False,
    'time': False,
    'console': True,
    'rqt-grid': False,
    'detail': False,
    'not-only-res': True
}

log = Log()


def mission_callback(msg):
    task = str(msg.task.data)
    req = str(msg.req.data)
    if DEBUG['console']:
        lib.print_mission(task, req)
    if task in ['qualify', 'qualification'] and req.lower() != 'sevinar':
        return find_qualify_pole()


def image_callback(msg):
    global IMAGE
    arr = np.fromstring(msg.data, np.uint8)
    bgr = cv.resize(cv.imdecode(arr, 1), (0, 0),
                    fx=SUB_SAMPLING, fy=SUB_SAMPLING)
    IMAGE = bgr.copy()


def message(state=0, pos=0, cx=0.0, cy=0.0, area=0.0):
    if(state > 0):
        himg, wimg = IMAGE.shape[:2]
        cx = lib.Aconvert(cx, wimg)
        cy = -1.0*lib.Aconvert(cy, himg)
    msg = vision_gate()
    msg.state = state
    msg.pos = pos
    msg.cx1 = cx
    msg.cy1 = cy
    msg.area = area
    if DEBUG['console'] or DEBUG['detail']:
        print msg
        rospy.sleep(0.05)
        lib.clear_screen()
    return msg


def is_verticle_pipe(cnt, percent, rect):
    """
        Information
        Pipe    width = 100
                height = 4
    """

    x, y, w, h = cv.boundingRect(cnt)
    if h <= w:
        return False

    (x, y), (w, h), angle = rect
    if not (angle >= -25 or angle <= -65):
        return False

    area_cnt = cv.contourArea(cnt)
    area_box = w * h
    w, h = max(w, h), min(w, h)
    if DEBUG['print']:
        print('area', (area_box, area_cnt, w, h))
    if area_box <= 50 or area_cnt <= 300 or w < 50:
        return False

    area_ratio_expected = 0.3
    area_ratio = area_cnt / area_box
    if DEBUG['print']:
        print('c2', area_ratio)
    if area_ratio < area_ratio_expected:
        return False

    wh_ratio_expected = (100/4.)/2

    wh_ratio = 1.0 * w / h
    if DEBUG['print']:
        print((wh_ratio, (w, h)))
    if wh_ratio < wh_ratio_expected * percent:
        return False

    return True


def find_pipe(binary):
    _, contours, _ = cv.findContours(
        binary, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)
    number_of_object = 2
    percent_pipe = 0.6
    result = []
    pipe = cv.cvtColor(binary, cv.COLOR_GRAY2BGR)

    for cnt in contours:
        (x, y), (w, h), angle = rect = cv.minAreaRect(cnt)
        w, h = max(w, h), min(w, h)
        if not is_verticle_pipe(cnt, percent_pipe, rect):
            continue

        if DEBUG['print']:
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
        closest_pair = []
        min_dist = 2000
        for i in range(len(result)):
            for j in range(i+1, len(result)):
                dist_y = abs(result[j][1] - result[i][1])
                dist_x = abs(result[j][0] - result[i][0])
                if dist_x >= 50 and dist_y < min_dist:
                    min_dist = dist_y
                    closest_pair = [result[i], result[j]]
        if closest_pair == []:
            return result[:1], 1
        else:
            return closest_pair, 2


def find_qualify_pole():
    global IMAGE, log
    if IMAGE is None:
        lib.img_is_none()
        return message(state=-1)

    log.update_time()

    display = IMAGE.copy()
    gray = cv.cvtColor(IMAGE.copy(), cv.COLOR_BGR2GRAY)
    obj = lib.bg_subtraction(gray,mode='neg')

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
        lib.publish_result(display, 'bgr', PUBLIC_TOPIC + 'display')
        lib.publish_result(vertical, 'gray', PUBLIC_TOPIC + 'mask/vertical')
        lib.publish_result(obj, 'gray', PUBLIC_TOPIC + 'mask')
        return message()
    elif mode == 1:
        lib.print_result("FOUNG ONE POLE", ct.YELLOW)
    elif mode == 2:
        lib.print_result("FOUND", ct.GREEN)

    cx1 = min(vertical_cx2)
    cx2 = max(vertical_cx1)
    cy1 = max(vertical_cy1)
    cy2 = min(vertical_cy2)

    cx1, cx2 = min(cx1, cx2), max(cx1, cx2)
    cy1, cy2 = min(cy1, cy2), max(cy1, cy2)

    cx1, cx2 = max(cx1, 0), min(cx2, wimg)
    cy1, cy2 = max(cy1, 0), min(cy2, himg)

    cv.rectangle(display, (int(cx1), int(cy1)),
                 (int(cx2), int(cy2)), (0, 255, 0), 3)
    cv.circle(display, (int((cx1+cx2)/2), int((cy1+cy2)/2)),
              3, (0, 255, 255), -1)

    area = 1.0*abs(cx2-cx1)*abs(cy2-cy1)/(himg*wimg)
    lib.publish_result(display, 'bgr', PUBLIC_TOPIC + 'display')
    lib.publish_result(vertical, 'gray', PUBLIC_TOPIC + 'mask/vertical')
    lib.publish_result(obj, 'gray', PUBLIC_TOPIC + 'mask')
    log.assume_pole(mode=mode, x=cx1, y=cy1)
    pos = log.assume_to_pos()
    log.save_data(state=mode, cx1=cx1, cx2=cx2, cy1=cy1, cy2=cy2)
    return message(state=mode, cx=(cx1+cx2)/2, cy=(cy1+cy2)/2, pos=pos, area=area)


if __name__ == '__main__':
    rospy.init_node('vision_qualification', anonymous=False)
    IMAGE_TOPIC = lib.get_topic("front")
    # IMAGE_TOPIC = '/vision/front/image_raw/compressed'
    rospy.Subscriber(IMAGE_TOPIC, CompressedImage, image_callback)
    rospy.Service('vision/qualification', vision_srv_gate(),
                  mission_callback)
    lib.print_result("INIT NODE QUALIFICATION", ct.GREEN)
    rospy.spin()
    lib.print_result("END PROGRAM", ct.YELLOW_HL+ct.RED)
