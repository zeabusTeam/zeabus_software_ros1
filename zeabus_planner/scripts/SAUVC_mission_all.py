#/usr/bin/python2
#################################################################################################
####
####	FILE		: SAUVC_all_mission.py
####	Author		: Supasan Komonlit
####	Create on	: 2018 , Nov 20
####	Purpose		: Last Code for do all mission in SAUVC 2019
####
#################################################################################################

import rospy
import math
import time

from auv_controller		import AUVController
from vision_collector	import VisionCollector
from zeabus_library.srv	import MissionResult

from std_msgs.msg		import Bool, Int8 , String

class SAUVC2019:

	def __inti__( self ):

		self.auv	=	
