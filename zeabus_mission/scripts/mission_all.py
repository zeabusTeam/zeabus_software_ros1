#!/usr/bin/python2
#################################################################################################
####
####	FILE		: mission_all.py
####	Maintain	: Supasan Komonlit
####	Create on	: 2019 , FEB 21
####	Purpose		: For competition in SAUVC2019
####
#################################################################################################
from __future__			import print_function
#===============>

import rospy
import math
import time

from vision_collector		import VisionCollector
from standard_mission		import StandardMission 

class MissionAll( StandardMission ):
	
	def __init__( self , name ):
		self.name = name

		StandardMission.__init__( self , self.name , "/mission/all" , self.callback )

		self.gate = VisionCollector("gate")
		self.drum = VisionCollector("drum")
		self.flare = VisionCollector("flare")

	def callback( self ):

if __name__ == "__main__":
	rospy.init_node("mission_all")
	MA =
