#!/usr/bin/python2
#################################################################################################
####
####	FILE		: 0_mission_navigator_1.py
####	Author		: Supasan Komonlit
####	Create on	: 2018 , Nov 16
####	Purpose		: For make mission gate	
####
#################################################################################################

import rospy
import math
import time

from auv_controller		import AUVController
from vision_collector	import VisionCollector
from zeabus_planner.srv	import mission_result

from std_msgs.msg		import Bool , String , Int8

class MissionNavigator:
	
	def __init__():
		
		self.auv	= AUVController( "mission_navigator" , True )
	
	

if __name__=="__main__":
	naviagator = MissionNavigator()
