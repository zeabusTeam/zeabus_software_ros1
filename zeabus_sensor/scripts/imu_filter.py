#!/usr/bin/python2.7
"""
    File name: imu_filter.py
    Author: skconan
    Date created: 2018/10/29
    Python Version: 2.7
    Description: 
"""

from sensor_msgs.msg import Imu
from std_msgs.msg import Float64
from geometry_msgs.msg import Twist
import rospy
import numpy as np
import matplotlib.pyplot as plt

queue = []
pub = rospy.Publisher("/imu/velocity", Twist, queue_size=1)
# pub_float = rospy.Publisher("/baro/float64", Float64, queue_size=90)
count = 0
# r = 0.05
var =[[],[],[]]
linear = [[],[],[]]
v = [[],[]]
acc = [[],[]]
def imu_filtering(x,y,z):
    global pub, queue, count, r, var, linear
    
    linear[0].append(x)
    linear[1].append(y)
    linear[2].append(z)

    for i in range(3):
        var[i].append(np.var(linear[i]))
    print(var[0][-1],var[1][-1],var[2][-1])
    if count % 100 == 0:
        var =[[],[],[]]
        linear = [[],[],[]]
        count = 0
    count += 1

def integration(f1,f2):
    # trapeze
    return ((f1 + f2)/2.)*(0.05)

def acc_to_velocity(acc_x,acc_y):
    global acc, v
    # acc_x 
    velocity = Twist()
    # if abs(acc_x) > 0.02:
    if abs(acc_x) > 0.1:
        acc[0].append(acc_x)
    # else:
    #     acc[0].append(0)

    # if not 0 <= acc[1] <= 0.15:
    if not 0.5 <= acc[1] <= 0.5:
        acc[1].append(acc_y)
    # else:
    #     acc[1].append(0)



    if len(acc[0]) >= 2 and len(acc[1]) >= 2:
        if acc_x == acc[0][-1]:
            acc_x = -acc[0][-1]
        
        if acc_y == acc[1][-1]:
            acc_y = -acc[1][-1]
        
        v[0].append(integration(acc[0][-2], acc[0][-1]))
        v[1].append(integration(acc[1][-2], acc[1][-1]))
        
        acc[0].pop(0)
        acc[1].pop(0)

    if len(v[0]) >= 2 and len(v[1]) >= 2: # and abs(v[0][-1] - v[0][-2]) > 0.01 :
        v[0][-1] += v[0][-2]
        v[1][-1] += v[1][-2]
        v[0].pop(0)    
        v[1].pop(0)    
     # and abs(v[1][-1] - v[1][-2]) > 0.01:
        
    if len(v[0]) and len(v[1]): 
        velocity.linear.x = v[0][-1]
        velocity.linear.y = v[1][-1]
        print(velocity)
        pub.publish(velocity)
   

def imu_callback(msg):
    # Input: 100Hz
    # Output: 80 - 90Hz
    x = msg.linear_acceleration.x
    y = msg.linear_acceleration.y
    z = msg.linear_acceleration.z

    # imu_filtering(x,y,z)
    acc_to_velocity(x,y)

if __name__ == '__main__':
    rospy.init_node('ImuFilter')
    rospy.Subscriber("/imu/sub_sampling", Imu, imu_callback)
    print("IMU FILTER")
    rospy.spin()
