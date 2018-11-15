#!/usr/bin/python2
#################################################################################################
####
####	FILE		: 1_mission_gate_1.py
####	Author		: Supasan Komonlit
####	Create on	: 2018 , Nov 15
####	Purpose		: For connection of Mission Planner with Control	
####
#################################################################################################

import rospy
import math
import time

from auv_controller		import AUVController
from vision_collector	import VisionCollector
from zeabus_planner.srv	import mission_result

from std_msgs.msg		import Bool , Int8 , String

class MissionGate:
	
	def __init__( self ):
		
		self.auv	= AUVController( "mission_gate" , True )

		print( "<=== MISSION GATE ===> Waitting Service of Gate" )
		rospy.wait_for_service( 'vision_gate' )
		print( "<=== MISSION GATE ===> Have Service of Gate OK ")

		self.vision	= VisionCollector( "gate" )

		self.rate	= rospy.Rate( 30 )
		self.mission_planner = rospy.Service('mission/planner' , mission_result , self.main_play)
		self.data_pub = rospy.Publisher('mission/echo_planner' , String , queue_size = 1 )

	def sleep( self , second):
		self.rate.sleep()
		self.auv.wait_time( second )

	def echo( self , message):
		self.data_pub.publish( String( message ) )

	def main_play( self , request ):
		self.sucess_mission = False
		# Mission Gate Will run start when have gate on image only situatuin
		self.vision.analysis_all( "gate" , "sevinar" , 5 )
		self.current_step = 0
		if( self.vision.have_object() ):
			self.current_step = 1
		else:
			print("<=== MISSION GATE ===> Don't found picture")
		
		if( self.current_step == 1 ):
			self.step_01()

		if( self.current_step == 2 ):
			self.step_02()

		if( self.current_step == 3 ):
			self.step_03()

		return Bool( self.sucess_mission ) , Int8( self.current_step )

	def step_01( self ):
		self.echo( "<========= MISSION GATE =========> MISSION GATE FIRST STEP " )
		count_unfound = 0
		while ( not rospy.is_shutdown() ):
			self.sleep( 0.1 )
			self.vision.analysis_all( "gate" , "sevinar" , 5 )
			self.vision.echo_data() # for print n_obj and center of x , y axis
			if( self.vision.have_object() ):
				count_unfound = 0
				if( self.vision.result['pos'] == 0 ):
					if( abs( self.vision.center_x() ) < 0.1 ):
						self.echo("Found Pos : 0 and Move Forward")
						self.auv.velocity( { 'x' : 0.4 } )
					elif( self.vision.center_x() < 0 ):
						self.echo("Found Pos ; 0 and Move Left")
						self.auv.velocity( { 'y' : 0.2 } )
					elif( self.vision.center_x() > 0 ):
						self.echo("Found Pos ; 0 and Move right")
						self.auv.velocity( { 'y' : -0.2 } )
				elif( self.vision.result['pos'] == -1 ):
					self.echo("Found Pos : -1 and Move Left")
					self.auv.velocity( {'y' : 0.2 } )
				elif( self.vision.result['pos'] == 1 ):
					self.echo("Found Pos : 1 and Move Right")
					self.auv.velocity( {'y' : 0.2 } )
				else: 
					self.echo("Warinig Don't have this condition")
			else:
				count_unfound += 1
				if( count_unfound > 5 ):
					self.echo("UNFOUND ROUND " + str( count_unfound ))
					self.current_step += 1
					self.echo("UnFound Where is gate go to dircetion")
				break

	def step_02( self ): # move to pass gat use time to estimate distance
		self.echo( "<========== MISSION GATE ==========> MISSION GATE SECOND STEP")
		limit_time = 4
		start_time = time.time()
		while( not rospy_is_shutdown() ):
			self.auv.velocity( {'x' : 1.5 } )
			self.sleep( 0.2 )
			diff_time = time.time() - start_time
			if( diff_time > limit_time ):
				self.current_step += 1
				break
			self.echo("Move forward now diff_time is " + str( diff_time ) )

	def step_03( self ): # spind and check have gate or not
		self.echo( "<========== MISSION GATE ==========> MISSION GATE THIRD STEP")
			
