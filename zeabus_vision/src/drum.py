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

class Log:
    def __init__(self):
        self.history = None
        self.time = None
        self.history = []
        self.shade = "light"
        self.max_history_length = 5

    def update_time(self):
        if self.time is not None and self.time - time() > 2:
            self.__init__()
        self.time = time()
    
    def swap_shade(self, shade):
        if self.history != [] and sum(self.history)/len(self.history) < 0.5:
            if shade.lower() == "dark":
                self.shade = "light"
            elif shade.lower() == "light":
                self.shade = "dark"
            self.history = []

    def append_history(self,state):
        if len(self.history) >= self.max_history_length:
            self.history[:-4]
        if state >= 2:
            self.history.append(1.0/state)
        else:
            self.history.append(state)


public_topic = '/vision/mission/drum/'
image = Image(sub_sampling=0.3)
DEBUG = {
    'by-pass-mat': False,
    'console': True
}


def mission_callback(msg):
    task = str(msg.task.data)
    req = str(msg.req.data)
    if DEBUG['console']:
        lib.print_mission(task, req)

    drum_option = ['red','blue']
    return_option = ['top-bottom','left-right','tb','lr']
    if task in drum_option and req in return_option:
        return find_drum(task,req)
    elif task == 'golf':
        return find_golf(task)

def message(state=0, cx1=0, cy1=0, cx2=0, cy2=0, forward=False, backward=False, left=False, right=False, area=0):
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


def get_mask(color, shade):
    image.get_hsv()
    blur = cv.medianBlur(image.hsv, 5)
    _,s,_ = cv.split(blur)

    foregroud_mask = cv.threshold(
        s, 0, 255, cv.THRESH_BINARY + cv.THRESH_OTSU)[1]

    if color == "blue":
        upper = np.array([120, 255, 255], dtype=np.uint8)
        lower = np.array([90, 160, 0], dtype=np.uint8)
    if color == "yellow":
        if shade == "dark":
            upper = np.array([47, 255, 255], dtype=np.uint8)
            lower = np.array([20, 17, 228], dtype=np.uint8)
        elif shade == "light":
            upper = np.array([60, 255, 255], dtype=np.uint8)
            lower = np.array([27, 160, 97], dtype=np.uint8)
    if color == "green":
        upper = np.array([90, 255, 255], dtype=np.uint8)
        lower = np.array([60, 160, 0], dtype=np.uint8)

    foregroud = cv.bitwise_and(image.hsv, image.hsv, mask=foregroud_mask)
    mask = cv.inRange(foregroud, lower, upper)
    return mask


def get_contour(mask, request):
    if request in ['red','blue'] or not DEBUG['by-pass-mat']:
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
        check_area = 270 if request == 'golf' else 2700
        if area < check_area:
            continue

        x, y, w, h = cv.boundingRect(cnt)
        cv.rectangle(image.display, (x, y), (x+w, y+h), lib.get_color('yellow'), 2)
        left_cy = lib.most_point(cnt,'left')[1]
        right_cy = lib.most_point(cnt,'right')[1]
        if abs(left_cy-right_cy) > 0.2 * h:
            continue

        cv.rectangle(image.display, (x, y), (x+w, y+h), lib.get_color('green'), 2)
        obj.append(cnt)

    if obj != []:
        return max(obj, key=cv.contourArea)
    return [] 


def get_excess(cnt):
    himg,wimg = image.display.shape[:2]
    x, y, w, h = cv.boundingRect(cnt)
    top_excess = (y < (0.05*wimg))
    right_excess = ((x+w) > 0.95*wimg)
    left_excess = (x < (0.05*wimg))
    bottom_excess = ((y+h) > 0.95*himg)
    return top_excess, bottom_excess, left_excess, right_excess


def get_cx(cnt):
    global image_result
    himg, wimg = image.display.shape[:2]
    area = cv.contourArea(cnt)/(himg*wimg)
    (cx, cy) = lib.center_of_contour(cnt)
    cv.circle(image.display, (cx, cy), 5, (0, 0, 255), -1)
    x, y, w, h = cv.boundingRect(cnt)
    cx1 = lib.Aconvert(x, wimg)
    cy1 = -1.0*lib.Aconvert(y, himg)
    cx2 = lib.Aconvert(x+w, wimg)
    cy2 = -1.0*lib.Aconvert(y+h, himg)
    return cx1, cy1, cx2, cy2, area


