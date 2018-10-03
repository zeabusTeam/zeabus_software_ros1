#!/usr/bin/python2

import rospy
import math

try:
	from zeabus_extension.manage_log import log
	from zeabus_extension.control_auv import control_auv 
except:
	print("Pleas install setup.bash in zeabus_extension package")
	exit()

from zeabus_vision.srv import vision_srv_gate
from zeabus_vision.msg import vision_gate

class play_gate:

	def __init__( self , rate , first_forward , first_survey , forward , survey ):
		self.data_vision = { "color" : None  , "cx_01" : 0 , "cx_02" : 0 ,
							 "cy_01" : 0 , "cy_02" : 0 , "area" : 0 }
		self.navigation = { "first_forward" : first_forward , "first_survey" : first_survey,
							"forward" : forwaard , "survey" : survey}

		print("Waiting gate service")
		rospy.wait_for_service('vision_gate')
		print("I found gate service")
		
		self.rate = rospy.Rate( rate)

		

if __name__=='__main__':
	rospy.init_node("Mission Gate")	
