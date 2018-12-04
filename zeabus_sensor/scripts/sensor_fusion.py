#!/usr/bin/python2.7

import rospy
import tf
from sensor_msgs.msg import Imu
import os
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
    def __init__(self):
        rospy.init_node("SensorFusion")
        rospy.Subscriber("/imu/data",Imu, self.imu_callback)
        rospy.Subscriber("/gx4_45_imu/data",Imu, self.old_imu_callback)
        
        self.orientation = None
        self.euler = [0,0,0]
        self.old_velocity = [0,0,0]
        self.velocity = [0,0,0]
        self.csv = CSVManager(PATH + "/csv/compare_old_new.csv")

    def imu_callback(self,msg):
        self.velocity[0] = msg.linear_acceleration.x    
        self.velocity[1] = msg.linear_acceleration.y    
        self.velocity[2] = msg.linear_acceleration.z    
        # self.quaternion = (
        #     msg.orientation.x,
        #     msg.orientation.y,
        #     msg.orientation.z,
        #     msg.orientation.w
        # )
        # self.euler = tf.transformations.euler_from_quaternion(self.quaternion)
        # print(self.euler)

    def old_imu_callback(self, msg):
        self.old_velocity[0] = msg.linear_acceleration.x    
        self.old_velocity[1] = msg.linear_acceleration.y    
        self.old_velocity[2] = msg.linear_acceleration.z
        self.run()

    def run(self):
        self.csv.writer(self.velocity + self.old_velocity)


if __name__ == '__main__':
    sf = SensorFusion()
    rospy.spin()