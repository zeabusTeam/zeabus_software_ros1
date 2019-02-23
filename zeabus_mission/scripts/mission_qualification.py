#!/usr/bin/python2
#################################################################################################
####
####	FILE		: mission_qualification.py
####	Maintain	: Supasan Komonlit
####	Create on	: 2019 , FEB 23
####	Purpose		: For qualification round in SAUVC2019
####
#################################################################################################
from __future__			import print_function
#===============>

import rospy
import math
import time 

from vision_collector		import VisionCollector

from standard_mission		import StandardMission

class MissionQualification( StandardMission ):

	def __init__( self , name ):
		self.name = name

		StandardMission.__init__( self , self.name , "/mission/qualification" , self.callback )

		self.vision = VisionCollector("qualification")

		print("QUALIFICATION FINISHED SETUP")

	def callback( self , message): # Divide to 2 part move to center and direction

		result = False

		self.reset_target( "xy" )
		self.reset_target( "yaw" )
		self.fix_z( -0.5 )

		self.echo( self.name , "START MISSION QUALIFICATION")

		self.collect_state()
		self.velocity_xy( 0.2 , 0 )
		while( not rospy.is_shutdown() ):
			self.sleep( 0.1 )
			self.vision.analysis_all( "qualification" , "sevinar" , 5 )
			if( self.vision.have_object() ):
				self.echo( self.name , "Now have object ")
				break
			else:
				self.echo( self.name , "Never found and distance is " + str( self.distance()) )
		self.reset_velocity("xy")
		self.run_type = 1		

		while( not rospy.is_shutdown() and not result ):
			if( self.run_type == 0 ): break
			elif( self.run_type == 1 ): self.move_forward()
			elif( self.run_type == 2 ): self.move_center()
			elif( self.run_type == 3 ): result = self.last_move()
			elif( self.run_type == 4 ): result = self.relative_move()
		
		return result 
			
	def move_forward( self ):
		self.velocity_xy( 0.2 , 0 )
		count_have_object = 0
		count_out_center = 0
		self.run_type = 0
		while( not rospy.is_shutdown() ):
			self.sleep(0.05)
			self.vision.analysis_all( "qualification" , "sevinar" , 5 )
			self.echo_vision( self.vision.echo_data() )
			if( self.vision.num_object() == 2):
				count_have_object = 0
				if( self.vision.center_x() < 0.2 ):
					self.echo( self.name , "MODE_FORWARD Move forward ok center")
					count_out_center = 0
				else:
					count_out_center += 1
					self.echo( self.name , "MODE_FORWARD Out center : " + 
							str( count_out_center ) )
				if( count_out_center == 5 ):
					self.run_type = 2
					break
				if( self.vision.area() > 0.5 ):
					self.run_type = 3
					break
			elif( self.vision.num_object() == 1 ):
				self.run_type = 4
				break
			else:
				count_have_object += 1
				self.echo( self.name , "MODE_FORWARD Count don't have object is " + 
						str( count_have_object ))
				if( count_have_object == 5 ):
					self.run_type = 3
					break
		self.reset_velocity( "xy" )	

	def move_center( self ):
		count_have_object = 0
		while( not rospy.is_shutdown() ):
			self.sleep(0.05)
			self.vision.analysis_all( "qualification" , "sevinar" , 5 )
			self.echo_vision( self.vision.echo_data() )
			if( self.vision.num_object() == 2 ):
				count_have_object = 0
				if( self.vision.center_x() < 0.2 ):
					self.run_type = 1
					break
				elif( self.vision.center_x() < 0 ):
					self.velocity( {'y' , 0.1 } )
					self.echo( self.name , "Now we move Left")
				elif( self.vision.center_x() > 0 ):
					self.velocity( {'y' , -0.1 } )
					self.echo( self.name , "Now we move Right")
			elif( self.vision.num_object() == 1 ):
				self.run_type = 4
				break
			else:
				count_have_object += 1
				self.echo( self.name , "Count don't have object is " + str( count_have_object))
				if( count_have_object == 5 ):
					self.run_type = 3
					break

	def relative_move( self ):
		self.vision.analysis_all( "qualification" , "servinar" , 5 )
		if( self.vision.center_x() < 0 ):
			self.echo( self.name , "Last move relative RIGHT")
			self.relative_xy( 3 , -1 )
		else:
			self.echo( self.name , "Last move relative LEFT")
			self.relative_xy( 3 , 1 )
		return True

	def last_move( self ):
		self.echo( self.name ,  "Last move forward by velocity_xy 0.3")
		self.velocity_xy( 0.3 , 0 )
		return True
			

					
if __name__ == "__main__" :
	rospy.init_node("mission_qualification")
	MG = MissionQualification( "mission_qualification")
	rospy.spin()	
