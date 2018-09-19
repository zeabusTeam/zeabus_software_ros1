import math , rospy, tf
import numpy as np
from geometry_msgs.msg import Quaternion, Twist

class expansion_quaternion:
	def inverse(self, quaternion):
		return np.array([ -quaternion[0], -quaternion[1], -quaternion[2] , quaternion[3]])

	def quaternion_to_msg(self, quaternion):
		message = Quaternion()
		message.x = quaternion[0]
		message.y = quaternion[1]
		message.z = quaternion[2]
		message.w = quaternion[3]
		return message

	def msg_to_quaternion(self, message):
		return np.array(([message.x, message.y, message.z, message.w]))

#reference is http://www.chrobotics.com/library/understanding-quaternions ; math.atan2(y/x) is arctan(y/x)
	def get_roll(self, quaternion):
		return math.atan2( 2*(quaternion[0]*quaternion[1] + quaternion[2]*quaternion[3]) , math.pow(quaternion[0],2) - math.pow(quaternion[1],2) - math.pow(quaternion[2],2) + math.pow(quaternion[3],3))
	
	def get_pitch(self, quaternion):
		return -math.asin( 2*(quaternion[1]*quaternion[3] - quaternion[0]*quaternion[2]))

	def get_yaw(self, quaternion):
		return math.atan2( 2*(quaternion[0]*quaternion[3] + quaternion[1]*quaternion[2]) , math.pow(quaternion[0],2) + math.pow(quaternion[1],2) - math.pow(quaternion[2],2) - math.pow(quaternion[3],3))

	def get_size(self, quaternion):
		size = 0
		for i in quaternion:
			size += math.pow(i,2)
		return math.sqrt(size)

	def get_normalize(self, quaternion):
		size = self.get_size(quaternion)
		for i in range (0,4):
			quaternion[i]/=size
		return quaternion

class expansion_twist:
	def msg_to_twist( self, message ):
		return np.array([message.linear.x, message.linear.y , message.linear.z, message.angular.x , message.angular.y , message.angular.z])

	def twist_to_msg( self, twist ):
		message = Twist()
		message.linear.x = twist[0]	
		message.linear.y = twist[1]	
		message.linear.z = twist[2]	
		message.angular.x = twist[3]	
		message.angular.y = twist[4]	
		message.angular.z = twist[5]
		return message	
