#!/usr/bin/python2
"""
    File name: map.py
    Author: skconan
    Date created: 2018/11/29
    Python Version: 2.7
    Description: Display object position and auv state 
"""

import cv2 as cv
import numpy as np
import tf
import rospy
import math
from nav_msgs.msg import Odometry
from std_msgs.msg import Bool, String
from zeabus_navigator.msg import object_position
from zeabus_library.srv import SetObject, GetObject, DelObject
from zeabus_library.msg import PoseEuler, ObjectResult


class Map:
    def __init__(self, map_row, map_col, area_row_mm, area_col_mm):
        self.map_row, self.map_col = map_row, map_col
        self.area_row, self.area_col = area_row_mm * 100, area_col_mm * 100
        self.ratio = min(map_row / (2. * self.area_row),
                         map_col / (2. * self.area_col))
        self.font = cv.FONT_HERSHEY_SIMPLEX

        self.map_obj = np.zeros((self.map_row, self.map_col), np.uint8)
        self.map_auv = np.zeros((self.map_row, self.map_col), np.uint8)
        self.map_detail = np.zeros((self.map_row, self.map_col), np.uint8)

        self.object = {}
        self.object_name = []

        self.object_name = ['gate', 'flare', 'drum']
        for name in self.object_name:
            self.object[name] = {'x': 0, 'y': 0, 'z': 0, 'yaw': 0}

        self.pixel = {}
        self.auv = {'x': 0, 'y': 0, 'z': 0, 'yaw': -3, 'radius': 5}
        self.auv_state = {'x': 0, 'y': 0, 'z': 0, 'yaw': math.radians(-100)}
        self.create_map_detail()

        cv.namedWindow('map')
        cv.moveWindow('map', 100, 100)
        cv.setMouseCallback('map', self.get_mouse_position)

        rospy.Subscriber('/auv/state', Odometry, self.get_auv_state)
        rospy.Service('nav/setObj', SetObject(), self.set_object_callback)
        rospy.Service('nav/getObj', GetObject(), self.get_object_callback)
        rospy.Service('nav/delObj', DelObject(), self.del_object_callback)

    def del_object_callback(self, msg):
        name = msg.name.data
        try:
            self.object.pop(name)
            self.object_name.remove(name)
            self.update_object()
            return Bool(True)
        except:
            return Bool(False)

    def get_object_callback(self, msg):
        name = msg.name.data
        object = ObjectResult()
        try:
            x = self.object[name]['x']
            y = self.object[name]['y']
            z = self.object[name]['z']
            yaw = self.object[name]['yaw']

            object.success = Bool(True)
            object.pose.position.x = x
            object.pose.position.y = y
            object.pose.position.z = z
            object.pose.orientation.yaw = yaw
            return object
        except:
            object.success = Bool(False)
            return object

    def set_object_callback(self, msg):
        name = msg.name.data
        pose = msg.pose
        position = pose.position
        x = position.x
        y = position.y
        z = position.z
        yaw = pose.orientation.yaw

        try:
            if name not in self.object_name:
                self.object_name.append(name)
                self.object[name] = {'x': 0, 'y': 0, 'z': 0, 'yaw': 0}

            self.object[name]['x'] = x
            self.object[name]['y'] = y
            self.object[name]['z'] = z
            self.object[name]['yaw'] = yaw
            self.update_object()
            return Bool(True)
        except:
            return Bool(False)

    def update_object(self):
        self.map_obj.fill(0)
        for name in self.object_name:
            if self.object[name]['x'] == 0:
                continue
            x = self.object[name]['y']
            y = self.object[name]['x']
            x = self.map_col // 2 - x * 100 * self.ratio
            y = self.map_row // 2 - y * 100 * self.ratio
            x = int(x)
            y = int(y)
            cv.putText(self.map_obj, name, (x, y), self.font,
                       1, (255), 2, cv.LINE_AA)

    def create_map_detail(self):
        cv.line(self.map_detail, (self.map_col // 2, 0),
                (self.map_col // 2, self.map_row), (200), 2)
        cv.line(self.map_detail, (0, self.map_row // 2),
                (self.map_col, self.map_row // 2), (200), 2)
        for i in range(20):
            cv.circle(self.map_detail, (int(self.map_col // 2),
                                        int(self.map_row // 2)), (int(100 * self.ratio)) * i, (200), 1)

    def insert_object(self, name, x, y):
        self.object[name]['x'] = x
        self.object[name]['y'] = y
        self.update_object()

    def get_mouse_position(self, event, x, y, flags, param):
        if event == cv.EVENT_LBUTTONDOWN:
            self.click = True
            self.pixel['x'], self.pixel['y'] = x, y

    def update_auv(self):
        x, y, z, yaw = self.state_robot_to_map()
        self.auv['x'] = int(x)
        self.auv['y'] = int(y)
        self.auv['z'] = int(z)
        self.auv['yaw'] = int(yaw)
        self.map_auv = np.zeros((self.map_row, self.map_col), np.uint8)

        yaw = math.degrees(yaw)
        if abs(yaw) < 45 or abs(yaw) > 135:  
            rect = (x, y + 20), (5, 40), -yaw
        else:
            if yaw > 0:
                rect = (x + 20, y) , (40, 5), 90-yaw
            else:
                rect = (x - 20, y) , (40, 5), -(90+yaw)

        box = cv.boxPoints(rect)
        box = np.int0(box)
        cv.drawContours(self.map_auv, [box], 0, (255), -1)
        cv.circle(self.map_auv, (x, y), self.auv['radius'], (255), -1)

    def run(self):
        while not rospy.is_shutdown():
            self.update_auv()
            map_display = cv.merge(
                (self.map_obj, self.map_auv, self.map_detail))

            cv.imshow('map', map_display)
            k = cv.waitKey(500) & 0xff
            if k == ord('q'):
                break

    def get_auv_state(self, data):
        self.pose = data.pose.pose
        pose = self.pose

        temp = (pose.orientation.x, pose.orientation.y,
                pose.orientation.z, pose.orientation.w)
        euler_angular = tf.transformations.euler_from_quaternion(temp)

        self.auv_state['x'] = pose.position.x
        self.auv_state['y'] = pose.position.y
        self.auv_state['z'] = pose.position.z

        self.auv_state['yaw'] = euler_angular[2]

    def state_robot_to_map(self):
        x = int(self.map_col // 2 - self.auv_state['y'] * 100 * self.ratio)
        y = int(self.map_row // 2 - self.auv_state['x'] * 100 * self.ratio)
        z = self.auv_state['z']
        yaw = self.auv_state['yaw']
        return x, y, z, yaw


if __name__ == '__main__':
    rospy.init_node("NavigatorMap")
    row_map, col_map, row_area, col_area = 1000, 1000, 20, 20
    map = Map(row_map, col_map, row_area, col_area)
    map.run()
