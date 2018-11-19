#!/usr/bin/python2
#################################################################################################
####
####	FILE		: vision_collector.py
####	Maintain	: Supasan Komonlit
####	Create on	: 2019 , Nov 14
####	Purpose		: For manage about analysis vision data
####
#################################################################################################

import rospy
import math

#service file of vision must send 2 data to use 
from zeabus_vision.srv import *
from zeabus_vision.msg import *

from std_msgs.msg import String

class VisionCollector:

	def __init__( self , mission_vision ):

		# this require
		if( mission_vision == 'gate'):
			self.gate_set_up()
		elif( mission_vision == 'flare' ):
			self.flare_set_up()	
		elif( mission_vision == 'drum' ):
			self.drum_set_up()
		else:
			print( "<=== VisionCollecotr ===> ERROR PLEASE Look condition on line 32")
			exit( 0 )
		
		# set variable for make to collect and use in data vision
		self.result			= {	'n_obj'		: 0		, 'area'		: 0
								, 'cx'		: 0		, 'cy'			: 0
								, 'cx_1'	: 0		, 'cx_2'		: 0
								, 'cy_1'	: 0		, 'cy_2'		: 0
								, 'left'	: False	, 'right'		: False
								, 'forward'	: False	, 'backward'	: False
								, 'pos'		: 0 }

		self.collect		= {	'n_obj'		: 0		, 'area'		: 0
								, 'cx'		: 0		, 'cy'			: 0
								, 'cx_1'	: 0		, 'cx_2'		: 0
								, 'cy_1'	: 0		, 'cy_2'		: 0
								, 'left'	: False	, 'right'		: False
								, 'forward'	: False	, 'backward'	: False
								, 'pos'		: 0 }

		self.data			= {	'n_obj'		: 0		, 'area'		: 0
								, 'cx'		: 0		, 'cy'			: 0
								, 'cx_1'	: 0		, 'cx_2'		: 0
								, 'cy_1'	: 0		, 'cy_2'		: 0
								, 'left'	: False	, 'right'		: False
								, 'forward'	: False	, 'backward'	: False
								, 'pos'		: 0 }

	def analysis_all( self , task , request , amont ):
		count_found = 0
		count_unfound = 0 
		self.reset_collect()
		while( count_found < amont and count_unfound < amont ):
			self.individual_data( task , request )
			if( self.data['n_obj'] >= 1 ):
				count_found += 1
				for run in self.can_sum:
					self.collect[ run ] += self.data[ run ]
			else:
				count_unfound += 1
		if( count_found == amont ):
			self.result['n_obj'] = 1
			for run in self.can_sum:
				self.result[run] = self.collect[run] / amont
			for run in self.not_sum:
				self.result[run] = self.data[run]
		else:
			self.result['n_obj'] = 0	
		
	def reset_collect( self ):		
		for run in self.collect.keys():
			self.collect[ run ] = 0

	def have_object( self ):
		if( self.result['n_obj'] == 1 ):
			return 	True
		else:
			return False

	def area( self ):
		return self.result['area']

	def echo_data( self ):
		print( "<=== VISION COLLECTOR ===> Object : " + str( self.result[ 'n_obj'] )
			+ " center_x : center_y " + str( self.center_x() ) + " : " + str( self.center_y() )
		)

	def echo( self ):
		if( not self.have_object() ):
			return str ( "<=== VISION COLLECTOR ===> DON\'T HAVE OBJECT")
		return str ( "<=== VISION COLLECTOR ===> Object : " + str( self.result[ 'n_obj'] )
			+ " center_x : center_y " + str( self.center_x() ) + " : " + str( self.center_y() )
		)
			
#################################################################################################
####
####	THIS SECTION IS INDIVIDUAL FOR EACH SERVICE OF ZEABUS TEAM VISION PART
####		AVAILABLE FOR USE
####
####	Note :	We use variable to declare function which want to use that make me have 
####			pattern to calculate vision data
#################################################################################################
	
##=================================> SERVICE VISION DRUM <=======================================
	def drum_set_up( self ):
		self.individual_data	= self.drum_data
		self.center_x			= self.drum_center_x
		self.center_y			= self.drum_center_y
		self.can_sum = [ 'cx_1' , 'cx_2' , 'cy_1' , 'cy_2' , 'area' ]
		self.not_sum = [ 'left' , 'right' , 'forward' , 'backward']
		self.request_data = rospy.ServiceProxy( "/vision_drum" , vision_srv_drum )

	def drum_data( self , task , request ):
		temporary = self.request_data( String( task ) , String( request ) ).data
		self.data['n_obj']		= temporary.state
		self.data['cx_1']		= temporary.cx1
		self.data['cx_2']		= temporary.cx2
		self.data['cy_1']		= temporary.cy1
		self.data['cy_2']		= temporary.cy2
		self.data['area']		= temporary.area
		self.data['left']		= temporary.left
		self.data['right']		= temporary.right
		self.data['forward']	= temporary.forward
		self.data['backward']	= temporary.backward

	def drum_center_x( self ):
		return (self.result['cx_1'] + self.result['cx_2'])/2

	def drum_center_y( self ):
		return (self.result['cy_1'] + self.result['cy_2'])/2

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
		self.data['n_obj']		= temporary.state
		self.data['cx_1']		= temporary.cx1
		self.data['cy_1']		= temporary.cy1
		self.data['cx_2']		= temporary.cx2
		self.data['cy_2']		= temporary.cy2
		self.data['area']		= temporary.area
		self.data['pos']		= temporary.pos

	def gate_center_x( self ):
		return ( self.result['cx_2'] + self.result['cx_1'] ) / 2

	def gate_center_y( self ):
		return ( self.result['cy_2'] + self.result['cy_1'] ) / 2

##================================> SERVICE VISION FLARE <=======================================
	def flare_set_up( self ):
		self.individual_data	= self.flare_data
		self.center_x			= self.flare_center_x
		self.center_y			= self.flare_center_y
		self.can_sum = [ 'cx' , 'cy' , 'area' ]
		self.not_sum = []
		self.request_data = rospy.ServiceProxy( "/vision_flare" , vision_srv_flare )

	def flare_data( self , task , request ):
		temporary = self.request_data( String( task ) , String( request ) ).data
		self.data['n_obj']		= temporary.state
		self.data['cx']			= temporary.cx
		self.data['cy']			= temporary.cy
		self.data['area']		= temporary.area

	def flare_center_x( self ):
		return self.result['cx']

	def flare_center_y( self ):
		return self.result['cy']

