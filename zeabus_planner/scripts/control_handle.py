#!/usr/bin/python2
################################################################################################
####
####	FILE		: control_handle.py
####	Author		: Supasan Komonlit
####	Create on	: 2019 , FEB 16
####	Purpose		: For connection of Mission Planner with Control	
####
################################################################################################
from __future__ import print_function
#===============>

import rospy
import math

from zeabus_library.srv import *

from geometry_msgs.msg import Twist , TwistStamped , Vector3
from std_msgs.msg import Header
from nav_msgs.msg import Odometry

from tf.transformations import euler_from_quaternion

class ControlAUV:

	data_velocity = { 'x':0 , 'y':0 , 'z':0 , 'roll':0 , 'pitch':0 , 'yaw':0 }

	def __init__( self , name_id = "general_robot"):
		
		self.header = Header()
		self.vector = Vector3()
		self.state = Odometry()
		self.twist_stamped = TwistStamped()

		self.set_name( name_id )

		self.save_state = [0 , 0 , 0 , 0 , 0 , 0]

		self.collect_state = [0 , 0 , 0 , 0 , 0 ]

		self.velocity_publisher	= rospy.Publisher("/mission/twist" ,TwistStamped ,queue_size = 1)

		self.cl_velocity_xy	= rospy.ServiceProxy("/control/velocity_xy" , OneVector3Stamped )
		self.cl_velocity_z	= rospy.ServiceProxy("/control/velocity_z" , OneVector3Stamped )
		self.cl_relative_xy	= rospy.ServiceProxy("/control/relative_xy" , OneVector3Stamped )
		self.cl_relative_z	= rospy.ServiceProxy("/control/relative_z" , OneVector3Stamped )
		self.cl_relative_yaw= rospy.ServiceProxy("/control/relative_yaw" , OneVector3Stamped )
		self.cl_fix_z		= rospy.ServiceProxy("/control/fix_z" , OneVector3Stamped )
		self.cl_fix_yaw		= rospy.ServiceProxy("/control/fix_yaw" , OneVector3Stamped )

		self.cl_reset_target	= rospy.ServiceProxy("/control/reset_target" , 
				TwoStringVector3Stamped )
		self.cl_reset_velocity	= rospy.ServiceProxy("/control/reset_velocity" , 
				TwoStringVector3Stamped )
		self.cl_check_position	= rospy.ServiceProxy("/control/check_position" , 
				TwoStringVector3Stamped )

		self.cl_get_target		= rospy.ServiceProxy("/control/get_target" , ThreeOdometry )

	def calculate_distance( self ):
		self.collect_target()
		return math.sqrt( math.pow( self.save_state[0] - self.collect_state[0] , 2 ) + 
				math.pow( self.save_state[1] - self.collect_state[1] , 2 ) ) 

	def relative_xy( self , x , y ):
		self.stamp_time()
		self.vector.x = x
		self.vector.y = y
        try:
			print( "Sending relative_xy x : y is " + str(x) + "," + str(y) )
			result = self.cl_relative_xy( self.header , self.vector ).result
        except rospy.ServiceException , error:
			print("Service relative_xy line 73 error :\n\t" + error )
        self.clear_vector()
        return result
	
	def relative_z( self , z ):
		self.stamp_time()
		self.vector.z = z
        try:
            print( "Sending relative_z : " + str(z) )
            result = self.cl_relative_z( self.header , self.vector ).result
        except rospy.ServiceException , error :
            print("Service relative_z line 84 error :\n\t" + error )
        self.clear_vector()
        return result

	def relative_yaw( self , yaw ):
		self.stamp_time()
		self.vector.z = yaw
        try:
            print( "Sending relative_yaw : " + str(yaw))
			result = self.cl_relative_yaw( self.header , self.vector ).result
        except rospy.ServiceException , error :
            print("Service relative_yaw line 95 error :\n\t" + error )
		self.clear_vector()
		return result

	def velocity_xy( self , x , y ):
		self.stamp_time()
		self.vector.x = x
		self.vector.y = y
        try:
            print( "Sending velocity_xy x : y is " + str(x) + "," + str(y) )
		    result = self.cl_velocity_xy( self.header , self.vector ).result
        except rospy.ServiceException , error :
            print("Service velocity_xy line 107 error :\n\t" + error )
		self.clear_vector()
		return result

	def velocity_z( self , z ):
		self.stamp_time()
		self.vector.z = z
        try:
            print( "Sending velocity_z z is " + str(z) )
    		result = self.cl_velocity_z( self.header , self.vector ).result
        except rospy.ServiceException , error L
            print( "Service velocity_z line 108 error :\n\t" + error )
		self.clear_vector()
		return result

	def velocity_yaw( self , yaw ):
		self.stamp_time()
		self.vector.z = yaw
        try:
            print( "Sending velocity_yaw is " + str(yaw) )
    		result = self.cl_velocity_yaw( self.header , self.vector ).result
        except rospy.ServiceException , error L
            print( "Service velocity_yaw line 129 error :\n\t" + error )
		self.clear_vector()
		return result

	def fix_z( self , z ):
		self.stamp_time()
		self.vector.z = z
        try:
            print( "Sending fix_z is " + str(z) )
    		result = self.cl_fix_z( self.header , self.vector ).result
        except rospy.ServiceException , error L
            print( "Service fix_z line 141 error :\n\t" + error )
		self.clear_vector()
		return result

	def fix_yaw( self , yaw ):
		self.stamp_time()
		self.vector.z = yaw 
        try:
            print( "Sending fix_yaw is " + str(yaw) )
    		result = self.cl_fix_yaw( self.header , self.vector ).result
        except rospy.ServiceException , error L
            print( "Service fix_yaw line 151 error :\n\t" + error )
		self.clear_vector()
		return result

	def reset_target( self , data ):
		self.stamp_time()
        try:
            print( "Sending reset_target is " + data )
    		result = self.cl_reset_target( self.header , data , self.vector ).result
        except rospy.ServiceException , error L
            print( "Service reset_target line 161 error :\n\t" + error )
		return result

	def reset_velocity( self , data ):
		self.stamp_time()
        try:
            print( "Sending reset_velocity is " + data )
    		result = self.cl_reset_velocity( self.header , data , self.vector ).result
        except rospy.ServiceException , error L
            print( "Service reset_velocity line 170 error :\n\t" + error )
		return result

	def check_position( self , data , adding ):
		self.stamp_time()
		self.clear_vector( adding )
        try:
            print( "Sending check_position data : adding is " + data + " : " + str( adding) )
    		result = self.cl_check_position( self.header , data , self.vector ).result
        except rospy.ServiceException , error L
            print( "Service check_position line 180 error :\n\t" + error )
		self.clear_vector()
		return result

	def save_target( self ):
		self.stamp_time()
        try:
            print( "Sending get_target" )
    		result = self.cl_get_target( "target" ).data
        except rospy.ServiceException , error L
            print( "Service get_target line 190 error :\n\t" + error )
		self.save_state[0] = result.pose.pose.position.x
		self.save_state[1] = result.pose.pose.position.y
		self.save_state[2] = result.pose.pose.position.z
		temp_orientation = [ result.pose.pose.orientation.x , result.pose.pose.orientation.y ,
							result.pose.pose.orientation.z , result.pose.pose.orientation.w ]
		[ self.save_state[3] , self.save_state[4] 
				, self.save_state[5] ] = euler_from_quaternion( temp_orientation ) 

	def collect_target( self ):
		self.stamp_time()
        try:
            print( "Sending get_target" )
    		result = self.cl_get_target( "target" ).data
        except rospy.ServiceException , error L
            print( "Service get_target line 205 error :\n\t" + error )
		self.collect_state[0] = result.pose.pose.position.x
		self.collect_state[1] = result.pose.pose.position.y
		self.collect_state[2] = result.pose.pose.position.z

	def velocity( self , data_velocity): # please send in dictionary type
		self.stamp_time()
		self.twist_stamped.header = self.header
		if( 'x' in data_velocity.keys() ):
			self.twist_stamped.twist.linear.x = data_velocity['x']
		else:
			self.twist_stamped.twist.linear.x = 0
		if( 'y' in data_velocity.keys() ):
			self.twist_stamped.twist.linear.y = data_velocity['y']
		else:
			self.twist_stamped.twist.linear.y = 0
		if( 'z' in data_velocity.keys() ):
			self.twist_stamped.twist.linear.z = data_velocity['z']
		else:
			self.twist_stamped.twist.linear.z = 0
		if( 'roll' in data_velocity.keys() ):
			self.twist_stamped.twist.angular.x = data_velocity['roll']
		else:
			self.twist_stamped.twist.angular.x = 0
		if( 'pitch' in data_velocity.keys() ):
			self.twist_stamped.twist.angular.y = data_velocity['pitch']
		else:
			self.twist_stamped.twist.angular.y = 0
		if( 'yaw' in data_velocity.keys() ):
			self.twist_stamped.twist.angular.z = data_velocity['yaw']
		else:
			self.twist_stamped.twist.angular.z = 0
		self.velocity_publisher.publish( self.twist_stamped )

#===============> SET UP HEADER PART
	def set_name( self , name ):
		self.header.frame_id = name

	def stamp_time( self ):
#		self.header.stamp = rospy.get_time()	
		None

	def clear_vector( self , number = 0.0 ):
		self.vector.x = number
		self.vector.y = number
		self.vector.z = number

if __name__=="__main__":
	rospy.init_node("test_mission")
	ch = ControlAUV("testing")
	printf("Welcome to testing control handle this test only command can run")

	ch.relative_xy( 10 , 10 )
	printf("Finsih test relative xy")

	ch.relative_z(-3)
	printf("Finish test relative z")

	ch.relative_yaw( 3.14 ):
	printf("Finish test relative yaw")
