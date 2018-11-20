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
        return find_golf(req)


def image_callback(msg):
    global bgr, image_result, sub_sampling
    arr = np.fromstring(msg.data, np.uint8)
    bgr = cv.resize(cv.imdecode(arr, 1), (0, 0),
                    fx=sub_sampling, fy=sub_sampling)
    h_img, w_img = bgr.shape[:2]
    bgr = cv.resize(bgr, (int(w_img/3), int(h_img/3)))
    image_result = bgr.copy()


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


def get_mask(img, color):
    blur = cv.medianBlur(img, 5)
    hsv = cv.cvtColor(blur, cv.COLOR_BGR2HSV)
    _, s, _ = cv.split(hsv)
    foregroud_mask = cv.threshold(
        s, 0, 255, cv.THRESH_BINARY + cv.THRESH_OTSU)[1]
    foregroud = cv.bitwise_and(hsv, hsv, mask=foregroud_mask)
    if color == "blue":
        upper = np.array([150, 255, 255], dtype=np.uint8)
        lower = np.array([85, 0, 0], dtype=np.uint8)
    if color == "yellow":
        upper = np.array([66, 255, 255], dtype=np.uint8)
        lower = np.array([0, 221, 85], dtype=np.uint8)
    if color == "green":
        upper = np.array([90, 255, 255], dtype=np.uint8)
        lower = np.array([60, 160, 0], dtype=np.uint8)
    mask = cv.inRange(hsv, lower, upper)
    cv.circle(mask,(119,270),106,(0),-1)
    return mask


def get_ROI(mask, obj='drop'):
    if obj == 'search':
        mat_mask = find_mat()
        if mat_mask.shape != mask.shape:
            return None
        publish_result(mat_mask, 'gray', public_topic+'mask/mat/processed')
        intersect = cv.bitwise_and(mat_mask, mask)
        contours = cv.findContours(
            intersect, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)[1]
    if obj == 'pick':
        mat_mask = temp_drum()
        if mat_mask.shape != mask.shape:
            return None
        publish_result(mat_mask, 'gray', public_topic+'mask/drum/processed')
        intersect = cv.bitwise_and(mat_mask, mask)
        contours = cv.findContours(
            intersect, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)[1]
    elif obj == 'drop' or obj == 'pick':
        contours = cv.findContours(
            mask, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)[1]
    ROI = []
    for cnt in contours:
        area = cv.contourArea(cnt)
        check_area = 1000
        if obj == 'pick':
            check_area = 200
        if area < check_area:
            continue
        ROI.append(cnt)
    return ROI


def get_excess(cnt):
    global image_result
    himg, wimg = image_result.shape[:2]
    x, y, w, h = cv.boundingRect(cnt)
    cv.rectangle(image_result, (x, y), (x+w, y+h), (0, 255, 0), 2)
    top_excess = (y < (0.05*wimg))
    right_excess = ((x+w) > 0.95*wimg)
    left_excess = (x < (0.05*wimg))
    bottom_excess = ((y+h) > 0.95*himg)
    return top_excess, bottom_excess, left_excess, right_excess


def get_cx(cnt):
    global image_result
    himg, wimg = image_result.shape[:2]
    area = cv.contourArea(cnt)/(himg*wimg)
    (cx, cy) = center_of_contour(cnt)
    cv.circle(image_result, (cx, cy), 5, (0, 0, 255), -1)
    x, y, w, h = cv.boundingRect(cnt)
    cx1 = Aconvert(x, wimg)
    cy1 = -1.0*Aconvert(y, himg)
    cx2 = Aconvert(x+w, wimg)
    cy2 = -1.0*Aconvert(y+h, himg)
    return cx1, cy1, cx2, cy2, area


def find_mat():
    global bgr
    mat = get_mask(bgr, "green")
    publish_result(mat, 'gray', public_topic+'mask/mat/unprocessed')
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

