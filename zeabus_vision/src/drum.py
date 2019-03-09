#!/usr/bin/python2.7
"""
    File name: drum.py
    Author: AyumiizZ
    Python Version: 2.7
    About: code for finding drum
"""
import rospy
import numpy as np
import cv2 as cv
from sensor_msgs.msg import CompressedImage
from zeabus_vision.msg import vision_drum
from zeabus_vision.srv import vision_srv_drum
import vision_lib as lib
import color_text as ct
from time import time
from image_lib import Image
import math
from operator import itemgetter

class Log:
    def __init__(self):
        self.time = None
        self.history = []
        self.shade = "light"
        self.max_history_length = 5

    def update_time(self):
        if self.time is not None and self.time - time() > 2:
            self.__init__()
        self.time = time()
        self.swap_shade()

    def swap_shade(self):
        if self.history != [] and sum(self.history)/len(self.history) < 0.5:
            if self.shade == "dark":
                self.shade = "light"
            elif self.shade == "light":
                self.shade = "dark"
            self.history = []

    def append_history(self, state):
        if len(self.history) >= self.max_history_length:
            self.history[:-4]
        if state >= 2:
            self.history.append(1.0/state)
        else:
            self.history.append(state)


public_topic = '/vision/mission/drum/'
image = Image(sub_sampling=0.5)
DEBUG = {
    'by-pass-mat': True,
    'console': True
}
log = Log()


def mission_callback(msg):
    task = str(msg.task.data)
    req = str(msg.req.data)
    if DEBUG['console']:
        lib.print_mission(task, req)

    drum_option = ['red', 'blue']
    return_option = ['top-bottom', 'left-right', 'tb', 'lr', 'center','c']
    if task == 'mat':
        return find_mat() 
    elif task in drum_option and req in return_option:
        return find_drum(task, req)
    elif task == 'golf':
        return find_golf(task)


def message(state=0, cx1=0, cy1=0, cx2=0, cy2=0, forward=False, backward=False, left=False, right=False, area=0):
    if state > 0:
        himg, wimg = image.display.shape[:2]
        cx1 = lib.Aconvert(cx1, wimg)
        cy1 = -1.0*lib.Aconvert(cy1, himg)
        cx2 = lib.Aconvert(cx2, wimg)
        cy2 = -1.0*lib.Aconvert(cy2, himg)
        area = 1.0*area/(himg*wimg)

    msg = vision_drum()
    msg.state = state
    msg.cx1 = cx1
    msg.cy1 = cy1
    msg.cx2 = cx2
    msg.cy2 = cy2
    msg.forward = forward
    msg.backward = backward
    msg.left = left
    msg.right = right
    msg.area = area
    print msg
    return msg


def get_mask(color, shade=None):
    image.get_hsv()
    blur = cv.medianBlur(image.hsv, 5)
    _, s, _ = cv.split(blur)
	
    # foregroud_mask = s

    foregroud_mask = cv.threshold(
        s, 0, 255, cv.THRESH_BINARY + cv.THRESH_OTSU)[1]

    if color == "blue":
        # upper = np.array([161, 197, 195], dtype=np.uint8)
        # lower = np.array([49, 9, 63], dtype=np.uint8)
        upper = np.array([120, 255, 255], dtype=np.uint8)
        lower = np.array([90, 160, 0], dtype=np.uint8)
    if color == "yellow":
        if shade == "dark":
            # upper = np.array([66, 255, 255], dtype=np.uint8)
            # lower = np.array([37, 98, 0], dtype=np.uint8)
            upper = np.array([47, 255, 255], dtype=np.uint8)
            lower = np.array([20, 17, 228], dtype=np.uint8)
        elif shade == "light":
            upper = np.array([60, 255, 255], dtype=np.uint8)
            lower = np.array([27, 160, 97], dtype=np.uint8)
    if color == "green":
        upper = np.array([90, 255, 255], dtype=np.uint8)
        lower = np.array([60, 160, 0], dtype=np.uint8)

    if color != 'red':
        foregroud = cv.bitwise_and(image.hsv, image.hsv, mask=foregroud_mask)
        mask = cv.inRange(foregroud, lower, upper)

    if color == "red":
        # upper = np.array([161, 197, 195], dtype=np.uint8)
        # lower = np.array([49, 9, 63], dtype=np.uint8)
        upper = np.array([179, 255, 255], dtype=np.uint8)
        lower = np.array([120, 0, 0], dtype=np.uint8)
        mask = cv.inRange(image.hsv, lower, upper)
    return mask


def get_contour(mask, request):
    if request in ['red', 'blue'] and not DEBUG['by-pass-mat']:
        mat_mask = get_mat()
        if mat_mask.shape != mask.shape:
            return cv.findContours(
                mask, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)[1]

        lib.publish_result(mat_mask, 'gray', public_topic+'mask/mat/processed')
        intersect = cv.bitwise_and(mat_mask, mask)
        return cv.findContours(
            intersect, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)[1]

    elif request == 'golf' or DEBUG['by-pass-mat']:
        return cv.findContours(
            mask, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)[1]


