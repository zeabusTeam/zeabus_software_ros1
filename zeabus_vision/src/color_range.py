#!/usr/bin/python2.7
import sys
import rospy
import rospkg
import cv2 as cv
import numpy as np
import constant as CONST
from vision_lib import *
from PyQt4.QtGui import *
from sensor_msgs.msg import CompressedImage

img = None
hsv = None
wait = False
click = False
number = None
mission = None
is_mask = False
sub_sampling = 0.5
camera_position = None
image_width, image_height = None, None
screen_width, screen_height = None, None

pixel = {}
pixel['x'], pixel['y'] = -1, -1


class window:
    global screen_width, screen_height, hsv, camera_position, mission, number

    def __init__(self):
        self.size = 250
        self.x = screen_width / 3 + 10
        self.y = 20
        self.lower = {}
        self.upper = {}
        self.lower_tmp = {}
        self.upper_tmp = {}
        self.select = {}
        self.path = rospkg.RosPack().get_path('zeabus_vision')

    def create(self, window_name):
        for name in window_name:
            cv.namedWindow(name, flags=cv.WINDOW_NORMAL)
            cv.moveWindow(name, self.x + self.x / 5, self.y + self.y / 5)
            cv.resizeWindow(name, self.size, self.size)
            self.update_position()
            self.create_range(name)

    def update_position(self):
        self.y += self.size
        if self.y + self.size >= screen_height:
            self.x += self.size
            self.y = 20

    def create_range(self, name):
        lower_param, upper_param = self.get_param(name)
        self.lower[name] = [lower_param]
        self.upper[name] = [upper_param]
        self.lower_tmp[name] = []
        self.upper_tmp[name] = []
        self.select[name] = False

    def push_range(self, name, lower, upper):
        self.lower[name].append(lower)
        self.upper[name].append(upper)

    def get_range(self, name):
        return self.lower[name][-1], self.upper[name][-1]

    def undo_range(self, name):
        if len(self.lower[name]) > 0:
            self.lower_tmp[name].append(self.lower[name][-1])
            self.upper_tmp[name].append(self.upper[name][-1])
            self.lower[name].pop()
            self.upper[name].pop()
            set_trackbar(self.lower[name][-1], self.upper[name][-1])
            print_result('UNDO')
        else:
            print_result('Cannot Undo')

    def redo_range(self, name):
        if len(self.lower_tmp[name]) > 0:
            self.lower[name].append(self.lower_tmp[name][-1])
            self.upper[name].append(self.upper_tmp[name][-1])
            self.lower_tmp[name].pop()
            self.upper_tmp[name].pop()
            set_trackbar(self.lower[name][-1], self.upper[name][-1])
            print_result('REDO')
        else:
            print_result('Cannot Redo')

    def reset_range(self, name):
        self.lower[name].append([179, 255, 255])
        self.upper[name].append([0, 0, 0])
        set_trackbar(self.lower[name][-1], self.upper[name][-1])
        print_result('RESET')

    def show_image(self, window_name):
        for name in window_name:
            result = cv.inRange(hsv, np.array(self.lower[name][-1], np.uint8),
                                np.array(self.upper[name][-1], np.uint8))
            cv.imshow(name, result)

    def range_str2list(self, str):
        str = str.split(',')
        return [int(str[0]), int(str[1]), int(str[2])]

    def range_list2str(self, list):
        seq = (str(list[0]), str(list[1]), str(list[2]))
        ch_join = ','
        return ch_join.join(seq)

    def get_param(self, name):
        self.param_lower = rospy.get_param(
            'color_range_' + str(camera_position) + '_' + str(number) + '_' + str(mission) + '/color_' + camera_position + '/lower_' + name, '179,255,255')
        self.param_upper = rospy.get_param(
            'color_range_' + str(camera_position) + '_' + str(number) + '_' + str(mission) + '/color_' + camera_position + '/upper_' + name, '0,0,0')
        self.param_lower = self.range_str2list(self.param_lower)
        self.param_upper = self.range_str2list(self.param_upper)
        return self.param_lower, self.param_upper

    def save(self):
        for name in self.lower:
            if(name == 'mask'):
                continue
            rospy.set_param(
                '/color_range_' + str(camera_position) + '_' + str(number) + '_' + str(mission) + '/color_' + camera_position + '/lower_' + name, self.range_list2str(self.lower[name][-1]))
            rospy.set_param(
                '/color_range_' + str(camera_position) + '_' + str(number) + '_' + str(mission) + '/color_' + camera_position + '/upper_' + name, self.range_list2str(self.upper[name][-1]))

        f = open(self.path + '/params/' + str(number) + '/color_' +
                 camera_position + '_' + str(mission) + '.yaml', 'w')
        x = self.genyaml()
        f.write(x)
        f.close()

        print_result('save')

    def genyaml(self):
        tmp = "color_" + camera_position + ":\n"
        for name in self.lower:
            if(name == 'mask'):
                continue
            tmp += " " + " upper_" + name + ": '" + self.range_list2str(self.upper[name][-1]) + "'\n\n" +\
                " " + " lower_" + name + ": '" + \
                self.range_list2str(self.lower[name][-1]) + "'\n\n"
        return tmp


