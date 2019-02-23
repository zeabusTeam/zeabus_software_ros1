#!/usr/bin/python2
################################################################################################
####
####	FILE		: mission_gate.py
####	Author		: Supasan Komonlit
####	Create on	: 2019 , FEB 16
####	Purpose		: For connection of Mission Planner with Control	
####
################################################################################################
from __future__ import print_function
#===============>

import rospy
import time
import math

from control_handle		import ControlAUV
from vision_collector	import VisionCollector

from std_msgs.msg		import String

class MissionGate:

	def __init__( self ):

#===============> STANDARD SETUP
		self.ch = ControlAUV("gate_planner")

		self.rate = rospy.Rate( 30 )
		self.data_pub = rospy.Publisher("/mission/echo_planner" , String , queue_size = 1 )

#===============> SPECIFIC CLASS
		self.vision = VisionCollector( "gate" )
	
#===============> STANDARD FUNCTION
	def sleep( self , second ):
		self.rate.sleep()
		time.sleep( second )

	def echo( self , message ):
		self.data_pub.publish( String( message ) )	
