#!/usr/bin/python2

import rospy
import math

from auv_controller import auv_controller 

from zeabus_vision.srv import vision_srv_gate
# message of vision is n_obj = -1 : wait image 0 : don't have 1>= have object
# pos = -1:left 0:middle 1:right
# cx1 cy1 cx2 cy2 area
from zeabus_vision.msg import vision_gate
from std_msgs.msg import String , Bool , Int8
from zeabus_planner.srv import mission_result

class mission_gate:

	def __init__( self ):
		
		# this object will init node when sencond argument is True
		self.auv = auv_controller("mission_gate" , True )

		self.result_vision = { "n_obj" : None , "pos" : None 
							, "cx_1" : 0 , "cx_2" : 0
							, "cy_1" : 0 , "cy_2" : 0 
							, "area":None }

		self.collect_vision = { "n_obj" : None , "pos" : None 
							, "cx_1" : 0 , "cx_2" : 0
							, "cy_1" : 0 , "cy_2" : 0 
							, "area" : None }

		self.data_vision = { "n_obj" : None , "pos" : None 
							, "cx_1" : 0 , "cx_2" : 0
							, "cy_1" : 0 , "cy_2" : 0 
							, "area" : None }

		print("Waiting gate service")
		rospy.wait_for_service('vision_gate')
		print("I found gate service")

		self.request_data = rospy.ServiceProxy('/vision_gate' , vision_srv_gate )
		self.mission_planner = rospy.Service('/mission_gate' , mission_result , self.main_play)

	def main_play( self , request ):
		return self.mission_result.Response( Bool( True ) , Int8( 1 )  )
		
	def reset_collect_vision( self ):
		self.collect_vision = { "n_obj":None , "pos":0 , "cx_1":0.0 , "cx_2":0.0
							, "cy_1":0.0 , "cy_2":0.0 , "area":0.0}
	
	def	analysis_data( self , amont ):
		found = 0
		unfound = 0
		self.reset_collect_vision()
		while( found < amont and unfound < amont ):
			self.request_result()
			if( self.collect_vision['n_obj'] >= 1 ):
				found += 1
				for i in ['cx_1' , 'cx_2' , 'cy_1' , 'cy_2' , 'area' , 'pos']:
					self.collect_vision[i] = self.data_vision[i]
			else:
				unfound += 1
		if( found == amont ):
			self.result_vision['n_obj'] = 1
			for i in ['cx_1' , 'cx_2' , 'cy_1' , 'cy_2' , 'area' , 'pos']:
				self.result_vision[i] = self.collect_vision[i] / amont
		elif( unfound == amont ):
			self.result_vision['n_obj'] = 0

	def request_result( self ):
		receive_data = self.request_data( first_order , second_order)
		self.data_vision['n_obj'] = receive_data.data.n_obj
		self.data_vision['pos'] = receive_data.data.pos
		self.data_vision['cx_1'] = receive_data.data.cx1	
		self.data_vision['cx_2'] = receive_data.data.cx2
		self.data_vision['cy_1'] = receive_data.data.cy1
		self.data_vision['cy_2'] = receive_data.data.cy2
		self.data_vision['area'] = receive_data.data.area

if __name__=="__main__":
	mission_01 = mission_gate()
	rospy.spin()
