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

	def step_01( self ):
		result = 0
		count_unfound = 0
		while( self.ok_state() ):
			self.sleep( 0.1 )
			self.vision.analysis_all( "gate" , "sevinar" , 5 )
			self.echo_vision( self.vision.echo_data() )

			if( count_unfound == 5 ):
				break

			if( not self.vision.have_object() ):
				count_unfound += 1
				continue
			count_unfound = 0
			if( self.vision.result['pos'] == 0 ):
				temp_center_x = self.vision.center_x() 
				if( abs( temp_center_x ) < 0.1 ):
					self.echo( self.name , "Found Pos : 0 and Move Forward" )
					self.velocity( {'x' : 0.2 } )
				elif( temp_center_x < 0 ):
					self.echo( self.name , "Found Pos : 0 and Move Left")
					self.velocity( {'y' : 0.1 } )
				else:
					self.echo( self.name , "Found Pos : 0 and Move Right")
					self.velocity( {'y' : -0.1} )
				if( self.vision.area() > 0.6 ):
					self.collect_state()
					self.echo( self.name , "Area are over let play direct")
					self.velocity_xy( 0.2 , 0 )
					self.sleep()
					self.relative_z( -0.3 )
					result = 1
					break

			elif( self.vision.result['pos'] == -1 ):
				self.echo( self.name , "Found Pos : -1 and Move Left" )
				self.velocity( { 'y' : 0.2 } )
			elif( self.vision.result['pos'] == 1 ):
				self.echo( self.name , "Found Pos : 1 and Move Right" )
				self.velocity( { 'y' : -0.2 } )

		return result
						
	def step_02( self , num_type ):
		self.echo( self.name , "Welcome to step 2 check movement")
		if( num_type ==1 ): # have fix velocity
			while( self.ok_state() ):
				self.sleep()
				distance = self.distance()
				self.echo( self.name , "Now distance " + str( distance ) )
				if( distance > 2.5 ):
					self.reset_velocity( "xy" )
					self.relative_z( 0.3 )
					break
			count_ok = 0
			while( not rospy.is_shutdown() ):
				self.sleep()
				if( self.check_position( "z" , 0.15 ) ):
					count_ok += 1
					if( count_ok == 5 ):
						break
				else:
					count_ok = 0
				self.echo( self.name , "Count ok depth is " + str( count_ok) ) 
			return True
		elif( num_type == 2 ): # non fix velocity
			count_ok = 0 
			while( self.ok_state() ):
				self.sleep()
				if( self.check_position( "xy" , 0.1 ) ):
					count_ok += 1
					if( count_ok == 5 ):
						break
				else:
					count_ok = 0
				self.echo( self.name , "Count ok xy is " + str( count_ok ) )
			count_ok = 0
			while( not rospy.is_shutdown() ):
				self.sleep()
				if( self.check_position( "z" , 0.15 ) ):
					count_ok += 1
					if( count_ok == 5 ):
						break
				else:
					count_ok = 0
				self.echo( self.name , "Count ok depth is " + str( count_ok ) )	 
			return True
		else:
			return False
					
if __name__ == "__main__" :
	rospy.init_node("mission_gate")
	MG = MissionGate( "mission_gate")
	rospy.spin()	
