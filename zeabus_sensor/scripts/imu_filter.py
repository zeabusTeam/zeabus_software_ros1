#!/usr/bin/python2.7

import rospy
import tf
from sensor_msgs.msg import Imu
import os
import numpy as np
PATH = os.path.dirname(os.path.abspath(__file__))
print(PATH)

class CSVManager:
    def __init__(self, file_path):
        self.f = open(file_path,'w+')

    def writer(self, data_list=[]):
        if len(data_list) == 0:
            return True
        data = data_list
        if type(data_list[0]) in [int, float]:
            data = []
            for d in data_list:
                data.append(str(d))
        data = ", ".join(data)
        self.f.write(data+"\n")

    def reader(self):
        # not now
        pass 

class SensorFusion:
    def __init__(self,axis=0):
        rospy.init_node("SensorFusion",anonymous=True)
        rospy.Subscriber("/imu/data",Imu, self.imu_callback)
        # rospy.Subscriber("/gx4_45_imu/data",Imu, self.imu_callback)
        
        self.orientation = None
        self.euler = [0,0,0]
        self.old_acc = [0,0,0]
        self.acc = [[0],[0],[0]]
        # self.csv = CSVManager(PATH + "/csv/compare_old_new.csv")
        self.count = 0
        self.axis = axis
        self.sampling = 50

    def imu_callback(self,msg):
        self.acc[0].append(msg.linear_acceleration.x)    
        self.acc[1].append(msg.linear_acceleration.y)    
        self.acc[2].append(msg.linear_acceleration.z)    
        # self.quaternion = (
        #     msg.orientation.x,
        #     msg.orientation.y,
        #     msg.orientation.z,
        #     msg.orientation.w
        # )
        # self.euler = tf.transformations.euler_from_quaternion(self.quaternion)
        # print(self.euler)
        self.run()

    def run(self):
        if len(self.acc[self.axis]) > 0 and len(self.acc[self.axis]) % self.sampling == 0:
            # # print("Count: " + str(count*self.sampling)+"   Mean | Max | Min | Variance | Total Variance |")
            # mean = np.mean(self.acc[self.axis][self.count*self.sampling:self.count*self.sampling + self.sampling])
            # max = np.max(self.acc[self.axis][self.count*self.sampling:self.count*self.sampling + self.sampling])
            # min = np.min(self.acc[self.axis][self.count*self.sampling:self.count*self.sampling + self.sampling])
            # var = np.var(self.acc[self.axis][self.count*self.sampling:self.count*self.sampling + self.sampling])
            mean = np.mean(self.acc[self.axis][1:self.count*self.sampling + self.sampling])
            max = np.max(self.acc[self.axis][1:self.count*self.sampling + self.sampling])
            min = np.min(self.acc[self.axis][1:self.count*self.sampling + self.sampling])
            var = np.var(self.acc[self.axis][1:self.count*self.sampling + self.sampling])
            # var = np.var(self.acc[self.axis][1:])
            range = abs(max - min)
            self.count += 1
            if self.count % 20 == 0 and self.count > 0:
                print("=============================== Axis: %s ========================================"%self.axis)
                print("%8s   %10s   %10s   %10s   %10s   %14s %14s" %("Count |", "Mean |", "Max |" ,"Min |","Range |" ,"Variance |", "Total Variance |"))
            print("%6d |  %+7.6f |  %+7.6f |  %+7.6f |  %+7.6f |  %+9.10f |  %+9.10f |"%(self.count*self.sampling,mean,max,min,range,var,var))

if __name__ == '__main__':
    axis = raw_input("Axis: ")
    ax = {'x':0,'y':1,'z':2}
    sf = SensorFusion(ax[axis.lower()])
    print("=============================== Axis: %s ========================================"%axis)
    print("%8s   %10s   %10s   %10s   %10s   %14s %14s" %("Count |", "Mean |", "Max |" ,"Min |","Range |" ,"Variance |", "Total Variance |"))
    rospy.spin()