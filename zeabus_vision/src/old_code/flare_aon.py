#!/usr/bin/python2.7
import rospy
import cv2 as cv
import numpy as np
from sensor_msgs.msg import CompressedImage
from zeabus_vision.msg import vision_flare
from zeabus_vision.srv import vision_srv_flare
from vision_lib import *
import color_text as ct

bgr = None
image_result = None
public_topic = '/vision/mission/flare/'
sub_sampling = 1
debug = False


def mission_callback(msg):
    print_result('mission_callback', ct.CYAN)
    task = str(msg.task.data)
    req = str(msg.req.data)
    color = ct.PURPLE if req == "near" else ct.CYAN
    print color + task + " " + req + ct.DEFAULT
    if task == 'flare' and req in ['near', 'far']:
        return find_flare(req)


def image_callback(msg):
    global bgr, image_result, sub_sampling
    arr = np.fromstring(msg.data, np.uint8)
    bgr = cv.resize(cv.imdecode(arr, 1), (0, 0),
                    fx=sub_sampling, fy=sub_sampling)
    h_img, w_img = bgr.shape[:2]
    bgr = cv.resize(bgr, (int(w_img/3), int(h_img/3)))
    image_result = bgr.copy()


def message(state=0, cx=0.0, cy=0.0, area=0.0):
    msg = vision_flare()
    msg.state = state
    msg.cx = cx
    msg.cy = cy
    msg.area = area
    print msg
    return msg


def get_mask(img):
    hsv = cv.cvtColor(img, cv.COLOR_BGR2HSV)
    hsv = equalize_hsv(hsv)
    # upper, lower = get_color_range('yellow', 'front', '1', 'flare')
    upper = np.array([60, 255, 255], dtype=np.uint8)
    lower = np.array([20, 200, 0], dtype=np.uint8)
    mask = cv.inRange(hsv, lower, upper)
    return mask


def get_ROI(mask, case):
    himg, wimg = mask.shape[:2]
    ROI = []
    contours = cv.findContours(
        mask, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)[1]
    for cnt in contours:
        area = cv.contourArea(cnt)
        area_condition = 8000 > area if case == 'near' else (area < 500 or area > 10000)
        if area_condition:
            continue
        if debug:
            print (case,'area',area,area_condition)
        x, y, w, h = cv.boundingRect(cnt)
        rect = cv.minAreaRect(cnt)
        (_, (wm, hm), angle) = rect
        # angle_condition = angle < -10
        w_h_ratio = max(1.0*wm/hm, 1.0*hm/wm)
        if debug:
            print angle
        if debug:
            box = cv.boxPoints(rect)
            box = np.int64(box)
            cv.drawContours(image_result, [box], 0, (0, 0, 255), 2)
        percent = (float(area)/(hm*wm))
        if debug:
            print ('percent',percent)
        if percent < 0.6:
            continue
        # top_excess = (y < (0.05*wimg))
        right_excess = ((x+w) > 0.95*wimg)
        left_excess = (x < (0.05*wimg))
        bottom_excess = ((y+h) > 0.95*himg)
        if case == 'near':
            if bottom_excess:
                ROI.append(cnt)
        elif case == 'far':
            if not right_excess and not left_excess and w_h_ratio >= 6 and h > w:
                ROI.append(cnt)
    return ROI


def get_cx(cnt):
    global image_result
    himg, wimg = image_result.shape[:2]
    rect = cv.minAreaRect(cnt)
    box = cv.boxPoints(rect)
    box = np.int64(box)
    cv.drawContours(image_result, [box], 0, (0, 255, 0), 2)
    (cx, cy) = center_of_contour(cnt)
    cv.circle(image_result, (cx, cy), 5, (0, 0, 255), -1)
    cx = Aconvert(cx, wimg)
    cy = -1.0*Aconvert(cy, himg)
    area = Aconvert(cv.contourArea(cnt), (himg*wimg))
    return cx, cy, area


def find_flare(req):
    global bgr
    if bgr is None:
        img_is_none()
        return message(state=-1)
    
    mask = get_mask(bgr)
    ROI = get_ROI(mask, case=req)
    mode = len(ROI)
    if mode == 0:
        print_result("NOT FOUND", ct.RED)
        publish_result(image_result, 'bgr', public_topic +
                       req + '/image_result')
        publish_result(mask, 'gray', public_topic + 'mask')
        return message()
    elif mode >= 1:
        color = ct.PURPLE if req == "near" else ct.CYAN
        if mode == 1:
            print_result("FOUND "+color+req+ct.GREEN+" FLARE", ct.GREEN)
        elif mode > 1:
            print_result("FOUND BUT HAVE SOME NOISE (" +
                         str(mode) + ")", ct.YELLOW)
        cx, cy, area = get_cx(cnt=max(ROI, key=cv.contourArea))
        publish_result(image_result, 'bgr', public_topic +
                       req + '/image_result')
        publish_result(mask, 'gray', public_topic + 'mask')
        return message(cx=cx, cy=cy, area=area, state=len(ROI))


if __name__ == '__main__':
    rospy.init_node('vision_flare', anonymous=False)

    image_topic = get_topic("front")
    rospy.Subscriber(image_topic, CompressedImage, image_callback)
    rospy.Service('vision_flare', vision_srv_flare(),
                  mission_callback)
    print_result("INIT NODE FLARE", ct.GREEN)

    rospy.spin()
    print_result("END PROGRAM", ct.YELLOW_HL+ct.RED)