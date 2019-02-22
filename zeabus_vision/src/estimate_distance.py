#!/usr/bin/python2.7
"""
    File name:estimate_distance.py
    Author: skconan
    Date created: 2019/02/16
    Python Version: 2.7
"""

import rospy
import cv2 as cv
import numpy as np
import matplotlib.pyplot as plt
from sensor_msgs.msg import CompressedImage
from nav_msgs.msg import Odometry
import math
import rospy
from operator import itemgetter
from vision_lib import *    
from geometry_msgs.msg import Point

bgr = None 
sub_sampling = 0.5
depth = None

def normalize(gray):
    return np.uint8(255 * (gray - gray.min()) / (gray.max() - gray.min()))

def image_callback(msg):
    global bgr, sub_sampling
    arr = np.fromstring(msg.data, np.uint8)
    bgr = cv.resize(cv.imdecode(arr, 1), None,
                    fx=sub_sampling, fy=sub_sampling)

def pressure_callback(msg):
    global depth, sub_sampling
    depth = msg.pose.pose.position.z

def estimate_distance():
    global bgr,depth
    position = Point()
    

    pub = rospy.Publisher('/vision/estimate_distance', Point, queue_size=10)

    while not rospy.is_shutdown():
        if bgr is None:
            continue
        else:
            position.x = 100
            position.z = 100
            position.y = 100
        
        gray = cv.cvtColor(bgr, cv.COLOR_BGR2GRAY)
        bg = cv.medianBlur(gray, 61)
        fg = cv.medianBlur(gray, 5)

        sub_sign = np.int16(fg) - np.int16(bg)
        sub_pos = np.clip(sub_sign.copy(),0,sub_sign.copy().max())
        sub_neg = np.clip(sub_sign.copy(),sub_sign.copy().min(),0)

        sub_pos = normalize(sub_pos)
        sub_neg = normalize(sub_neg)

        publish_result(sub_pos,'gray','sub_pos')
        # publish_result(sub_neg,'gray','sub_neg')

        _, obj = cv.threshold(
            sub_pos, 0, 255, cv.THRESH_BINARY + cv.THRESH_OTSU
        )

        _, contours, _ = cv.findContours(
            obj.copy(), cv.RETR_EXTERNAL, cv.CHAIN_APPROX_NONE
        )
        display = cv.cvtColor(sub_neg.copy(), cv.COLOR_GRAY2BGR)
        bg = np.uint8(bg)
        fg = np.uint8(fg)


        r = 0
        circle = []
        for cnt in contours:
            area_cnt = cv.contourArea(cnt)
            (x,y),radius = cv.minEnclosingCircle(cnt)
            center = (int(x),int(y))
            radius = radius
            area_cir = math.pi*(radius**2)
            
            if area_cnt < 400:
                continue
            if area_cir <= 0 or area_cnt/area_cir < 0.8:
                continue
            print(area_cnt)
            cv.circle(display,center,int(radius),(255,0,0),-1)
            circle.append([x,y,radius])
       
        row, col = gray.shape
        if len(circle) > 0:
            circle = sorted(circle, key=itemgetter(2),reverse=True) 
            circle = circle[0]
            x,y,radius = circle
            cv.circle(display,(int(x),int(y)),int(radius),(0,0,255),2)
            diameter = 2.*radius
            pixel_per_cm = diameter/4.3
            print("radius",radius)
            print("pixel_per_cm",pixel_per_cm)
            print("depth",depth)
                
            x_distance_pixel = row/2.-y
            y_distance_pixel = col/2.-x
            print("row col",row,col)
            print("x y",x,y)
            print("x_distance_pixel y_distance_pixel:",x_distance_pixel,y_distance_pixel)
            x_distance_cm = float(x_distance_pixel) / pixel_per_cm
            y_distance_cm = float(y_distance_pixel) / pixel_per_cm
            print("x_distance_cm:",x_distance_cm)
            print("y_distance_cm:",y_distance_cm)
            x_distance_meter = x_distance_cm/100.
            y_distance_meter = y_distance_cm/100.
            print("x_meter:",x_distance_meter)   
            print("y_meter:",y_distance_meter)   
            cv.circle(display,(int(x),int(y)),int(radius),(0,255,255),-1)
            position.x = x_distance_meter
            position.y = y_distance_meter

        pub.publish(position)

        publish_result(obj,'gray','obj')
        
        publish_result(bgr,'bgr','original_bgr')
        publish_result(bg,'gray','bg')
        publish_result(fg,'gray','fg')
        publish_result(display,'bgr','display')

        rospy.sleep(1)

        k = cv.waitKey(1) & 0xff
        if k == ord('q'):
            break

    cv.destroyAllWindows()


if __name__ == '__main__':
    rospy.init_node('vision_estimate_distance', anonymous=False)
    image_topic = "/vision/bottom/image_raw/compressed"
    rospy.Subscriber(image_topic, CompressedImage, image_callback) 
    rospy.Subscriber("/sensor/pressure", Odometry, pressure_callback) 
    estimate_distance()