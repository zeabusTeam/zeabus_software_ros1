#!/usr/bin/python2
#################################################################################################
####
####	FILE		: mission_gate.py
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

from vision_collector		import VisionCollector

from standart_mission		import StandardMission

class MissionGate( StandardMission ):

	def __init__( self , name ):
		self.name = name

		StandardMission.__init__( self , self.name , "/mission/gate" , self.callback )

		self.vision = VisionCollector("gate")

	def callback( self ): # Divide to 2 part move to center and direction

		result = False

		self.echo( self.name , "START MISSION GATE")

		count = 0

		count += self.step_01()

		if( count != 0 ):
			result = self.step_02( count )

		if( result ):
			result = self.step_03() # check gate	
		else:
			self.echo( self.name , "Wrong count get into function step_2")

		return result

	def step_01( self ):
		result = 0
		count_unfound = 0
		while( not rospy.is_shutdown() ):
			self.sleep( 0.1 )
			self.vision.analysis_all( "gate" , "sevinar" , 5 )
			self.echo( self.vision.echo_data() )

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
				if( self.vision.area() > 0.7 ):
					self.collect_state()
					self.echo( self.name , "Area are over let play direct")
					self.velocity_xy( 0.2 , 0 )
					self.sleep()
					self.relative_z( -0.3 )
					result = 1
					break

			elif( self.vision.result['pos'] == -1 ):
				self.echo( self.name , "Found Pos : -1 and Move Left" )
				self.velocity( { 'y' : 0.15 } )
				if( self.vision.area() > 0.6 ):
					self.echo( self.name , "Area are over let play")
					self.relative_z( -0.3 )
					self.sleep()
					self.relative_xy( 5 , -2 )
					result = 2
					break
				
			elif( self.vision.result['pos'] == 1 ):
				self.echo( self.name , "Found Pos : 1 and Move Right" )
				self.velocity( { 'y' : -0.15 } )
				if( self.vision.area() > 0.6 ):
					self.echo( self.name , "Area are over let play")
					self.relative_z( -0.3 )
					self.sleep()
					self.relative_xy( 5 , 2 )
					result = 2
					break

		return result
						
	def step_02( self , num_type ):
		self.echo( self.name , "Welcome to step 2 check movement")
		if( num_type ==1 ): # have fix velocity
			while( not rospy.is_shutdown() ):
				self.sleep()
				distance = self.distance()
				self.echo( self.name , "Now distance " + str( distance ) )
				if( distance > 5 ):
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
			while( not rospy.is_shutdown() ):
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

	def step_03( self ): # this step we ensure to have pass gate only
		self.echo( self.name , "Weclome to step 3 check gate")
		self.relative_yaw( math.pi )
		count_ok = 0 
		while( not rospy.is_shutdown() ):
			self.sleep()
			if( self.check_position( "yaw" , 0.1 ) ):
				count_ok += 1
				if( count_ok == 5 ):
					break
			else:
				count_ok = 0
			self.echo( self.name , "Count ok yaw is " + str( count_ok ) )
		count_unfound = 0
		while( not rospy.is_shutdown() ):
			self.sleep()
			self.vision.analysis_all("gate" , "sevinar" , 5 )
			self.echo( self.vision.echo_data() ):
			if( not self.vision.echo_data() ):
				count_unfound += 1
				continue
			count_unfound = 0
			if( self.vision.result['pos'] == 0 ):
				if( self.vision.area() > 0.7 ):
					self.echo( self.name , "Pos is 0 & overlimit finish this job")
					break
				else:
					self.echo( self.name , "Pos is 0 & Move forward")
					self.velocity( { 'x' : 0.2 } )
			elif( self.vision.result['pos'] == 1 ):
				if( self.vision.area() > 0.4 ):
					self.echo( self.name , "Pos is 1 & overlimit finish this job")
					break
				else:
					self.echo( self.name , "Pos is 1 & Move Right")
					self.velocity( { 'y' : -0.15} )
			elif( self.vision.result['pos'] == -1 ):
				if( self.visio.area() > 0.4 ):
					self.echo( self.name , "Pos is -1 & overlimit finish this job")
					break
				else:
					self.echo( self.name , "Pos is -1 & Moe Left")
					self.velocity( {'y' : 0.15 } )
		return True
			 
			
					
if __name__ == "__main__" :
	rospy.init_node("mission_gate")
	MG = MissionGate( "mission_gate")
	rospy.spin()	
