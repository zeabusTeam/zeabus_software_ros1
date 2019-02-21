#!/usr/bin/env python
"""
    File name:gate.py
    Author: skconan, AyumiizZ
    Date created: 2018/10/13
    Date edit: 2018/1/23 
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
PUBLIC_TOPIC = '/vision/mission/gate/'
SUB_SAMPLING = 0.3
DEBUG = {
    'time': False,
    'console': True,
    'detail': False,
    'not-only-res': True
}


def mission_callback(msg):
    task = str(msg.task.data)
    req = str(msg.req.data)
    if DEBUG['console']:
        lib.print_mission(task, req)
    if task == 'gate':
        return find_gate()


def image_callback(msg):
    global IMAGE
    arr = np.fromstring(msg.data, np.uint8)
    bgr = cv.resize(cv.imdecode(arr, 1), (0, 0),
                    fx=SUB_SAMPLING, fy=SUB_SAMPLING)
    IMAGE = bgr.copy()


def message(state=0, pos=0, x1=0.0, y1=0.0, x2=0.0, y2=0.0, area=0.0):
    """
        group value into massage
    """
    # convert x,y to range -1 - 1
    himg, wimg = IMAGE.shape[:2]
    x1 = lib.Aconvert(x1, wimg)
    x2 = lib.Aconvert(x2, wimg)
    y1 = -1.0*lib.Aconvert(y1, himg)
    y2 = -1.0*lib.Aconvert(y2, himg)

    # group value into vision_gate
    msg = vision_gate()
    msg.state = state
    msg.pos = pos
    msg.cx1 = x1
    msg.cy1 = y1
    msg.cx2 = x2
    msg.cy2 = y2
    msg.area = area
    if DEBUG['console'] or DEBUG['detail']:
        print msg
    return msg


def what_align(cnt):
    """
        return direction of pipe
    """
    x, y, w, h = cv.boundingRect(cnt)
    return 'h' if w > h else 'v'


def is_pipe(cnt, percent, rect):
    """
        Information
        Pipe    width = 150
                height = 40
    """
    (x, y), (w, h), angle = rect
    if -70 <= angle and angle <= -20:
        return False

    w, h = max(w, h), min(w, h)
    wh_ratio = 1.0 * w / h
    wh_ratio_expected = (150 / 4.) / 2
    if wh_ratio < wh_ratio_expected * percent:
        return False

    area_cnt = cv.contourArea(cnt)
    area_box = w * h
    if area_box < 10 or area_cnt <= 500:
        return False

    area_ratio_expected = 0.50
    area_ratio = area_cnt / area_box
    if area_ratio < area_ratio_expected:
        return False

    return True


def find_pipe(binary, align):
    _, contours, _ = cv.findContours(
        binary, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)
    number_of_object = 2 if align == 'v' else 1
    percent_pipe = 0.8 if align == 'v' else 0.2
    topic = 'mask/pipe/vertical' if align == 'v' else 'mask/pipe/horizontal'
    color = (0, 255, 255) if align == 'v' else (255, 0, 255)
    pipe = cv.cvtColor(binary, cv.COLOR_GRAY2BGR)
    result = []

    for cnt in contours:
        (x, y), (w, h), angle = rect = cv.minAreaRect(cnt)
        this_align = what_align(cnt)
        if not is_pipe(cnt, percent_pipe, rect) or not this_align == align:
            continue
        (w, h) = (min(w, h), max(w, h)) if align == 'v' else (max(w, h), min(w, h))
        box = cv.boxPoints(rect)
        box = np.int64(box)
        cv.drawContours(pipe, [box], 0, color, 2)
        result.append([int(x), int(y), int(w), int(h), angle])

    lib.publish_result(pipe, 'bgr', PUBLIC_TOPIC + topic)
    result = sorted(result, key=itemgetter(3) if align == 'v' else itemgetter(2), reverse=True)

    if len(result) <= number_of_object:
        return result, len(result)

    # find closet
    closest_pair = []
    min_dist = 2000
    for i in range(len(result)):
        for j in range(i+1, len(result)):
            dist_x = abs(result[j][0] - result[i][0])
            dist_y = abs(result[j][1] - result[i][1])
            if dist_x >= 50 and dist_y < min_dist:
                min_dist = dist_y
                closest_pair = [result[i], result[j]]

    if closest_pair == []:
        return result[:1], 1
    return closest_pair, 2


def find_gate():
    global IMAGE
    if IMAGE is None:
        lib.img_is_none()
        return message(state=-1)

    display = IMAGE.copy()
    gray = cv.cvtColor(IMAGE.copy(), cv.COLOR_BGR2GRAY)
    obj = lib.bg_subtraction(gray)

    kernel_box = lib.get_kernel(ksize=(7, 7))

    kernel_vertical = lib.get_kernel(ksize=(1, 21))
    vertical = cv.erode(obj.copy(), kernel_vertical)
    vertical = cv.dilate(vertical.copy(), kernel_box)
    kernel_erode = lib.get_kernel(ksize=(3, 11))
    vertical = cv.erode(vertical.copy(), kernel_erode)

    kernel_horizontal = lib.get_kernel(ksize=(21, 1))
    horizontal = cv.erode(obj.copy(), kernel_horizontal)
    horizontal = cv.dilate(horizontal.copy(), kernel_box)
    kernel_erode = lib.get_kernel(ksize=(11, 3))
    horizontal = cv.erode(horizontal.copy(), kernel_erode)

    vertical_pipe, no_pipe_v = find_pipe(vertical, 'v')
    horizontal_pipe, no_pipe_h = find_pipe(horizontal, 'h')

    # horizontal
    if no_pipe_h > 0:
        x, y, w, h, angle = horizontal_pipe[0]
        cv.rectangle(display, (int(x - w / 2.), int(y - h / 2.)),
                     (int(x + w / 2.), int(y + h / 2.)), (0, 255, 0), 2)
        horizontal_x = [(x - w / 2.), (x + w / 2.)]
        horizontal_y = [(y - h / 2.), (y + h / 2.)]

    vertical_x1 = []
    vertical_x2 = []
    vertical_y1 = []
    vertical_y2 = []
    for res in vertical_pipe:
        x, y, w, h, angle = res
        cv.rectangle(display, (int(x - w / 2.), int(y - h / 2.)),
                     (int(x + w / 2.), int(y + h / 2.)), (108, 105, 255), 2)
        vertical_x1.append((x - w / 2.))
        vertical_x2.append((x + w / 2.))
        vertical_y1.append((y - h / 2.))
        vertical_y2.append((y + h / 2.))
        
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

    if mode == 0:
        lib.print_result("NOT FOUND", ct.RED)
        lib.publish_result(display, 'bgr', PUBLIC_TOPIC + 'display')
        lib.publish_result(vertical, 'gray', PUBLIC_TOPIC + 'mask/vertical')
        lib.publish_result(horizontal, 'gray',
                           PUBLIC_TOPIC + 'mask/horizontal')
        lib.publish_result(obj, 'gray', PUBLIC_TOPIC + 'mask')
        return message()

    elif mode == 1:
        lib.print_result("FOUND GATE", ct.GREEN)
        x1 = max(min(vertical_x1),min(vertical_x2))
        x2 = min(max(vertical_x1),max(vertical_x2))
        y1 = max(min(vertical_y1),max(horizontal_y))
        y2 = min(vertical_y2)
    elif mode == 2:
        cx_h = np.mean(horizontal_x)
        cx_v = np.mean(vertical_x1 + vertical_x2)
        if cx_h > cx_v:
            lib.print_result("FOUND ONE LEFT V AND ONE H", ct.YELLOW)
            x1 = max(min(horizontal_x),max(vertical_x2))
            x2 = max(horizontal_x)
            y1 = max(max(horizontal_y),min(vertical_y1))
            y2 = min(vertical_y2)
        else:
            x1 = min(horizontal_x)
            x2 = min(min(vertical_x1),max(horizontal_x))
            y1 = max(max(horizontal_y),min(vertical_y1))
            y2 = min(vertical_y2)
    elif mode == 3:
        lib.print_result("FOUND ONE H", ct.YELLOW)
        x1 = min(horizontal_x)
        x2 = max(horizontal_x)
        y1 = max(horizontal_y)
        y2 = himg
    elif mode == 4:
        lib.print_result("FOUND TWO V", ct.YELLOW)
        x1 = min(vertical_x2)
        x2 = max(vertical_x1)
        y1 = max(vertical_y1)
        y2 = min(vertical_y2)
    right_excess = (x2 > 0.95*wimg)
    left_excess = (x1 < (0.05*wimg))
    if (right_excess and not left_excess):
        pos = 1
    elif (not right_excess and left_excess):
        pos = -1
    else:
        pos = 0

    cv.rectangle(display, (int(x1), int(y1)),
         (int(x2), int(y2)), (0, 255, 0), 3)
    cv.circle(display, (int((x1+x2)/2), int((y1+y2)/2)),
              3, (0, 255, 255), -1)
    area = 1.0*abs(x2-x1)*abs(y1-y2)/(himg*wimg)
    lib.publish_result(display, 'bgr', PUBLIC_TOPIC + 'display')
    lib.publish_result(vertical, 'gray', PUBLIC_TOPIC + 'mask/vertical')
    lib.publish_result(horizontal, 'gray', PUBLIC_TOPIC + 'mask/horizontal')
    lib.publish_result(obj, 'gray', PUBLIC_TOPIC + 'mask')
    return message(state=mode, x1=x1, x2=x2, y1=y1, y2=y2, pos=pos, area=area)


if __name__ == '__main__':
    rospy.init_node('vision_gate', anonymous=False)
    rospy.Subscriber(lib.get_topic("front"), CompressedImage, image_callback)
    rospy.Service('vision/gate', vision_srv_gate(),
                  mission_callback)
    lib.print_result("INIT NODE GATE", ct.GREEN)
    rospy.spin()
    lib.print_result("END PROGRAM", ct.YELLOW_HL+ct.RED)