def get_obj(mask, request):
    contours = get_contour(mask, request)
    obj = []
    for cnt in contours:
        area = cv.contourArea(cnt)
        check_area = 450 if request == 'golf' else 4500
        if area < check_area:
            continue

        x, y, w, h = cv.boundingRect(cnt)
        cv.rectangle(image.display, (x, y), (x+w, y+h),
                     lib.get_color('yellow'), 2)
        left_cy = lib.most_point(cnt, 'left')[1]
        right_cy = lib.most_point(cnt, 'right')[1]
        # if abs(left_cy-right_cy) > 0.2 * h:
        #    continue

        cv.rectangle(image.display, (x, y), (x+w, y+h),
                     lib.get_color('yellow'), 2)
        obj.append(cnt)

    if obj != []:
        return max(obj, key=cv.contourArea)
    return []


def get_excess(cnt):
    himg, wimg = image.display.shape[:2]
    x, y, w, h = cv.boundingRect(cnt)
    top_excess = (y < (0.05*wimg))
    right_excess = ((x+w) > 0.95*wimg)
    left_excess = (x < (0.05*wimg))
    bottom_excess = ((y+h) > 0.95*himg)
    return top_excess, bottom_excess, left_excess, right_excess


def get_cx(cnt, return_option=None):
    himg, wimg = image.display.shape[:2]
    if return_option is not None:
        (cx, cy) = lib.center_of_contour(cnt)
        cv.circle(image.display, (cx, cy), 5, (0, 0, 255), -1)
    x, y, w, h = cv.boundingRect(cnt)
    cv.rectangle(image.display, (x, y), (x+w, y+h),
                     lib.get_color('green'), 3)
    area = w * h
    if return_option in [None,'c','center']:
        cx1, cx2 = max(min(x,x+w),0),min(max(x,x+w),wimg)
        cy1, cy2 = max(min(y,y+h),0),min(max(y,y+h),himg)
    elif return_option in ['top-bottom','tb']:
        cx1, cy1 = lib.most_point(cnt,'bottom')
        cx2, cy2 = lib.most_point(cnt,'top')
    elif return_option in ['left-right','lr']:
        cx1, cy1 = lib.most_point(cnt,'left')
        cx2, cy2 = lib.most_point(cnt,'right')
    return cx1, cy1, cx2, cy2, area


def get_mat():
    mat = get_mask("green")
    lib.publish_result(mat, 'gray', public_topic+'mask/mat/unprocessed')
    contours = cv.findContours(
        mat, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)[1]
    max_cnt = max(contours, key=cv.contourArea)
    rect = cv.minAreaRect(max_cnt)
    box = cv.boxPoints(rect)
    box = np.int64(box)
    himg, wimg = mat.shape[:2]
    mat_mask = np.zeros((himg, wimg), np.uint8)
    cv.drawContours(mat_mask, [box], 0, (255), -1)
    return mat_mask

def find_mat():
    if image.bgr is None:
        lib.img_is_none()
        return message(state=-1)
    image.renew_display()
    himg, wimg = image.display.shape[:2]
    mat_mask = get_mask("green")
    contours = cv.findContours(
        mat_mask, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)[1]
    processed_contours = []
    for cnt in contours:
        if cv.contourArea(cnt) < 1000:
            continue
        processed_contours.append(cnt)
    state = len(processed_contours)
    lib.publish_result(mat_mask, 'gray', public_topic + 'mask/mat')
    lib.publish_result(image.display, 'bgr', public_topic+'display/mat')
    if state == 0:
        lib.print_result("CANNOT FOUND MAT", ct.RED)
        return message()
    lib.print_result("FOUND MAT",ct.GREEN)
    return message(state=1)

def temp_function(cnt):
    himg, wimg = image.display.shape[:2]
    mat_mask = np.zeros((himg, wimg), np.uint8)
    (x,y),radius = cv.minEnclosingCircle(cnt)
    center = (int(x),int(y))
    radius = int(radius)
    cv.circle(mat_mask,center,radius,(255),-1)
    lib.publish_result(mat_mask,'gray',public_topic+'mask/enclose')

