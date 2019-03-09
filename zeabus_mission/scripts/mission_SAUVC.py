#!/usr/bin/python2
#################################################################################################
####
####	FILE		: mission_gate.py
####	Maintain	: Supasan Komonlit
####	Create on	: 2019 , MAR 10
####	Purpose		: For competition in SAUVC2019 about mission gate and vision don't have
####				  affect by reflect	
#################################################################################################
from __future__			import print_function
#===============>

import rospy
import math
import time

from vision_collector		import VisionCollector
from standard_mission		import StandardMission

class MissionAll( StandardMission ):
	
	def __init__( self , name ):
		self.name = name 

		StandardMission.__init__( self , self.name , "/mission/all" , self.callback )

		self.vision_gate = VisionCollector( "gate" )
		self.vision_flare = VisionCollector( "flare" )
		self.vision_drum = VisionCollector( "drum" )

		self.state = False

		self.echo( self.name , "ALL MISSION SETUP FINISHED")
		
		self.time_start = time.time()

	def callback( self , request ):
		
		result = False

		self.state = request.data 
		if( not self.ok_state() ):
			return False

		self.echo( self.name , "START MISSION GATE" )

		self.reset_target( "yaw" )
		self.reset_target( "xy" )
		self.reset_velocity( "xy" )

		self.target_state()
		self.start_yaw = self.temp_state[5]

		self.fix_z( -0.8 )
		self.wait_state( "z" , 0.15 , 5 )

		self.find_gate( 4 , 30 , 2 , 15 , 1 ) # d_f , t_f , d_s , t_s

	def over_move( self , distance , time , setup = False ):
		if( setup ):
			self.time_start = time.time()
			self.collect_state()
			return True
		else:
			if( self.distance() > distance ):
				return True
#			elif( ( time.time() - self.time_start ) > time ):
#				return True
			else:
				return False

	def find_gate( self, distance_forward, time_forward, distance_survey, time_survey, direct):
		self.velocity_xy( 0.2 )
		self.over_move( distance_forward , time_forward , True )
		finish_gate = False
		count_found = 0 
		while( self.ok_state() and not( finish_gate )):
			self.sleep( 0.06 )
			self.vision_gate( "gate" , "sevinar" , 5 )
			if( self.vision_gate.have_object() ):
				if( self.vision_gate.num_object == 2 ):
					self.reset_velocity( "xy" )
					self.play_gate()
					finish_gate = True	
				else:
					count_found += 1 
					if( count_found == 5 ):
						self.reset_velocity( "xy" )
						self.play_gate()
						finish_gate = True
			else:
				count_found = 0
			if( self.over_move( distance_forward , time_forward ) ):
				break
		self.reset_velocity( "xy" )
		self.wait_state( "xy" , 0.15 , 5 )
		count_found = 0
		self.over_move( distance_survey , time_survey , True )
		while( self.ok_state() and not( finish_gate ) ):
			self.sleep( 0.06 )
			self.vision_gate( "gate" , "sevinar" , 5 )
			if( self.vision_gate.have_object() ):
				if( self.vision_gate.num_object == 2 ):
					self.reset_velocity( "xy" )
					self.play_gate()
					finish_gate = True
				else:
					count_found += 1 
					if( count_found == 5 ):
						self.reset_velocity( "xy" )
						self.play_gate()
						finish_gate = True
			else:
				count_found = 0
			if( self.over_move( distance_survey , distance_forward ) ):
				break 
		

if __name__=="__main__":
	rospy.init_node( "mission_all" )
	MA = MissionAll( "mission_all" )
	rospy.spin()
