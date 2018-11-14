#!/usr/bin/python2

import rospy
import math
import time

from auv_controller		import AUVController
from vision_collector	import VisionCollector
from zeabus_planner.srv	import mission_result

from std_msgs.msg			import Bool , Int8

class MissionGate:
	
	def __init__( self ):
		
		self.auv	= AUVController( "mission_gate" , True )

		print( "<=== MISSION GATE ===> Waitting Service of Gate" )
		rospy.wait_for_service( 'vision_gate' )
		print( "<=== MISSION GATE ===> Have Service of Gate OK ")

		self.vision	= VisionCollector( "gate" )

		self.rate	= rospy.Rate( 30 )
		self.mission_planner = rospy.Service('mission/planner' , mission_result , self.main_play)

	def main_play( self , request ):
		self.sucess_mission = False
		# Mission Gate Will run start when have gate on image only situatuin
		self.vision.analysis_all( 5 )
