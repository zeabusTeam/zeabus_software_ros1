#!/usr/bin/python2.7
"""
    File name: control_rate_pwm_auv_state.py
    Author: skconan
    Date created: 2018/10/29
    Python Version: 2.7
    Description: change rate
"""

from sensor_msgs.msg import Imu
from std_msgs.msg import Float64
from hg_ros_pololu.msg import Pwm
from geometry_msgs.msg import Twist
from nav_msgs.msg import Odometry
import rospy
import numpy as np
import matplotlib.pyplot as plt
import rospkg
queue = []
# pub = rospy.Publisher("/gx4_45_imu/data", Imu, queue_size=1)
# pub = rospy.Publisher("/pwm_auv_state", Float64[], queue_size=1)
count = 0
# r = 0.05
path = rospkg.RosPack().get_path('zeabus_sensor') + '/scripts'
rate = 1/20.
f = open(path+'/auv_state_pwm_00.csv','w+')
pwm = Pwm()
pwm.pwm = [1500]*8
auv_state = Odometry()
imu = Imu()
get_data = True

pub_pwm = rospy.Publisher("/pwm/sub_sampling", Pwm, queue_size=1)
pub_imu = rospy.Publisher("/imu/sub_sampling", Imu, queue_size=1)
pub_state = rospy.Publisher("/state/sub_sampling", Odometry, queue_size=1)

def change_rate():
    global rate,auv_state,pwm, count, f, get_data, imu
    pub_imu.publish(imu)
    pub_pwm.publish(pwm)
    pub_state.publish(auv_state)
    string = str(imu.linear_acceleration.x) + ", " + str(imu.linear_acceleration.y) + "\n"
    f.write(string)
    rospy.sleep(rate)

def pwm_callback(msg):
    global pwm, get_data
    # print(msg,get_data)
    # if msg is None:
    #     get_data = False
    if get_data:
        print("getdata PWM")
        for i in range(8):
            pwm.pwm[i]= (msg.pwm[i])

def auv_state_callback(msg):
    global auv_state, get_data
    # print(msg,get_data)
    # if msg is None:
    #     get_data = False
    if get_data:
        print("getdata AUV STATE")
        auv_state.twist.twist.linear.x = (msg.twist.twist.linear.x)
        auv_state.twist.twist.linear.y = (msg.twist.twist.linear.y)

def imu_callback(msg):
    global imu, get_data
    if msg is None:
        get_data = False
    if get_data:
        print("getdata IMU")
        # imu.linear_acceleration.x = msg.linear_acceleration.z
        # imu.linear_acceleration.y = msg.linear_acceleration.x
        imu.linear_acceleration.x = msg.linear_acceleration.x
        imu.linear_acceleration.y = msg.linear_acceleration.y

if __name__ == '__main__':
    rospy.init_node('SubSampling')
    # rospy.Subscriber("/gx4_45_imu/data", Imu, imu_callback)
    rospy.Subscriber("/imu/data", Imu, imu_callback)
    rospy.Subscriber("/auv/state", Odometry, auv_state_callback)
    rospy.Subscriber("/pwm", Pwm, pwm_callback)
    # print("IMU FILTER")
    # rospy.spin()
    while not rospy.is_shutdown():
        # get_data = False
        change_rate()
        # get_data = True

