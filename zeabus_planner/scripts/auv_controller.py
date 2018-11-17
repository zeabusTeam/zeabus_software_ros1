#!/usr/bin/python2
#################################################################################################
####
####	FILE		: auv_controller.py
####	Author		: Supasan Komonlit
####	Create on	: 2018 , Nov 14
####	Purpose		: For connection of Mission Planner with Control	
####
#################################################################################################
from __future__ import print_function

import rospy
import time
import math

from zeabus_library.srv import *
from zeabus_elec_ros_hardware_interface.srv import Torpedo

from zeabus_library.msg import Point3 , State , Type2

from std_msgs.msg import String , Int64
from geometry_msgs.msg import Twist

class AUVController:

	data_velocity = { 'x':0 , 'y':0 , 'z':0 , 'roll':0 , 'pitch':0 , 'yaw':0 }


	def __init__( self , name_user = "" , want_init_node = False ):

		self.name = String()
		self.message = String()

		if want_init_node :
			if name_user == "":
				rospy.init_node( "robot_controller")
				self.set_name("robot_controller")
			else:
				rospy.init_node( name_user )
				self.set_name( name_user)

		# Publisher for send velocity
		self.velocity_data = Twist()
		self.velocity_publisher		= rospy.Publisher('/zeabus/cmd_vel', Twist , queue_size = 10)

		# Request group for send request to control
		self.request_absolute_xy	= rospy.ServiceProxy('/fix_abs_xy'		, two_point )
		self.request_relative_xy	= rospy.ServiceProxy('/fix_rel_xy'		, two_point )
		self.request_absolute_depth	= rospy.ServiceProxy('/fix_abs_depth'	, one_point )
		self.request_relative_depth = rospy.ServiceProxy('/fix_rel_depth'	, one_point )
		self.request_absolute_yaw	= rospy.ServiceProxy('/fix_abs_yaw'		, one_point )
		self.request_relative_yaw	= rospy.ServiceProxy('/fix_rel_yaw'		, one_point )
		self.request_check_state	= rospy.ServiceProxy('/ok_position'		, check_position )
		self.request_know_target	= rospy.ServiceProxy('/know_target'		, get_target )
		self.request_mode_control	= rospy.ServiceProxy('/mode_control'	, number_service )
		self.release_ball			= rospy.ServiceProxy('/fire_torpedo'	, Torpedo )
		self.hold_ball				= rospy.ServiceProxy('/hold_torpedo'	, Torpedo )

	# Control Point XY Group this use name of function to describe what you want
	def absolute_xy( self , value_x , value_y ):
		try:
			result = self.request_absolute_xy( value_x , value_y , self.name )
		except rospy.ServiceException , error :
			print("Service XY Failse error : " + error)

	def absolute_x( self , value ):
		self.absolute_xy( value , 0 )

	def absolute_y( self , value ):
		self.absolute_xy( 0 , value )

	def relative_xy( self , value_x , value_y ):
		try:
			result = self.request_relative_xy( value_x , value_y , self.name )
		except rospy.ServiceException , error :
			print("Service XY Failse error : " + error)

	def relative_x( self , value ):
		self.relative_xy( value , 0 )

	def relative_y( self , value ):
		self.relative_xy( 0 , value )

	# Control Point Z Group this use name of function to descripe you want relative or absolute
	def absolute_z( self , value ):
		try:
			result = self.request_absolute_depth( value , self.name )
		except rospy.ServiceException , error :
			print("Service Depth Failse error : " + error )

	def relative_z( self , value ):
		try:
			result = self.request_relative_depth( value , self.name )
		except rospy.ServiceException , error :
			print("Service Depth Failse error : " + error )

	# Control Point YAW Group this use name of function to descripe you want relative or absolute
	def absolute_yaw( self , value ):
		try:
			result = self.request_absolute_yaw( value , self.name )
		except rospy.ServiceException , error :
			print("Service yaw Failse error : " + error )

	def relative_yaw( self , value ):
		try:
			result = self.request_relative_yaw( value , self.name )
		except rospy.ServiceException , error :
			print("Service yaw Failse error : " + error )
		
	def set_name( self , name ):
		self.name.data = name

	def set_mode( self , mode ):
		try:
			result = self.request_mode_control( Int64( mode ) ).ok
		except rospy.ServiceException , error :
			print("Service mode Failse error : " + error)

	def receive_target( self , type_state ):
		result = self.request_know_target( String( type_state ))
		return [ result.target_01 , result.target_02 ]

	def velocity( self , data_velocity): # please send in dictionary type
		if( 'x' in data_velocity.keys() ):
			self.velocity_data.linear.x = data_velocity['x']
		else:
			self.velocity_data.linear.x = 0
		if( 'y' in data_velocity.keys() ):
			self.velocity_data.linear.y = data_velocity['y']
		else:
			self.velocity_data.linear.y = 0
		if( 'z' in data_velocity.keys() ):
			self.velocity_data.linear.z = data_velocity['z']
		else:
			self.velocity_data.linear.z = 0
		if( 'roll' in data_velocity.keys() ):
			self.velocity_data.angular.x = data_velocity['roll']
		else:
			self.velocity_data.angular.x = 0
		if( 'pitch' in data_velocity.keys() ):
			self.velocity_data.angular.y = data_velocity['pitch']
		else:
			self.velocity_data.angular.y = 0
		if( 'yaw' in data_velocity.keys() ):
			self.velocity_data.angular.z = data_velocity['yaw']
		else:
			self.velocity_data.angular.z = 0
		self.velocity_publisher.publish( self.velocity_data )

	def check_state( self , type_state , value):
		return self.request_check_state( String( type_state ) , value , self.name ).ok

	def fire_gripper( self ):
		result = self.release_ball( 0 )
		return result

	def pull_gripper( self ):
		result = self.hold_ball( 0 )
		return result

	def wait_time( self , second ):
		print( " Now sleep for " + str( second ) , end = '-------> ')
		time.sleep( second )
		print( "Wake Up")
		
