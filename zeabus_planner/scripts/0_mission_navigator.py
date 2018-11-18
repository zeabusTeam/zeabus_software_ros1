#!/usr/bin/python2
#################################################################################################
####
####	FILE		: 0_mission_navigator_1.py
####	Author		: Supasan Komonlit
####	Create on	: 2018 , Nov 16
####	Purpose		: For make mission gate	
####
#################################################################################################

import rospy
import math
import time

from auv_controller		import AUVController
from vision_collector	import VisionCollector
from zeabus_planner.srv	import mission_result

from std_msgs.msg		import Bool , String , Int8

class MissionNavigator:
	
	def __init__( self ):
		
		self.auv	= AUVController( "mission_navigator" , True )
		self.vision_gate = VisionCollector('gate')
		self.vision_flare = VisionCollector('flare')

		self.mission_gate	= rospy.ServiceProxy('/mission/gate'	, mission_result )
		self.mission_flare	= rospy.ServiceProxy('/mission/flare'	, mission_result )

	def play_all( self ):
		self.start_yaw = self.auv.receive_target( 'yaw' )
		

if __name__=="__main__":
	naviagator = MissionNavigator()
	naviagator.play_all()
