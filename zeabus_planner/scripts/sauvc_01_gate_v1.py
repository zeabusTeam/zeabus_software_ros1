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


#function last_play and setup have depth required to check
class play_gate:

	def __init__( self , rate ):
		self.result_vision = { "color" : None  , "cx_1" : 0 , "cx_2" : 0 ,
							 "cy_1" : 0 , "cy_2" : 0 , "area" : 0 }

		self.collect_vision = { "-1 " : 0 , "0" : 0 , 
								"1": { "amont" : 0 , "cx_1" : 0 , "cx_2" : 0 , 
								"cy_1" : 0 , "cy_2" : 0 , "avg_area" : 0}}

		self.data_vision = {"n_obj" : -1 , "cx_1" : 0 , "cx_2" : 0 ,
							"cy_1" : 0 , "cy_2" : 0 , "area" : 0}

		print("Waiting gate service")
		rospy.wait_for_service('vision_gate')
		print("I found gate service")
		
		self.rate = rospy.Rate( rate)

		self.already_setup = False

	def setup( self , first_forward , first_survey , forward , survey ):
		
		self.already_setup = True

		self.navigation = { "first_forward" : first_forward , "first_survey" : first_survey,
							"forward" : forward , "survey" : survey}

		self.log_command = log( "zeabus_planner" , "log" , "02_gate_command")
		self.log_vision = log( "zeabus_planner" , "log" , "02_gate_vision")
		self.client_gate = rospy.ServiceProxy('vision_gate' , vision_srv_gate )

		self.auv = control_auv( "play gate")
		self.auv.absolute_depth( "-1.3")
		self.past_step = 'don\'t move'	# don't move = 0, forward = 1 , right = 2 , left = 3

	def move_forward( self, distance )
		self.log_command.write("Now I move forward distance is " + str( distance) , True , 0)
		self.auv.collect_position()
		while( rospy.is_shutdown() and self.auv.calculate_distance() < distance ):
			self.auv.velocity( x = 0.1 )
			self.rate.sleep()
			self.all_analysis()
			if( self.result_vision['color'] == None ):
				self.log_command.write("Not found all")
				self.rate.sleep()
			elif( self.result_vision['color'] == 'green')
				self.log_command.write("I found green")
				self.rate.sleep()
				break
			elif( self.result_vision['color'] == 'red')
				self.log_command.write("I found red")
				self.rate.sleep()
				break
		self.log_command.write("Out Loop")
		print("Out Loop")
		if( self.result_vision['color'] == None):
			if( self.past_step == 'don\'t move' ):
				self.past_step = 'forward':
				if( self.navigation['fist_survey'] < 0 )
					self.survey_right( abs(self.navigation['first_survey']) )
				else:
					self.survey_left( abs(self.navigation['first_survey']) )
			elif( self.past_step == 'right' )
				self.past_step = 'forward' 
				self.survey_lety( self.navigation['survey'])
			elif( self.past_step == 'left' )
				self.past_step = 'forward'
				self.survey_right( self.navigation['survey'])
		elif( self.result_vision['color'] in [ 'red' , 'green']):
			self.last_play()

	def survey_left( self , distance )
		self.log_command.write("Now I move servey left distance is " + str( distance) , True , 0)
		self.auv.collect_position()
		while( rospy.is_shutdown() and self.auv.calculate_distance() < distance ):
			self.auv.velocity( y = 0.3 )
			self.rate.sleep()
			self.all_analysis()
			if( self.result_vision['color'] == None ):
				self.log_command.write("Not found all")
				self.rate.sleep()
			elif( self.result_vision['color'] == 'green')
				self.log_command.write("I found green")
				self.rate.sleep()
				break
			elif( self.result_vision['color'] == 'red')
				self.log_command.write("I found red")
				self.rate.sleep()
				break
		self.log_command.write("Out Loop")
		print("Out Loop")
		if( self.result_vision['color'] == None):
			self.past_step = 'lefy'
			self.move_forward( self.navigation['forward'])
		elif( self.result_vision['color'] in [ 'red' , 'green']):
			self.last_play()

	def survey_right( self , distance )
		self.log_command.write("Now I move servey right distance is " +str( distance), True , 0)
		self.auv.collect_position()
		while( rospy.is_shutdown() and self.auv.calculate_distance() < distance ):
			self.auv.velocity( y = -0.3 )
			self.rate.sleep()
			self.all_analysis()
			if( self.result_vision['color'] == None ):
				self.log_command.write("Not found all")
				self.rate.sleep()
			elif( self.result_vision['color'] == 'green')
				self.log_command.write("I found green")
				self.rate.sleep()
				break
			elif( self.result_vision['color'] == 'red')
				self.log_command.write("I found red")
				self.rate.sleep()
				break
		self.log_command.write("Out Loop")
		print("Out Loop")
		if( self.result_vision['color'] == None):
			self.past_step = 'lefy'
			self.move_forward( self.navigation['forward'])
		elif( self.result_vision['color'] in [ 'red' , 'green']):
			self.last_play()

	def last_play( self ):
		self.log_command.write("Now is last part" , True , 0)
		self.auv.collect_position()
		self.log_command.write("I will move to center before" , False , 1)
		print( "move to center ")
		while( rospy.is_shutdown() ):
			self.rate.sleep()
			self.all_analysis( 5 )
			if( self.result_vision['color'] == None):
				self.log_command.write("I faliure")
				print("faliure while move to center")
				exit()
			if( abs(self.find_center('y')) < 0.1 ):
				print("change depth")
				self.log_command.write("Don't depth already I will move depth" , False , 2)
				self.auv.absolute_depth(-1.6)
			elif( self.find_center('x') < -0.2 ):
				print("move left to center")
				self.log_command.write("Move to left")
				self.auv.velocity( 0.08 )
			elif( self.find_center('x') > 0.2 ):
				print("move right to center")
				self.log_command.write("Move to right")
				self.auv.velocity( -0.08 )
			else:
				break
		self.log_command.write("Now it center move forward")
		self.auv.collect_position()

		

			
	def all_analysis( self, amont ):
		self.result_vision = { "color" : None  , "cx_1" : 0 , "cx_2" : 0 ,
							 "cy_1" : 0 , "cy_2" : 0 , "area" : 0 }
		for color in ['green' , 'red']:
			self.analysis( color , amont)
			if( self.result_vision != None):
				break

	def analysis( self , color , amont):
		self.log_vision.write( "target color of gate is " + color , True , 0)
		print( "find color : " + color + " is ", end='' )
		self.reset_data()
		while( not rospy.is_shutdown() ):
			request_vision( String("gate") , String(color) )
			if( self.data_vision["n_obj"] == -1 ):
				self.collect_vision["-1"] += 1
				if( self.collect_vision["-1"] > amont ):
					print("not found")
					break

			elif( self.data_vision["n_obj"] == 0):
				self.collect_vision["0"] += 1
				if( self.collect_vision["0"] > amont):
					print("not found")
					break

			else:
				self.collect_vision['1']['amont'] += 1
				self.split_2_data( "cx_1" , "cx_2" )
				self.split_2_data( "cy_1" , "cy_2" )
				self.collect_vision['1']['area'] += self.data_vision['area']
				if( self.collect_vision['1']['amont'] > amont):
					self.result_vision['color'] = color
					self.result_vision['cx_1'] = self.collect_vision['1']['cx_1']/amont
					self.result_vision['cx_2'] = self.collect_vision['1']['cx_2']/amont
					self.result_vision['cy_1'] = self.collect_vision['1']['cy_1']/amont
					self.result_vision['cy_2'] = self.collect_vision['1']['cy_2']/amont
					self.result_vision['area'] = self.collect_vision['1']['area']/amont
					print("Found cx are " + str(self.result_vision['cx_1']) + " : " +
							str(self.result_vision['cx_2']) + " and cy are " + 
							str(self.result_vision['cy_1']) + " : " + 
							str(self.result_vision['cy_2']) )
					break
			

	def	request_vision( self , first_order , second_order ):
		receive_data = self.client_gate( first_order , second_order)
		self.data_vision['n_obj'] = receive_data.data.n_obj
		self.data_vision['cx_1'] = receive_data.data.cx1	
		self.data_vision['cx_2'] = receive_data.data.cx2
		self.data_vision['cy_1'] = receive_data.data.cy1
		self.data_vision['cy_2'] = receive_data.data.cy2
		self.data_vision['area'] = receive_data.data.area
		if( self.data_vision['n_obj'] in [0 , -1]):
			self.log_vision.write("NOT FOUND" , False , 0)
		else:
			self.log_vision.write("FOUND color is " + second_order , False , 0)
			self.log_vision.write("point of x are " + str( self.data_vision['cx_1'] ) +
								":" + str( self.data_vision['cx_2']) , False , 1)
			self.log_vision.write("point of y are " + str( self.data_vision['cy_1'] ) +
								":" + str( self.data_vision["cy_2"]) , False , 1)
			self.log_vision.write("area is " + str( self.data_vision["area"]))

	def split_2_data( self , first , second ): # for decision 2 valaue to x1 or x2
		if( self.data_vision[first] < self.data_vision[second] ):
			self.collect_vision["1"][first] += self.data_vision[first]
			self.collect_vision["1"][second] += self.data_vision[second]
		else:
			self.collect_vision["1"][first] += self.data_vision[second]
			self.collect_vision["1"][second] += self.data_vision[first]
			
	def find_center( self , axis ):
		if( axis == 'y' ):
			return ( self.result_vision['cy_2'] + self.result_vision['cy_1'] ) / 2
		elif( axis == 'x'):
			return ( self.result_vision['cx_2'] + self.result_vision['cx_1'] ) / 2
		else:
			print("invalid input function find_center")
			exit()
			return -1

	def reset_data( self ):
		self.collect_vision = { "-1 " : 0 , "0" : 0 , 
								"1": { "amont" : 0 , "cx_1" : 0 , "cx_2" : 0 , 
								"cy_1" : 0 , "cy_2" : 0 , "avg_area" : 0}}

if __name__=='__main__':
	rospy.init_node("Mission Gate")	
