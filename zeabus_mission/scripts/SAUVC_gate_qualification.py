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

		count += self.step_01()

		if( count != 0 ):
			result = self.step_02( count )

		self.fix_z( -0.5 )
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
					self.reset_target( "xy" )
				while( self.ok_state() ): # Loop for move direct to pier
					self.sleep( 0.1 )
					if( abs( self.vision.center_x() ) < 0.2 ):
						self.velocity( { 'x' : 0.1 } )
					else:
						self.velocity( { 'y' : math.copysign( 0.1 , self.vision.center_x()*-1 )})
					if( self.vision.distance_x() > 0.6 ):
						next_part = True
					
					
		
					
if __name__ == "__main__" :
	rospy.init_node("mission_gate")
	MG = MissionGate( "mission_gate")
	rospy.spin()	