def camera_callback(msg):
    global img, wait, hsv, image_width, image_height, mission, camera_position, sub_sampling
    if wait:
        return
    arr = np.fromstring(msg.data, np.uint8)
    img_data = cv.resize(cv.imdecode(arr, 1), (0, 0),
                         fx=sub_sampling, fy=sub_sampling)
    img = pre_process(img_data, mission)
    hsv = cv.cvtColor(img, cv.COLOR_BGR2HSV)


def draw_circle(event, x, y, flags, param):
    global pixel, click
    if event == cv.EVENT_LBUTTONDOWN:
        click = True
        pixel['x'], pixel['y'] = x, y


def has_color(window_name, k):
    for name in window_name:
        if k == ord(name[0]) and k != ord('m'):
            return name, True
    return None, False


def set_trackbar(lower, upper):
    [hmin, smin, vmin], [hmax, smax, vmax] = lower, upper
    cv.setTrackbarPos('Hmin', 'image', hmin)
    cv.setTrackbarPos('Smin', 'image', smin)
    cv.setTrackbarPos('Vmin', 'image', vmin)
    cv.setTrackbarPos('Hmax', 'image', hmax)
    cv.setTrackbarPos('Smax', 'image', smax)
    cv.setTrackbarPos('Vmax', 'image', vmax)


def get_trackbar():
    Hmin = cv.getTrackbarPos('Hmin', 'image')
    Smin = cv.getTrackbarPos('Smin', 'image')
    Vmin = cv.getTrackbarPos('Vmin', 'image')
    Hmax = cv.getTrackbarPos('Hmax', 'image')
    Smax = cv.getTrackbarPos('Smax', 'image')
    Vmax = cv.getTrackbarPos('Vmax', 'image')
    lower = [Hmin, Smin, Vmin]
    upper = [Hmax, Smax, Vmax]
    return lower, upper


def compare_range(l, u, l1, u1):
    return not (l == l1 and u == u1)


def nothing(x):
    pass


