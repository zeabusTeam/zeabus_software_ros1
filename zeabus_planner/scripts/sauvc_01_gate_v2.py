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

# message of vision is n_obj = -1:wait image 0:don't have 1>= have object
# pos = -1:left 0:middle 1:right
# cx1 cy1 cx2 cy2 area 


#function last_play and setup have depth required to check
class play_gate:

	def __init__( self , rate ):
		self.result_vision = { "n_obj":None , "pos":None , "cx_1":0 , "cx_2":0
							, "cy_1":0 , "cy_2":0 , "area":None}

		self.collect_vision = { "n_obj":None , "pos":None , "cx_1":0 , "cx_2":0
							, "cy_1":0 , "cy_2":0 , "area":None}

		self.data_vision = { "n_obj":None , "pos":None , "cx_1":0 , "cx_2":0
							, "cy_1":0 , "cy_2":0 , "area":None}

		print("Waiting gate service")
		rospy.wait_for_service('vision_gate')
		print("I found gate service")
		
		self.rate = rospy.Rate( rate)
		self.already_setup = False
		self.past_mode = None

	def setup( self , first_forward , first_survey , forward , survey ):

		self.first_forward = first_forward
		self.first_survey = first_survey
		self.forward = forward
		self.survey = survey

		self.log_command = log( "zeabus_planner" , "log" , "02_gate_command")
		self.log_vision = log( "zeabus_planner" , "log" , "02_gate_vision")
		self.client_gate = rospy.ServiceProxy('vision_gate' , vision_srv_gate )

		self.auv = control_auv( "play gate")
		self.auv.absolute_depth( -3.8)
		self.past_step = 'don\'t move'	# don't move = 0, forward = 1 , right = 2 , left = 3
		
		self.already_setup = True

	def play( self ):
		
		if( self.already_setup ) : print( "OK You have setup")
		else: self.setup( 5 , 4 , 1 , 2)

		self.auv.absolute_depth( -3.7)	
		self.log_command.write("Waiting ok depth" , True , 0)
		print("Waiting Depth")
		while( not rospy.is_shutdown() and not self.auv.ok_position("z" , 0.2) ):
			self.rate.sleep()	
		self.log_command.write("depth is OK next wait yaw" , False , 1)
		print("Waiting yaw")
#		while( not rospy.is_shutdown() and not self.auv.ok_position("yaw" , 0.1) ):
#			self.rate.sleep()		
		self.log_command.write("I will start now", True , 0)
		self.auv.collect_position()
		while( not rospy.is_shutdown() and self.auv.calculate_distance() < 1 ):
			self.auv.velocity( x = 0.1 )
		print("Now play move forward")
		self.move_forward( self.first_forward)

# message of vision is n_obj = -1:wait image 0:don't have 1>= have object
# pos = -1:left 0:middle 1:right
# cx1 cy1 cx2 cy2 area

	def center_x (self):
		result = ( self.result_vision['cx_2'] + self.result_vision['cx_1'] ) / 2
		print("cx_2 : cx_1 " + str(self.result_vision['cx_2']) + " : " + str( self.result_vision['cx_1']))
		print("center x is result : " + str( result ))
		return result

	def center_y (self):
		return ( self.result_vision['cy_2'] + self.result_vision['cy_1'] ) / 2
 
	def found_mode( self ):
		self.log_command.write("Now it is last mode")
		print("Last mode")
		while( not rospy.is_shutdown() ):
			self.rate.sleep()
			self.analysis_data( 5 )
			if( self.result_vision['pos'] == 0):
				if( abs( self.center_x() ) < 0.1):
					print( "Last move and center is " + str(self.center_x()))
					self.auv.collect_position()
					temporary = 0
					while( not rospy.is_shutdown()):
						print("Last loop while")
						self.rate.sleep()
						self.analysis_data( 5 )
						if( self.result_vision['n_obj'] == 0 ):
							temporary += 1
							print("don't found count is " + str(temporary))
							if( temporary < 2 ):
								continue
							self.auv.collect_position()
							while( not rospy.is_shutdown() and self.auv.calculate_distance()<4):
								self.rate.sleep()
								print("Move direct 3 meter")
								self.auv.velocity(x = 0.4)
							self.auv.absolute_depth( 0 )
							break
						elif( abs( self.center_x()) < 0.04 ):
							print("move forward")
							self.auv.velocity( x = 0.3)
						elif( self.center_x() < -0.06 ):
							print("move left")
							self.auv.velocity( y = 0.04)
						elif( self.center_x() > 0.06):
							print("move right")
							self.auv.velocity( y = -0.04)
					break
				elif( self.center_x() < -0.1 ):
					print("Move left")
					self.auv.velocity( y = 0.1)
				else:
					print("Move right")
					self.auv.velocity( y = -0.1)
			elif( self.result_vision['pos']== -1):
				print("move left")
				self.auv.velocity( y = 0.1 )
			elif( self.result_vision['pos']== 1):
				print("move right")
				self.auv.velocity( y = -0.1)
			else:
				print("Want to find middle")
				self.auv.velocity( y = self.result_vision['pos'] * -1 * 0.2 )

	def move_forward( self , distance):
		self.log_command.write("Now I move forward and distance " + str( distance ) , True , 0 )	
		self.auv.collect_position()
		print("Wait yaw")
