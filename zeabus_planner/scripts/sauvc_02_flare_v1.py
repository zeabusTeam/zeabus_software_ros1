#!/usr/bin/python2

import rospy

from manage_log import log
from control_auv import control_auv 
from zeabus_vision.srv import vision_srv_flare
from zeabus_vision.msg import vision_flare

class play_flare:

	def __init__( self ):
		self.already_setup = False

		print("Waiting flare service")
		rospy.wait_for_service('vision_flare')
		print("I found flare service")
		

	def set_up( self ):
		self.already_setup = True
		self.log_command = log( "zeabus_planner" , "log" , "02_flare_command")
		self.log_vision = log( "zeabus_planner" , "log" , "02_flare_vision")
		self.client_flare = rospy.ServiceProxy('vision_flare' , vision_srv_flare )
		self.data_have = False
		self.data_center_x = 0
		self.data_center_y = 0
		self.data_area = 0

	def play( self ):
		if( not self.already_setup ):
			self.set_up()
		
		self.log_command.write("I will start now", True , 0)

	def request_data( self , amont ):
		round_found = 0
		round_unfound = 0

if __name__=='__main__':

	rospy.init_node(" Mission Flare ")		

	flare = plat_flare()
	flare.set_up()
	flare_start()
