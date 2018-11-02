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

class play_gripper:

	def __init__( self , rate ):
		self.auv = control_auv("play_gripper")
		self.rate = rospy.Rate( rate )
		self.client_drum = rospy.ServiceProxy('vision_pick' , vision_srv_drum )
		self.log_command = log( "zeabus_planner" , "log" , "04_gripper_command" )
		self.log_vision = log( "zeabus_planner" , "log" , "04_gripper_vision" )

	def reset_vision_data( self ):
		self.result_vision = { "n_obj" : -1 , "cx" : 0 , "cy" : 0 , "left" : False 
								,	"right" : False , "forward" : False , "backward" : False 
								,	"area" : 0 }

		self.collect_vision = { "n_obj" : -1 , "cx" : 0 , "cy" : 0 , "left" : False 
								,	"right" : False , "forward" : False , "backward" : False 
								,	"area" : 0 }

	def play( self ):
		print("I will play for see ball before disappear")
		while( not rospy.is_shutdown() ):
			self.rate.sleep()
			self.analysis_all( 5 )
			if( self.result_vision['n_obj'] == 1 ):
				if( self.result_vision['cx'] <= -0.08 ):
					print("\tMove right")
					self.auv.velocity( y = -0.1 )
				elif( self.result_vision['cx'] > 0.3):
					print("\tMove left")
					self.auv.velocity( y = 0.1 )
				else:
					print("\tI agree to move forward")
					self.auv.velocity( x = 0.1 )
					while( not rospy.is_shutdown() and self.result_vision['cy'] < -0.5 ):
						self.rate.sleep()
						self.auv.velocity( x = 0.1 )
						print("\t\t Now cy is " + str( self.result_vision['cy']))
						self.rate.sleep()
						print("\t\t wait yaw")
						while( not rospy.is_shutdown() and not self.auv.ok_position( "yaw",0.1)):
							self.rate.sleep()
						
					print("Ok let ball disapear")	


	def analysis_all( self , amont ):
		self.reset_vision_data()
		found = 0
		unfound = 0
		self.log_vision.write("Want to find Drum" , True , 0 )
		while( found < amont and unfound < amont ):
			self.individual_analysis()
			if( self.collect_vision[ "n_obj"] < 1 ):
				self.log_vision.write("Not Found" , False ,1 )
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
		return( "[ " + str( self.collect_vision["left"] ) + " , " + 
				str( self.collect_vision["right"] ) + " , " + 
				str( self.collect_vision["forward"] ) + " , " + 
				str( self.collect_vision["backward"]) + " ]")

	def individual_analysis( self ):
		temporary = self.client_drum( String("drum") , String("drop")).data
		if( temporary.n_obj == 1 ):
			self.collect_vision[ "n_obj" ] = temporary.n_obj
			self.collect_vision[ "cx" ] += temporary.cx
			self.collect_vision[ "cy" ] += temporary.cy
			self.collect_vision[ "left" ] = temporary.left
			self.collect_vision[ "right" ] = temporary.right
			self.collect_vision[ "forward" ] = temporary.forward
			self.collect_vision[ "backward" ] = temporary.backward
			self.collect_vision[ "area" ] += temporary.area
		else:
			self.collect_vision[ "n_obj" ] = temporary.n_obj