#		while( not rospy.is_shutdown() and not self.auv.ok_position("yaw" , 0.1 )):
#			self.rospy.sleep()
		print("Let go")
		while( not rospy.is_shutdown() and self.auv.calculate_distance() < distance):
			self.analysis_data( 5 )
			self.auv.velocity( x = 0.1 )
			if( self.result_vision['n_obj'] == 1):
				self.log_command.write("Now I found" , False , 0)
				break
			else:
				self.rate.sleep()
			self.rate.sleep()
		if( self.result_vision['n_obj'] == 1):
			self.found_mode()
		elif( self.past_mode == "right"):
			self.past_mode = "forward"
			self.survey_right( self.survey )
		elif( self.past_mode == "left"):
			self.past_mode = "forward"
			self.survey_left( self.survey )
		else:
			self.survey_left( self.first_survey )
		
	def survey_right( self , distance ):
		self.log_command.write("Now on servey right distance is " + str( distance ) , True , 0 )
		self.log_command.write("Wait ok yaw " )
		print("Wait yaw")
#		while( not rospy.is_shutdown() and not self.auv.ok_position("yaw" , 0.1 )):
#			self.rospy.sleep()
		print("Let go")
		while( not rospy.is_shutdown() and self.auv.calculate_distance() < distance):
			self.analysis_data( 5 )
			self.auv.velocity( y = -0.3)
			if( self.result_vision['n_obj'] == 1):
				self.log_command.write("Now I found" , False , 0)
				break
			else:
				self.rate.sleep()
			selfrate.sleep()
		if( self.result_vision['n_obj'] == 1):
			self.found_mode()
		else:
			self.past_mode = "right"
			self.move_forward( self.survey )

	def survey_left( self , distance ):
		self.log_command.write("Now on servey left distance is " + str( distance ) , True , 0 )
		self.log_command.write("Wait ok yaw " )
		print("Wait yaw")
#		while( not rospy.is_shutdown() and not self.auv.ok_position("yaw" , 0.1 )):
#			self.rospy.sleep()
		print("Let go")
		while( not rospy.is_shutdown() and self.auv.calculate_distance() < distance):
			self.analysis_data( 5 )
			self.auv.velocity( y = 0.3)
			if( self.result_vision['n_obj'] == 1):
				self.log_command.write("Now I found" , False , 0)
				break
			else:
				self.rate.sleep()
			selfrate.sleep()
		if( self.result_vision['n_obj'] == 1):
			self.found_mode()
		else:
			self.past_mode = "left"
			self.move_forward( self.survey )

	def analysis_data( self , amont):
		self.log_vision.write("Analysis data" , True , 0)
		found = 0 ;
		unfound = 0;
		self.reset_collect_vision()
		while( found < amont and unfound < amont):
			self.request_vision( String("gate") , String("Aon Baka") )
			if( self.data_vision['n_obj'] in [0 , -1]):
				unfound+=1
			else:
				found+=1;
				self.collect_vision['n_obj'] = self.data_vision['n_obj']
				self.collect_vision['pos'] = self.data_vision['pos']
				self.collect_vision['cx_1'] += self.data_vision['cx_1']
				self.collect_vision['cx_2'] += self.data_vision['cx_2']
				self.collect_vision['cy_1'] += self.data_vision['cy_1']
				self.collect_vision['cy_2'] += self.data_vision['cy_2']
		if( found == amont):
			self.result_vision['n_obj'] = 1
			self.result_vision['pos'] = self.data_vision['pos']
			self.result_vision['cx_1'] = self.collect_vision['cx_1']/amont
			self.result_vision['cx_2'] = self.collect_vision['cx_2']/amont
			self.result_vision['cy_1'] = self.collect_vision['cy_1']/amont
			self.result_vision['cy_2'] = self.collect_vision['cy_2']/amont
		else:
			self.result_vision['n_obj'] = 0
				
	def reset_collect_vision( self ):
		self.collect_vision = { "n_obj":None , "pos":None , "cx_1":0 , "cx_2":0
							, "cy_1":0 , "cy_2":0 , "area":0}
		
	
	def	request_vision( self , first_order , second_order ):
		receive_data = self.client_gate( first_order , second_order)
		self.data_vision['n_obj'] = receive_data.data.n_obj
		self.data_vision['pos'] = receive_data.data.pos
		self.data_vision['cx_1'] = receive_data.data.cx1	
		self.data_vision['cx_2'] = receive_data.data.cx2
		self.data_vision['cy_1'] = receive_data.data.cy1
		self.data_vision['cy_2'] = receive_data.data.cy2
		self.data_vision['area'] = receive_data.data.area
		if( self.data_vision['n_obj'] in [0 , -1]):
			self.log_vision.write("NOT FOUND" , False , 0)
		else:
			self.log_vision.write("FOUND color is " + str(second_order) , False , 0)
			self.log_vision.write("point of x are " + str( self.data_vision['cx_1'] ) +
								":" + str( self.data_vision['cx_2']) , False , 1)
			self.log_vision.write("point of y are " + str( self.data_vision['cy_1'] ) +
								":" + str( self.data_vision["cy_2"]) , False , 1)
			self.log_vision.write("area is " + str( self.data_vision["area"]))

if __name__=='__main__':
	rospy.init_node("Mission Gate")	
	gate = play_gate( 30 )
	gate.setup( 4 , 4 , 1 , 2)
	gate.play()
