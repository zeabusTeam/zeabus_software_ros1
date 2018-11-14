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

	def __init__( self , mission_vision ):

		if( mission_vision == 'gate'):

		elif( mission_vision == 'flare' ):
	
		else( mission_vision == 'drum' ):

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
		count_found = 0
		count_unfound = 0 
		self.reset_collect_vision()
		while( count_found < amont and count_unfound < amont ):
			self.individual_data( task , request )
			if( self.data_vision['n_obj'] >= 1 ):
				count_found += 1
				for run in self.can_sum:
					self.collect_vision[ run ] += self.data_vision[ run ]
			else:
				count_unfound += 1
		if( count_found == amont ):
			self.result_vision['n_obj'] = 1
			for run in self.can_sum:
				self.result_vision[run] = self.collect_vision[run] / amont
			for run in self.not_sum:
				self.result_vision[run] = self.data_vision[run]
		else:
			self.result_vision['n_obj'] = 0	
		
	def reset_collect_vision( self ):		
		for run in self.collect_vision.keys():
			self.collect_vision[ run ] = 0

	def have_object( self ):
		if( self.result_vision['n_obj'] == 1 ):
			return 	True
		else:
			return False
			
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
		self.request_data = rospy.ServiceProxy( "/vision_drum" , vision_srv_drum )

	def drum_data( self , task , request ):
		temporary = self.request_data( String( task ) , String( request ) ).data
		self.data_vision['n_obj']		= temporary.n_obj
		self.data_vision['cx']			= temporary.cx
		self.data_vision['cy']			= temporary.cy
		self.data_vision['area']		= temporary.area
		self.data_vision['left']		= temporary.left
		self.data_vision['right']		= temporary.right
		self.data_vision['forward']		= temporary.forward
		self.data_vision['backward']	= temporary.backward

	def drum_center_x( self ):
		return self.result_vision['cx']

	def drum_center_y( self ):
		return self.result_vision['cy']

##=================================> SERVICE VISION GATE <=======================================
	def gate_set_up( self ):
		self.individual_data	= self.gate_data
		self.center_x			= self.gate_center_x
		self.center_y			= self.gate_center_y
		self.can_sum = [ 'cx_1' , 'cx_2' , 'cy_1' , 'cy_2' , 'area' , 'pos' ]
		self.not_sum = []
		self.request_data = rospy.ServiceProxy( "/vision_gate" , vision_srv_gate )

	def gate_data( self , task , request ):
		temporary = self.request_data( String( task ) , String( request ) ).data
		self.data_vision['n_obj']		= temporary.n_obj
		self.data_vision['cx_1']		= temporary.cx1
		self.data_vision['cy_1']		= temporary.cy1
		self.data_vision['cx_2']		= temporary.cx2
		self.data_vision['cy_2']		= temporary.cy2
		self.data_vision['area']		= temporary.area
		self.data_vision['pos']			= temporary.pos

	def gate_center_x( self ):
		return ( self.result_vision['cx_2'] + self.result_vision['cx_1'] ) / 2

	def gate_center_y( self ):
		return ( self.result_vision['cy_2'] + self.result_vision['cy_1'] ) / 2

##================================> SERVICE VISION FLARE <=======================================
	def flare_set_up( self ):
		self.individual_data	= self.flare_data
		self.center_x			= self.flare_center_x
		self.center_y			= self.flare_center_y
		self.can_sum = [ 'cx' , 'cy' , 'area' ]
		self.not_sum = []
		self.request_data = rospy.ServiceProxy( "/vision_flare" , vision_srv_flare )

	def individual_data( self , task , request ):
		temporary = self.request_data( String( task ) , String( request ) ).data
		self.data_vision['n_obj']		= temporary.n_obj
		self.data_vision['cx']			= temporary.cx
		self.data_vision['cy']			= temporary.cy
		self.data_vision['area']		= temporary.area

	def flare_center_x( self ):
		return self.result_vision['cx']

	def flare_center_y( self ):
		return self.result_vision['cy']

