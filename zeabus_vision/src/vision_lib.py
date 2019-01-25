import cv2 as cv
import rospy
import numpy as np
from cv_bridge import CvBridge
from sensor_msgs.msg import Image
import color_text as ct
# import math


def print_result(msg, color=ct.DEFAULT):
    """
        print ('<----------') + str(msg) + ('---------->')
        #len of <---msg---> = 50
    """
    print '<{:-^80}>'.format(' ' + color + str(msg) + ct.DEFAULT + ' ')


def img_is_none():
    print(ct.RED + 'img is none.\nPlease check topic name or check camera is running' + ct.DEFAULT)


def publish_result(img, type, topicName):
    """
        publish picture
    """
    if img is None:
        img = np.zeros((200, 200))
        type = "gray"
    bridge = CvBridge()
    pub = rospy.Publisher(
        str(topicName), Image, queue_size=10)
    if type == 'gray':
        msg = bridge.cv2_to_imgmsg(img, "mono8")
    elif type == 'bgr':
        msg = bridge.cv2_to_imgmsg(img, "bgr8")
    elif type == 'bgra':
        msg = bridge.cv2_to_imgmsg(img, "bgra8")
    pub.publish(msg)


def get_topic(camera):
    if camera == 'front':
        return '/vision/front/image_rect_color/compressed'
    elif camera == 'bottom':
        return '/vision/bottom/image_raw/compressed'


def range_str2array(string):
    string = string.split(',')
    return np.array([int(string[0]), int(string[1]), int(string[2])], dtype=np.uint8)


def get_kernel(shape='rect', ksize=(5, 5)):
    if shape == 'rect':
        return cv.getStructuringElement(cv.MORPH_RECT, ksize)
    elif shape == 'ellipse':
        return cv.getStructuringElement(cv.MORPH_ELLIPSE, ksize)
    elif shape == 'plus':
        return cv.getStructuringElement(cv.MORPH_CROSS, ksize)
    else:
        return None

# def get_color_range(color, camera_position, number, mission):
#     lower = None
#     upper = None
#     color_list = CONST.COLOR_LIST
#     print('color_range_' + str(camera_position) + '_' + str(number) +
#           '_' + str(mission) + '/color_' + camera_position + '/lower_' + color)
#     if color in color_list:
#         lower = rospy.get_param(
#             'color_range_' + str(camera_position) + '_' + str(number) + '_' + str(mission) + '/color_' + camera_position + '/lower_' + color, '0,0,0')
#         upper = rospy.get_param(
#             'color_range_' + str(camera_position) + '_' + str(number) + '_' + str(mission) + '/color_' + camera_position + '/upper_' + color, '179,255,255')
#         lower = range_str2array(lower)
#         upper = range_str2array(upper)
#         print "FOUND"
#     print(lower, upper)
#     return lower, upper


def bg_subtraction(gray):
    min_gray = 20
    max_gray = 250
    bg = cv.medianBlur(gray, 61)
    fg = cv.medianBlur(gray, 7)
    sub = np.uint8(abs(fg - bg))
    obj1 = np.zeros(gray.shape, np.uint8)
    obj2 = np.zeros(gray.shape, np.uint8)
    obj1[sub > min_gray] = 255
    obj2[sub < max_gray] = 255
    obj = cv.bitwise_and(obj1, obj2)
    return obj


def Aconvert(inp, full):
    inp = float(inp)
    full = float(full)
    res = (inp - (full / 2.0)) / (full / 2.0)
    return res


def equalize(mono):
    mono = cv.equalizeHist(mono)
    return mono


def equalize_hsv(hsv):
    h, s, v = cv.split(hsv)
    s = equalize(s)
    v = equalize(v)
    hsv = cv.merge((h, s, v))
    return hsv


def equalize_bgr(bgr):
    b, g, r = cv.split(bgr)
    b = equalize(b)
    g = equalize(g)
    r = equalize(r)
    bgr = cv.merge((b, g, r))
    return bgr


def debug_head():
    print ct.WHITE_HL + ct.BLACK + '<{:-^80}>'.format(' Debug ')


def debug_end():
    print '<{:-^80}>'.format(' End Debug ') + ct.DEFAULT


def center_of_contour(cnt):
    M = cv.moments(cnt)
    cx = int(M["m10"] / M["m00"])
    cy = int(M["m01"] / M["m00"])
    return (cx, cy)


def clear_screen():
    print("\033[H\033[J")

def pre_process(bgr, mission):
    return bgr

# def add_grid(bgr):
#     himg, wimg = bgr.shape[:2]
#     grid = np.ones((himg,wimg,3),dtype=np.uint8)
#     scale = [1.0*i/10 for i in range(1,10)]
#     h = [int(round(i*himg)) for i in scale]
#     w = [int(round(i*wimg)) for i in scale]
#     for i in h:
#         cv.line(grid,(0,i),(wimg,i),(0,0,0),1)
#     for i in w:
#         cv.line(grid,(0,i),(wimg,i),(0,0,0),1)
#     print(h)
#     print(w)
#     return grid

def gen_grid(data, shape):
    himg, wimg = shape[:2]
    grid = np.ones((himg,wimg,3),dtype=np.uint8)
    scale = [1.0*i/10 for i in range(1,10)]
    h = [int(round(i*himg)) for i in scale]
    w = [int(round(i*wimg)) for i in scale]
    graph_color = (175,175,175)
    for i in h:
        cv.line(grid,(0,i),(wimg,i),graph_color,1)
    for i in w:
        cv.line(grid,(i,0),(i,himg),graph_color,1)
    cv.line(grid,(0,h[4]),(wimg,h[4]),graph_color,3)
    cv.line(grid,(w[4],0),(w[4],himg),graph_color,3)
    for key in data:
        pos = data[key]
        pos = (int(pos[0]),int(pos[1]))
        cv.circle(grid,pos,3,(0,255,0),-1)
        cv.putText(grid,key,pos,cv.FONT_HERSHEY_COMPLEX_SMALL,2,(255,255,255),2)    
    return grid

def print_mission(task,req=''):
    print_result('mission_callback', ct.CYAN)
    req = 'BLANK' if req == '' else req
    print("task is " + ct.UNDERLINE + task + ct.DEFAULT +
          " and req is " + ct.UNDERLINE + req + ct.DEFAULT)