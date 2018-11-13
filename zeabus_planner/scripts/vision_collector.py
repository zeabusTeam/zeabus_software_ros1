#!/usr/bin/python2

#################################################################################################
####	FILE		: vision_collector.py
####	Maintain	: Supasan Komonlit
####	Create on	: 2019 , Nov 14
####	Purpose		: For manage about analysis vision data
#################################################################################################

import rospy
import math

#service file of vision must send 2 data to use 
from zeabus_vision.srv import *
from zeabus_vision.msg import *

from std_msgs.msg import String

class vision_collector:

	def __init__( self , topic_service , type_service ):

		print( "MESSAGE vision_type_center" , topic_service , type_service )
		self.request_data = rospy.ServiceProxy( topic_service , type_service )
		print( "MESSAGE vision_type_center" , topic_service , type_service )

		self.result_vision	= {	'n_obj'		: 0		, 'area'		: 0
								, 'cx'		: 0		, 'cy'			: 0
								, 'cx_1'	: 0		, 'cx_2'		: 0
								, 'cy_1'	: 0		, 'cy_2'		: 0
								, 'left'	: False	, 'right'		: False
								, 'forward'	: False	, 'backward'	: False
								, 'pos'		: 0 }

		self.collect_vision	= {	'n_obj'		: 0		, 'area'		: 0
								, 'cx'		: 0		, 'cy'			: 0
								, 'cx_1'	: 0		, 'cx_2'		: 0
								, 'cy_1'	: 0		, 'cy_2'		: 0
								, 'left'	: False	, 'right'		: False
								, 'forward'	: False	, 'backward'	: False
								, 'pos'		: 0 }

		self.data_vision	= {	'n_obj'		: 0		, 'area'		: 0
								, 'cx'		: 0		, 'cy'			: 0
								, 'cx_1'	: 0		, 'cx_2'		: 0
								, 'cy_1'	: 0		, 'cy_2'		: 0
								, 'left'	: False	, 'right'		: False
								, 'forward'	: False	, 'backward'	: False
								, 'pos'		: 0 }

	def action_analysis( self , task , request , amont ):
			

		

#################################################################################################
####
####	THIS SECTION IS INDIVIDUAL FOR EACH SERVICE OF ZEABUS TEAM VISION PART
####		AVAILABLE FOR USE
####
#################################################################################################
	
##=================================> SERVICE VISION DRUM <=======================================
	def drum_set_up( self ):
		self.individual_data	= self.drum_data
		self.center_x			= self.drum_center_x
		self.center_y			= self.drum_center_y
		self.can_sum = [ 'cx' , 'cy' , 'area' ]
		self.not_sum = [ 'left' , 'right' , 'forward' , 'backward']

##=================================> SERVICE VISION GATE <=======================================
	def gate_set_up( self ):
		self.individual_data	= self.gate_data
		self.center_x			= self.gate_center_x
		self.center_y			= self.gate_center_y
		self.can_sum = [ 'cx_1' , 'cx_2' , 'cy_1' , 'cy_2' , 'area' , 'pos' ]
		self.not_sum = []

##================================> SERVICE VISION FLARE <=======================================
	def flare_set_up( self ):
		self.individual_data	= self.gate_data
		self.center_x			= self.gate_center_x
		self.center_y			= self.gate_center_y
		self.can_sum = [ 'cx' , 'cy' , 'area' ]
		self.not_sum = []

