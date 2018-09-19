#!/usr/bin/python2

import rospy, math , tf

import numpy

from coriolis import coriolis_rb , coriolis_a 
from hydrodynamic import hydrodynamic_q , hydrodynamic_i
from gravity_buoyancy import gravitational_buoyancy_force  

from nav_msgs.msg import Odometry
from geometry_msgs.msg import Twist

class feedback_linearlization:
	
	def __init__( self ):
		rospy.init_node('feedback_linearlization')
		
		self.position = [0 , 0 , 0 , 0 , 0 , 0]
		self.velocity = [0 , 0 , 0 , 0 , 0 , 0]

		rospy.Subscriber('/auv/state' , Odometry , self.listen_state)
		rospy.Subscriber('/feedback/cmd_vel', Twist , self.listen_cmd_vel)

		self.publish_thruster = rospy.Publisher('/cmd_vel' , Twist , queue_size = 1)

# ( m , i_xx , i_yy , i_zz ) argument of coriolis_rb
		self.c_rb = coriolis_rb(	45	,1.03		,0.038		,0.06		)

# ( a_1 , a_2 , a_3 , b_1 , b_2 , b_3 ) argument for input constant
		self.c_a = coriolis_a( 	[3 ,  0, 0]		, [3 , 0 , 0]			, [0 , 3 , 0],
								[0 , 0.001 , 0]		, [0 , 0 , 0.001]	, [0 , 0 , 0.001]
							 )

# ( x , y , z , k , m , n)
		self.d_i = hydrodynamic_i(	0.3,	0.3,	0.3,	0.001,	0.001,	0.001)
# ( x , y , z , k , m , n)
		self.d_q = hydrodynamic_q( 	85.25,	100,	173.975,	0.399,	8.925,	23.296)

# (w , b , x_cm , x_cb , y_cm , y_cb , z_cm , z_cb):
		self.g_n = gravitational_buoyancy_force( 441 , 390 , 0 , 0 , 0 , 0 , 0 , 0)

		rospy.spin()

	def listen_state( self , message ):
		euler_angular = tf.transformations.euler_from_quaternion(
							( message.pose.pose.orientation.x , message.pose.pose.orientation.y,
							  message.pose.pose.orientation.z , message.pose.pose.orientation.w)
																)
		self.position[0] = message.pose.pose.position.x
		self.position[1] = message.pose.pose.position.y
		self.position[2] = message.pose.pose.position.z
		self.position[3] = euler_angular[0]
		self.position[4] = euler_angular[1]
		self.position[5] = euler_angular[2]

		self.velocity[0] = message.twist.twist.linear.x
		self.velocity[1] = message.twist.twist.linear.y
		self.velocity[2] = message.twist.twist.linear.z
		self.velocity[3] = message.twist.twist.linear.x
		self.velocity[4] = message.twist.twist.linear.y
		self.velocity[5] = message.twist.twist.linear.z

	def listen_cmd_vel( self , message):		
		self.thruster= numpy.asmatrix( [
										[ self.velocity[0] ],
										[ self.velocity[1] ],
										[ self.velocity[2] ],
										[ self.velocity[3] ],
										[ self.velocity[4] ],
										[ self.velocity[5] ],
										])

		self.output = self.thruster - ( self.c_rb.input_value( self.velocity[0]
												,self.velocity[1]
												,self.velocity[2])
						+ self.c_a.input_value(  self.velocity[0]
												,self.velocity[1]
												,self.velocity[2]
												,self.velocity[3]
												,self.velocity[4]
												,self.velocity[5])) * self.thruster - (
								self.d_i.constant_matrix +
								self.d_q.input_value( 	self.velocity[0] ,
														self.velocity[1] ,
														self.velocity[2] ,
														self.velocity[3] ,
														self.velocity[4] ,
														self.velocity[5])
							) * self.thruster - self.g_n.input_value( 
								self.position[3] , self.position[4] , self.position[5])

		self.publish_thruster.publish( self.convert_output() )
	
	def convert_output( self ):
		data_twist = Twist()	
		data_twist.linear.x = self.output[0 , 0]	
		data_twist.linear.y = self.output[1 , 0]	
		data_twist.linear.z = self.output[2 , 0]	
		data_twist.angular.x = self.output[3 , 0]	
		data_twist.angular.y = self.output[4 , 0]	
		data_twist.angular.z = self.output[5 , 0]
		
		return data_twist	

if __name__=='__main__':
	
	filter_control = feedback_linearlization()
