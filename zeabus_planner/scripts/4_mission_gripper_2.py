#!/usr/bin/python2
#################################################################################################
####
####	FILE		: 4_mission_gripper_2.py
####	Author		: Supasan Komonlit
####	Create on	: 2018 , Nov 15
####	Purpose		: For make mission pickball this will control robot z by code	
####
#################################################################################################

import rospy
import math
import time

from auv_controller		import AUVController
from vision_collector	import VisionCollector
from zeabus_planner.srv	import mission_result

from std_msgs.msg		import Bool , Int8 , String

class mission_gripper:

	def __init__( self ):

		self.auv	= AUVController( "mission_gripper" , True )
	
		print("<=== MISSION GRIPPER ===> Checking service of DRUM ")
		rospy.wait_for_service('vision_drum')
		print("<=== MISSION GRIPPER ===> Have Service OK")
		
		self.vision	= VisionCollector( "drum" )
		self.past_action = { 'direction' : None , 'value' : None }

		self.rate = rospy.Rate( 30 )
		self.mission_planner = rospy.Service('mission/gripper' , mission_result , self.main_play)
		self.data_pub = rospy.Publisher('mission/echo_planner' , String , queue_size = 1 )

	def sleep( self , second):
		self.rate.sleep()
		self.auv.wait_time( second )

	def echo( self , message):
		self.data_pub.publish( String( message ) )

	def main_play( self , request ):
		self.sucess_mission = False
		self.current_step = 1 # have 3 step to play
		self.request_velocity = { 'x' : 0 , 'y' : 0 , 'z':0}
		
		self.step_01() 

		if( self.current_step == 2 ):
			self.step_02()
		else:
			print("<=== MISSION GRIPPER ===> MISSION GRIPPER ABORTED!!!")

		if( self.current_step== 3 ):
			self.step_03()
		else:
			print("<=== MISSION GRIPPER ===> MISSION GRIPPER ABORTED!!!")

		return Bool( self.sucess_mission ) , Int8( self.current_step )

	def reset_request( self ):
		self.request_velocity = {'x': 0 , 'y' : 0 , 'z' : 0}

	def check_depth( self , target ):
		return self.auv.receive_target('z')[0] < target

	def print_request( self ):
		return ( "VELOCITY x :" + str( self.request_velocity['x'] ) +
				"==== y :" + str( self.request_velocity['y']) +
				"==== z :" + str( self.request_velocity['z']))

	def check_range( self , axis , value , start , length , velocity):
		if( start <= value and valur <= start+length ):
			self.request_velocity[ axis ] = 0 
		elif( value < start):
			self.request_velocity[ axis ] = -1 * velocity
		else:
			self.request_velocity[ axis ] = velocity

	def step_01( self ):
		self.echo("<=== MISSION GRIPPER ===> STEP01 MISSION GRIPPER START MOVE BALL CENTER")
		count_unfound = 0
		while( not rospy.is_shutdown() and not self.check_depth( -0.75 ) ):
			self.echo( "current_depth " + str( self.auv.receive_target('z')[0] ) )
			self.sleep( 0.15 )
			self.request_velocity['z'] = -1.1
			self.vision.analysis_all( 'drum' , "pick" , 5 )
			if( self.vision.have_object() ):
				count_unfound = 0
				self.check_range( "x" , self.vision.center_y() , 0.0 , 0.2 , 0.2 )
				self.check_range( "y" , self.vision.center_x() , 0.1 , 0.3 , -0.2 )
				self.echo( self.print_request() )
				self.auv.velocity( self.request_velocity)
			else:
				count_unfound += 1
				self.echo("<=== MISSION GRIPPER ===> WARNNING DO NOT FIND OBJECT")
				if( count_unfound == 3 ):
					break
			

if __name__=="__main__":
	mission_04 = mission_gripper()	
	rospy.spin()
