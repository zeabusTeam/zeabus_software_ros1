#!/usr/bin/python2
#################################################################################################
####
####	FILE		: SAUVC_mission_gripper.py
####	Author		: Supasan Komonlit
####	Create on	: 2018 , Nov 18
####	Purpose		: For make mission pickball this will control robot z by code	
####
#################################################################################################

import rospy
import math
import time

from auv_controller		import AUVController
from vision_collector	import VisionCollector
from zeabus_library.srv	import MissionResult

from std_msgs.msg		import Bool , Int8 , String

class MissionGripper:

	def __init__( self ):

		self.auv	= AUVController( "mission_gripper" , True )	

		self.rate = rospy.Rate( 30 )
		self.data_pub = rospy.Publisher('mission/echo_planner' , String , queue_size = 1 )

		print("<=== MISSION GRIPPER ===> Waitting service of DRUM ")
		rospy.wait_for_service('vision_drum')
		print("<=== MISSION GRIPPER ===> Have Service OK")

		self.mission_planner = rospy.Service('mission/gripper' , MissionResult , self.main_play)

		self.vision	= VisionCollector( "drum" )	
		print( "<========== FINISH SETUP MISSION GRIPPER ==========>")

	def sleep( self , second):
		self.rate.sleep()
		self.auv.wait_time( second )

	def echo( self , message):
		self.data_pub.publish( String( message ) )

	def main_play( self , request ):
		self.sucess_mission = False
		self.current_step = 2 # have 3 step to play
		self.request_velocity = { 'x' : 0 , 'y' : 0 , 'z':0}
		
		#self.step_01() 

		if( self.current_step == 2 ):
			self.step_02()
		else:
			print("<=== MISSION GRIPPER ===> MISSION GRIPPER ABORTED!!!")

		if( self.current_step == 3 ):
			self.step_03()

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
		if( start <= value and value <= start+length ):
			self.request_velocity[ axis ] = 0 
		elif( value < start):
			self.request_velocity[ axis ] = -1 * velocity
		else:
			self.request_velocity[ axis ] = velocity

	def step_01( self ):
		self.echo("<=== MISSION GRIPPER ===> STEP01 MISSION GRIPPER START MOVE BALL CENTER")
		self.reset_request()
		count_unfound = 0
		while( not rospy.is_shutdown() ): # and not self.check_depth( -0.8 ) ):
			self.echo( "current_depth " + str( self.auv.receive_target('z')[0] ) )
			self.sleep( 0.15 )
			self.request_velocity['z'] = -1.2
			self.vision.analysis_all( "drum" , "pick" , 5 )
			self.echo( self.vision.echo_data() )
			if( self.vision.have_object() ):
				count_unfound = 0
				self.check_range( "x" , self.vision.center_y() , 0.0 , 0.2 , 0.2 )
				self.check_range( "y" , self.vision.center_x() , 0.1 , 0.3 , -0.2 )
				if( self.request_velocity['x'] == 0 and self.request_velocity['y'] == 0):
					break
				self.echo( self.print_request() )
				self.auv.velocity( self.request_velocity)
			else:
				count_unfound += 1
				self.echo("<=== MISSION GRIPPER ===> WARNNING DO NOT FIND OBJECT")
				if( count_unfound == 3 ):
					break
		if( count_unfound == 0 ):
			self.current_step += 1

	def step_02( self ):
		self.echo("<=== MISSION GRiPPER ===> STEP02 PICK BALL UP")
		start_time = time.time()
		diff_time = time.time() - start_time
		self.reset_request()
		self.request_velocity['z'] = -1.05
		while( not rospy.is_shutdown() and diff_time < 20 ):
			self.echo( "Time capture is " + str( diff_time ))
			self.sleep( 0.08 )
			self.vision.analysis_all( 'drum' , "pick" , 5 )
			if( self.vision.have_object() ):
				self.check_range( "x" , self.vision.center_y() , -0.5 , 0.0 , 0.20 )
				self.check_range( "y" , self.vision.center_x() , -0.7 , -0.2 , -0.20 )
				self.echo( self.print_request() )
			else:
				self.request_velocity['x'] = -0.15
				self.request_velocity['y'] = 0.3
				self.echo( self.print_request() )
			self.auv.velocity( self.request_velocity )
			diff_time = time.time() - start_time
		self.reset_request()
		self.echo( "<=== MISSION GRIPPER ===> Time out go up")
		while( not rospy.is_shutdown() and not self.check_depth( -0.6 ) ):
			self.sleep( 0.2 )
			self.request_velocity['z'] = -0.8
			self.auv.velocity( self.request_velocity )			 
		self.current_step += 1	

	def step_03( self ):
		self.echo( "<=== MISSION GRIPPER ===> STEP THIRD CHECK BALL")
		# Check Have Ball All not
		
		# FInish Check
		self.current_step += 1
		self.sucess_mission = True
		

if __name__=="__main__":
	mission_04 = MissionGripper()	
	rospy.spin()
