#!/usr/bin/python2.7
import rospy
import numpy as np
import cv2 as cv
from sensor_msgs.msg import CompressedImage
from zeabus_vision.msg import vision_drum
from zeabus_vision.srv import vision_srv_drum
from vision_lib import *
import color_text as ct

bgr = None
image_result = None
public_topic = '/vision/mission/drum/'
sub_sampling = 1
c = 0


def mission_callback(msg):
    print_result('mission_callback', ct.CYAN)
    task = str(msg.task.data)
    req = str(msg.req.data)

    print(msg)

    drum_option = ['search', 'drop']
    golf_option = ['pick']
    if task == 'drum' and req in drum_option:
        return find_drum(req)
    elif task == 'drum' and req in golf_option:
        return find_golf()


def image_callback(msg):
    global bgr, image_result, sub_sampling
    arr = np.fromstring(msg.data, np.uint8)
    bgr = cv.resize(cv.imdecode(arr, 1), (0, 0),
                    fx=sub_sampling, fy=sub_sampling)
    h_img, w_img = bgr.shape[:2]
    bgr = cv.resize(bgr, (int(w_img/3), int(h_img/3)))
    image_result = bgr.copy()


def message(n_obj=0, cx=0, cy=0, forward=False, backward=False, left=False, right=False, area=0):
    msg = vision_drum()
    msg.n_obj = n_obj
    msg.cx = cx
    msg.cy = cy
    msg.forward = forward
    msg.backward = backward
    msg.left = left
    msg.right = right
    msg.area = area
    print msg
    return msg


def get_mask(img, color):
    blur = cv.medianBlur(img, 5)
    hsv = cv.cvtColor(blur, cv.COLOR_BGR2HSV)
    _, s, _ = cv.split(hsv)
    foregroud_mask = cv.threshold(
        s, 0, 255, cv.THRESH_BINARY + cv.THRESH_OTSU)[1]
    foregroud = cv.bitwise_and(hsv, hsv, mask=foregroud_mask)
    if color == "blue":
        upper = np.array([120, 255, 255], dtype=np.uint8)
        lower = np.array([90, 160, 0], dtype=np.uint8)
    if color == "yellow":
        upper = np.array([59, 255, 255], dtype=np.uint8)
        lower = np.array([30, 160, 0], dtype=np.uint8)
    if color == "green":
        upper = np.array([90, 255, 255], dtype=np.uint8)
        lower = np.array([60, 160, 0], dtype=np.uint8)
    mask = cv.inRange(foregroud, lower, upper)
    return mask


def get_ROI(mask,obj):
    if obj == 'search':
        mat_mask = find_mat()
        if mat_mask.shape != mask.shape:
            return None
        publish_result(mat_mask, 'gray', public_topic+'mask/mat_mask')
        intersect = cv.bitwise_and(mat_mask, mask)
        contours = cv.findContours(
            intersect, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)[1]
    elif obj == 'drop':
        contours = cv.findContours(
            mask, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)[1]
    ROI = []
    for cnt in contours:
        area = cv.contourArea(cnt)
        if area < 1000:
            continue
        ROI.append(cnt)
    return ROI


def find_mat():
    global bgr
    mat = get_mask(bgr, "green")
    publish_result(mat, 'gray', public_topic+'mask/mat')
    contours = cv.findContours(
        mat, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)[1]
    max_cnt = max(contours, key=cv.contourArea)
    if cv.contourArea(max_cnt) < 1000:
        return -1, -1, -1, -1
    rect = cv.minAreaRect(max_cnt)
    box = cv.boxPoints(rect)
    box = np.int64(box)
    himg, wimg = mat.shape[:2]
    mat_mask = np.zeros((himg, wimg), np.uint8)
    cv.drawContours(mat_mask, [box], 0, (255), -1)
    return mat_mask
    # x, y, w, h = cv.boundingRect(max_cnt)
    # cv.rectangle(image_result, (x, y), (x+w, y+h), (0, 255, 0), 2)
    # return x, y, w, h


def find_drum(objective):
    global bgr, c
    if bgr is None:
        img_is_none()
        return message(n_obj=-1)
    himg, wimg = bgr.shape[:2]
    if himg > 1000 or wimg > 1000:
        print_result("size bug plz wait", color=ct.RED)
        return message(n_obj=-2)
    drum_mask = get_mask(bgr, "blue")
    ROI = get_ROI(drum_mask, objective)
    if ROI is None:
        print_result("size bug plz wait", color=ct.RED)
        return message(n_obj=-2)
    if ROI == []:
        return message()
    c += 1
    cnt = max(ROI, key=cv.contourArea)
    x, y, w, h = cv.boundingRect(cnt)
    cv.rectangle(image_result, (x, y), (x+w, y+h), (0, 255, 0), 2)
    publish_result(drum_mask, 'gray', public_topic+'mask/blue')
    publish_result(image_result, 'bgr', public_topic+'image_result')
    print c
    return message()


def find_golf():
    global bgr
    if bgr is None:
        img_is_none()
        return message(n_obj=-1)
    pass


if __name__ == '__main__':
    rospy.init_node('vision_drum', anonymous=False)

    image_topic = get_topic("bottom")
    rospy.Subscriber(image_topic, CompressedImage, image_callback)
    rospy.Service('vision_drum', vision_srv_drum(),
                  mission_callback)
    print_result("INIT NODE DRUM", ct.GREEN)

    rospy.spin()
    print_result("END PROGRAM", ct.YELLOW_HL+ct.RED)
