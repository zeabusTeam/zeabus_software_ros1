#!/usr/bin/python2.7
"""
    File name: pressure_filter.py
    Author: skconan
    Date created: 2018/10/29
    Python Version: 2.7
    Description: Filter spike noise
"""

from nav_msgs.msg import Odometry
from zeabus_elec_ros_peripheral_bridge.msg import barometer
from std_msgs.msg import Float64
import rospy
import numpy as np
import matplotlib.pyplot as plt

queue = []
#pub = rospy.Publisher("/barometer/data", Odometry, queue_size=1)
pub = rospy.Publisher("/sensor/pressure", Odometry, queue_size=1)
# pub_float = rospy.Publisher("/baro/float64", Float64, queue_size=90)
count = 0
r = 0.05


def mean_filtering(z):
    global pub, queue, count, r
    queue.append(z)
    len_q = len(queue)
    if len_q >= 10:
        first = queue[0]
        threshold = np.mean(queue)
        if abs(threshold) - r < abs(first) < abs(threshold) + r:
            m = Odometry()
            m.pose.pose.position.z = first
            pub.publish(m)
            # pub_float.publish(first)

        queue = queue[1:]
    # count += 1



def baro_callback(msg):
    # Input: 100Hz
    # Output: 80 - 90Hz
    pose = msg.pose.pose
    z = pose.position.z
    mean_filtering(z)


if __name__ == '__main__':
    rospy.init_node('PressureFilter')
    rospy.Subscriber("/baro/odom", Odometry, baro_callback)
    print("BARO FILTER")
    rospy.spin()
