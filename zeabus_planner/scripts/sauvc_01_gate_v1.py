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

	def __init__( self , rate ):
		self.data_vision = { "color" : None  , "cx_01" : 0 , "cx_02" : 0 ,
							 "cy_01" : 0 , "cy_02" : 0 , "area" : 0 }

		print("Waiting gate service")
		rospy.wait_for_service('vision_gate')
		print("I found gate service")
		
		self.rate = rospy.Rate( rate)

		self.already_setup = False

	def setup( self , first_forward , first_survey , forward , survey ):
		
		self.already_setup = True

		self.navigation = { "first_forward" : first_forward , "first_survey" : first_survey,
							"forward" : forwaard , "survey" : survey}

		self.log_command = log( "zeabus_planner" , "log" , "02_gate_command")
		self.log_vision = log( "zeabus_planner" , "log" , "02_gate_vision")
		self.client_flare = rospy.ServiceProxy('vision_gate' , vision_srv_gate )

	

if __name__=='__main__':
	rospy.init_node("Mission Gate")	
