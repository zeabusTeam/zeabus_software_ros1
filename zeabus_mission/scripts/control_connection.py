#!/usr/bin/python2
#################################################################################################
####
####	FILE		: control_connection.py
####	Maintain	: Supasan Komonlit
####	Create on	: 2019 , FEB 20
####	Purpose		: For connection with control part
####
#################################################################################################
from __future__ import print_function
#===============>

import rospy
import math

from zeabus_library.srv import OneVector3Stamped, TwoStringVector3Stamped, ThreeOdometry, TwoBool

from std_msgs.msg import Header
from geometry_msgs.msg import Twist , TwistStamped , Vector3
from nav_msgs.msg import Odometry

from tf.transformations import euler_from_quaternion

################################### DETAILED OF SERVICE ########################################
##	OneVector3Stamped		: relative_${xy, z, yaw} , fix_$(z, yaw) , velocity_$(xy, z, yaw)
##	TwoStringVector3Stamped	: reset_target reset_velocity check_position 
##	ThreeOdometry			: get_state
##	TwoBool					: free_xy
################################################################################################

class ControlConnection:

	data_velocity = { 'x':0 , 'y':0 , 'z':0 , 'roll':0 , 'pitch':0 , 'yaw':0 }
	def __init__( self , name_id = "general_robot" ):

		self.header	= Header()
		self.vector = Vector3()
		self.state	= Odometry()
		self.twist_stamped = TwistStamped() 	

		self.set_name( name_id )

		self.save_state		= [ 0 , 0 , 0 , 0 , 0 , 0 ]

		self.temp_state		= [ 0 , 0 , 0 , 0 , 0 , 0 ]

		self.current_state	= [ 0 , 0 , 0 , 0 , 0 , 0 ]

		self.velocity_publisher = rospy.Publisher("/mission/twist", TwistStamped, queue_size= 1)

		self.cl_velocity_xy	= rospy.ServiceProxy("/control/velocity_xy"	, OneVector3Stamped )
		self.cl_velocity_z	= rospy.ServiceProxy("/control/velocity_z"	, OneVector3Stamped )
		self.cl_velocity_yaw= rospy.ServiceProxy("/control/velocity_yaw", OneVector3Stamped )
		self.cl_relative_xy	= rospy.ServiceProxy("/control/relative_xy"	, OneVector3Stamped )
		self.cl_relative_z	= rospy.ServiceProxy("/control/relative_z"	, OneVector3Stamped )
		self.cl_relative_yaw= rospy.ServiceProxy("/control/relative_yaw", OneVector3Stamped )
		self.cl_fix_z		= rospy.ServiceProxy("/control/fix_z"		, OneVector3Stamped )
		self.cl_fix_yaw		= rospy.ServiceProxy("/control/fix_yaw"		, OneVector3Stamped )

		self.cl_reset_target	= rospy.ServiceProxy("/control/reset_target" , 
				TwoStringVector3Stamped )
		self.cl_reset_velocity	= rospy.ServiceProxy("/control/reset_velocity" , 
				TwoStringVector3Stamped )
		self.cl_check_position	= rospy.ServiceProxy("/control/check_position" ,
				TwoStringVector3Stamped ) 

		self.cl_get_state		= rospy.ServiceProxy("/control/get_target" , ThreeOdometry )

		self.cl_free_xy			= rospy.ServiceProxy("/control/free_xy" , TwoBool )

	def distance( self ):
		self.target_state()
#		print( self.save_state )
#		print( self.temp_state )
		return math.sqrt( math.pow( 1.0*self.save_state[0] - 1.0*self.temp_state[0] , 2 ) + 
				math.pow( 1.0*self.save_state[1] - 1.0*self.temp_state[1] , 2 )	)

#===============> PUBLISHER PART

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

#===============> SERVICE OF THREEODOMETRY

	def collect_state( self ):
		result = self.service_three_odometry( self.cl_get_state , "target" , "get target state")
		self.save_state[0]	= result.pose.pose.position.x
		self.save_state[1]	= result.pose.pose.position.y
		self.save_state[2]	= result.pose.pose.position.z
		temp_orientation	= [ result.pose.pose.orientation.x, result.pose.pose.orientation.y,
								result.pose.pose.orientation.z, result.pose.pose.orientation.w ]
		[self.save_state[3] , self.save_state[4] , 
				self.save_state[5] ] = euler_from_quaternion( temp_orientation )

	def get_state( self ):
		result = self.service_three_odometry( self.cl_get_state, "current", "get current state")
		self.current_state[0]	= result.pose.pose.position.x
		self.current_state[1]	= result.pose.pose.position.y
		self.current_state[2]	= result.pose.pose.position.z
		temp_orientation	= [ result.pose.pose.orientation.x, result.pose.pose.orientation.y,
								result.pose.pose.orientation.z, result.pose.pose.orientation.w ]
		[self.current_state[3] , self.current_state[4] , 
				self.current_state[5] ] = euler_from_quaternion( temp_orientation )

	def target_state( self ):
		result = self.service_three_odometry( self.cl_get_state , "target" , "get target state")
		self.temp_state[0]	= result.pose.pose.position.x
		self.temp_state[1]	= result.pose.pose.position.y
		self.temp_state[2]	= result.pose.pose.position.z
		temp_orientation	= [ result.pose.pose.orientation.x, result.pose.pose.orientation.y,
								result.pose.pose.orientation.z, result.pose.pose.orientation.w ]
		[self.temp_state[3] , self.temp_state[4] , 
				self.temp_state[5] ] = euler_from_quaternion( temp_orientation )


