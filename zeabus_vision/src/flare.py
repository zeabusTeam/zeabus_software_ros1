#!/usr/bin/python2.7
"""
    File name: flare.py
    Author: kam7411
    Python Version: 2.7
    About: code for finding flare
"""
import rospy
import cv2 as cv
import numpy as np
from sensor_msgs.msg import CompressedImage
from zeabus_vision.msg import vision_flare
from zeabus_vision.srv import vision_srv_flare
import color_text as ct
import vision_lib as lib

IMAGE = None
PUBLIC_TOPIC = '/vision/mission/flare/'
SUB_SAMPLING = 1
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

    if task == 'flare' and req in ['near','far']:
        return find_flare(req)


def image_callback(msg):
    global IMAGE
    arr = np.fromstring(msg.data, np.uint8)
    bgr = cv.resize(cv.imdecode(arr, 1), (0, 0),
                    fx=sub_sampling, fy=sub_sampling)
    IMAGE = bgr.copy()


def message(state=0, cx=0.0, cy=0.0, area=0.0):
    """
        group value into massage
    """
    # convert x,y to range -1 - 1
    himg,wimg = IMAGE.shape[:2]
    cx = lib.Aconvert(cx, wimg)
    cy = -1.0*lib.Aconvert(cy, himg)

    # group value into vision_flare
    msg = vision_flare()
    msg.state = state
    msg.cx = cx
    msg.cy = cy
    msg.area = area
    print msg
    return msg


def get_mask(img):
    hsv = cv.cvtColor(img, cv.COLOR_BGR2HSV)
    # upper, lower = get_color_range('yellow', 'front', '1', 'flare')
    # upper = np.array([48, 255, 255], dtype=np.uint8)
    # lower = np.array([14, 0, 0], dtype=np.uint8)
    upper = np.array([60, 255, 255], dtype=np.uint8)
    lower = np.array([2, 0, 0], dtype=np.uint8)
    mask = cv.inRange(hsv, lower, upper)
    return mask

def is_verticle_pipe(cnt, percent, rect):
    """
        Information
        Pipe    width = 100
                height = 4
    """
    pipe = {
        'width' = 100.0,
        'height' = 4.0
    }
    x, y, w, h = cv.boundingRect(cnt)
    if h <= w:
        return False

    (x, y), (w, h), angle = rect
    if not (angle >= -20 or angle <= -70):
        return False

    area_cnt = cv.contourArea(cnt)
    area_box = w * h
    w, h = max(w, h), min(w, h)

    print('area', (area_box, area_cnt, w, h))
    if area_box <= 50 or area_cnt <= 300 or w < 50:
        return False

    area_ratio_expected = 0.3
    area_ratio = area_cnt / area_box
    print('c2', area_ratio)
    if area_ratio < area_ratio_expected:
        return False

    wh_ratio_expected = (pipe['width']/pipe['height'])/2

    wh_ratio = 1.0 * w / h
    print((wh_ratio, (w, h)))
    if wh_ratio < wh_ratio_expected * percent:
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


def find_flare(req):
    global IMAGE
    if IMAGE is None:
        lib.img_is_none()
        return message(state=-1)
    mask = cv.bitwise_not(get_mask(bgr))
    obj = lib.bg_subtraction(gray)

    kernel_box = lib.get_kernel(ksize=(7, 7))
    kernel_vertical = lib.get_kernel(ksize=(1, 25))
    vertical = cv.erode(obj.copy(), kernel_vertical)
    vertical = cv.dilate(vertical.copy(), kernel_box)
    kernel_erode = lib.get_kernel(ksize=(3, 11))
    vertical = cv.erode(vertical.copy(), kernel_erode)

    vertical_pipe, no_v_pipe = find_pipe(vertical)
    mode = no_v_pipe
    if mode == 0:
        lib.print_result("NOT FOUND", ct.RED)
        lib.publish_result(display, 'bgr', PUBLIC_TOPIC + 'display')
        lib.publish_result(vertical, 'gray', PUBLIC_TOPIC + 'mask/vertical')
        lib.publish_result(horizontal, 'gray',
                           PUBLIC_TOPIC + 'mask/horizontal')
        lib.publish_result(obj, 'gray', PUBLIC_TOPIC + 'mask')
        return message()
    x, y, w, h, angle = vertical_pipe[0]
    cv.rectangle(display, (int(x - w / 2.), int(y - h / 2.)),
                 (int(x + w / 2.), int(y + h / 2.)), (0, 255, 0), 2)
    vertical_x = [(x - w / 2.), (x + w / 2.)]
    vertical_y = [(y - h / 2.), (y + h / 2.)]
    cx1,cx2 = min(cx1,cx2),max(cx1,cx2)
    cy1,cy2 = min(cy1,cy2),max(cy1,cy2)
            #     lib.print_result("NOT FOUND", ct.RED)
    #     lib.publish_result(image_result, 'bgr', PUBLIC_TOPIC + 'image_result')
    #     lib.publish_result(mask, 'gray', PUBLIC_TOPIC + 'mask')
    #     return message()

    # ROI = get_ROI(mask, case=req)
    # mode = len(ROI)
    # if mode == 0:
    #     lib.print_result("NOT FOUND", ct.RED)
    #     lib.publish_result(image_result, 'bgr', PUBLIC_TOPIC + 'image_result')
    #     lib.publish_result(mask, 'gray', PUBLIC_TOPIC + 'mask')
    #     return message()
    # elif mode >= 1:
    #     if mode == 1:
    #         lib.print_result("FOUND A FLARE", ct.GREEN)
    #     elif mode > 1:
    #         lib.print_result("FOUND BUT HAVE SOME NOISE (" +
    #                      str(mode) + ")", ct.YELLOW)
    #     cx, cy, area = get_cx(cnt=max(ROI, key=cv.contourArea))
    #     lib.publish_result(image_result, 'bgr', PUBLIC_TOPIC + 'image_result')
    #     lib.publish_result(mask, 'gray', PUBLIC_TOPIC + 'mask')
    #     return message(cx=cx, cy=cy, area=area, state=len(ROI))

if __name__ == '__main__':
    rospy.init_node('vision_flare', anonymous=False)

    IMAGE_TOPIC = lib.get_topic("front")
    rospy.Subscriber(IMAGE_TOPIC, CompressedImage, image_callback)
    rospy.Service('vision/flare', vision_srv_flare(),
                  mission_callback)
    lib.print_result("INIT NODE FLARE", ct.GREEN)

    rospy.spin()
    lib.print_result("END PROGRAM", ct.YELLOW_HL+ct.RED)
