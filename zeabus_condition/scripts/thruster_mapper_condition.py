#!/usr/bin/env python

import rospy # import standard library of ros system for python language
from hg_ros_pololu.msg	import Pwm
from geometry_msgs.msg	import Twist

from numpy import *
import math

import LookUpPWM_2018 as look_up_pwm_table

class thruster_mapper:

	def __init__( self , type_subscriber = 1):
		rospy.init_node("thruster_mapper")

		self.pwm_publisher	= rospy.Publisher('/pwm' , Pwm , queue_size = 1 )

		if( type_subscriber == 1):
			rospy.Subscriber("/cmd_vel" , Twist , self.listen_force_callback);
		else:
			rospy.Subscriber("/cmd_vel" , Twist , self.listen_force_callback);

# direction for 8 thruster x y z
		self.direction_xyz	= array([	
			[ 0								, 0								, 1		]
		,	[ 0								, 0								, 1		]
		,	[ 0								, 0								, 1		]
		,	[ 0								, 0								, 1		]
		,	[ 1/math.sqrt(2)				, -1/math.sqrt(2)				, 0		]
		,	[ 1/math.sqrt(2)				, 1/math.sqrt(2)				, 0		]
		,	[ -1/math.sqrt(2)				, -1/math.sqrt(2)				, 0		]
		,	[ -1/math.sqrt(2)				, 1/math.sqrt(2)				, 0		]
		])

		self.direction_angular = array([
			[ 1		, 1		, 0		]
		,	[ -1	, 1		, 0		]
		,	[ 1		, -1	, 0		]
		,	[ -1	, -1	, 0		]
		,	[ 0		, 0		, -1	]
		,	[ 0		, 0		, 1		]
		,	[ 0		, 0		, 1		]
		,	[ 0		, 0		, -1	]
		])

		self.max_force	= array([	[ -0.63 , 0.95 ]
								,	[ -0.63 , 0.95 ]
								,	[ -0.63 , 0.95 ]
								,	[ -0.63 , 0.95 ]
								,	[ -1.25 , 1.67 ] 
								,	[ -1.25 , 1.67 ] 
								,	[ -1.25 , 1.67 ] 
								,	[ -1.25 , 1.67 ] ])
		
		self.min_force = array([	[ -0.048, 0.045]
								,	[ -0.048, 0.045]
								,	[ -0.048, 0.045]
								,	[ -0.048, 0.045]
								,	[ -0.01	, 0.01]
								,	[ -0.01	, 0.01]
								,	[ -0.01	, 0.01]
								,	[ -0.01	, 0.01]])

	def listen_force_callback( self , message ):
		pwm_command		= Pwm()
		pwm_command.pwm	= [1500]*8

		target_force	= array([	message.linear.x	,	message.linear.y
								,	message.linear.z	,	message.angular.x
								,	message.angular.y	,	message.angular.z
						])

		print( "==========> target_force <==========" )
		print( target_force )
		
		using_force		= array([ 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0])

		split_force_yaw = target_force[5]/4
		for run in range( 4 , 8 ):
			using_force[run] = split_force_yaw * self.direction_angular[run][2]

		print( "==========> using_force " + str( split_force_yaw ) + " <==========" )
		print( using_force )

		# consider condtion for calculate axis of x , y
		condition_direction = 0
		if( abs(message.linear.x) > abs( message.linear.y) ):
			condition_direction = 1 # interest only x axis
		elif( abs(message.linear.y) > 0 ):
			condition_direction = 2 # interest only y axis
		
		if( condition_direction == 1): # thruster for force only x2
			split_force_x = target_force[0]/4
			for run in range( 4 , 8 ):
				using_force[run] += split_force_x / self.direction_xyz[run][0]
			print( "==========> using_force x " + str( split_force_x ) + "<==========" )
			print( using_force )

		elif( condition_direction == 2):
			split_force_y = target_force[1]/4 
			for run in range( 4 , 8 ):
				using_force[run] += split_force_y / self.direction_xyz[run][1]
			print( "==========> using_force y " + str( split_force_y ) + "<==========" )
			print( using_force )

		else:
			print("<--------------- don't calculate for x y z -------------->")
			
			
				

if __name__=="__main__":
	thruster = thruster_mapper( 1 )
	rospy.spin()