#===============> SERVICE OF TWOSTRINGVECTOR3STAMPED

	def reset_target( self , type_reset ): # have 3 type xy z and yaw
		self.clear_vector()
		result = self.service_two_vector( self.cl_reset_target ,type_reset , self.vector , 
				"reset_target : " + str(type_reset) )
		return result

	def reset_velocity( self , type_reset ): # have 4 type x y z yaw
		self.clear_vector()
		result = self.service_two_vector( self.cl_reset_velocity , type_reset , self.vector ,
				"reset_velocity : " + str(type_reset) )
		return result

	def check_position( self , type_check , adding ):
		self.clear_vector( adding )
		result = self.service_two_vector( self.cl_check_position , type_check , self.vector ,
				"check_position : " + str(type_check) )
		return result

#===============> SERVICE OF ONEVECTOR3STAMPED
	def velocity_xy( self , x , y ):
		self.clear_vector()
		self.vector.x = x
		self.vector.y = y
		result = self.service_one_vector( self.cl_velocity_xy , self.vector , "velocity_xy")
		return result

	def velocity_z( self , z ):
		self.clear_vector( z )
		result = self.service_one_vector( self.cl_velocity_z , self.vector , "velocity_z")
		return result

	def velocity_yaw( self , yaw ):
		self.clear_vector( yaw )
		result = self.service_one_vector( self.cl_velocity_yaw , self.vector , "velocity_yaw")
		return result

	def relative_xy( self , x , y ):
		self.clear_vector()
		self.vector.x = x
		self.vector.y = y
		result = self.service_one_vector( self.cl_relative_xy , self.vector , "relative_xy")
		return result

	def relative_z( self , z ):
		self.clear_vector()
		self.vector.z = z
		result = self.service_one_vector( self.cl_relative_z , self.vector , "relative_z")
		return result

	def relative_yaw( self , yaw ):
		self.clear_vector()
		self.vector.z = yaw 
		result = self.service_one_vector( self.cl_relative_yaw , self.vector , "relative_yaw")
		return result

	def fix_z( self , z ):
		self.clear_vector()
		self.vector.z = z 
		result = self.service_one_vector( self.cl_fix_z , self.vector , "fix_z")
		return result

	def fix_yaw( self , yaw ):
		self.clear_vector()
		self.vector.z =yaw 
		result = self.service_one_vector( self.cl_fix_yaw , self.vector , "fix_yaw")
		return result

#===============> SERVICE OF TWOBOOL
	def free_xy( self , data ):
		result = self.service_two_bool( self.cl_free_xy , data , "target_free_xy")
		return result
		
#===============> FUNCTION FOR CALL SERVICE

	def service_one_vector( self , service , vector3 , message = ""):
		self.stamp_time()
		try:
			result = service( self.header , vector3 ).result
		except rospy.ServiceException , error :
			print("\x1B[1;31mservice one vector of " + str(message) + " :\n\t\x1B[0;37m" + 
					str(error) )
			result = False
		return result

	def service_two_vector( self , service , string , vector3 , message = "" ):
		self.stamp_time()
		try:
			result = service( self.header , string , vector3 ).result
		except rospy.ServiceException , error :
			print("\x1B[1;31mservice two vector of " + str(message) + " :\n\t\x1B[0;37m" + 
					str(error) )
			result = False
		return result

	def service_three_odometry( self , service , string , message = ""):
		try:
			result = service( string ).data
		except rospy.ServiceException , error :
			print("\x1B[1;31mservice three odometry of " + str(message) , " :\n\t\x1B[0;37m" + 
					str(error) )
		return result

	def service_two_bool( self , service , data , message = ""):
		try:
			result = service( string ).result
		except rospy.ServiceException , error :
			print("\x1B[1;31mservice two bool of " + str(message) , " :\n\t\x1B[0;37m" + 
					str(error) )
		return result

#===============> SET UP HEADER PART
	def set_name( self , name ):
		self.header.frame_id = name

	def stamp_time( self ):
		self.header.stamp = rospy.Time.now()

	def clear_vector( self , number = 0.0 ):
		self.vector.x = number
		self.vector.y = number
		self.vector.z = number

if( __name__ == "__main__" ):
	rospy.init_node("testing_control_connection")

	rate = rospy.Rate(10)

	ch = ControlConnection("testing_robot")

	print("Welcome to testing control")

	ch.velocity_xy( 0.5 , 0.2 )
	print("Already send velocity_xy 0.5 : 0.2")

	ch.collect_state()
	distance = ch.distance()
	print("Now distance is " + str( distance) )

	print("R P Y is " + str(ch.save_state[3]) + " : " + str(ch.save_state[4]) + " : " + 
			str(ch.save_state[5] ) )

	ch.relative_xy( 2 , 0 )
	print("Now command to relative_xy is 2 , 0 ")

	ok_position = 0
	print("Waiting for ok")
	while( not rospy.is_shutdown() ):
		rate.sleep()
		if( ch.check_position( "xy" , 0.2 ) ):
			count_ok += 1
		else:
			count_ok = 0
		if( count_ok == 5 ):
			break
		print("Wait xy count is " + str( count_ok ) )
	
