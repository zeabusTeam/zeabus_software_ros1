#!/usr/bin/python2
#################################################################################################
####
####	FILE		: mission_gate.py
####	Maintain	: Supasan Komonlit
####	Create on	: 2019 , FEB 21
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

class MissionGate( StandardMission ):

	def __init__( self , name ):
		self.name = name

		StandardMission.__init__( self , self.name , "/mission/gate" , self.callback )

		self.vision = VisionCollector("gate")

		self.echo( self.name ,  "GATE SETUP FINISHED")

		self.state = False

	def callback( self , request): # Divide to 2 part move to center and direction

		result = False

		self.state = request.data
		if( not self.ok_state() ):
			return False

		self.echo( self.name , "START MISSION GATE")
	
		count = 0

		value_velocity = 0.1

		count = self.step_01( value_velocity )

		result = self.step_02( value_velocity )

		self.fix_z( -0.5 )

		self.echo( self.name , "I Finish to de this mission ")
		return result

	def step_01( self , value_velocity ): # Move left or right untill find first pier
		current_fix_velocity = False
		count_found = 0
		next_part = False
		while( self.ok_state() and ( not next_part ) ):
			self.sleep( 0.1 )
			self.vision.analysis_all( "gate" , "sevinar" , 5 )
			self.echo_vision( self.vision.echo_specific() )
			if( self.vision.have_object() ):
				if( current_fix_velocity ):
					self.reset_velocity( "xy" )
					current_fix_velocity = False
					self.reset_target( "xy"
				if( abs( self.vision.center_x() ) < 0.25 ):
					self.velocity( { 'x' : 0.1 } )
				else:
					self.velocity( { 'y' : math.copysign( 0.1 , self.vision.center_x  ) * -1 } ) 
				if( self.vision.distance_x() > 0.5 ):
					next_part = True 
			else:
				if( not current_fix_velocity ):
					self.velocity_xy( 0 , value_velocity )
					current_fix_velocity = True
				self.echo( self.name , "Now I move velocity in y axis is " + str(value_velocity))
		return value_velocity
					
	def step_02( self , value_velocity ):
		# first move out to unfound pier
		self.velocity_xy( 0 , math.copysign( 0.05 , value_velocity) )
		result = False
		while( self.ok_state() ):
			self.sleep( 0.1 )
			self.vision.analysis_all( "gate" , "sevinar" , 5 )
			self.echo_vision( self.vision.echo_specific() )
			if( self.vision.have_object() ):
				self.echo( self.name , "I still found gate")
			else:
				self.echo( self.name , "Now I don't found pier we will end this tast")	
				break
		self.fix_z( -1 )
		start_time = time.time()
		diff_time = 0 
		while( self.ok_state() and diff_time < 6 ):
			self.sleep( 0.1 ):
			diff_time = time.time() - start_time
			self.echo( self.name , "Countdown move diff_time is " + str( diff_time ) )
		self.reset_velocity( "xy" )
		self.reset_target( "xy" )
		start_time = time.time()
		diff_time = 0
		self.velocity_xy( 0.2 , 0 )
		while( self.ok_state() and diff_time < 5 ):
			self.sleep( 0.1 ):
			diff_time = time.time() - start_time
			self.echo( self.name , "Countdown last move diff time is " + str( diff_time ) )
		if( diff_time > 5 ): result = True
		self.reset_velocity( "xy" )
		self.fix_z( -0.5 )
		return result
		
					
if __name__ == "__main__" :
	rospy.init_node("mission_gate")
	MG = MissionGate( "mission_gate")
	rospy.spin()	
