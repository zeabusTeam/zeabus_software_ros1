#!/use/bin/python2

import rospy

from geometry_msgs.msg import Twist, Pose
from nav_msgs.msg import Odometry
from zeabus_controller.srv import *

class control_auv:

	data_velocity = {'x':0 , 'y':0 , 'z':0 , 'roll':0 , 'pitch':0 , 'yaw':0}
	origin_velocity = {'x':0 , 'y':0 , 'z':0 , 'roll':0 , 'pitch':0 , 'yaw':0}

	# parameter want_init_node is if you want to init node please True
	#			this make for protected situation init node overlap
	def __init__( self , want_init_node = False , name_node = ""):

		if want_init_node : 
			if name_node == "":
				rospy.init_node( "robot_control" )
			else:
				rospy.init_node( name_node )
		else:
			print( "I will node init node ")

		self.velocity_data = Twist()

		self.publisher_velocity = rospy.Publisher('/zeabus/cmd_vel' , Twist , queue_size = 10 )
		self.client_absolute_depth	= rospy.ServiceProxy('/fix_abs_depth',  fix_abs_depth )
		self.client_absolute_yaw	= rospy.ServiceProxy('/fix_abs_yaw' , fix_abs_yaw )
		self.client_relative_yaw	= rospy.ServiceProxy('/fix_rel_yaw' , fix_rel_yaw )

	def absolute_depth( self , depth ):
		try:
			result = self.client_absolute_depth( depth )
			print( "Finish client absolute depth : " + str(depth))
		except rospy.ServiceException, e:
			print "Service call failed: %s"%e

	def absolute_yaw( self , yaw ):
		try:
			result = self.client_absolute_yaw( yaw )
			print( "Finish client absolute yaw : " + str(yaw))
		except rospy.ServiceException, e:
			print "Service call failed: %s"%e

	def relative_yaw( self, yaw ):
		try:
			result = self.client_relative_yaw( yaw )
			print( "Finish client relative yaw : " + str(yaw))
		except rospy.ServiceException, e:
			print "Service call failed: %s"%e

	def velocity( self , **data_velocity ):
		print( "recevice ---------------> data_velocity is ")
		print data_velocity
		self.velocity_data.linear.x = data_velocity['x']
		self.velocity_data.linear.y = data_velocity['y']
		self.velocity_data.linear.z = data_velocity['z']
		self.velocity_data.angular.x = data_velocity['roll']
		self.velocity_data.angular.y = data_velocity['pitch']
		self.velocity_data.angular.z = data_velocity['yaw']

		self.publisher_velocity.publish( self.velocity_data )
		data_velocity = origin_velocity
		print( "reset ----------> data_velocity is ")
		print data_velocity
