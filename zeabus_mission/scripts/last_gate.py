#!/usr/bin/python2
#################################################################################################
####
####	FILE		: mission_gate.py
####	Maintain	: Supasan Komonlit
####	Create on	: 2019 , MAR 09
####	Purpose		: For competition in SAUVC2019 about mission gate and vision don't have
####				  affect by reflect	
#################################################################################################
from __future__			import print_function
#===============>

import rospy
import math
import time 

from vision_collector		import VisionCollector

from standard_mission		import StandardMission

# Line 46 assign direction of left or right : negative is right postion is left

class MissionGate( StandardMission ):

	def __init__( self , name ):
		self.name = name 

		StandardMission.__init__( self, self.name , "/mission/gate" , self.callback )

		self.vision = VisionCollector( "gate" )

		self.echo( self.name , "Mission Gate Setup Finish")

		self.state = False

		self.start_time = time.time()

	def callback( self , request ):

		result = False

		self.state = request.data
		if( not self.ok_state() ):
			return False

		self.echo( self.name , "START MISSION GATE" )

		self.find_gate( -0.1 )

	def over_time( self , distance , time , setup = False )
		if( setup ):
			self.start_time = time.time()
			self.collect_state()
			return True
		else:
			diff_time = time.time() - self.start_time
			distance = self.distance()
			self.echo( self.name , "Distance : time are " + str( distance ) + 
					" : " + str( diff_time ) )
			if( distance < distance and diff_time < time):
				return True
			else:
				return False

	def find_gate( self , direction ):
		self.echo( self.name , "I will find gate" )
		self.over_time( 2 , 10 , True )
		while( self.ok_state() ): 