def temp_drum():
    global bgr
    mat = get_mask(bgr, "blue")
    publish_result(mat, 'gray', public_topic+'mask/drum/unprocessed')
    contours = cv.findContours(
        mat, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)[1]
    himg, wimg = mat.shape[:2]
    mat_mask = np.zeros((himg, wimg), np.uint8)
    if len(contours) == 0:
		return mat_mask
    max_cnt = max(contours, key=cv.contourArea)
    (x,y),radius = cv.minEnclosingCircle(max_cnt)
    center = (int(x),int(y))
    radius = int(radius)
    print(center,radius)
    cv.circle(mat_mask,center,radius,(255),-1)
    return mat_mask

def find_drum(objective):
    global bgr
    if bgr is None:
        img_is_none()
        return message(state=-1)
    himg, wimg = bgr.shape[:2]
    if himg > 1000 or wimg > 1000:
        print_result("size bug plz wait", color=ct.RED)
        return message(state=-2)
    drum_mask = get_mask(bgr, "blue")
    ROI = get_ROI(drum_mask, objective)
    if ROI is None:
        print_result("size bug plz wait", color=ct.RED)
        return message(state=-2)
    mode = len(ROI)
    if mode == 0:
        print_result("CANNOT FOUND DRUM", ct.RED)
        publish_result(drum_mask, 'gray', public_topic+'mask/drum')
        publish_result(image_result, 'bgr', public_topic+'image_result')
        return message()
    elif mode >= 1:
        if mode == 1:
            print_result("FOUND DRUM", ct.GREEN)
        elif mode > 1:
            print_result("FOUND BUT HAVE SOME NOISE (" +
                         str(mode) + ")", ct.YELLOW)
        cnt = max(ROI, key=cv.contourArea)
        cx1, cy1, cx2, cy2, area = get_cx(cnt)
        forward, backward, left, right = get_excess(cnt)
        publish_result(drum_mask, 'gray', public_topic+'mask/drum')
        publish_result(image_result, 'bgr', public_topic+'image_result')
        return message(state=mode, cx1=cx1, cy1=cy1, cx2=cx2, cy2=cy2, forward=forward,
                       backward=backward, left=left, right=right, area=area)


def find_golf(objective):
    global bgr
    if bgr is None:
        img_is_none()
        return message(state=-1)
    himg, wimg = bgr.shape[:2]
    if himg > 1000 or wimg > 1000:
        print_result("size bug plz wait", color=ct.RED)
        return message(state=-2)
    golf_mask = get_mask(bgr, "yellow")
    ROI = get_ROI(golf_mask, objective)
    if ROI is None:
        print_result("size bug plz wait", color=ct.RED)
        return message(state=-2)
    mode = len(ROI)
    if mode == 0:
        print_result("CANNOT FOUND GOLF", ct.RED)
        publish_result(golf_mask, 'gray', public_topic+'mask/golf')
        publish_result(image_result, 'bgr', public_topic+'image_result')
        return message()
    elif mode >= 1:
        if mode == 1:
            print_result("FOUND GOLF", ct.GREEN)
        elif mode > 1:
            print_result("FOUND BUT HAVE SOME NOISE (" +
                         str(mode) + ")", ct.YELLOW)
        cnt = max(ROI, key=cv.contourArea)
        cx1, cy1, cx2, cy2, area = get_cx(cnt)
        forward, backward, left, right = get_excess(cnt)
        publish_result(golf_mask, 'gray', public_topic+'mask/golf')
        publish_result(image_result, 'bgr', public_topic+'image_result')
        return message(state=mode, cx1=cx1, cy1=cy1, cx2=cx2, cy2=cy2, forward=forward,
                       backward=backward, left=left, right=right, area=area)


if __name__ == '__main__':
    rospy.init_node('vision_drum', anonymous=False)

    image_topic = get_topic("bottom")
    rospy.Subscriber(image_topic, CompressedImage, image_callback)
    rospy.Service('vision_drum', vision_srv_drum(),
                  mission_callback)
    print_result("INIT NODE DRUM", ct.GREEN)

    rospy.spin()
    print_result("END PROGRAM", ct.YELLOW_HL+ct.RED)
