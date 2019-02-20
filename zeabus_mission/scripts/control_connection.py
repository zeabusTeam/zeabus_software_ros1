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

from zeabus_library.srv import OneVector3Stamped , TwoStringVector3Stamped , ThreeOdometry

from std_msgs.msg import Header
from geometry_msgs.msg import Twist , TwistStamped , Vector3
from nav_msgs.msg import Odometry

################################### DETAILED OF SERVICE ########################################
##	OneVector3Stamped		: relative_${xy, z, yaw} , fix_$(z, yaw) , velocity_$(xy, z, yaw)
##	TwoStringVector3Stamped	: reset_target reset_velocity check_position 
##	ThreeOdometry			: get_state
################################################################################################

class ControlConnection:

	data_velocity = { 'x':0 , 'y':0 , 'z':0 , 'roll':0 , 'pitch':0 , 'yaw':0 }
	def __init__( self , name_id = "general_robot" ):

		self.header	= Header()
		self.vector = Vector3()
		self.state	= Odometry()
		self.twist_stamped = TwistStamped() 	

		self.set_name( name_id )

		self.save_state = [ 0 , 0 , 0 , 0 , 0 , 0 ]

		self.temp_state = [ 0 , 0 , 0 , 0 , 0 , 0 ]	

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
		self.vector.z = z
		result = self.service_one_vector( self.cl_relative_z , self.vector , "relative_z")
		return result

	def relative_yaw( self , yaw ):
		self.vector.z = yaw 
		result = self.service_one_vector( self.cl_relative_yaw , self.vector , "relative_yaw")
		return result

	def fix_z( self , z ):
		self.vector.z = z 
		result = self.service_one_vector( self.cl_fix_z , self.vector , "fix_z")
		return result

	def fix_yaw( self , yaw ):
		self.vector.z =yaw 
		result = self.service_one_vector( self.cl_fix_yaw , self.vector , "fix_yaw")
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
			result = False
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
	ch = ControlConnection("testing_robot")

	print("Welcome to testing control")

	ch.velocity_xy( 0.5 , 0.2 )
	print("Already send velocity_xy 0.5 : 0.2")
