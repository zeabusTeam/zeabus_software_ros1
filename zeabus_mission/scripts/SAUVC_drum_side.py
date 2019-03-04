#!/usr/bin/python2
#################################################################################################
####
####	FILE		: SAUVC_drum_side.py
####	Maintain	: Supasan Komonlit
####	Create on	: 2019 , FEB 28
####	Purpose		: For competition in SAUVC2019
####
#################################################################################################
from __future__			import print_function
#===============>

import rospy
import math
import time

from vision_collector	import VisionCollector

from standard_mission	import StandardMission

class MissionDrum( StandardMission ):

	def __init__( self , name ):
		self.name = name 

		StandardMission.__init__( self , self.name , "/mission/drop" , self.callback )

		self.vision = VisionCollector( "drum" )

		self.state = False

		self.echo( self.name , "DROP BALL SEtuP FINISHER" )

	def callback( self , request ):

		result = False
	
		self.state = request.data
		if( not self.ok_state() ):
			return False

		self.echo( self.name , "START MISSION DROP BALL")

		self.wait_state( "yaw" , 0.1 , 5 )

		self.step_01()
		
		return True

	def step_01( self ): 
		count_Time = False
		self.velocity_z( -0.05 )
		value_x = 0
		value_y = 0
		start_time = 0
		ever_drop = False
		while( self.ok_state() ):
			self.vision.analysis_all( "drum" , "drop" , 5 )
			self.echo_vision( self.vision.echo_special() )
			if( self.vision.result["cx_2"] > 0.4 ):
				value_x = -0.1
			elif( self.vision.result["cx_2"] < 0.1 ):
				value_x = 0.1
			else:
				value_x = 0
			if( not self.vision.result["forward"] ):
				value_y = -0.1
			elif( not self.vision.result["backward"] ):
				value_y = 0.1
			else:
				value_y = 0
			self.echo( self.name ,  "I command velocity x : y are " + str(value_x) + " : " +
					str(value_y) )
			if( not count_Time ):
				self.target_state()
				if( self.temp_state[2] < -1.3 ):
					count_Time = True
					start_time = time.time()
					self.free_xy( True )
			else:
				diff_time = time.time() - start_time
				self.echo( self.name , "Count time for drop " + str(diff_time) )
				if( diff_time > 5 ):
					break
				elif( diff_time > 2 and not ever_drop ):
					self.echo( self.name , "We have to command drop ball" )
					self.fire_golf()
				else:
					continue
			self.velocity( {'x' : value_x , 'y' : value_y } )

		self.reset_velocity( "z" ) 
		self.fix_z(-0.5)
		self.wait_state( "z" , 0.1 , 5 )
		self.free_xy( False )
		self.reset_target("xy")

if __name__=="__main__":
	rospy.init_node( "mission_drum" )
	MD = MissionDrum( "mission_drum" )
	rospy.spin()
