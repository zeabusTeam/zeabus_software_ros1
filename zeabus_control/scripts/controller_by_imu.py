#!/usr/bin/python2

from sensor_msgs.msg import Imu
from std_msgs.msg import Float32
from geometry_msgs.msg import Twist
import rospy
import cv2 as cv
from colorama import *
import numpy as np

DEBUG = True


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
        self.velocity = [0] * 3
        self.KP = [0] * 3
        self.KI = [0] * 3
        self.KD = [0] * 3
        self.error = [[0, 0, 0]] * 3
        self.ctrl_signal = [0] * 3
        self.prev_ctrl_signal = [0] * 3
        self.tunning = np.zeros((400, 800), np.uint8)
        self.acc = None

        rospy.Subscriber("/gx4_45_imu/data", Imu, self.imu_callback)

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

    def imu_callback(self, msg):
        self.acc = msg.linear_acceleration
        i = 0
        for a in [self.acc.x, self.acc.y, self.acc.z]:
            if abs(a) > 1.01:
                if a < 0:
                    self.velocity[i] -= a
                elif a>0:
                    self.velocity[i] -= a
            i += 1

    def calulate(self, axis):
        """
            Discrete model
                            e-2 e-1 e0
            self.error =    0   1   2
        """
        current_error = self.velocity[axis]
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
        force = Twist()
        force.linear.x = self.ctrl_signal[0]
        force.linear.y = self.ctrl_signal[1]
        force.linear.z = self.ctrl_signal[2]
        force.angular.x = 0
        force.angular.y = 0
        force.angular.z = 0
        print_debug("FORCE", "RED")
        print_debug(str(force), "GREEN")
        self.pub_force.publish(force)

    def publish_data(self):
        print_debug("ACCELERATION", "RED")
        print_debug(str(self.acc), "GREEN")
        print_debug("VELOCITY", "RED")
        print_debug(str(self.velocity), "GREEN")
        print_debug("ERROR", "RED")
        print_debug(str(self.error[0]), "GREEN")
        print_debug(str(self.error[1]), "GREEN")
        print_debug(str(self.error[2]), "GREEN")
        print_debug("PID", "RED")
        print_debug(str(self.KP), "GREEN")
        print_debug(str(self.KI), "GREEN")
        print_debug(str(self.KD), "GREEN")
        self.pub_x.publish(Float32(self.velocity[0]))
        self.pub_y.publish(Float32(self.velocity[1]))
        self.pub_z.publish(Float32(self.velocity[2]))

        self.pub_error_x.publish(Float32(self.error[0][0]))
        self.pub_error_y.publish(Float32(self.error[1][0]))
        self.pub_error_z.publish(Float32(self.error[2][0]))

    def run(self):
        while not rospy.is_shutdown():
            for i in range(3):
                self.KP[i] = cv.getTrackbarPos(
                    'KP_' + str(i), 'Tunning') / 100.
                self.KI[i] = cv.getTrackbarPos(
                    'KI_' + str(i), 'Tunning') / 100.
                self.KD[i] = cv.getTrackbarPos(
                    'KD_' + str(i), 'Tunning') / 100.

            for i in range(3):
                self.calulate(i)

            self.control_force2thruster()
            self.publish_data()
            cv.imshow('Tunning', self.tunning)
            cv.waitKey(1)
            rospy.sleep(0.1)

if __name__ == '__main__':
    ctrl = ControllerImu()
    ctrl.run()
