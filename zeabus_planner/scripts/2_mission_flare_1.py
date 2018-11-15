#!/usr/bin/python2
#################################################################################################
####
####	FILE		: 2_mission_flare_1.py
####	Author		: Supasan Komonlit
####	Create on	: 2018 , Nov 15
####	Purpose		: For doing mission Flare	
####
#################################################################################################

import rospy
import math
import time 

from auv_controller		import AUVController
from vision_collector	import VisionCollector
from zeabus_planner.srv	import mission_result

from std_msgs.msg		import Bool , Int8 , String

# LINE 24 to 47 is standard code of mission part by Supasan Komonlit

class MissionFlare:
	
	def __init__( self):
		 
		self.auv	= AUVController( "mission_flare" , True )

		print( "<=== MISSION GATE ===> Waitting Service of flare" )
		rospy.wait_for_service( 'vision_flare' )
		print( "<=== MISSION GATE ===> Have Service of flare OK ")

		self.vision	= VisionCollector( "flare" )

		self.rate	= rospy.Rate( 30 )
		self.mission_planner = rospy.Service('mission/flare' , mission_result , self.main_play)
		self.data_pub = rospy.Publisher('mission/echo_planner' , String , queue_size = 1 )

	def sleep( self , second):
		self.rate.sleep()
		self.auv.wait_time( second )

	def echo( self , message):
		self.data_pub.publish( String( message ) )

	def main_play( self , request ):
		self.sucess_mission = False
		self.current_step = 0

		self.vision.analysis_all( "flare" , "far" , 5 )
		if( self.vision.have_object() ):
			self.current_step += 1
	
		#First step move to the flare let go and try to find in near mode 
		#	if found in near mode I will send to second step play in mear mode
		if( self.current_step == 1 )

		return Bool( self.sucess_mission ) , Int8( self.current_step )
