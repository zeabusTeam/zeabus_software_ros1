#!/usr/bin/python2

import rospy
import math

from auv_controller import auv_controller 

from zeabus_vision.srv import vision_srv_flare
# message have four varaible 
# n_obj = -1 , o :: Don't have image or object
# cx = center in x axis , cy = center in y cxis area = area of this picture
from zeabus_vision.msg import vision_flare
from zeabus_planner.msg import mission_result

class mission_flare:

	def __init__( self ):

		self.auv = auv_controller("mission_flare" , True )

		self.result_vision = { "n_obj" : None , "pos" : None 
							, "cx" : 0 , "cy" : 0
							, "area":None }

		self.collect_vision = { "n_obj" : None , "pos" : None 
							, "cx" : 0 , "cy" : 0
							, "area" : None }

		self.data_vision = { "n_obj" : None , "pos" : None 
							, "cx" : 0 , "cy" : 0
							, "area" : None }

		print("Waiting flare service")
		rospy.wait_for_service('vision_flare')
		print("I found flare service")

		self.rate = rospy.Rate( 30 )
		self.request_data = rospy.ServiceProxy('/vision_flare' , vision_srv_flare )
		self.mission_planner = rospy.Service('/mission_flare' , mission_result , self.main_play)

	def main_play( self , request )
		self.can_spin = False
		self.sucess_mission = False

		self.play_forward_far()
		if( self.can_spin ):
			self.play_spin()

		return Bool( self.sucess_mission ) , Int8( 1 )

	def play_forward_far( self ):
		self.auv.ve

	def analysis_data( self , amont , type_vision):
		found = 0
		unfound = 0 
		self.reset_collect_vision( type_vision )
		while( found < amont and unfound < amont ):
			self.request_result()
			if( self.data_vision['n_obj'] >= 1 ):
				found += 1
				for run in self.data_vision.keys():
					if( run == 'n_obj' ):
						continue
					elif( run == 'pos'):
						self.collect_vision[ run ] = self.data_vision[ run ]
					else:
						self.collect_vision[ run ] += self.data_vision[ run ]
				else:
					unfound += 1
		if( found == amont ):
			print("======> analysis_data" , type_vision , "Last Result is FOUND ")
			for run in self.data_vision.keys():
				if( not ( run == 'pos' or run == 'n_obj' ) ):
					self.result_vision[ run ] = self.collect_vision[ run ] / amont
		else:
			print("======> analysis_data" , type_vision , "Last Result is UNFOUND")
			self.result_vision['n_obj'] = 0

	def request_result( self , type_vision ):
		receive_data = self.receive_data( String('flare') , String( type_vision ) ).data
		self.data_vision['n_obj']	= receive_data.n_obj
		self.data_vision['cx']		= receive_data.cx
		self.data_vision['cy']		= receive_data.cy
		self.data_vision['area']	= receive_data.area
		self.data_vision['pos']		= receive_data.pos
		
		
	def reset_collect_vision( self ):
		self.result_vision = { "n_obj" : None , "pos" : None 
							, "cx" : 0 , "cy" : 0
							, "area":None }	
