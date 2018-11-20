#!/usr/bin/python2
#################################################################################################
####
####	FILE		: SAUVC_mission_gripper.py
####	Author		: Supasan Komonlit
####	Create on	: 2018 , Nov 18
####	Purpose		: For make mission pickball this will control robot z by code	
####
#################################################################################################

import rospy
import math
import time

from auv_controller		import AUVController
from vision_collector	import VisionCollector
from zeabus_library.srv	import MissionResult

from std_msgs.msg		import Bool , Int8 , String

class MissionGripper:

	def __init__( self ):

		self.auv	= AUVController( "mission_gripper" , True )	

		self.rate = rospy.Rate( 30 )
		self.data_pub = rospy.Publisher('mission/echo_planner' , String , queue_size = 1 )

		print("<=== MISSION GRIPPER ===> Waitting service of DRUM ")
		rospy.wait_for_service('vision_drum')
		print("<=== MISSION GRIPPER ===> Have Service OK")

		self.mission_planner = rospy.Service('mission/gripper' , MissionResult , self.main_play)

		self.vision	= VisionCollector( "drum" )	
		print( "<========== FINISH SETUP MISSION GRIPPER ==========>")

	def sleep( self , second):
		self.rate.sleep()
		self.auv.wait_time( second )

	def echo( self , message):
		self.data_pub.publish( String( message ) )

	def main_play( self , request ):
		self.sucess_mission = False
		self.current_step = 1 # have 3 step to play
		self.request_velocity = { 'x' : 0 , 'y' : 0 , 'z':0}
		
		if( self.current_step == 1 ):
			self.step_01()
		
		if( self.current_step == 2 ):
			self.step_02()
		else:
			print("<=== MISSION GRIPPER ===> MISSION GRIPPER ABORTED!!!")

		if( self.current_step == 3 ):
			self.step_03()

		return Bool( self.sucess_mission ) , Int8( self.current_step )

	def reset_request( self ):
		self.request_velocity = {'x': 0 , 'y' : 0 , 'z' : 0}

	def check_depth( self , target ):
		return self.auv.receive_target('z')[0] < target

	def print_request( self ):
		return ( "VELOCITY x :" + str( self.request_velocity['x'] ) +
				"==== y :" + str( self.request_velocity['y']) +
				"==== z :" + str( self.request_velocity['z']))

	def check_range( self , axis , value , start , length , velocity):
		if( start <= value and value <= start+length ):
			self.request_velocity[ axis ] = 0 
		elif( value < start):
			self.request_velocity[ axis ] = -1 * velocity
		else:
			self.request_velocity[ axis ] = velocity

	def step_01( self ):
		self.echo("<=== MISSION GRIPPER ===> STEP01 FIND BALL")
		result = self.find_ball()
		if( result ):
			self.current_step += 1
		

	def step_02( self ):
		self.echo("<=== MISSION GRiPPER ===> STEP02 PICK BALL UP")
		start_time = time.time()
		diff_time = time.time() - start_time
		diff_up_time = 0
		start_up_time = 0
		ever_unfound = False
		self.auv.set_mode( 2 )
		self.reset_request()
		while( not rospy.is_shutdown() and diff_time < 15 ):
			self.echo( "Time capture is " + str( diff_time ))
			self.sleep( 0.08 )
			self.vision.analysis_all( 'drum' , "pick" , 5 )
			if( self.vision.have_object() ):
				start_time = time.time()
				self.echo( self.vision.echo() )
				if( ever_unfound ):
					self.echo( "Ever_unfound Mode")
					start_up_time = time.time()
					diff_up_time = time.time() -start_up_time 
					while( not rospy.is_shutdown() and diff_up_time < 4 ):
						self.auv.velocity( { 'z' : -0.8 , 'y' : -0.12 })
						diff_up_time = time.time() - start_up_time
				self.request_velocity['z'] = -1.2
				self.check_range( "x" , self.vision.center_y() , -0.32 , 0.16 , 0.15 )
				self.check_range( "y" , self.vision.center_x() , -0.68 , -0.42 , -0.18 )
				self.echo( "FOUND OBJECT " + self.print_request() )
			else:
				self.request_velocity['z'] = -1.6
				self.request_velocity['x'] = -0.13
				self.request_velocity['y'] = 0.22
				self.echo( "NO" + self.print_request() )
			self.auv.velocity( self.request_velocity )
			diff_time = time.time() - start_time
			if( diff_time > 7 ):
				self.ever_unfound = True	
		self.reset_request()
		self.echo( "<=== MISSION GRIPPER ===> Time out go up")
		while( not rospy.is_shutdown() and not self.check_depth( -0.6 ) ):
			self.sleep( 0.2 )
			self.request_velocity['z'] = -0.8
			self.auv.velocity( self.request_velocity )			 
		self.current_step += 1

	def step_03( self ):
		self.echo( "<=== MISSION GRIPPER ===> STEP THIRD CHECK BALL")
		# Check Have Ball All not
		result = True
		while( not rospy.is_shutdown() and result ):
			result = self.find_ball()	
		self.echo( "Finish Wd don't found ball")
		# FInish Check
		self.current_step += 1
		self.sucess_mission = True

	def find_ball( self ):
		start_time = time.time()
		diff_time = time.time() - start_time
		limit_time = 20
		self.echo( "<=== MISSION GRIPPER ===> MODE FIND BALL")
		# split to 4 step to find around drum
		self.auv.set_mode( 1 )	
		self.auv.absolute_z( -3.7 )
		self.echo( "Will survey from left of drum")
		same_direction = True
		#####################################################################################
		start_time = time.time()
		while( not rospy.is_shutdown() and same_direction and diff_time < limit_time):
			self.sleep( 0.08 )
			self.vision.analysis_all( "drum" , "drop" , 5)
			if( not self.vision.have_object() ):
				self.echo( "Warning vision don't found object")
				return False 
			self.echo( self.vision.echo_specific() )
			if( self.vision.result['cx_1'] < 0 ):
				self.request_velocity['y'] = 0.3
			elif( self.vision.result['cx_1'] > 0):
				self.request_velocity['y'] = -0.2
			else:
				self.request_velocity['y'] = 0
				same_direction = False
			if( abs(self.vision.center_y() ) < 0.3 ):
				self.request_velocity['x'] = 0
			elif( self.vision.center_y() < 0 )
				self.request_velocity['x'] = -0.2
			else:
				self.request_velocity['x'] = 0.2
			self.vision.analysis_all( "drum" , "pick")
			if( self.vision.have_object() ):
				self.echo( "GRIPPER FOUND BALL")
				return True
			else:
				self.echo( "DON'T HAVE BALL")
			diff_time = time.time() - start_time

		self.echo( "We wil survey down")
		start_time = time.time()
		diff_time = time.time() - start_time
		while( not rospy.is_shutdown() and diff_time < 4 ):
			self.auv.velocity( { 'y' : -0.17 } )
			self.vision.analysis_all( 'drum' , 'pick' , 5 )
			if( self.vision.have_object() ):
				self.echo( "GRIPPER FOUND BALL")
				return True
			self.sleep( 0.1 )
		same_direction = True
		#####################################################################################
		start_time = time.time()
		while( not rospy.is_shutdown() and same_direction and diff_time < limit_time):
			self.sleep( 0.08 )
			self.vision.analysis_all( "drum" , "drop" , 5 )
			if( not self.vision.have_object() ):
				self.echo( "Warinig vision don't found object")
				return False
			self.echo( self.vision.echo_specific() )
			if( self.vision.result['cy_1'] < -0.7 ):
				self.request_velocity['x'] = -0.2 
			elif( self.vision.result['cy_1'] > -0.3 ):
				self.request_velocity['x'] = 0.2
			else:
				self.request_velocity['x'] = 0
				same_direction = False
			if( abs(self.vision.center_x() < 0.3 ) )
				self.request_velocity['y'] = 0
			elif( self.vision.center_x() < 0 ):
				self.request_velocity['y'] = 0.22
			else:
				self.request_velocity['y'] = -0.22
			self.vision.analysis_all( "drum" , "pick" , 5 )
			if( self.vision.have_object() ):
				self.echo( "GRIPPER FOUND BALL")
				return True
			else:
				self.echo( "DON'T HAVE BALL")
			diff_time = time.time() - start_time

		self.echo( "We will survey right")
		start_time = time.time()
		diff_time = time.time() - start_time
		while( not rospy.is_shutdown() and diff_time < 4 ):
			self.auv.velocity( { 'x' : 0.2 } )
			self.vision.analysis_all( 'drum' , 'pick' , 5 )
			if( self.vision.have_object() ):
				self.echo( "GRIPPER FOUND BALL")
				return True
			self.sleep( 0.1 )
		same_direction = True
		#####################################################################################
		start_time = time.time()
		while( not rospy.is_shutdown() and same_direction and diff_time < limit_time):
			self.sleep( 0.08 )
			self.vision.analysis_all( "drum" , "drop" , 5 )
			if( not self.vision.have_object() ):
				self.echo( "Warinig vision don't found object")
				return False
			self.echo( self.vision.echo_specific() )
			if( self.vision.result['cx_2'] < 0.2 ):
				self.request_velocity['y'] = 0.23 
			elif( self.vision.result['cx_2'] > 0.7 ):
				self.request_velocity['y'] = -0.2
			else:
				self.request_velocity['y'] = 0
				same_direction = False
			if( abs(self.vision.center_y() < 0.3 ) )
				self.request_velocity['x'] = 0
			elif( self.vision.center_x() < 0 ):
				self.request_velocity['x'] = 0.22
			else:
				self.request_velocity['x'] = -0.22
			self.vision.analysis_all( "drum" , "pick" , 5 )
			if( self.vision.have_object() ):
				self.echo( "GRIPPER FOUND BALL")
				return True
			else:
				self.echo( "DON'T HAVE BALL")
			diff_time = time.time() - start_time

		self.echo( "We will survey up")
		start_time = time.time()
		diff_time = time.time() - start_time
		while( not rospy.is_shutdown() and diff_time < 7 ):
			self.auv.velocity( { 'y' : 0.2 } )
			self.vision.analysis_all( 'drum' , 'pick' , 5 )
			if( self.vision.have_object() ):
				self.echo( "GRIPPER FOUND BALL")
				return True
			self.sleep( 0.1 )
		same_direction = True
		#####################################################################################
		start_time = time.time()
		while( not rospy.is_shutdown() and same_direction and diff_time < limit_time):
			self.sleep( 0.08 )
			self.vision.analysis_all( "drum" , "drop" , 5 )
			if( not self.vision.have_object() ):
				self.echo( "Warinig vision don't found object")
				return False
			self.echo( self.vision.echo_specific() )
			if( self.vision.result['cy_2'] < 0.4 ):
				self.request_velocity['x'] = -0.22 
			elif( self.vision.result['cy_2'] > 0.7 ):
				self.request_velocity['x'] = -0.2
			else:
				self.request_velocity['x'] = 0
				same_direction = False
			if( abs(self.vision.center_x() < 0.3 ) )
				self.request_velocity['y'] = 0
			elif( self.vision.center_x() < 0 ):
				self.request_velocity['y'] = 0.22
			else:
				self.request_velocity['y'] = -0.22
			self.vision.analysis_all( "drum" , "pick" , 5 )
			if( self.vision.have_object() ):
				self.echo( "GRIPPER FOUND BALL")
				return True
			else:
				self.echo( "DON'T HAVE BALL")
			diff_time = time.time() - start_time
		return False

if __name__=="__main__":
	mission_04 = MissionGripper()	
	rospy.spin()
