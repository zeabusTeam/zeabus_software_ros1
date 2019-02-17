#!/usr/bin/python2
################################################################################################
####
####	FILE		: mission_gate.py
####	Author		: Supasan Komonlit
####	Create on	: 2019 , FEB 16
####	Purpose		: For connection of Mission Planner with Control	
####
################################################################################################
from __future__ import print_function
#===============>

import rospy
import time
import math

from control_handle		import ControlAUV
from vision_collector	import VisionCollector

from std_msgs.msg		import String

class MissionGate:

	def __init__( self ):

#===============> STANDARD SETUP
		self.ch = ControlAUV("gate_planner")

		self.rate = rospy.Rate( 30 )
		self.data_pub = rospy.Publisher("/mission/echo_planner" , String , queue_size = 1 )

#===============> SPECIFIC CLASS
		self.vision = VisionCollector( "gate" )
	
#===============> STANDARD FUNCTION
	def sleep( self , second ):
		self.rate.sleep()
		time.sleep( second )

	def echo( self , message ):
		self.data_pub.publish( String( message ) )

#===============> SPECIFIC FUNCTION
	def main_play( self , move_x , move_y ):
		self.echo( "==========> MISSION GATE START! ")

		self.fix_z( -3.5 )
		self.ch.save_target()
		self.start_yaw = self.ch.save_position[5]

		count_ok = 0
		self.echo( "Waiting Depth are OK")
		while(not rospy.is_shutdown() ):
			self.sleep( 0.1 )	
			if( self.auv.check_position( "z" , 0.1 ) ):
				count_ok += 1
			else:
				count_ok = 0
			if( count_ok == 5 ):
				break
			self.echo( "Wait Depth count is " + str( count_ok ) )
		
		count_ok = 0
		self.echo( "Waiting YAW are OK")
		while( not rospy.is_shutdown() ):
			self.sleep( 0.1 )
			if( self.auv.check_position( "yaw" , 0.08 ) ):
				count_ok += 1
			else:
				count_ok = 0
			if( count_ok == 5 ):
				break
			self.echo( "Wait Yaw count is " + str( count_ok ) )

		self.survey_mode( self.vision , "gate" , "sevinar" , 3 , 4 , 1.5 , 3 )
		if( not self.find_object ):
			self.echo( "Why return from survey if don't found")
			return 0
		self.echo("Let play")
		self.step_01()
		self.step_02()

	def step_01( self ):
		while( not rospy.is_shutdown() ):
			self.sleep( 0.1 )
			self.vision.analysis_all( "gate" , "sevinar" , 5 )
			self.vision.echo_data() # for print n_obj and center of x ,  y axis
			if( self.vision.have_object() ):
				count_unfound = 0
				if( self.vision.result['pose'] == 0 ):
					if( abs( self.vision.center_x() ) < 0.1 ):
						self.echo("Found Pos : 0 and Move Forward")
						self.auv.velocity( {'x' : 0.2})
						if( self.vision.area() > 0.7 ):
							self.echo("Area are over let play direct and spin")
							self.ch.velocity_xy( 1 , 0 )
							self.sleep( 0.1 )
							self.echo("I command to fix velocity for odom frame")
							self.ch.relative_yaw( math.pi )
							self.echo("I command to rotate pi radian")
							break
					elif( self.vision.center_x() < 0 )
						self.echo("Found Pos : 0 and Move Left")
						self.auv.velocity( {'y' : 0.4})
					elif( self.vision.center_x() > 0 )
						self.echo("Found Pos : 0 and Move right")
						self.auv.velocity( {'y' : -0.4})
				elif( self.vision.result['pos'] == -1 ):
					self.echo("Found Pos : -1 and Move Left")
					self.auv.velocity( {'y' : 0.5} )
				elif( self.vision.result['pos'] == 1 ):
					self.echo("Found Pos : 1 and Move right")
					self.auv.velocity( {'y' : -0.5 } )
				else:
					self.echo("Warning Don't have this condition")

		def step_02( self ):
			while( not rospy.is_shutdown() ):
				self.sleep( 0.1 )
				self.vision.analysis_all( "gate" , "sevinar" , 5 )
				self.vision.echo_data()

