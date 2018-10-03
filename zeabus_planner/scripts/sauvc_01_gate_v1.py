#!/usr/bin/python2

import rospy
import math

try:
	from zeabus_extension.manage_log import log
	from zeabus_extension.control_auv import control_auv 
except:
	print("Pleas install setup.bash in zeabus_extension package")
	exit()

from zeabus_vision.srv import vision_srv_gate
from zeabus_vision.msg import vision_gate
from std_msgs.msg import String

class play_gate:

	def __init__( self , rate ):
		self.result_vision = { "color" : None  , "cx_01" : 0 , "cx_02" : 0 ,
							 "cy_01" : 0 , "cy_02" : 0 , "area" : 0 }

		self.collect_vision = { "-1 " : 0 , "0" : 0 , 
								"1": { "amont" : 0 , "cx_1" : 0 , "cx_2" : 0 , 
								"cy_1" : 0 , "cy_2" : 0 , "avg_area" : 0}}

		self.data_vision = {"n_obj" : -1 , "cx1" : 0 , "cx2" : 0 ,
							"cy01" : 0 , "cy02" : 0 , "area" : 0}

		print("Waiting gate service")
		rospy.wait_for_service('vision_gate')
		print("I found gate service")
		
		self.rate = rospy.Rate( rate)

		self.already_setup = False

	def setup( self , first_forward , first_survey , forward , survey ):
		
		self.already_setup = True

		self.navigation = { "first_forward" : first_forward , "first_survey" : first_survey,
							"forward" : forwaard , "survey" : survey}

		self.log_command = log( "zeabus_planner" , "log" , "02_gate_command")
		self.log_vision = log( "zeabus_planner" , "log" , "02_gate_vision")
		self.client_gate = rospy.ServiceProxy('vision_gate' , vision_srv_gate )

	def far_analysis( self , color , amont):
		self.log_vision( "target color of gate is " + color , True , 0)
		print( "find color : " + color , end='' )
		self.reset_data()
		while( True ):
			request_vision( String("gate") , String(color) )
			if( self.data_vision["n_obj"] == -1 ):
				self.collect_vision["-1"] += 1
			elif( self.data_vision["n_obj"] == 0):
				self.collect_vision["0"] += 1
			else:
				self.collect_vision['1']['amont'] += 1
				self.collect_vision['1']['avg_x'] += 

	def	request_vision( self , first_order , second_order ):
		receive_data = self.client_gate( first_order , second_order)
		self.data_vision['n_obj'] = receive_data.data.n_obj
		self.data_vision['cx1'] = receive_data.data.cx1	
		self.data_vision['cx2'] = receive_data.data.cx2
		self.data_vision['cy1'] = receive_data.data.cy1
		self.data_vision['cy2'] = receive_data.data.cy2
		self.data_vision['area'] = receive_dataa.data.area

	def split_2_data( self , first , second ): # for decision 2 valaue to x1 or x2
		if( self.data_vision[first] < self.data_vision[second] ):
			self.collect_vision["1"][first] = self.data_vision[first]
			self.collect_vision["1"][second] = self.data_vision[second]
		else:
			self.collect_vision["1"][first] = self.data_vision[second]
			self.collect_vision["1"][second] = self.data_vision[first]
			

	def reset_data( self ):
		self.collect_vision = { "-1 " : 0 , "0" : 0 , 
								"1": { "amont" : 0 , "avg_x" : 0 , "avg_y" : 0 , "avg_area" : 0}}

if __name__=='__main__':
	rospy.init_node("Mission Gate")	
