#!/usr/bin/python2
from nav_msgs.msg import Odometry
from zeabus_navigator.msg import object_position
import cv2 as cv
import numpy as np
import tf
import rospy
import math

class Map:
    def __init__(self, row_map, col_map, ratio):
        self.row, self.col = row_map, col_map
        self.ratio = ratio
        self.map_obj = np.zeros((row_map, col_map),np.uint8)
        self.map_auv = np.zeros((row_map, col_map),np.uint8)
        self.map_detail = np.zeros((row_map, col_map),np.uint8)
        self.object_name = ['gate', 'flare', 'drum']
        self.object_color = {'gate':10, 'flare':100, 'drum':200}
        self.object = {}
        for name in self.object_name:
            self.object[name]['x'] = 0
            self.object[name]['y'] = 0
            self.object[name]['z'] = 0
            self.object[name]['yaw'] = 0
        self.auv = {'x':0,'y':0,'z':0,'radius':5}
        self.color = 0
        self.pixel = {}
        self.auv_state = {}
        self.create_map_detail()
        rospy.Subscriber('/auv/state', Odometry, self.get_auv_state)
        rospy.Subscriber('/map/update_object', object_position, self.update_object_position)

    def update_object_position(self,msg):
        name = msg.object
        x = msg.x
        y = msg.y
        z = msg.z

        self.object[name]['x'] = x
        self.object[name]['y'] = y
        self.object[name]['z'] = z
        self.draw_object()

    def draw_object(self):
        self.map_obj.fill(0)
        for name in self.object_name:
            x = self.object[name]['x'] 
            y = self.object[name]['y'] 
            x = (2000 - y*100) / self.ratio
            y = (2000 - x*100) / self.ratio
            cv.circle(self.map_obj, (x,y), 10, self.object_color[name])

    def create_map_detail(self):
        cv.line(self.map_detail, (self.col//2,0),(self.col//2,self.row), (200), 2)
        cv.line(self.map_detail, (0,self.row//2),(self.col,self.row//2), (200), 2)
        for i in range(20):
            cv.circle(self.map_detail, (self.col//2, self.row//2), (100//ratio)*i, (200),2)
    
    def insert_object(self, name, x, y):
        self.object[name]['x'] = x
        self.object[name]['y'] = y
        self.draw_object()

    def update_color(self):
        self.color += 20

    def get_mouse_position(self, event, x, y, flags, param):
        if event == cv.EVENT_LBUTTONDOWN:
            self.click = True
            self.pixel['x'], self.pixel['y'] = x, y

    def update_auv(self, x, y, z, yaw):
        print("YAW:", yaw)
        x = int(x)
        y = int(y)
        z = int(z)
        self.auv['x'] = x
        self.auv['y'] = y
        self.auv['z'] = z
        self.map_auv = np.zeros((row_map, col_map),np.uint8)
        rect = (x, y+self.auv['radius']), (5, 20), math.degrees(-yaw)
        box = cv.boxPoints(rect)
        box = np.int0(box)
        cv.drawContours(self.map_auv, [box], 0, (255), -1)
        cv.circle(self.map_auv, (x,y), self.auv['radius'], (255), -1)

    def run(self):
        cv.namedWindow('map')
        cv.moveWindow('map', 100, 100)
        cv.setMouseCallback('map', self.get_mouse_position)

    
        # map_display = cv.bitwise_or(self.map, self.map_auv)
        map_display = cv.merge((self.map_obj,self.map_auv,self.map_detail))
        # map_display = cv.merge((map_display,map_display,map_display))
        cv.imshow('map',map_display) 
        cv.waitKey(1)   
        rospy.sleep(0.5)

    def get_auv_state(self,data):
        self.pose = data.pose.pose
        pose = self.pose

        temp = (pose.orientation.x, pose.orientation.y, pose.orientation.z, pose.orientation.w)
        euler_angular = tf.transformations.euler_from_quaternion(temp)

        self.auv_state['x'] = pose.position.x
        self.auv_state['y'] = pose.position.y
        self.auv_state['z'] = pose.position.z

        self.auv_state['roll'] = euler_angular[0]
        self.auv_state['pitch'] = euler_angular[1]
        self.auv_state['yaw'] = euler_angular[2]

    def state_robot_to_map(self):
        x = (2000 - map.auv_state['y']*100) / self.ratio
        y = (2000 - map.auv_state['x']*100) / self.ratio
        z = map.auv_state['z']
        yaw = map.auv_state['yaw']  
        return x, y, z, yaw

if __name__ == '__main__':
    rospy.init_node("NavigationMap")
    row_map, col_map, ratio = 1000, 1000, 4
    map = Map(row_map, col_map, ratio)
    while not rospy.is_shutdown():
        map.run()
        x, y, z, yaw = map.state_robot_to_map()
        map.update_auv(x, y, z, yaw)