def get_mat():
    mat = get_mask(image.bgr, "green")
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


def find_drum(color, return_option):
    if image.bgr is None:
        lib.img_is_none()
        return message(state=-1)
    himg, wimg = image.display.shape[:2]
    drum_mask = get_mask(image.display, color)
    obj = get_obj(drum_mask, color)
    state = len(obj)
    if state == 0:
        lib.print_result("CANNOT FOUND DRUM", ct.RED)
        lib.publish_result(drum_mask, 'gray', public_topic+'mask/drum')
        lib.publish_result(image.display, 'bgr', public_topic+'image_result')
        return message()
    lib.print_result("FOUND DRUM", ct.GREEN)
    cv.circle(image.display,lib.most_point(obj,'right'),5,(255,255,0),-1)
    cv.circle(image.display,lib.most_point(obj,'left'),5,(0,255,255),-1)
    cx1, cy1, cx2, cy2, area = get_cx(obj)
    forward, backward, left, right = get_excess(obj)
    lib.publish_result(drum_mask, 'gray', public_topic+'mask/drum')
    lib.publish_result(image.display, 'bgr', public_topic+'display')
    return message(state=state, cx1=cx1, cy1=cy1, cx2=cx2, cy2=cy2, forward=forward,
                    backward=backward, left=left, right=right, area=area)

def find_golf(objective):
    global last_time, his, start_shade, shade
    if IMAGE is None:
        lib.img_is_none()
        return message(state=-1)
    if(last_time == 0 or time()-last_time > 5 or his is None):
        his = []
    himg, wimg = IMAGE.shape[:2]
    if himg > 1000 or wimg > 1000:
        lib.print_result("size bug plz wait", color=ct.RED)
        last_time = time()
        return message(state=-2)
    if len(his) == 5 and 1 not in his:
        start_shade += 1
        start_shade %= len(shade)
    print(shade[start_shade],his)
    golf_mask = get_mask(IMAGE, "yellow",shade=shade[start_shade])
    ROI = get_ROI(golf_mask, objective)
    if ROI is None:
        lib.print_result("size bug plz wait", color=ct.RED)
        last_time = time()
        return message(state=-2)
    state = len(ROI)
    if state == 0:
        lib.print_result("CANNOT FOUND GOLF", ct.RED)
        lib.publish_result(golf_mask, 'gray', public_topic+'mask/golf')
        lib.publish_result(image_result, 'bgr', public_topic+'image_result')
        last_time = time()
        append_history(0)
        return message()
    elif state >= 1:
        if state == 1:
            lib.print_result("FOUND GOLF", ct.GREEN)
            append_history(1)
        elif state > 1:
            lib.print_result("FOUND BUT HAVE SOME NOISE (" +
                         str(state) + ")", ct.YELLOW)
            append_history(1.0/state)
        cnt = max(ROI, key=cv.contourArea)
        cx1, cy1, cx2, cy2, area = get_cx(cnt)
        forward, backward, left, right = get_excess(cnt)
        lib.publish_result(golf_mask, 'gray', public_topic+'mask/golf')
        lib.publish_result(image_result, 'bgr', public_topic+'image_result')
        last_time = time()
        return message(state=state, cx1=cx1, cy1=cy1, cx2=cx2, cy2=cy2, forward=forward,
                       backward=backward, left=left, right=right, area=area)


if __name__ == '__main__':
    rospy.init_node('vision_drum', anonymous=False)

    image_topic = lib.get_topic("bottom")
    rospy.Subscriber(image_topic, CompressedImage, image.callback)
    rospy.Service('vision/drum', vision_srv_drum(),
                  mission_callback)
    lib.print_result("INIT NODE DRUM", ct.GREEN)

    rospy.spin()
    lib.print_result("END PROGRAM", ct.YELLOW_HL+ct.RED)
