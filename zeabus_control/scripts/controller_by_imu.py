#!/usr/bin/python2

from sensor_msgs.msg import Imu
from nav_msgs.msg import Odometry
from std_msgs.msg import Float32
from geometry_msgs.msg import Twist
import rospy
import cv2 as cv
from colorama import *
import numpy as np

DEBUG = True
def print_float(input):
    string = ""
    for i in range(3):
        string += "{:<10.3f}".format(input[i])
    print_debug(string, "GREEN")

def print_debug(text, mode="DEBUG"):
    color_mode = {
        "BLUE": Fore.BLUE,
        "GREEN": Fore.GREEN,
        "YELLOW": Fore.YELLOW,
        "RED": Fore.RED,
        "END": Style.RESET_ALL,
    }
    if DEBUG:
        print(color_mode[mode] + text + color_mode["END"])


class ControllerImu:
    def __init__(self):
        rospy.init_node("ControllerByImu")
        self.KP = [0] * 3
        self.KI = [0] * 3
        self.KD = [0] * 3
        self.error = [[0, 0, 0]] * 3
        self.ctrl_signal = [0] * 3
        self.prev_ctrl_signal = [0] * 3
        # 4220 65901
        self.tunning = np.zeros((400, 800), np.uint8)
        self.vel_current = [0] * 3
        self.vel_previous = [0] * 3
        self.acc_current = [0] * 3
        self.acc_previous = [0] * 3
        self.distance = [0]*3
        self.force = Twist()
        # self.time_previous = self.time_previous.nsecs
        # print(self.time_previous)
        self.time_current = 0
        self.time_previous = 0
        self.is_init = False
        rospy.Subscriber("/imu/data", Imu, self.imu_callback)
        rospy.Subscriber("/barometer/data", Odometry, self.pressure_callback)

        self.pub_force = rospy.Publisher("/cmd_vel", Twist, queue_size=1)

        self.pub_x = rospy.Publisher("/ctrl_imu_x", Float32, queue_size=1)
        self.pub_y = rospy.Publisher("/ctrl_imu_y", Float32, queue_size=1)
        self.pub_z = rospy.Publisher("/ctrl_imu_z", Float32, queue_size=1)

        self.pub_error_x = rospy.Publisher(
            "/ctrl_imu_error_x", Float32, queue_size=1)
        self.pub_error_y = rospy.Publisher(
            "/ctrl_imu_error_y", Float32, queue_size=1)
        self.pub_error_z = rospy.Publisher(
            "/ctrl_imu_error_z", Float32, queue_size=1)
        cv.namedWindow('Tunning')
        cv.moveWindow('Tunning', 100, 100)

        for i in range(3):
            for j in ['P', 'I', 'D']:
                cv.createTrackbar('K' + str(j) + '_' + str(i),
                                  'Tunning', 0, 1000, self.nothing)
                cv.setTrackbarPos('K' + str(j) + '_' +
                                  str(i), 'Tunning', self.KP[i])

    def nothing(self):
        pass

    # def offset_acceleration(self):
    #     for i in range(3):
    #         self.acc_current[i] = int(self.acc_current[i]*100) + self.acc_offset[1] 


    def integration(self,f1,f2,delta_t):
        # trapeze
        return ((f1 + f2)/2.)*delta_t

    def acc_to_vel(self):
        delta_t = self.time_current - self.time_previous
        delta_t *= 1e-9
        for i in range(3):
            if abs(self.acc_current[i] - self.acc_previous[i]) > 0.1: 
                self.vel_current[i] = self.integration(self.acc_previous[i],self.acc_current[i],delta_t)
            else:
                self.vel_current[i] = self.integration(self.acc_previous[i],self.acc_current[i],0)

    def vel_to_distance(self):
        delta_t = self.time_current - self.time_previous
        delta_t *= 1e-9
        for i in range(2):            
            if abs(self.vel_current[i] - self.vel_previous[i]) > 0.05: 
                self.distance[i] += self.integration(self.vel_previous[i],self.vel_current[i],delta_t)
            
    def collect_previous_state(self):
        for i in range(3):
            self.acc_previous[i] = self.acc_current[i]
            self.vel_previous[i] = self.vel_current[i]
        # self.time_previous = self.time_current
        self.time_previous = rospy.get_rostime().nsecs
        # print(self.time_previous)

    def imu_callback(self, msg):
        self.time_current = rospy.get_rostime().nsecs
        # print(self.time_current)
        self.acc_current[0] = msg.linear_acceleration.y
        self.acc_current[1] = msg.linear_acceleration.x
        self.acc_current[2] = msg.linear_acceleration.z

        if self.is_init:
            self.acc_to_vel()      
            self.vel_to_distance()  
            for i in range(3):
                self.calculate(i)
            self.control_force2thruster()
            # self.publish_data()

        self.collect_previous_state()
        self.is_init = True

    def pressure_callback(self, msg):
        self.distance[2] = msg.pose.pose.position.z

    def calculate(self, axis):
        """
            Discrete model
                            e-2 e-1 e0
            self.error =    0   1   2
        """
        if axis < 2:
            current_error = self.vel_current[axis]
        else:
            current_error = self.distance[axis]

        self.error[axis][0] = self.error[axis][1]
        self.error[axis][1] = self.error[axis][2]
        self.error[axis][2] = current_error

        self.ctrl_signal[axis] = (
            self.prev_ctrl_signal[axis] +
            self.KP[axis] * (self.error[axis][2] - self.error[axis][1]) +
            self.KI[axis] * (self.error[axis][2]) +
            self.KD[axis] * (self.error[axis][2] - 2 *
                             self.error[axis][1] + self.error[axis][0])
        )

    def control_force2thruster(self):
        self.force.linear.x = self.ctrl_signal[0]
        self.force.linear.y = self.ctrl_signal[1]
        self.force.linear.z = self.ctrl_signal[2]
        self.force.angular.x = 0
        self.force.angular.y = 0
        self.force.angular.z = 0
        # print_debug("FORCE", "RED")
        # print_debug(str(force), "GREEN")
        self.pub_force.publish(self.force)

    

    def publish_data(self):
        print_debug("ACCELERATION", "RED")
        print_float(self.acc_current)
        print_debug("VELOCITY", "RED")
        print_float(self.vel_current)
        print_debug("DISTANCE", "RED")
        print_float(self.distance)
        print_debug("ERROR", "RED")
        print_float([self.error[0][2], self.error[1][2], self.error[2][2]])
        # print_debug(str(self.error[0]), "GREEN")
        # print_debug(str(self.error[1]), "GREEN")
        # print_debug(str(self.error[2]), "GREEN")
        print_debug("FORCE", "RED")
        print_float([self.force.linear.x,self.force.linear.y,self.force.linear.z])
        print_debug("X          Y          z", "RED")
        print_float(self.KP)
        print_float(self.KI)
        print_float(self.KD)
        self.pub_x.publish(Float32(self.vel_current[0]))
        self.pub_y.publish(Float32(self.vel_current[1]))
        self.pub_z.publish(Float32(self.vel_current[2]))

        self.pub_error_x.publish(Float32(self.error[0][0]))
        self.pub_error_y.publish(Float32(self.error[1][0]))
        self.pub_error_z.publish(Float32(self.error[2][0]))

    def run(self):
        cv.imshow('Tunning', self.tunning)
        while not rospy.is_shutdown():
            for i in range(3):
                self.KP[i] = cv.getTrackbarPos(
                    'KP_' + str(i), 'Tunning') / 100.
                self.KI[i] = cv.getTrackbarPos(
                    'KI_' + str(i), 'Tunning') / 100.
                self.KD[i] = cv.getTrackbarPos(
                    'KD_' + str(i), 'Tunning') / 100.
            self.publish_data()

            rospy.sleep(1/50.)
            cv.waitKey(1)

if __name__ == '__main__':
    ctrl = ControllerImu()
    ctrl.run()
