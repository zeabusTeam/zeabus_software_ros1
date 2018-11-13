#!/usr/bin/python2

import rospy
import math

from zeabus_library.srv import *
from zeabus_elec_ros_hardware_interface.srv import Torpedo

from zeabus_control.msg import Point3 , State , Type2

from std_msgs.msg import String , Int64
from geometry_msgs.msg import Twist

class auv_controller:

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

		self.velocity_data = Twist()

		self.velocity_publisher		= rospy.Publisher('/zeabus/cmd_vel', Twist , queue_size = 10)
		self.request_absolute_depth	= rospy.ServiceProxy('/fix_abs_depth' , one_point )
		self.request_relative_depth = rospy.ServiceProxy('/fix_rel_depth' , one_point )
		self.request_absolute_yaw	= rospy.ServiceProxy('/fix_abs_yaw' , one_point )
		self.request_relative_yaw	= rospy.ServiceProxy('/fix_rel_yaw' , one_point )
		self.request_check_state	= rospy.ServiceProxy('/ok_position' , check_position )
		self.request_know_target	= rospy.ServiceProxy('/know_target' , get_target )
		self.request_mode_control	= rospy.ServiceProxy('/mode_control' , number_service )
		self.release_ball			= rospy.ServiceProxy('/fire_torpedo' , Torpedo )
		self.hold_ball				= rospy.ServiceProxy('/hold_torpedo' , Torpedo )

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

	def reset_velocity( self ):
		self.velocity_data.linear.x = 0
		self.velocity_data.linear.y = 0
		self.velocity_data.linear.z = 0
		self.velocity_data.angular.x = 0
		self.velocity_data.angular.y = 0
		self.velocity_data.angular.z = 0

	def receive_target( self , type_state ):
		result = self.request_know_target( String( type_state ))
		return [ result.target_01 , result.target_02 ]

	def velocity( self , type_velocity , value ):
		self.reset_velocity()
		if( type_velocity == "x"):
			self.velocity_data.linear.x = value
		elif( type_velocity == "y"):
			self.velocity_data.linear.y = value	
		self.velocity_publisher.publish( self.velocity_data )

	def ok_state( self , type_state , value):
		return self.request_check_state( String( type_state ) , value , self.name ).ok

	def fire_gripper( self ):
		result = self.release_ball( 0 )
		return result

	def pull_gripper( self ):
		result = self.hold_ball( 0 )
		return result
