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

		self.rate = rospy.Rate( 30 )
		self.request_data = rospy.ServiceProxy('/vision_gate' , vision_srv_gate )
		self.mission_planner = rospy.Service('/mission_gate' , mission_result , self.main_play)

	def main_play( self , request ):
		self.can_play_backward = False
		self.sucess_mission = False
		self.auv.set_mode( 1 )
		self.play_forward()	
		if( self.can_play_backward ):
			self.play_backward()

		return Bool( self.sucess_mission ) , Int8( 1 ) 

	def play_forward( self ):
		print( "<--- MISSION GATE ---> PLAY FORWARD MODE ")
		count_unfound = 0
		while( not rospy.is_shutdown() and count_unfound < 3 ):
			self.rate.sleep()
			self.analysis_data( 5 )
			if( self.result_vision['n_obj'] < 1 ):
				print( "<--- MISSION GATE ---> Not found gate")
				count_unfound += 1
				continue
			if( self.result_vision['pos'] == 0 ):
				if( abs(self.find_center('x')) < 0.03 ):
					self.auv.velocity( 'x' , 0.3 )
					print( "------> POS : 0 -----> FORWARD")
					self.can_play_backward = True
				else:
					self.auv.velocity( 'y' , math.copysign( 0.4 , -1*self.find_center('x')))	
					print( "------> POS : 0 -----> SURVEY " , self.find_center('x'))
					self.can_play_backward = True
			elif( self.result_vision['pos'] == -1 ):
				print( "------> POS : -1 -----> LEFT")
				self.auv.velocity( 'y' , 0.4 )
				self.can_play_backward = False
			elif( self.result_vision['pos'] == 1 ):
				print( "------> POS : 1 -----> RIGHT")
				self.auv.velocity( 'y' , -0.4 )
				self.can_play_backward = False
			
	def play_backward( self ):
		print( "<--- MISSION GATE ---> PLAY BACKWORD MODE " )
		self.auv.relative_yaw( 3.14 )
		count = 0;
		while( not rospy.is_shutdown() ):
			self.rate.sleep()
			if( self.auv.ok_state( 'yaw' , 0.05 ) ):
				count += 1
			else:
				count = 0
			print("<- BACKWARD -> Waitting yaw " + str( count ))
			if(  count > 10 ):
				break
		print("<--- MISSION GATE ---> Move BACKWARD")	
		while( not rospy.is_shutdown() ):
			self.rate.sleep()
			self.auv.velocity( 'x' , -0.5)
			self.analysis_data( 5 )
			count = 0
			if( self.result_vision['n_obj'] > 0 ):
				count += 1
				print("<- BACKWARD -> Found " + str( count ))
			else:
				count = 0
			if( count > 2 ):
				self.sucess_mission = True
				break

		print("<--- MISSION GATE ---> FINISH BACKWARD")
			
			
	def find_center( self , type_center ):
		if( type_center == 'x'):
			return ( self.result_vision['cx_1'] + self.result_vision['cx_2'] )/2
		elif( type_center == 'y'):
			return ( self.result_vision['cy_1'] + self.result_vision['cy_2'] )/2
		else:
			print("<---------- WARNING GATE ----------> Don't found this type")
			return -5
		
	def reset_collect_vision( self ):
		self.collect_vision = { "n_obj":None , "pos":0 , "cx_1":0.0 , "cx_2":0.0
							, "cy_1":0.0 , "cy_2":0.0 , "area":0.0}
	
	def	analysis_data( self , amont ):
		found = 0
		unfound = 0
		self.reset_collect_vision()
		while( found < amont and unfound < amont ):
			self.request_result()
			if( self.data_vision['n_obj'] >= 1 ):
				found += 1
				for i in ['cx_1' , 'cx_2' , 'cy_1' , 'cy_2' , 'area' , 'pos']:
					self.collect_vision[i] = self.data_vision[i]
			else:
				unfound += 1
		if( found == amont ):
			print("Last result is FOUND")
			self.result_vision['n_obj'] = 2
			for i in ['cx_1' , 'cx_2' , 'cy_1' , 'cy_2' , 'area' , 'pos']:
				self.result_vision[i] = self.collect_vision[i] / amont
		elif( unfound == amont ):
			print("Last result is UNFOUND")
			self.result_vision['n_obj'] = 0

	def request_result( self ):
		receive_data = self.request_data( String('gate') , String("Sevinar"))
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
