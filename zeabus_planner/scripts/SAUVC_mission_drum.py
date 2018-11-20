#!/usr/bin/python2
#################################################################################################
####
####	FILE		: SAUVC_mission_drum.py
####	Author		: Supasan Komonlit
####	Create on	: 2018 , Nov 20
####	Purpose		: For make mission release ball and target to release middle of drum	
####
#################################################################################################

import rospy
import math
import time

from auv_controller		import AUVController
from vision_collector	import VisionCollecotr
from zeabus_library.srv	import MissionResult

from std_msgs.msg		import Bool , Int8 , String

class MissionDrop:

	def __init__( self ):
		self.auv		= AUVController( "mission_drum" , True )
		
		self.rate		= rospy.Rate( 30 )
		self.data_pub	= rospy.Publisher('missin/echo_planner' , String , queue_size = 1)

		print("<=== MISSION DRUM ===> Waitting service of DRUM")
		rospy.wait_for_service('vision_drum')
		print("<=== MISSION DRUM ====> HAVE SERVICE OK")

		self.mission_planner = rospy.Service('mission/gripper' , MissionResult , self.main_play)

		self.vision = VisionCollecotr( "drum" )
		print("<========== FINISH SETUP MISSION DRUM ==========>")
		self.reset_request()

	def sleep( self , second ):
		self.rate.sleep()
		self.auv.wait_time( second )

	def echo( self , message ):
		self.data_pub.publish( String( message ) )

	def reset_request( self ):
		self.request_velocity = { 'x' : 0 , 'y': 0 , 'z':0 }

	def print_request( self ):
		return( "REQUEST VALOCITY X : " + str( self.request_velocity['x'] ) +
				"==== Y : " + str( self.request_velocity['y'] ) +
				"==== Z : " + str( self.request_velocity['z'] )
		)

	def main_play( self , request ):
		self.sucess_mission = False
		self.current_step = 0

		self.vision.analysis_all( "drum" , "drop" , 5 )
		if( self.vision.have_object() ):
			self.current_step += 1
		
		if( self.current_step == 1 ):
			self.step_01() # go down to drum

		if( self.current_step == 2 ):
			self.step_02() # try to make a little center

		return Bool( self.sucess_mission ) , Int8( self.current_step )

	def step_01( self ):
		self.auv.set_mode( 1 )
		start_time = time.time()
		diff_time = time.time() - start_time
		self.reset_request()
		while( not rospy.is_shutdown() and diff_time < 20 ):
			self.request_velocity['z'] = -1.25
			diff_time = time.time() - start_time
			self.vision.analysis_all( "drum" , "drop" , 5)
			if( self.vision.have_object() ):
				if( self.vision.result[ 'forward' ] and self.vision.result['backward']):
					self.request_velocity['x'] = 0
				elif( self.vision.result['forward'] ) :
					self.request_velocity['x'] = -0.2
				elif( self.vision.result['backward'] )
					self.request_velocity['x'] = 0.2
				else:
					self.request_velocity['x'] = 0
				if( self.vision.result['cx_2'] > 0.9 ):
					self.request_velocity['y'] = 0.2
				elif( self.vision.result['cx_2'] < 0.3 ):
					self.request_velocity['y'] = -0.2
				else:
					self.request_velocity['y'] = 0
				self.echo( self.print_request() )	
			else:
				break
		self.vision.analysis_all( "drum" , "drop" , 5 )
		if( self.vision.have_object() ):
			self.current_step += 1

	def step_02( self ):
		self.auv.set_mode( 2 )
		start_time = time.time()
		diff_time = time.time() - start_time 
		while( not rospy.is_shutdown() and diff_time < 30):
			self.request_velocity['z'] = -1.3
			diff_time = time.time() - start_time
			self.vision.analysis_all( "drum" , "drop" , 5 )
			if( self.vision.have_object() ):	
				if( self.vision.result['cx_2'] > 0.8 ):
					self.request_velocity['y'] = -0.4
				elif( self.vision.result['cx_2'] < 0.5 ):
					self.request_velocity['y'] = 0.4
				else:
					self.echo("DROP BALL PLEASE")
					self.sucess_mission = True
					break
				self.echo( self.print_request() )
			else:
				self.echo( "<====================== DON\'T FOUND DRUM =======================>")
		if( not self.sucess_mission ):
			self.echo("DROP BALL TIME OUT")
			self.sucess_mission = True
			

if __name__=="__main__":
	mission_03 = MissionDrop()
	rospy.spin