def find_drum(color, return_option):
    if image.bgr is None:
        lib.img_is_none()
        return message(state=-1)
    image.renew_display()
    himg, wimg = image.display.shape[:2]
    drum_mask = get_mask(color)
    obj = get_obj(drum_mask, color)
    state = obj != []
    if state == 0:
        lib.print_result("CANNOT FOUND DRUM", ct.RED)
        lib.publish_result(drum_mask, 'gray', public_topic+'mask')
        lib.publish_result(image.display, 'bgr', public_topic+'display')
        return message()
    temp_function(obj)
    lib.print_result("FOUND DRUM", ct.GREEN)
    cv.circle(image.display, lib.most_point(
        obj, 'right'), 10, (255, 255, 0), -1)
    cv.circle(image.display, lib.most_point(obj, 'left'), 10, (0, 255, 255), -1)
    cx1, cy1, cx2, cy2, area = get_cx(obj,return_option=return_option)
    t1 = lib.Aconvert(cx1, wimg)
    ty1 = -1.0*lib.Aconvert(cy1, himg)
    t2 = lib.Aconvert(cx2, wimg)
    ty2 = -1.0*lib.Aconvert(cy2, himg)
    forward, backward, left, right = get_excess(obj)
    cv.putText(image.display,"pt1 = ({:.2f},{:.2f})".format(t1,ty1), (0,himg-100), cv.FONT_HERSHEY_SIMPLEX, 2, 255,5)
    cv.putText(image.display,"pt2 = ({:.2f},{:.2f})".format(t2,ty2), (0,himg-20), cv.FONT_HERSHEY_SIMPLEX, 2, 255,5)
    lib.publish_result(drum_mask, 'gray', public_topic+'mask')
    lib.publish_result(image.display, 'bgr', public_topic+'display')
    return message(state=state, cx1=cx1, cy1=cy1, cx2=cx2, cy2=cy2, forward=forward,
                   backward=backward, left=left, right=right, area=area)


# def find_golf(objective):
#     if image.bgr is None:
#         lib.img_is_none()
#         return message(state=-1)
#     log.update_time()
#     himg, wimg = image.bgr.shape[:2]
#     golf_mask = get_mask("yellow", shade=log.shade)
#     obj = get_obj(golf_mask, objective)
#     state = len(obj)
#     log.append_history(state)
#     if state == 0:
#         lib.print_result("CANNOT FOUND GOLF", ct.RED)
#         lib.publish_result(golf_mask, 'gray', public_topic+'mask/golf')
#         lib.publish_result(image.display, 'bgr', public_topic+'image_result')
#         return message()
#     elif state >= 1:
#         if state == 1:
#             lib.print_result("FOUND GOLF", ct.GREEN)
#         cx1, cy1, cx2, cy2, area = get_cx(obj)
#         forward, backward, left, right = get_excess(obj)
#         lib.publish_result(golf_mask, 'gray', public_topic+'mask/golf')
#         lib.publish_result(image.bgr, 'bgr', public_topic+'image_result')
#         return message(state=state, cx1=cx1, cy1=cy1, cx2=cx2, cy2=cy2, forward=forward,
#                        backward=backward, left=left, right=right, area=area)

def find_golf(objective):
    if image.bgr is None:
        lib.img_is_none()
        return message(state=-1)
    image.renew_display()
    image.get_gray()
    obj = lib.bg_subtraction(image.gray,mode='pos')
    contours= cv.findContours(obj.copy(), cv.RETR_EXTERNAL, cv.CHAIN_APPROX_NONE)[1]
    circle = []
    for cnt in contours:
        area_cnt = cv.contourArea(cnt)
        (x,y),radius = cv.minEnclosingCircle(cnt)
        center = (int(x),int(y))
        area_cir = math.pi*(radius**2)
        if area_cir <= 0 or area_cnt/area_cir < 0.8:
            continue
        cv.circle(image.display,center,int(radius),lib.get_color('yellow'),3)
        circle.append([x,y,radius])

    state = len(circle)

    if state == 0:
        lib.print_result("CANNOT FOUND GOLF", ct.RED)
        lib.publish_result(obj, 'gray', public_topic+'golf/mask')
        lib.publish_result(image.display, 'bgr', public_topic+'golf/display')
        return message()
    himg, wimg = image.display.shape[:2]
    lib.print_result("FOUND GOLF", ct.GREEN)
    circle = sorted(circle, key=itemgetter(2),reverse=True) 
    circle = circle[0]
    x,y,radius = circle
    cv.circle(image.display,(int(x),int(y)),int(radius),lib.get_color('green'),3)
    x1, x2 = max(x-radius, 0), min(x+radius, wimg)
    y1, y2 = max(y-radius, 0), min(y+radius, himg)
    area = math.pi*(radius**2)
    forward, backward, left, right = get_excess(obj)
    lib.publish_result(obj, 'gray', public_topic+'golf/mask')
    lib.publish_result(image.display, 'bgr', public_topic+'golf/display')
    return message(state=state, cx1=x1, cy1=y1, cx2=x2, cy2=y2, forward=forward,
                    backward=backward, left=left, right=right, area=area)

if __name__ == '__main__':
    rospy.init_node('vision_drum', anonymous=False)

    image_topic = image.topic("bottom")
    rospy.Subscriber(image_topic, CompressedImage, image.callback)
    rospy.Service('vision/drum', vision_srv_drum(),
                  mission_callback)
    lib.print_result("INIT NODE DRUM", ct.GREEN)

    rospy.spin()
    lib.print_result("END PROGRAM", ct.YELLOW_HL+ct.RED)
