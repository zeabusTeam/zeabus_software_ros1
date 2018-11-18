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
from zeabus_planner.srv	import MissionResult

from std_msgs.msg		import Bool , String , Int8

class MissionNavigator:
	
	def __init__( self ):
		
		self.auv	= AUVController( "mission_navigator" , True )
		self.vision_gate = VisionCollector('gate')
		self.vision_flare = VisionCollector('flare')
		self.vision_drum = VisionCollector('drum')

		self.mission_gate	= rospy.ServiceProxy('/mission/gate'	, MissionResult )
		self.mission_flare	= rospy.ServiceProxy('/mission/flare'	, MissionResult )
		self.mission_gripper= rospy.ServiceProxy('/mission/gripper'	, MissionResult )
	
		self.rate = rospy.Rate(30)

	def play_all( self ):
		self.start_yaw = self.auv.receive_target( 'yaw' )
		self.auv.set_mode( 0 )

		self.auv.absolute_z( -0.5 )
		count_ok = 0 
		print("Waiting OK Z")
		while( not rospy.is_shutdown() ):
			if( self.auv.check_state("z" , 0.05 ) ):
				count_ok += 1
			else:
				count_ok = 0
			if( count_ok >= 5):
				break
		print("Finish Wait go forward")
			
		self.auv.survey( "x" , 4 , 0.4 )
		print("Search drum")
		while( not rospy.is_shutdown() ):
			self.rate.sleep()
			self.vision_drum.analysis_all( "drum" , "drop" , 5 )
			if( self.vision_drum.have_object() ):
				self.auv.set_mode( 1 )
				break
		print("Found drum")
		while( not rospy.is_shutdown() ):
			self.rate.sleep()
			self.vision_drum.analysis_all( "drum" , "pick" , 5 )
			if( self.vision_drum.have_object() ):
				print("Found Ball")
				break
			else:
				self.vision_drum.analysis_all("drum" , "drum" , 5)
				if ( not self.vision_drum.result['forward'] ):
					self.auv.velocity( { 'x' : 0.3})
				elif ( self.vision_drum.result['right'] ):
					self.auv.velocity( { 'y' : 0.3})
		print("found Ball")
		self.mission_gripper( Bool( True ) )	
		

if __name__=="__main__":
	naviagator = MissionNavigator()
	naviagator.play_all()
