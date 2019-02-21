#!/usr/bin/python2
#################################################################################################
####
####	FILE		: mission_gate.py
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

from standart_mission		import StandardMission

class MissionGate( StandardMission ):

	def __init__( self ):
		self.name = "GateMission"

		StandardMission.__init__( self , self.name , "/mission/gate" , self.callback )


	def callback( self ):
		print("Test name is " + str( self.name ) )

if __name__ == "__main__" :
	rospy.init_node("mission_gate")
	MG = MissionGate()
	rospy.spin()	
