import rospy
import math

from geometry_msgs.msg import Twist, Pose
from nav_msgs.msg import Odometry
from zeabus_controller.srv import *
from std_msgs.msg import String

class control_auv:

	data_velocity = {'x':0 , 'y':0 , 'z':0 , 'roll':0 , 'pitch':0 , 'yaw':0}
	origin_velocity = {'x':0 , 'y':0 , 'z':0 , 'roll':0 , 'pitch':0 , 'yaw':0}

	# parameter want_init_node is if you want to init node please True
	#			this make for protected situation init node overlap
	def __init__( self , name_node = "" , want_init_node = False):

		self.collect_x = 0
		self.collect_y = 0

		self.name = String()
		self.message = String()
		if want_init_node : 
			if name_node == "":
				rospy.init_node( "robot_control" )
				self.set_name("robot_control")
			else:
				rospy.init_node( name_node )
				self.set_name( name_node)
		else:
			print( "I will node init node ")

		print(type(self.name))

		self.velocity_data = Twist()

		self.publisher_velocity = rospy.Publisher('/zeabus/cmd_vel' , Twist , queue_size = 10 )
		self.client_absolute_depth	= rospy.ServiceProxy('/fix_abs_depth' ,  fix_abs_depth )
		self.client_absolute_yaw	= rospy.ServiceProxy('/fix_abs_yaw' , fix_abs_yaw )
		self.client_relative_yaw	= rospy.ServiceProxy('/fix_rel_yaw' , fix_abs_yaw )
		self.client_target_position = rospy.ServiceProxy('/know_target' , target_service )

		self.message.data = "xy"
		receive_data = self.client_target_position( self.message )
		self.collect_x = receive_data.target_01
		self.collect_y = receive_data.target_02

	def set_name( self , name ):
		self.name.data = name

	def absolute_depth( self , depth ):
		try:
			result = self.client_absolute_depth( depth , self.name)
			print( "Finish client absolute depth : " + str(depth))
		except rospy.ServiceException, e:
			print "Service call failed: %s"%e

	def absolute_yaw( self , yaw ):
		try:
			result = self.client_absolute_yaw( yaw , self.name )
			print( "Finish client absolute yaw : " + str(yaw))
		except rospy.ServiceException, e:
			print "Service call failed: %s"%e

	def relative_yaw( self, yaw ):
		try:
			result = self.client_relative_yaw( yaw , self.name )
			print( "Finish client relative yaw : " + str(yaw))
		except rospy.ServiceException, e:
			print "Service call failed: %s"%e

	def velocity( self , **data_velocity ):
		try:
			self.velocity_data.linear.x = data_velocity['x']
		except:
			self.velocity_data.linear.x = 0

		try:
			self.velocity_data.linear.y = data_velocity['y']
		except:
			self.velocity_data.linear.y = 0

		try:
			self.velocity_data.linear.z = data_velocity['z']
		except:
			self.velocity_data.linear.z = 0

		try:
			self.velocity_data.angular.x = data_velocity['roll']
		except:
			self.velocity_data.angular.x = 0

		try:
			self.velocity_data.angular.y = data_velocity['pitch']
		except:
			self.velocity_data.angular.y = 0

		try:
			self.velocity_data.angular.z = data_velocity['yaw']
		except:
			self.velocity_data.angular.z = 0

		self.publisher_velocity.publish( self.velocity_data )

	def collect_position( self ):
		self.message.data = "xy"
		receive_data = self.client_target_position( self.message )
		self.collect_x = receive_data.target_01
		self.collect_y = receive_data.target_02

	def calculate_distance( self ):
		self.message.data = "xy"
		receive_data = self.client_target_position( self.message )
		return math.sqrt(
						math.pow( receive_data.target_01 - self.collect_x , 2 ) +
						math.pow( receive_data.target_02 - self.collect_y , 2 )
					)
