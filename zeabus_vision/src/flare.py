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
public_topic = '/vision/mission/flare'
sub_sampling = 1


def mission_callback(msg):
    print_result('mission_callback', ct.CYAN)
    task = str(msg.task.data)
    req = str(msg.req.data)

    print(msg)
    if task == 'flare' and req == 'near':
        return find_near_flare()
    elif task == 'flare' and req == 'far':
        return find_far_flare()


def image_callback(msg):
    global bgr, image_result, sub_sampling
    arr = np.fromstring(msg.data, np.uint8)
    bgr = cv.resize(cv.imdecode(arr, 1), (0, 0),
                    fx=sub_sampling, fy=sub_sampling)
    h_img, w_img = bgr.shape[:2]
    bgr = cv.resize(bgr, (int(w_img/3), int(h_img/3)))
    image_result = bgr.copy()


def message(n_obj=0, cx=0.0, cy=0.0, area=0.0):
    msg = vision_flare()
    msg.n_obj = n_obj
    msg.cx = cx
    msg.cy = cy
    msg.area = area
    print n_obj
    return msg

def get_mask(img):
    hsv = cv.cvtColor(img, cv.COLOR_BGR2HSV)
    upper, lower = get_color_range('yellow', 'front', '1', 'flare')
    mask = cv.inRange(hsv, lower, upper)
    return mask

def get_ROI(mask,case):
    himg, wimg = mask.shape[:2]
    ROI = []
    contours = cv.findContours(mask, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)[1]
    for cnt in contours:
        check_area = 500
        if case == 'near' :
            check_area = 1500
        if cv.contourArea(cnt) < check_area :
            continue
        x, y, w, h = cv.boundingRect(cnt)
        top_excess = (y < 0.05*image_result)
        right_excess = ((x+w) > 0.95*image_result)
        left_excess = (x < (0.05*image_result))
        bottom_excess = ((y+h) > 0.95*image_result)
        if case == 'near' :
            if (top_excess and  bottom_excess) and not(left_excess or right_excess) :
                ROI.append(cnt)
        else :
            h_w_ratio = 1.0*h/w
            if not(left_excess or right_excess) and h_w_ratio >= h_w_ratio*50/100 and h_w_ratio <= h_w_ratio *150/100 :
                ROI.append(cnt)
    return ROI
def get_cx(cnt) :
    global image_result
    himg, wimg = image_result.shape[:2]
    x, y, w, h = cv.boundingRect(cnt)
    cv.rectangle(image_result, (x, y), (x+w, y+h), (0, 255, 0), 2)
    cx = x + (w/2)
    cy = y + (h/2)
    cv.circle(image_result, (cx, cy), 5, (0, 0, 255), -1)
    cx = Aconvert(cx, wimg)
    cy = -1.0*Aconvert(cy, himg)
    area = (1.0*w*h)/(wimg*himg)
    return cx, cy, area


def find_near_flare():
    global bgr
    if bgr is None:
        img_is_none()
        return message(n_obj=-1)
    mask = get_mask(image_result)
    ROI = get_ROI(mask,case = 'near')
    if len(ROI) == 0 :
        mode = 1
        print_result("NOT FOUND", color_text.RED)
    elif len(ROI) == 1 :
        mode = 2
        print_result("FOUND A FLARE", color_text.GREEN)
    elif len(ROI) > 1 :
        mode = 2
        print_result("FOUND BUT HAVE SOME NOISE", color_text.YELLOW)
    if mode == 1 :
        publish_result(image_result, 'bgr', pub_topic + 'image_result')
        publish_result(mask, 'gray', pub_topic + 'mask')
        return message()
    elif mode == 2:
        cx, cy, area = get_cx(cnt = max(ROI,key=cv.contourArea))
        publish_result(image_result, 'bgr', pub_topic + 'image_result')
        publish_result(mask, 'gray', pub_topic + 'mask')
        return message(cx=cx, cy=cy, area=area, n_obj =len(ROI))
    

def find_far_flare():
    global bgr
    if bgr is None:
        img_is_none()
        return message(n_obj=-1)
    mask = get_mask(image_result)
    ROI = get_ROI(mask,case = 'far')
     if len(ROI) == 0 :
        mode = 1
        print_result("NOT FOUND", color_text.RED)
    elif len(ROI) == 1 :
        mode = 2
        print_result("FOUND A FLARE", color_text.GREEN)
    elif len(ROI) > 1 :
        mode = 2
        print_result("FOUND BUT HAVE SOME NOISE", color_text.YELLOW)
    if mode == 1 :
        publish_result(image_result, 'bgr', pub_topic + 'image_result')
        publish_result(mask, 'gray', pub_topic + 'mask')
        return message()
    elif mode == 2:
        cx, cy, area = get_cx(cnt = max(ROI,key=cv.contourArea))
        publish_result(image_result, 'bgr', pub_topic + 'image_result')
        publish_result(mask, 'gray', pub_topic + 'mask')
        return message(cx=cx, cy=cy, area=area, n_obj =len(ROI))


if __name__ == '__main__':
    rospy.init_node('vision_flare', anonymous=False)
    
    image_topic = get_topic("front")
    rospy.Subscriber(image_topic, CompressedImage, image_callback)
    rospy.Service('vision_flare', vision_srv_flare(),
                  mission_callback)
    print_result("INIT NODE FLARE", ct.GREEN)

    rospy.spin()
    print_result("END PROGRAM", ct.YELLOW_HL+ct.RED)
