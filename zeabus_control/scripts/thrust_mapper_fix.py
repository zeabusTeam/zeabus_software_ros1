#!/usr/bin/env python
"""
    File name: thrust_mapper_fix.py
    Author: robin
    Date created: 2018/11/14
    Python Version: 2.7
"""

import math
import rospy
import numpy as np
import LookUpPWM_2018 as lup
from hg_ros_pololu.msg import Pwm
from geometry_msgs.msg import Twist


class ThrustMapper:
    def __init__(self):
        rospy.init_node('Thrust_mapper')
        self.pwm_publisher = rospy.Publisher('/pwm', Pwm, queue_size=1)
        rospy.Subscriber("/cmd_vel", Twist, self.torque_callback)

        cos_45 = math.cos(math.radians(45))
        sin_45 = math.sin(math.radians(45))

        self.direction_linear = np.array([
            [0, 0, 1],  			# thruster 1
            [0, 0, 1],  			# thruster 2
            [0, 0, 1],  			# thruster 3
            [0, 0, 1],  			# thruster 4
            [cos_45, -sin_45, 0],  	# thruster 5
            [cos_45, sin_45, 0],  	# thruster 6
            [-cos_45, -sin_45, 0],  # thruster 7
            [-cos_45, sin_45, 0]  	# thruster 8
        ])

        self.distance = np.array([
            [0.332,  0.2202, -0.023],  	# thruster 1
            [0.332, -0.2202, -0.023],  	# thruster 2
            [-0.332,  0.2202, -0.023],  # thruster 3
            [-0.332, -0.2202, -0.023],  # thruster 4
            [0.3536,  0.3536, -0.023],  # thruster 5
            [0.3536, -0.3536, -0.023],  # thruster 6
            [-0.3536,  0.3536, -0.023], # thruster 7
            [-0.3536, -0.3536, -0.023], # thruster 8
        ])

        self.min_force = np.array([	[-0.048, 0.045],	
									[-0.048, 0.045],	
									[-0.048, 0.045],	
									[-0.048, 0.045],	
									[-0.01	, 0.01],	
									[-0.01	, 0.01],	
									[-0.01	, 0.01],	
									[-0.01	, 0.01],
        ])

        self.direction_angular = np.array([
            np.multiply(self.distance[0], self.direction_linear[0]),
            np.multiply(self.distance[1], self.direction_linear[1]),
            np.multiply(self.distance[2], self.direction_linear[2]),
            np.multiply(self.distance[3], self.direction_linear[3]),
            np.multiply(self.distance[4], self.direction_linear[4]),
            np.multiply(self.distance[5], self.direction_linear[5]),
            np.multiply(self.distance[6], self.direction_linear[6]),
            np.multiply(self.distance[7], self.direction_linear[7]),
        ])
        

        self.direction = np.concatenate((
            self.direction_linear,
            self.direction_angular
        ),axis=1)

        self.direction_inverse = np.linalg.pinv(self.direction)

    def torque_callback(self, message):
        pwm_command = Pwm()
        pwm_command.pwm = [1500] * 8

        force = np.array([message.linear.x, message.linear.y, message.linear.z,
                      message.angular.x, message.angular.y, message.angular.z])

        torque = np.matmul(self.direction_inverse.T,force.T)
       
        # torque = torque.T

        for run in range(0, 8):
            if(torque[run] < 0):
                if(torque[run] > self.min_force[run][0] / 2):
                    torque[run] = 0
                elif(torque[run] > self.min_force[run][0]):
                    torque[run] = self.min_force[run][0]
            else:
                if(torque[run] < self.min_force[run][1] / 2):
                    torque[run] = 0
                elif(torque[run] < self.min_force[run][1]):
                    torque[run] = self.min_force[run][1]

        cmd = []
        for i in range(0, 4):
            cmd.append(lup.lookup_pwm_02(torque[i]))
            if(torque[i] == 0):
                cmd[i] = 1500

        for i in range(4, 8):
            cmd.append(lup.lookup_pwm_01(torque[i]))
            if(torque[i] == 0):
                cmd[i] = 1500

        for i in range(0, 8):
			pwm_command.pwm[i] = cmd[i]  # thrust i
        

        pwm = pwm_command.pwm
        print '=========== PWM ==========='
        print(str(pwm[0]) + "\t" + str(pwm[1]) + "\t"
              + str(pwm[2]) + "\t" + str(pwm[3]) + "\n"
              + str(pwm[4]) + "\t" + str(pwm[5]) + "\t"
              + str(pwm[6]) + "\t" + str(pwm[7]))

        self.pwm_publisher.publish(pwm_command)


if __name__ == '__main__':
    ThrustMapper()
    rospy.spin()
