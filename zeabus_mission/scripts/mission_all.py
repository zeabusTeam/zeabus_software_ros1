#!/usr/bin/python2
#################################################################################################
####
####	FILE		: mission_all.py
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
from standard_mission		import StandardMission 

from zeabus_library.srv		import TwoBool

class MissionAll( StandardMission ):
	
	def __init__( self , name ):
		self.name = name

		StandardMission.__init__( self , self.name , "/mission/all" , self.callback )

		self.gate = VisionCollector("gate")
		self.drum = VisionCollector("drum")
		self.flare = VisionCollector("flare")
		self.mission_gate = rospy.ServiceProxy( "/mission/gate" , TwoBool )
		self.mission_flare = rospy.ServiceProxy( "/mission/flare" , TwoBool )
		self.mission_drum = rospy.ServiceProxy( "/mission/drum" , TwoBool )

		self.start_time = time.time()

		self.start_yaw = 0

	def callback( self ):
		
		# This function will call by switch we must reset data target
		self.reset_velocity( "xy" )
		self.reset_target( "xy")
		self.reset_target( "yaw" )
		self.fix_z( -0.5 )

		self.collect_state()
		self.start_yaw = self.save_state[5]
	
		self.echo( self.name , "START ALL MISSION at yaw is " + str( self.start_yaw ) )

		count_ok = 0
		while( not rospy.is_shutdown() ):
			if( self.check_position("z" , 0.1 ) ):
				count_ok += 1
				if( count_ok == 5 ): break
			else:
				count_ok = 0
			self.echo( self.name , "Depth ok is " + str( count_ok ) )

		# In survey mode this ensure you will make mission finish 
		self.survey_mode( 3, 6, 1, -4, self.gate, "gate", "sevinar", 5, self.mission_gate)

		self.fix_z( -3.8 )
		count_ok = 0
		while( not rospy.is_shutdown() ):
			if( self.check_position("z" , 0.1 ) ):
				count_ok += 1
				if( count_ok == 5 ): break
			else:
				count_ok = 0
			self.echo( self.name , "Depth ok is " + str( count_ok ) )

		self.fix_yaw( self.start_yaw - ( math.pi/2 ) )
		count_ok = 0
		while( not rospy.is_shutdown() ):
			if( self.check_position("yaw" , 0.1 ) ):
				count_ok += 1
				if( count_ok == 5 ): break
			else:
				count_ok = 0
			self.echo( self.name , "yaw ok is " + str( count_ok ) )
		
		self.survey_mode( 3, 6, 1, -4, self.flare, "flare", "far", 5, self.mission_flare() )
		
		return True
		
		
	# WARNING ! survey mode will use after rotation 
	# step is move forward move slide move forward and move inverse slide
	def survey_mode( self , first_forward , first_slide , value_forward , value_slide ,
			vision , first_argument , second_argument , third_argument , service_call ):
		type_movement = 1 # 1 is first_forward , 2 is first_slide , 3 forward , 4 second_slide
		current_fix_velocity = False
		count_have_object = 0
		while( not rospy.is_shutdown() ):
			self.sleep( 0.1 )
			vision.analysis_all( first_argument , second_argument , third_argument )
			self.echo( self.name , "Count found object is " + str( count_have_object ) )
			if( vision.have_object() ):
				count_have_object += 1
				if( count_have_object == 5 ):
					result_play_game = service_call( True )
					self.echo( self.name , "Send to Play Game")
					if( result_play_game ):
						self.echo( self.name ,  "Oh Finish")
						break
					self.echo( self.name , "Oh Noooooo Don't finish continue survey")
					count_have_object = 0					
				if( current_fix_velocity ):
					self.echo( sefl.name ,  "I reset target")
					self.reset_velocity( "xy" )
					current_fix_velocity = False
				continue
			else:
				count_have_object = 0

			# Manage about movement mode
			if( type_movement == 1 ):
				if( not current_fix_velocity ):
					self.velocity_xy( 0.2 , 0 )
					current_fix_velocity = True
					self.check_distance( first_forward , True )
				elif( self.check_distance( first_forward ) ):
					self.echo( self.name ,  "Change to type_movement 2")
					if( current_fix_velocity ):
						self.reset_velocity( "xy" )
						current_fix_velocity = False
					type_movement = 2

			elif( type_movement == 2 ):
				if( not current_fix_velocity ):
					self.velocity_xy( 0.0 , math.copysign( 0.15 , first_slide ) )
					current_fix_velocity = True
					self.check_distance( abs(first_slide) , True )
				elif( self.check_distance( abs(first_slide) ) ):
					self.echo( self.name ,  "Change to type_movement 3")
					if( current_fix_velocity ):
						self.reset_velocity( "xy" )
						current_fix_velocity = False
					type_movement = 3

			elif( type_movement == 3 ):
				if( not current_fix_velocity ):
					self.velocity_xy( 0.2 , 0 )
					current_fix_velocity = True
					self.check_distance( value_forward , True )
				elif( self.check_distance( value_forward ):
					self.echo( self.name ,  "Change to type_movement 4")
					if( current_fix_velocity ):
						self.reset_velocity( "xy" )
						current_fix_velocity = False
					type_movement = 4
		
			elif( type_movement == 4 ):	
				if( not current_fix_velocity ):
					self.velocity_xy( 0 , math.copysign( 0.15 , value_slide) )
					current_fix_velocity = True
					self.check_distance( abs(value_slide) , True )
				elif( self.check_distance( abs(value_slide) ):
					self.echo( self.name ,  "Change to type_movement 4")
					if( current_fix_velocity ):
						self.reset_velocity( "xy" )
						current_fix_velocity = False
					first_slide = maht.copysign( value_slide , first_slide )
					first_forward = math.copysign( value_slide , first_forward )
					type_movement = 1
		
		
		
	def check_distance( self , limit , setup = False ):
		if( setup ):
			self.collect_state()
		return self.check_result( self.distance() , limit )	
	
	def check_time( self , limit , setup = False ):
		if( setup ):
			self.start_time = time.time()
		return self.check_result( time.time() - self.start_time() , limit )

	def check_result( self , data , limit ):
		self.echo( self.name , "DATA : LIMIT " + str(data) + " : " + str(limit) + " m or s" )
		if( data => limit ): return True
		else: return False
	

if __name__ == "__main__":
	rospy.init_node( "mission_all" )
	MA = MissionAll( "mission_all" )
	rospy.spin()
