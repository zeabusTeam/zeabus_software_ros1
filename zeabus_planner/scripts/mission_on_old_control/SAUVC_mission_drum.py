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
from vision_collector	import VisionCollector
from zeabus_library.srv	import MissionResult

from std_msgs.msg		import Bool , Int8 , String

class MissionDrop:

	def __init__( self ):
		self.auv		= AUVController( "mission_drum" , True )
		
		self.rate		= rospy.Rate( 30 )
		self.data_pub	= rospy.Publisher('mission/echo_planner' , String , queue_size = 1)

		print("<=== MISSION DRUM ===> Waitting service of DRUM")
		rospy.wait_for_service('vision_drum')
		print("<=== MISSION DRUM ====> HAVE SERVICE OK")

		self.mission_planner = rospy.Service('mission/drum' , MissionResult , self.main_play)

		self.vision = VisionCollector( "drum" )
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
		self.echo( "<===== MISSION DRUM =====> START MISSION DRUM BY SUB PART")
		self.sucess_mission = False
		self.current_step = 0

		self.vision.analysis_all( "drum" , "drop" , 5 )
		if( self.vision.have_object() ):
			self.echo("Check have drum is ok")
			self.current_step += 1
		else:
			self.echo("Check Don't Have Drum is BAD")
		
		if( self.current_step == 1 ):
			self.step_01() # go down to drum

		if( self.current_step == 2 ):
			self.step_02() # try to make a little center

		return Bool( self.sucess_mission ) , Int8( self.current_step )

	def step_01( self ):
		self.echo( "<===== MISSION DRUM =====> Welcome to step_01 for move to center")
		self.auv.set_mode( 1 )
		start_time = time.time()
		diff_time = time.time() - start_time
		self.reset_request()
		result = True
		while( not rospy.is_shutdown() and diff_time < 20 ):
			self.request_velocity['z'] = -1.25
			diff_time = time.time() - start_time
			self.vision.analysis_all( "drum" , "drop" , 5)
			self.echo( self.vision.echo_specific() )
			if( self.vision.have_object() ):
				if( self.vision.result[ 'forward' ] and self.vision.result['backward']):
					self.request_velocity['x'] = 0
				elif( not self.vision.result['forward'] ) :
					self.request_velocity['x'] = -0.2
				elif( not self.vision.result['backward'] ):
					self.request_velocity['x'] = 0.25
				else:
					self.request_velocity['x'] = 0
				if( self.vision.result['cx_2'] > 0.8 ):
					self.request_velocity['y'] = -0.2
				elif( self.vision.result['cx_2'] < 0.6 ):
					self.request_velocity['y'] = 0.3
				else:
					self.request_velocity['y'] = 0
				self.echo( self.print_request() + " AND Diff time is " + str( diff_time ))
				self.auv.velocity( self.request_velocity )	
			else:
#				result = False
#				break
				self.auv.velocity( {'y' : 0.3 , 'x' : -0.1 })
				self.echo("Assign force by my self")	
		if( result ):
			self.current_step += 1

	def step_02( self ):
		self.echo( "<===== MISSION DRUM =====> Welcome to step_02 for move to center")
		start_time = time.time()
		diff_time = time.time() - start_time 
		while( not rospy.is_shutdown() and diff_time < 5):
			diff_time = time.time() - start_time
			self.auv.velocity( { 'x' : -0.2 , 'y' : 0.35 , 'z' : -1.3 })
			self.sleep( 0.1 )
			self.echo( "move x : -0.2 , y : 0.35 , z : -1.3 time is " + str( diff_time) )
		self.reset_request()
		if( not self.sucess_mission ):
			self.echo("DROP BALL TIME OUT")
			self.sucess_mission = True
			
if __name__=="__main__":
	mission_03 = MissionDrop()
	rospy.spin()
