#!/usr/bin/python2.7
"""
    File name: imu_acc2vel.py
    Author: skconan
    Date created: 2018/12/04
    Python Version: 2.7
    Description: For integate acceleration to velocity
"""

import rospy
import tf
from sensor_msgs.msg import Imu
import numpy as np
from matplotlib import pyplot as plt


class Acc2Vel:
    def __init__(self):
        rospy.init_node("Acc2Vel", anonymous=True)
        ns = "zeabus_sensor"
        name = "imu"
        imu_rate = rospy.get_param(ns + "/" + name + "/imu_rate", 500)
        pub_topic = rospy.get_param(
            ns + "/" + name + "/pub_topic", "/imu/data")

        rospy.Subscriber(pub_topic, Imu, self.imu_callback)
        # rospy.Subscriber("/gx4_45_imu/data",Imu, self.imu_callback)

        self.imu_rate = imu_rate
        self.orientation = None
        self.euler = [0, 0, 0]
        self.acc = [[0], [0], [0]]
        # self.csv = CSVManager(PATH + "/csv/compare_old_new.csv")
        self.count = 0
        self.window_size = 50
        self.variance = 0.0000000125
        self.diff = 0.000594
        self.velocity = [0, 0, 0]
        print("Wait subscriber")

    def imu_callback(self, msg):
        self.acc[0].append(msg.linear_acceleration.x)
        self.acc[1].append(msg.linear_acceleration.y)
        self.acc[2].append(msg.linear_acceleration.z)
        self.run()

    def run(self):
        if len(self.acc[0]) >= 10000:
            for axis in range(3):
                self.acc[axis] = self.acc[axis][-self.window_size:]

        if len(self.acc[0]) >= self.window_size:
            for axis in range(3):
                data = self.acc[axis][-self.window_size:]
                var = np.var(data)
                max = np.max(data)
                min = np.min(data)
                mean = np.mean(data)
                if var > self.variance:
                    acc = max
                    if abs(min) > max:
                        acc = min

                    if abs(acc - mean) > self.diff:
                        self.velocity[axis] += acc * (1. / self.imu_rate)

                print("\n%8s   %10s   %10s   %10s   %14s   %10s" %
                      ("Axis |", "Mean |", "Max |", "Min |", "Variance |", "Velocity |"))
                print("%6d |  %+7.6f |  %+7.6f |  %+7.6f |  %+9.10f |  %+7.6f |" %
                      (axis, mean, max, min, var,self.velocity[axis]))


if __name__ == '__main__':
    acc2vel = Acc2Vel()
    rospy.spin()
