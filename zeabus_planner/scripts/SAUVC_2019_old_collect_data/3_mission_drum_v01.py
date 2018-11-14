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
		self.mission_planner = rospy.Service('/mission_drum' , mission_result , self.main_play)

	def main_play( self , request ):
		self.current_depth = self.auv.receive_target('z')[0]
		self.sucess_mission = False
		count_down = 0
		print( "<--- MISSION DRUM ---> Start depth at " + str( self.current_depth))
		while( not rospy.is_shutdown() ):
			self.rate.sleep()
			self.analysis_all( 5)
			if( self.current_depth < self.max_depth + 0.5 ):
				last_result = self.check_type_vision()	
				if( last_type == 1):
					print("<--- MISSION DRUM ---> OK Release ball")
					self.sucess_mission = True
					break
				elif( last_result == 2):
					print("<--- MISSION DRUM ---> Move Right")
					self.auv.velocity( "y" , -0.3)
				elif( last_result == 3):
					print("<--- MISSION DRUM ---> Move BACK")
					self.auv.velocity( "x" , -0.25 )
				elif( last_result == 4):
					print("<--- MISSION DRUM ---> Move FORWARD")
			elif( self.auv.ok_state( 'z' , 0.15 ) ):
				count_down = 0
				last_result = self.check_type_vision()
				if( last_result in [1 , 2 ] ):
					print("<--- MISSION DRUM ---> GO Deeper" )
					self.auv.relative_z( -0.25 )
				elif( self.result_vision['right'] ):
					print("<--- MISSION DRUM ---> Move right for center" )
					self.auv.velocity( 'y' , -0.3 )
				else:
					print("I don't know")
					if( abs(self.result_vision['cy']) < 0.05 ):
						print("====>but cy is ok go deep ")
						self.auv.relative_z( -0.2 )
					else:
						self.auv.velocity( 'x' 
									, math.copysign( 0.3 , self.result_vision( 'cy' ) ))
			else:
				count_down += 1
				if( count_down > 45 ):
					print( "<--- I Think it long time release--->")
					self.sucess_mission = True
					break
						
		return Bool( self.sucess_mission ) , Int8( 1 ) 
		
	# type 1 False only Right 2 True only Top and Bottom and Right
	def check_type_vision( self ):
		if( self.result_vision['forward']	and self.result_vision['backward'] ):
			if( self.result_vision['right'] ):
				return 2
			else:
				return 1
		elif( self.result_vision['forward']):
			return 3
		elif( self.result_vision['backward']):
			return 4
		elif( self.result_vision['right']):
			return 5
		else:
			return 6

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
			
	def individual_analysis( self ):
		temporary = self.request_data( String("drum") , String("drop")).data
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
