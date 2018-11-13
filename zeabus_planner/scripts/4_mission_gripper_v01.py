#!/usr/bin/python2

import rospy
import math

from auv_controller import auv_controller 

from zeabus_vision.srv import vision_srv_drum
from zeabus_vision.msg import vision_drum
from std_msgs.msg import String , Bool , Int8
from zeabus_planner.srv import mission_result

class mission_drum:

	def __init__( self ):

		# this object will init node when sencond argument is True
		self.auv = auv_controller("mission_drum" , True )

		self.result_vision = { "n_obj" : -1			, "area" : None
								, "cx" : 0			, "cy" : 0 
								, "left" : False	,	"right" : False 
								, "forward" : False , "backward" : False 
							}

		self.collect_vision = { "n_obj" : -1		, "area" : None
								, "cx" : 0			, "cy" : 0 
								, "left" : False	,	"right" : False 
								, "forward" : False , "backward" : False 
							}

		self.data_vision	= { "n_obj" : -1			, "area" : None
								, "cx" : 0			, "cy" : 0 
								, "left" : False	,	"right" : False 
								, "forward" : False , "backward" : False 
							}

		print("Waiting drum service")
		rospy.wait_for_service('vision_drum')
		print("I found drum service")

		self.max_depth = -5
		self.last_count = 60 # chose have value equal frequency * 2
		self.rate = rospy.Rate( 30 )
		self.request_data = rospy.ServiceProxy('/vision_drum' , vision_srv_drum )
		self.mission_planner = rospy.Service('/mission_gripper' , mission_result , self.main_play)

	def main_play( self , request ):
		self.current_depth = self.auv.receive_target('z')[0]
		self.go_depth = False
		self.sucess_mission = False
		print("SetMode Control")
		self.auv.set_mode( 1 ) # 1 is not fix x and y and use velocity type open loop
		self.auv.absolute_z( -1.3 )
		print("Waiting Depth")
		while( not rospy.is_shutdown() and not self.ok_state('z' , 0.1) ):
			self.rate.sleep()
		self.prepare_pick()
		self.count = 0
		if( self.go_depth ):
			self.auv.absolute_z( -2 )
			while( not rospy.is_shutdown() and self.count < 120 ):
				self.rate.sleep()
		return Bool( self.sucess_mission ) , Int8( 1 ) 
		
	def prepare_pick( self ):
		print("<= MISSION PICK =>" , " PREPARE TO PICK " , "MOVE CENTER PICTURE X")
		self.move_down = False
		while( not rospy.is_shutdown() ):
			self.analysis_all( 5 ):
			self.rate.sleep()
			if( self.result_vision['n_obj'] == 0 ):
				break
			if( abs( self.result_vision['cx'] ) < 0.05 ):
				self.move_down = True
				break
			elif( self.result_vision > 0 ):
				print("Move Right")
				self.auv.velocity( "y" , -0.5 )
			else:
				print("Move Left")
				self.auv.velocity( "y" , 0.5 )
		print("<= MISSION PICK =>" , "PREPARE TO PICK" , "MOVE DOWN PICTUR Y")
		self.move_disapper = False
		while( not rospy.is_shutdown() and self.move_down ):
			self.analysis_all( 5)
			self.rate.sleep()
			if( self.result_vision['n_obj'] == 0):
				break
			if( self.result_vision['cy'] > -0.8 and self.result_vision['cy'] < -0.5 ):
				self.move_disapper = True
				break
			elif( self.result_vision['cy'] => -0.5 ):
				print("Move Forware")
				self.auv.velocity('x' , -0.3 )
			else:
				print("Move Down")
				self.auv.velocity('x' , 0.6 )
		self.go_depth = False
		print("<= MISSION PICK =>" , "PREPARE TO PICK" , "MOVE DOWN DiSAPPER")
		while( not rospy.is_shutdown() and self.mode_disapper ):
			self.rate.sleep()
			self.auv.velocity('y' , 0.4 )
			self.analysis_all( 5 )
			if( self.result_vision['n_obj'] == 0):
				self.go_depth = True
				break
		print("OK Let GO") 
			
			

	def analysis_all( self , amont ):
		self.reset_vision_data()
		found = 0
		unfound = 0
		while( found < amont and unfound < amont ):
			self.individual_analysis()
			if( self.collect_vision[ "n_obj"] < 1 ):
				unfound += 1
			else:
				found += 1
		if( found == amont):
			self.result_vision = self.collect_vision
			self.result_vision["cx"] /= amont
			self.result_vision["cy"] /= amont
			self.result_vision["area"] /= amont
			self.result_vision["left"] = self.collect_vision["left"]
			self.result_vision["right"] = self.collect_vision["right"]
			self.result_vision["forward"] = self.collect_vision["forward"]
			self.result_vision["backward"] = self.collect_vision["backward"]
		else:
			self.result_vision["n_obj"] = 0
		print( "Result Vision " , self.result_vision )
			
	def individual_analysis( self ):
		temporary = self.request_data( String("drum") , String("pick")).data
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

	def reset_vision_data( self ):
		self.collect_vision = { "n_obj" : -1 , "cx" : 0 , "cy" : 0 , "left" : False 
								,	"right" : False , "forward" : False , "backward" : False 
								,	"area" : 0 }
if __name__=="__main__":
	mission_03 = mission_drum()
	rospy.spin()