def select_color():
    global pixel, img, wait, hsv, click, is_mask, image_height, image_width, screen_height, screen_width
    window_name = ['mask'] + CONST.COLOR_LIST

    cv.namedWindow('image_bgr', flags=cv.WINDOW_NORMAL)
    cv.moveWindow('image_bgr', 400, 400)
    cv.resizeWindow('image_bgr', (screen_width / 3) +
                    30, (screen_height / 3) + 30)

    cv.namedWindow('image', flags=cv.WINDOW_NORMAL)
    cv.moveWindow('image', 20, 20)
    cv.resizeWindow('image', (screen_width / 3), screen_height)
    cv.createTrackbar('Hmin', 'image', 0, 179, nothing)
    cv.createTrackbar('Smin', 'image', 0, 255, nothing)
    cv.createTrackbar('Vmin', 'image', 0, 255, nothing)
    cv.createTrackbar('Hmax', 'image', 0, 179, nothing)
    cv.createTrackbar('Smax', 'image', 0, 255, nothing)
    cv.createTrackbar('Vmax', 'image', 0, 255, nothing)
    cv.createTrackbar('m <-> c', 'image', 0, 2, nothing)
    cv.createTrackbar('shoot_x', 'image', 0, image_width, nothing)
    cv.createTrackbar('shoot_y', 'image', 0, image_height, nothing)
    set_trackbar([179, 255, 255], [0, 0, 0])
    cv.setTrackbarPos('m <-> c', 'image', 1)
    cv.setTrackbarPos('shoot_x', 'image', int(image_width / 2))
    cv.setTrackbarPos('shoot_y', 'image', int(image_height / 2))
    cv.setMouseCallback('image', draw_circle)

    w = window()
    w.create(window_name)

    while(img is None):
        rospy.sleep(0.01)

    while not rospy.is_shutdown():

        key = cv.waitKey(1) & 0xff
        if key == ord('p') and wait == False and not click:
            wait = True
        elif key == ord('p') and wait == True and not click:
            wait = False

        name, status = has_color(window_name, key)
        lower, upper = w.get_range('mask')
        lower_bar, upper_bar = get_trackbar()

        if click and is_mask:
            h, s, v = hsv[pixel['y'], pixel['x']]
            [hl, sl, vl], [hu, su, vu] = w.get_range('mask')
            lower_current = [min(h, hl), min(s, sl), min(v, vl)]
            upper_current = [max(h, hu), max(s, su), max(v, vu)]
            w.push_range('mask', lower_current, upper_current)
            set_trackbar(lower_current, upper_current)
        elif compare_range(lower, upper, lower_bar, upper_bar):
            w.push_range('mask', lower_bar, upper_bar)
        elif status:
            if w.select[name]:
                lower_current, upper_current = w.get_range('mask')
                w.push_range(name, lower_current, upper_current)
                cv.setTrackbarPos('m <-> c', 'image', 2)
                is_mask = False
            else:
                lower_current, upper_current = w.get_param(name)
                w.push_range('mask', lower_current, upper_current)
                set_trackbar(lower_current, upper_current)
                cv.setTrackbarPos('m <-> c', 'image', 0)
                is_mask = True
            w.select[name] = not w.select[name]
        elif is_mask:
            if key == ord('z'):
                w.undo_range('mask')
            elif key == ord('x'):
                w.redo_range('mask')
            elif key == ord('c'):
                w.reset_range('mask')
        elif key == ord('s'):
            w.save()
        elif key == ord('q'):
            break
        x = cv.getTrackbarPos('shoot_x', 'image')
        y = cv.getTrackbarPos('shoot_y', 'image')
        w.show_image(window_name)
        cv.circle(hsv, (int(x), int(y)), 5, (100, 255, 255), -1)
        cv.imshow('image', hsv)
        cv.imshow('imageBGR', img)
        click = False
        status = False
    cv.destroyAllWindows()


if __name__ == '__main__':
    rospy.sleep(1)
    
    rospy.init_node('color_range_main', anonymous=True)
    namespace = sys.argv[1]
    #############################################################################################
    camera_position = rospy.get_param(namespace+'/camera_position', 'null')
    camera_topic = rospy.get_param(namespace+'/camera_topic', 'null')
    mission = rospy.get_param(namespace+'/mission', 'null')
    number = rospy.get_param(namespace+'/number', 'null')
    #############################################################################################
    clear_screen()
    print_result('TOPIC: ' + str(camera_topic))
    print_result('CAMERA: ' + str(camera_position))
    print_result('MISSION: ' + str(mission))
    print_result('NUMBER: ' + str(number))

    k = str(raw_input("Press y or Y to continue, Any key to exit: "))
    if not k.lower() == 'y':
        exit(0)
    #############################################################################################
    print_result('INITIAL VARIABLE')
    app = QApplication(sys.argv)
    screen_resolution = app.desktop().screenGeometry()
    screen_width, screen_height = screen_resolution.width(), screen_resolution.height()

    if camera_position == 'bottom':
        image_width, image_height = CONST.IMAGE_BOTTOM_WIDTH, CONST.IMAGE_BOTTOM_HEIGHT
    else:
        image_width, image_height = CONST.IMAGE_FRONT_WIDTH, CONST.IMAGE_FRONT_HEIGHT
    #############################################################################################

    print_result('SUBSCRIBER')
    rospy.Subscriber(camera_topic, CompressedImage, camera_callback)

    print_result('SELECT COLOR')
    select_color()
