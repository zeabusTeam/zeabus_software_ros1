#!/usr/bin/python2

import rospy
import math
import time

from auv_controller		import auv_controller
from vision_collector	import vision_collector
from zeabus_planner.srv	import mission_result

from std_msgs.msg			import Bool , Int8

class mission_gripper:

	def __init__( self ):

		self.auv	= auv_controller( "mission_gripper" , True )
	
		print("<=== MISSION GRIPPER ===> Checking service of DRUM ")
		rospy.wait_for_service('vision_drum')
		print("<=== MISSION GRIPPER ===> Have Service OK")
		
		self.vision	= vision_collector( "drum" )
		self.past_action = { 'direction' : None , 'value' : None }

		self.rate = rospy.Rate( 30 )
		self.mission_planner = rospy.Service('mission/gripper' , mission_result , self.main_play)

	def main_play( self , request ):
		self.sucess_mission = False
		# first step is move in x axis of robot for move center y axis of picture
		# second step is move in y axis of robot to disappear of ball
		# third step is quickly Go Dowm and make ball always disappear
		self.step = 1 # have 3 step to play
		
		self.start_depth = -0.8 # this variable is important that have affect on step_03
		self.auv.absolute_z( -0.8 )
		
		self.step_01() 

		if( self.step == 2 ):
			self.step_02()
		else:
			print("<=== MISSION GRIPPER ===> MISSION GRIPPER ABORTED!!!")

		if( self.step== 3 ):
			self.step_03()
		else:
			print("<=== MISSION GRIPPER ===> MISSION GRIPPER ABORTED!!!")

		return Bool( self.sucess_mission ) , Int8( self.step )

	def collect_action( self , direction , value ):
		print( "<=== MISSION GRIPPER ===> Collect action " + direction + " : " + str( value ) )
		self.past_action['direction'] = direction
		self.past_action['value'] = value

	def check_picture_y( self , center_y ):
		max_point = -0.2
		min_point = -0.7
		if( min_point < center_y and center_y > max_point ):
			return True
		else:
			return False

	def sleep( self , time ):
		self.auv.wait_time( time )
		self.rate.sleep()

	def step_01( self ):
		print("<=== MISSION GRIPPER ===> step_01 move forward or backward")
		limit_abort_mission = 5
		count_abort_mission = 0
		while( not rospy.is_shutdown() ):
			self.sleep( 0.2 )
			self.vision.action_analysis( 5 , "drum" , "pick" )
			self.vision.echo_data()
			if( self.have_object ):	
				count_abort_mission = 0
				if( self.check_picture_y( self.vision.center_y() ) ):
					self.step += 1
					break
				elif( self.vision.center_y() < -0.7 ):
					self.auv.velocity( 'x' , 0.4 )
					self.collect_action( "x" , 0.4 )
				else:
					self.auv.velocity( 'x' , -0.3 )
					self.collect_action( "x" , -0.3 )
			else:
				count_abort_mission = 0
				if( count_abort_mission > limit_abort_mission ):
					break
				else:
					self.auv.velocity( self.past_action['direction'] , self.past_action['value'])

	def step_02( self ):
		print("<=== MISSION GRIPPER ===> step_02 move right or left")
		limit_next_step = 5
		count_next_step = 0
		while( not rospy.is_shutdown() ):
			self.sleep( 0.2 )
			self.vision.action_analysis( 5 ,  "drum" , "pick" )
			self.vision.echo_data()
			if( self.have_object ):
				if( self.vision.center_y() > 0 ):
					self.auv.velocity( 'x' , -0.4 ) 
					self.collect_action( "x" , -0.4 )
				else:
					self.auv.velocity( "y" , 0.5 )
					self.collect_action( "y" )
			else:
				count_next_step = 0
				if( count_next_step > limit_next_step ):
					self.step += 1
					break

	def step_03( self ):
		print("<=== MISSION GRIPPER ===> step_03 last step " )
		limit_time = 30
		self.start_time = time.time()
		self.auv.absolute_z( self.start_depth - 1.2 )
		while( not rospy.is_shutdown() ):
			diff_time = time.time() - self.start_time
			print( "Time to run this mission " + str( diff_time ))
			self.rate.sleep()
			self.vision.action_analysis( 5 , "drum" , "pick" )
			self.vision.echo_data()
			if( self.have_object ):
				if( self.vision.center_x() > -0.1 ):
					self.auv.velocity( 'y' , 0.5 )
				else:
					self.auv.velocity( 'x' , -0.4 )
			if( diff_time > limit_time ):
				self.sucess_mission = True
				break
	
			
if __name__=="__main__":
	mission_04 = mission_gripper()	
	rospy.spin()
