#!/usr/bin/python2
#################################################################################################
####
####	FILE		: mission_all.py
####	Maintain	: Supasan Komonlit
####	Create on	: 2019 , FEB 21
####	Purpose		: For competition in SAUVC2019
####
#################################################################################################
from __future__			import print_function
#===============>

import rospy
import math
import time

from standard_mission		import StandardMission 

from zeabus_library.srv		import TwoBool

class TestStandardMission( StandardMission ):

	def __init__( self , name ):
		
		self.name = name

		StandardMission.__init__( self , self.name , "/mission/all" , self.callback )

	def callback( self , message ):

		self.state = message.data

		if( not self.ok_state() ):
			print("Have service call aborted mission")
			return False

		start_time = time.time()

		while( self.ok_state() ):
			self.sleep()
			if( time.time() - start_time > 2 ):
				print("In loop doing")
				start_time = time.time()

		print( "End callback")

		return True	

if __name__=="__main__":
	rospy.init_node( "testing_mission")
	TSM = TestStandardMission("Testing_mission")
	rospy.spin()
