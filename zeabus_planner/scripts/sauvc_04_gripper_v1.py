#!/usr/bin/python2

import rospy
import math

try:
	from zeabus_extension.manage_log import log
	from zeabus_extension.control_auv import control_auv 
except:
	print("Pleas install setup.bash in zeabus_extension package")
	exit()

from zeabus_vision.srv import vision_srv_drum
from zeabus_vision.msg import vision_drum
from std_msgs.msg import String

class play_gripper:

	def __init__( self , rate ):
		self.auv = control_auv("play_gripper")
		self.rate = rospy.Rate( rate )
		self.client_drum = rospy.ServiceProxy('vision_pick' , vision_srv_drum )
		self.log_command = log( "zeabus_planner" , "log" , "04_gripper_command" )
		self.log_vision = log( "zeabus_planner" , "log" , "04_gripper_vision" )

	def reset_vision_value( self ):
		self.result_vision = { "n_obj" : 0 , "cx" : 0 , "cy" : 0 , "area" : 0 }
		self.collect_vision = { "n_obj" : 0 , "cx" : 0 , "cy" : 0 , "area" : 0 }

	def play( self ):
		print("I will play for see ball befor disappear")
		while( not rospy.is_shutdown() ):
			self.rate.sleep()
			self.analysis_all( 5 )
			if( self.result_vision['n_obj'] == 1 ):
				if( self.result_vision['cx'] <= -0.08 ):
					print("\tMove right")
					self.auv.velocity( y = -0.1 )
				elif( self.result_vision['cx'] > 0.3):
					print("\tMove left")
					self.auv.velocity( y = 0.1 )
				else:
					print("\tI agrre to move forward")
					self.auv.velocity( x = 0.1 )
					while( not rospy.is_shutdown() and self.result_vision['cy'] < -0.5 ):
						self.rate.sleep()
						self.auv.velocity( x = 0.1 )
						print("\t\t Now cy is " + str( self.result_vision['cy']))
						self.rate.sleep()
						print("wait yaw")
						while( not rospy.is_shutdown() and not self.auv.ok_position( "yaw",0.1)):
							self.rate.sleep()
					print("Ok let ball disapear")	

	def analysis_all( self , amont ):
		None

	def individual_analysis( self ):
		None
