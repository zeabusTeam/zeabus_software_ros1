#!/usr/bin/python2
#################################################################################################
####
####	FILE		: 4_mission_gripper_2.py
####	Author		: Supasan Komonlit
####	Create on	: 2018 , Nov 15
####	Purpose		: For make mission pickball this will control robot z by code
####				:	And will pick ball by make ball beside and pick that	
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
		if( start <= value and valur <= start+length ):
			self.request_velocity[ axis ] = 0 
		elif( value < start):
			self.request_velocity[ axis ] = -1 * velocity
		else:
			self.request_velocity[ axis ] = velocity

	def step_01( self ):
		None

if __name__=="__main__":
	mission_04 = mission_gripper()	
	rospy.spin()
