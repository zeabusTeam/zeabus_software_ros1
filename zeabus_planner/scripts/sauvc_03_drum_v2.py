#!/usr/bin/python2

import rospy
import math

try:
	from zeabus_extension.manage_log import log
	from zeabus_extension.control_auv import control_auv 
except:
	print("Pleas install setup.bash in zeabus_extension package")
	exit()

from zeabus_vision.srv import vision_srv_drum
from zeabus_vision.msg import vision_drum
from std_msgs.msg import String

class play_drum:
	
	def __init__( self , rate , first_forward , first_survey , forward , survey ):
		self.rate = rospy.Rate( rate )
		self.first_forward = first_forward;
		self.first_survey = first_survey;
		self.forward = forward
		self.survey = survey
		self.past_mode = None

		# mode -2 first_forward first_survey forward survey forward reverse_survey -2 -1 0 1 2
		self.mode = -2;
		self.log_command = log( "zeabus_planner" , "log" , "02_drum_command" )
		self.log_vision = log( "zeabus_planner" , "log" , "02_drum_vision" )
		self.client_drum = rospy.ServiceProxy('vision_drum' , vision_srv_drum )
		self.auv = control_auv( "play drum")

		self.result_vision = { "n_obj" : -1 , "cx" : 0 , "cy" : 0 , "left" : False 
								,	"right" : False , "forward" : False , "backward" : False 
								,	"area" : 0 }

		self.collect_vision = { "n_obj" : -1 , "cx" : 0 , "cy" : 0 , "left" : False 
								,	"right" : False , "forward" : False , "backward" : False 
								,	"area" : 0 }

	def survey_mode( self ):
		print("Now Survey Mode" )
		self.distance = 0
		if( self.mode == -2 ):	
			self.distance = self.first_forward 
		elif( self.mode == -1 ):
			self.distance = self.first_survey
		elif( self.mode == 0 ):
			self.distance = self.forward
		elif( self.mode in [ 1 , 2 ]):
			self.distance = self.survey

		if( self.mode in [-2 , 0 ] ):
			self.log_command.write("Move forward distance is " + str( self.distance ) , True , 0)
			print("\t Move Forward distance is " + str( self.distance))
		elif( self.mode in [-1 , 1 ] ):
			self.past_mode = 1
			self.log_command.write("Move left distance is " + str( self.distance ) , True , 0)
			print("\t Move Left distance is " + str( self.distance))
		elif( self.mode in [-1 , 1 ] ):
		else:
			self.past_mode = 2
			self.log_command.write("Move right distance is " + str( self.distance ) , True , 0)
			print("\t Move Right distance is " + str( self.distance))
		target_found = 2
		count_found = 0		
# move survey and try to find object
		while( not rospy.is_shutdown() and self.auv.calculate_distance() < self.distance ):
			if( self.mode in [-2 , 0 ] ):
				self.auv.velocity( x = 0.15 )
				print( "move_forward : " + str( self.auv.calculate_distance() ) )
			elif( self.mode in [-1 , 1 ]):
				self.auv.velocity( y = 0.2 )
				print( "move_left : " + str( self.auv.calculate_distance() ) )
			else:
				self.auv.velocity( y = -0.2 )
				print( "move_right : " + str( self.auv.calculate_distance() ) )
			self.rate.sleep()
			self.analysis_all( 5 )
			if( self.result_vision["n_obj"] == 1):
				print("Found Count is " + count_found )
				count_found += 1
				if( count_found == target_found ):
					print("Found Change Mode to use")
					break
			else:
				if( count_found > 0 ):
					print("Now Don't Found")
					count_found = 0
# finish survey decision to change mode
		if( count_found == target_found ):
			print("Found equal target_found")
			self.move_to_center()
		else:
			if( self.mode == -2 ):
				self.mode = -1
			elif( self.mode == -1 ):
				self.mode = 0
			elif( self.mode == 0 ):
				if( self.past_mode == 1 ):
					self.mode = 2
				elif( self.past_mode == 2):
					self.mode = 1
			else( self.mode in [1 , 2]):
				self.mode = 0
			self.survey_mode()

	def move_to_center( self ):
		print("Now I want to move to center")
		self.log_command.write("Now Last mode move to center of drum" , True , 0 )
		count_unfound = 0
		while( not rospy.is_shutdown() ):
			self.rate.sleep()
			self.analysis_all()
			if( self.result_vision["n_obj"] != 1):
				print("Why not found in this mode")
				count_unfound += 1
				if( count_unfound == 5):
					print("I don't play I not found 5 round")
					break
			else:
				if( abs(self.result_vision['cx']) < 0.1 ):
					print("cx is ok move forward or backward")
					if( abs(self.result_vision['cy'] < 0.15) ):
						print("\tcy is ok too")
						self.auv.absolute_depth( -4.3 )
						print("Ok go to depth")
						while( not rospy.is_shutdown() and not self.auv.ok_position("z" , 0.05)):
							slef.rate.sleep()
						print("Finish Game")
						break
					elif( self.result_vision['cy'] < 0 ):
						print("\tmove backword")
					else:
						print("\tmove forward")
				elif( self.result_vision['cx'] < 0):
					print("Last mode move left")
					self.auv.velocity( y = 0.12)
				else:
					print("Last mode move right")
					self.auv.velocity( y = -0.12)
		print("Try to drop ball")

	def analysis_all( self , amont ):
		self.reset_vision_data()
		found = 0
		unfound = 0
		self.log_vision.write("Want to find Drum" , True , 0 )
		while( found < amont and unfound < amont ):
			if( collect_vision[ "n_obj"] < 1 ):
				self.log_vision.wirte("Not Found" , False ,1 )
				unfound += 1
			else:
				self.log_vision.write("Found Result is " + self.print_result() , False , 1)
				found += 1
		if( found == amont):
			print( "The Result of Vision is Found! result is " + self.print_result()  )
			self.result_vision = self.collect_vision
			self.result_vision["cx"] /= amont
			self.result_vision["cy"] /= amont
			self.result_vision["area"] /= amont
		else:
			print( "The Result of Vision is UnFound!!!")
			self.result_vision["n_obj"] = 0
			
	def print_result( self ):
		return "[ " + str( collect_vision["left"] ) + " , " + 
				str( collect_vision["right"] ) + " , " + 
				str( collect_vision["forward"] ) + " , " + 
				str( collect_vision["backward"]) + " ]"

	def individual_analysis( self ):
		temporary = self.client_drum( String("drum") , String("blue")).data
		if( temporary.n_obj == 1 ):
			collect_vision[ "n_obj" ] = temporary.n_obj
			collect_vision[ "cx" ] += temporary.cx
			collect_vision[ "cy" ] += temporary.cy
			collect_vision[ "left" ] = temporary.left
			collect_vision[ "right" ] = temporary.right
			collect_vision[ "forward" ] = temporary.forward
			collect_vision[ "backward" ] = temporary.backward
			collect_vision[ "area" ] += temporary.area
		else:
			collect_vision[ "n_obj" ] = temporary.n_obj

	def reset_vision_data( self ):
		self.result_vision = { "n_obj" : -1 , "cx" : 0 , "cy" : 0 , "left" : False 
								,	"right" : False , "forward" : False , "backward" : False 
								,	"area" : 0 }

		self.collect_vision = { "n_obj" : -1 , "cx" : 0 , "cy" : 0 , "left" : False 
								,	"right" : False , "forward" : False , "backward" : False 
								,	"area" : 0 }
