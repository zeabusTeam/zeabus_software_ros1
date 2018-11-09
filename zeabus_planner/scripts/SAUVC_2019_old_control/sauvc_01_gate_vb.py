#!/usr/bin/python2

import rospy
import math

try:
	from zeabus_extension.manage_log import log
	from zeabus_extension.control_auv import control_auv 
except:
	print("Pleas install setup.bash in zeabus_extension package")
	exit()

class play_gate:

	def __init__(self , rate ):

		self.data_gate = {"red":-0.15 , "green":-0.15 }
		self.already_setup = False;
		
		print("Waitting gate service")

		print("I found Gate Service")

		self.rate = rospy.Rate( rate )

	def set_up( self , forward_direction = 4 , side_of_gate = None):

		self.log_command = log( "zeabus_planner" , "log" , "01_gate_command")
		self.log_vision = log( "zeabus_planner" , "log" , "01_gate_vision")
		self.auv = control_auv("play_gate")
		self.past_move = None
		self.forward_direction = forward_direction
		self.side_of_gate = side_of_gate
		self.already_setup = True;

		self.color = None
		

	def play( self ):

		if( self.already_setup ):
			print("Warning you never setup for play gate. I will set default value")
			self.set_up();

		#Start go to forward 
		self.go_forward( self.forward_direction )

	def go_forward_direction( self , distance ):
		self.auv.collect_position()
		self.log_command.write("I will go to forward" , True , 0 )
		print("Now survey forward")

		while( not rospy.is_shutdown() ):			
			self.auv.velocity( x = 0.1 )
			self.rate.sleep()

			self.request_data( "far" , "green" , 5)

			if( self.auv.calculate_distance() > distance ): 
				self.log_command.write("Now distance is over " + str(distance) , False , 3)
				break;
		
		if( self.color == None):
			if( self.past_move == None ):
				if( self.side_of_gate == "left"):
					self.go_survey_left( 2 )
				else:
					self.go_survey_right( 2 )
			elif( self.past_move == "left"):
				self.log_command.write("past I move left now I will move right" , False , 3)
				self.go_survey_right( 2 )
			else:
				self.log_command.write("past I move right now I will move left" , False , 3)
				self.go_survey_left( 2 )
		elif( self.color == "red")
			self.log_command.write("I found red I will play red" , False , 3)
			self.play_near()
		else:
			self.log_command.write("I found green I will play green" , False , 3)
			self.play_near()
		
	def play_near( self ):
		self.log_command.write("Now I will play near that is long step" , True , 0)
		self.request_data( "near" , "red" , 5 )
	
		self.log_command.write("I will move to near gate")

	def go_survey_left( distance ):
		None
	
	def go_survey_right( distance ):
		None

	def	request_data( self , type_vision , target_color , round_find ):
		None

	def find_gate( self , type_vision , target_color ):
		None

if __name__=='__main__':
	rospy.init_node("Mission Gate")	
