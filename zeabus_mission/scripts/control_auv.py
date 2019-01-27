#!/usr/bin/python2
################################################################################################
##
##	FILE				: control_auv.py
##  AUTHOR				: Supasan Komonlit
##  Date Created		: 2019, JAN 27
##  Date lasr modified	: 2019, ??? ??
##	Purpose				: Connection Control Part
##
##	Maintainer			: Supasan Komonlit
##	e-mail				: supasan.k@ku.th
##	version				: 1.0.0
##	status				: Production
##
##	??????
################################################################################################

from __future__ import print_function

import rospy
import time
import math

from zeabus_library.srv import *

from zeabus_library.msg import Twist

from std_msgs.msg import String, Int64 , Float64

class ControlAUV:

	data_velocity = { 'x':0 , 'y':0 , 'z':0 , 'roll':0 , 'pitch':0 , 'yaw':0 }

	def __init__( self , name_user = "" , want_init_node = False ):
		
		self.name = ""
		self.message = ""

		if( want_init_node ):
			if( name_user == "" ):
				rospy.init_node( "control_robot" )
				self.set_name( "control_robot" )
			else:
				rospy.init_node( name_user )
				self.self( name_use )

		self.velocity_data = Twist()
		self.velocity_publisher = rospy.Publisher("/control/twist" , Twist , queue_size = 10 )

		self.relative_xy	= rospy.ServiceProxy("/control/relative_xy" , TwoPoint )
		self.mode			= rospy.ServiceProxy("/control/mode" , OneInt)
		self.fix_point		= rospy.ServiceProxy("/control/fix_point" , StrPoint)
		self.check_position = rospy.ServiceProxy("/control/check_position" , StrPoint)
		self.get_target		= rospy.ServiceProxy("/control/get_target" , GetTwoPoint )

	def relative_xy( self , value_x , value_y ):
		try:
			result = self.relative_xy( "yaw" , value_x , value_y )
		except rospy.ServiceException , error :
			print("Service relative_xy Failuer error : " + error , end = "\n\n" )

	def absolute_z( self , value ):
		try:
			result = self.fix_point( "z" , value )
		except rospy.ServiceException , error :
			print("Service absolute_z Failuer error : " + error , end = "\n\n" )
	def absolute_yaw( self , value ):
		try:
			result = self.fix_point( "yaw" , value )
		except rospy.ServiceException , error :
			print("Service absolute_yaw Failuer error : " + error , end = "\n\n" )

	def relative_yaw( self , value ):
		try:
			result = self.fix_point( "+yaw" , value )
		except rospy.ServiceException , error :
			print("Service relative_yaw Failuer error : " + error , end = "\n\n" )

	def receive_target( self , type_state ):
		result = self.get_target( type_state )
		return [ result.x , result.y ]

	def check_state( self , type_state , value ):
		return self.check_position( type_state , value )

	def set_name( self , name ):
		self.name = name 

	def wait_time( self , second ):
		time.sleep( second )

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
