#!/usr/bin/python2
#################################################################################################
####
####	FILE		: SAUVC_qualification_vision.py
####	Maintain	: Supasan Komonlit
####	Create on	: 2019 , FEB 28
####	Purpose		: For connection with control part
####
#################################################################################################
from __future__ import print_function
#===============>

################################# EXPAND MISSION SOLUTION #######################################
####	This file is mission for qualification round for SAUVC2019 
####	In case have only dvl don't use vision
#################################################################################################

import	rospy
import	math
import	time

from vision_collector		import VisionCollector

from standard_mission		import StandardMission

class MissionQualification( StandardMission ):
	
	def __init__( self , name ):
		self.name = name

		StandardMission.__init__( self , self.name , "/mission/qualification" , self.callback )

		self.vision = VisionCollector( "qualification" )
	
		print("MISSION QUALIFICATION FINISHED SETUP")

		self.state = False

	def callback( self , message ):
		
		result = False

		self.state = message.data

		if( not self.ok_state() ):
			self.echo( self.name , "Aborted mission qualification")
			return False

		# This function will call by switch we must to reset data target
		self.reset_velocity( "xy" )
		self.reset_target( "xy" )
		self.reset_target( "yaw" )
		self.fix_z( -0.3 )

		self.echo( self.name , "START MISSION QUALIFICATION")


		self.collect_state()
		self.velocity_xy( 0.4 , 0 )
		while( self.ok_state() ):
			self.sleep(0.2)
			self.echo( self.name , "Now move direct" )
		self.reset_velocity( "xy" )
		self.reset_target("xy")
		self.fix_z( -0.2 )

		return True
			
if __name__ == "__main__":
		rospy.init_node("mission_qualification")
		MQ = MissionQualification( "mission_qualification" )
		rospy.spin()	
