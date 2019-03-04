#!/usr/bin/python2
#################################################################################################
####
####	FILE		: SAUVC_pick_center.py
####	Maintain	: Supasan Komonlit
####	Create on	: 2019 , FEB 28
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

#===============> EXPLAIN CODE	
#	This code will want to use position of golf in frame 
#	Make that position to target and We straight down to collect
#	Don't use explicit from left-right of drum

class MissionPick( StandardMission ):

	def __init__( self , name ):
		self.name = name
	
		StandardMission.__init__( self , self.name , "/mission/pick" , self.callback )

		self.vision = VisionCollector( "drum" )

		self.echo( self.name , "PICK GOLF SETUP FINISHED")

		self.state = False

	def callback( self , request ):

		result = False

		self.state = request.data
		if( not self.ok_state( )):
			return False

		self.fix_z( -0.5 )
		self.reset_target("xy")
		self.wait_state( "z" , 0.1 , 5 )

		found_object = self.step_01()

		if( not found_object ):
			self.echo( self.name , "Don't found object mission aborted")
			return False
		else:
			self.step_02()
			result = True

		return result
		

	def step_01( self ): # find golf in a drum
		found_golf = False
		mode_control = 1 # mode control is survey around drum
						# 1 is left  2 is right 3 is up 4 is down
		value_x = 0 
		value_y = 0

		count_change_mode = 0
		count_found_golf = 0
		limit_mode = 10
		self.echo( self.name , "We command to hold for pick golf")
		self.hold_golf()
		self.echo( self.name , "We command finish")
		while( self.ok_state() ):
			self.sleep( 0.1 )
			if( mode_control == 1 ):
				self.vision.analysis_all( "blue" , "left-right" , 5 )
				self.echo_vision( self.vision.echo_special() )
				if( self.vision.result["cx_1"] < -0.5 ):
					value_y = 0.1
				elif( self.vision.result["cx_1"] > 0.0 ):
					value_y = -0.1
				else:
					value_y = 0
				if( abs( self.vision.result["cy_1"] ) < 0.5 ):
					value_x = 0
				else:
					value_x = math.copysign( 0.05 , self.vision.result["cy_1"] )
			elif( mode_control == 2 ):
				self.vision.analysis_all( "blue" , "left-right" , 5 )
				self.echo_vision( self.vision.echo_special() )
				if( self.vision.result["cx_2"] > 0.5 ):
					value_y = -0.05
				elif( self.vision.result["cx_2"] < 0.0 ):
					value_y = 0.05
				else:
					value_y = 0
				if( abs( self.vision.result["cy_2"] ) < 0.5 ):
					value_x = 0
				else:
					value_x = math.copysign( 0.05 , self.vision.result[ "cy_2" ] )	
			self.echo( self.name + " Mode " + str( mode_control ) , 
					"We command velocity x : y --> " + str( value_x ) + " : " + str( value_y ) )
			self.velocity( {'x' : value_x , 'y' : value_y } )
			self.sleep( 0.1 )
			self.vision.analysis_all( "golf" , "sevinar" , 5 )
			self.echo_vision( self.vision.echo_data() )
			# This part will discus about data from vision
			if( self.vision.have_object() ):
				count_found_golf = 0
				self.echo( self.name , "Find golf move to center of golf")
				while( self.ok_state() and abs(count_found_golf) > 3 ):
					self.sleep(0.1)
					self.vion.analysis_all( "golf" , "sevinar" , 5 )
					if( self.vision.have_object() ):
						count_found_golf += 1
						self.velocity( { 'x' : math.copysign( 0.1 , self.vision.center_y())
								, 'y' : math.copysign( 0.1 , -1 * self.vision.center_x() ) } )
					else:
						count_found_golf -= 0
				if( count_found_golf == 3 ):
					return True

			if( value_x == 0 and value_y == 0 ):
				count_change_mode += 1
				self.echo( self.name , "count_change_mode " + str( count_change_mode ))
				if( count_change_mode == limit_mode ):
					mode_control += 1
					count_change_mode = 0
					if( mode_control > 2 ):
						return False
		print("Finish loop")

		def step_02( self ): # this part we will pick a ball
			self.echo( self.name , "We will start to pick ball")
			self.fix_z( -1.1  )
			count_ok_depth = 0 
			value_x = 0
			value_y = 0
			while( self.ok_state() ):
				self.vision.analysis_all( "golf" , "sevinar" , 5 )
				self.echo_vision( self.vision.echo_data() )
				if( self.vision.center_x() < -0.5 ): value_y = -0.08
				elif( self.vision.center_x() > -0.1 ): value_y = 0.08
				else: value_y = 0
				if( self.vision.center_y() < 0.1 ): value_x = 0.08
				elif( self.vision.center_y() > 0.5 ): value_x = -0.08
				else: value_x = 0
				self.echo( self.name + " vel x : y is" +str( value_x ) +" : " +str( value_y ) )
				self.velocity( {'x' : value_x , 'y' : value_y } )
				
				if( self.check_position( "z" , 0.15 ) ):
					count_ok_depth += 1
				else:
					count_ok_depth = 0
				if( value_x == 0 and value_y == 0 ):
					self.echo( self.name , "Target on lock" )
					if( count_ok_depth > 5 ):
						self.echo( self.name , "We will move down now ")
						start_time = time.time()
						diff_time = 0
						self.velocity_z( -0.1 )
						self.free_xy( True )
						while( self.ok_state() and diff_time < 10 ):
							self.sleep( 0.1 )
							self.reset_target( "xy" )
							diff_time = time.time() - start_time
							self.echo( self.name , "Now time is " + diff_time )
						self.reset_velocity( "z" )
						self.fix_z( -0.5 )
						self.free_xy( False )
						break
			self.echo( self.name , "Finish pick golf it ok?")
			
			

if __name__=="__main__":
	rospy.init_node( "mission_pick" )
	MP = MissionPick( "mission_pick" )
	rospy.spin()
