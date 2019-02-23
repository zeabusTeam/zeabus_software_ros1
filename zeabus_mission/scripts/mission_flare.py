#!/usr/bin/python2
#################################################################################################
####
####	FILE		: mission_flare.py
####	Maintain	: Supasan Komonlit
####	Create on	: 2019 , FEB 23
####	Purpose		: For competition in SAUVC2019
####
#################################################################################################
from __future__			import print_function
#===============>

import rospy
import math
import time 

from vision_collector		import VisionCollector

from standard_mission		import StandardMission

class MissionGate( StandardMission ):

	def __init__( self , name ):
		self.name = name

		StandardMission.__init__( self , self.name , "/mission/flare" , self.callback )

		self.vision = VisionCollector("flare")

		print("FLARE FINISHED SETUP")

	def callback( self , message): # Divide to 2 part move to center and direction

		result = False

		self.echo( self.name , "START MISSION FLARE")

		result = self.step_01()

		if( result ): result = self.step_02()

		return result

	def step_01( self ):  # play in far mode and try to find in near mode
		self.echo( self.name , "PLAY STEP ONE TO FIND FLAR IN FAR MODE")
		count_unfound = 0
		while( not rospy.is_shutdown() ):
			self.sleep( 0.1 )
			self.vision.analysis_all( "flare" , "far" , 5 )
			self.echo_vision( self.vision.echo_data() )	
			if( self.vision.have_object() ):
				count_unfound = 0
				if( abs(self.vision.center_x() ) < 0.3 ):
					self.velocity( {'x' :  0.1 } )
					self.echo( self.name , "FARMODE Move FORWARD")
				elif( self.vision.center_x() < 0 ):
					self.velocity( {'y' :  0.08 } )
					self.echo(  self.name ,"FARMODE MOVE LEFT")
				elif( self.vision.center_x() > 0 ):
					self.velocity( {'y' : -0.08 } )
					self.echo(  self.name ,"FARMODE MOVE RIGHT")
				else:
					self.echo(  self.name ,
							"\tERROR IN LINE 59 =====================================")

			else:
				count_unfound += 1
				self.echo( self.name , "WARNING UNFOUND : " + str( count_unfound ) )
				if( count_unfound == 5 ):
					break
			self.vision.analysis_all( "flare" , "near" , 5 )
			if( self.vision.have_object() ):
				self.echo( self.name , "FOUND IN NEAR MODE" )
				self.reset_target( "xy" )
				break

		if( count_unfound == 5 ):
			return False
		else:
			return True
			
	def step_02( self ): # play in near mode
		self.echo( self.name , "PLAY STEP TWO TO FIND FLAR AND TARGET DASH")
		count_unfound = 0 # this variable is about picture
		count_ok = 0 # this variable is about position
		haved_reset_target = False
		while( not rospy.is_shutdown() ):
			self.sleep( 0.1 )
			self.vision.analysis_all( "flare" , "near" , 5 )
			self.echo_vision( self.vision.echo_data() )
	
			if( self.vision.have_object() ):
				count_unfound = 0
				if( abs(self.vision.center_x() ) < 0.5 ):
					if( not haved_reset_target ): 
						self.reset_target("xy")
						self.echo( self.name , "NEARMODE Renew target")
					self.echo( self.name , "NEARMODE That center we will move direct if we can")
					if( self.check_position( "xy" , 0.05 ) ):
						count_ok += 1
						if( count_ok == 5 ):
							self.velocity_xy( 0.2 , 0 )
							self.echo( self.name , "NEARMODE GO DIRECT")	
							break
					else:
						count_ok = 0
				elif( self.vision.center_x() < 0 ):
					self.velocity( { 'y' : 0.05 })
					self.echo( self.name , "NEARMODE MOVE LEFT")
				elif( self.vision.center_x() > 0 ):
					self.velocity( { 'y' : -0.05 })
					self.echo( self.name , "NEARMODE MOVE RIGHT")
				else:
					self.echo( self.name ,  
							"\tERROR IN LINE 112 =====================================")
			else:
				count_unfound += 1
				if( count_unfound == 5 ):
					break

		while( not rospy.is_shutdown() and count_unfound < 5 ):
			self.sleep( 0.1)
			self.vision.analysis_all( "flare" , "near" , 5 )
			self.echo_vision( self.vision.echo_data() )
			if( self.vision.have_object() ):
				count_unfound = 0
			else:
				count_unfound += 1

		self.fix_z(-0.1)

		return True
					
if __name__ == "__main__" :
	rospy.init_node("mission_flare")
	MG = MissionGate( "mission_flare")
	rospy.spin()	