#===============> STANDARD FUNCTION
	def survey_mode(	self			, vision		, task		, request	, 
						first_forward	, first_survey	, forward	, survey	):
		self.find_object = False
		time_first_forward = True
		time_first_survey = True
		# Big Loop make survey still find object
		self.echo(	"Data for SURVEY TASK : " + task + " REQUEST : " + request + 
					" And distance are " + str( first_forward ) + " : " + str( first_survey) +
					" : " + str( forward ) + " : " + str( survey ) )
		while( not rospy.is_shutdown() and not self.find ):
			# This for go forward
			if( time_first_forward ):
				distance = first_forward
				time_first_forward = False
			else:
				distance = forward
			self.echo( "We will move forward and search distance is " + str( distance ) )
			self.ch.save_position()
			self.ch.velocity_xy( math.copysign( 0.3 , distance ) , 0 )
			while( not rospy.is_shutdown() and not self.find_object ):
				self.sleep( 0.1 )
				current_distance = self.ch.calculate_distance()
				self.echo( "SURVEY DISTANCE IS " + str( current_distance ) )
				if( current_distance > distance ):
					break
				vision.analysis_all( task , request , 5 )
				self.echo( vision.echo_data() )
				self.find_object = vision.have_object()

			self.ch.reset_velocity( "xy" )
			if( self.find_object ): break
			count_ok = 0 
			while(not rospy.is_shutdown() ):
				self.sleep( 0.1 )	
				if( self.auv.check_position( "xy" , 0.1 ) ):
					count_ok += 1
				else:
					count_ok = 0
				if( count_ok == 5 ):
					break
				self.echo( "Wait distance counr is " + str( count_ok ) )

#===============> This for survey round three
			if( time_first_survey ):
				distance = first_survey
				time_first_survey = False
			else:
				distance = math.copysign( survey , first_survey )
			self.echo( "We will move survey and search distance is " + str( distance ) )
			self.ch.save_position()
			self.ch.velocity_xy( 0 , math.copysign( 0.3 , distance )  )
			while( not rospy.is_shutdown() and not self.find_object ):
				self.sleep( 0.1 )
				current_distance = self.ch.calculate_distance()
				self.echo( "SURVEY DISTANCE IS " + str( current_distance ) )
				if( current_distance > distance ):
					break
				vision.analysis_all( task , request , 5 )
				self.echo( vision.echo_data() )
				self.find_object = vision.have_object()

			self.ch.reset_velocity("xy")
			if( self.find_object ): break
			count_ok = 0 
			while(not rospy.is_shutdown() ):
				self.sleep( 0.1 )	
				if( self.auv.check_position( "xy" , 0.1 ) ):
					count_ok += 1
				else:
					count_ok = 0
				if( count_ok == 5 ):
					break
				self.echo( "Wait distance counr is " + str( count_ok ) )

#===============> This for forward step three
			distance = forward
			self.echo( "We will move forward and search distance is " + str( distance ) )
			self.ch.save_position()
			self.ch.velocity_xy( 0.2 , 0 )
			while( not rospy.is_shutdown() and not self.find_object ):
				self.sleep( 0.1 )
				if( current_distance > distance ):
					break
				vision.analysis_all( task , request , 5 )
				self.echo( vision.echo_data() )
				self.find_object = vision.have_object()

			self.ch.reset_velocity("xy")
			if( self.find_object ): break
			count_ok = 0 
			while(not rospy.is_shutdown() ):
				self.sleep( 0.1 )	
				if( self.auv.check_position( "xy" , 0.1 ) ):
					count_ok += 1
				else:
					count_ok = 0
				if( count_ok == 5 ):
					break
				self.echo( "Wait distance count is " + str( count_ok ) )

#===============> This for survey round four
			distance = math.copysign( survey , first_survey * -1 )
			self.echo( "We will move survey and search distance is " + str( distance ) ) 
			self.ch.save_position()
			self.ch.velocity_xy( 0.2 , 0 )
			while( not rospy.is_shutdown() and not self.find_object ):
				self.sleep( 0.1 )
				current_distance = self.ch.calculate_distance()
				self.echo( "SURVEY DISTANCE IS " + str( current_distance ) )
				if( self.auv.check_state( 'xy' , 0.06) ):
					break
				vision.analysis_all( task , request , 5 )
				self.echo( vision.echo_data() )
				self.find_object = vision.have_object()
			self.ch.reset_velocity("xy")
			if( self.find_object ): break
			count_ok = 0 
			while(not rospy.is_shutdown() ):
				self.sleep( 0.1 )	
				if( self.auv.check_position( "xy" , 0.1 ) ):
					count_ok += 1
				else:
					count_ok = 0
				if( count_ok == 5 ):
					break
