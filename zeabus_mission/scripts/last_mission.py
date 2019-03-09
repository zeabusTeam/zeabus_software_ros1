#!/usr/bin/python2
#################################################################################################
####
####	FILE		: mission_gate.py
####	Maintain	: Supasan Komonlit
####	Create on	: 2019 , MAR 09
####	Purpose		: For competition in SAUVC2019 about mission gate and vision don't have
####				  affect by reflect	
#################################################################################################
from __future__			import print_function
#===============>

import rospy
import math
import time 

from vision_collector		import VisionCollector

from standard_mission		import StandardMission

# Line 46 assign direction of left or right : negative is right postion is left
# mission gate function find_gate , play_gate
# mission drum function find_mat , find_drum , play_drum
# mission ball function play_pick , pick_ball , move_to_center , step_01

class Mission( StandardMission ):

	def __init__( self , name ):
		self.name = name 

		StandardMission.__init__( self, self.name , "/mission/all" , self.callback )

		self.vision_gate = VisionCollector( "gate" )
		self.vision_drum = VisionCollector( "drum")

		self.echo( self.name , "Mission Gate Setup Finish")

		self.state = False

		self.start_time = time.time()

	def callback( self , request ):

		result = False

		self.target_drum = "blue"

		self.state = request.data
		if( not self.ok_state() ):
			return False

		self.reset_target( "yaw" )
		self.reset_target( "xy" )
		self.target_state()
		self.start_yaw = self.temp_state[ 5 ]
		self.fix_z( -1 )

		self.wait_state( "z" , 0.15 , 5 )	

		self.echo( self.name , "START MISSION GATE at start yaw is " + self.start_yaw )

		result = self.find_gate( -0.1 )
		
		if( result ): result = self.find_mat( 1 )

		if( result ): result = self.play_pick()

#		if( result ): result = self.find_flare()

	def over_time( self , distance , time , setup = False )
		if( setup ):
			self.start_time = time.time()
			self.collect_state()
			return True
		else:
			diff_time = time.time() - self.start_time
			distance = self.distance()
			self.echo( self.name , "Distance : time are " + str( distance ) + 
					" : " + str( diff_time ) )
			if( self.distance() < distance and diff_time < time):
				return True
			else:
				return False

	def move_to_center( self ):
		self.fix_z( -0.8 )
		value_x = 0
		value_y = 0
		while( self.ok_state() ):
			self.sleep( 0.05 )
			self.vision_drum.analysis_all( self.target_drum , "center" , 5 )
			if( abs( self.vision_drum.center_x() ) < 0.2 ): value_y = 0
			else: value_y = math.copysign( 0.1 , self.vision_drum.center_x() * -1 )
			if( abs( self.vision_drum.center_y() ) < 0.2 ): value_x = 0
			else: value_x = math.copysign( 0.1 , self.vision_drum.center_y() )
			self.velocity_xy( { 'x' : value_x , 'y' : value_y } )
			if( self.check_position( "z" , 0.1 ) ):
				break

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
				self.vision_drum.analysis_all( self.target_drum , "left-right" , 5 )
				self.vision_drum( self.vision_drum.echo_special() )
				if( self.vision_drum.result["cx_1"] < -0.5 ):
					value_y = 0.1
				elif( self.vision_drum.result["cx_1"] > 0.0 ):
					value_y = -0.1
				else:
					value_y = 0
				if( abs( self.vision_drum.result["cy_1"] ) < 0.5 ):
					value_x = 0
				else:
					value_x = math.copysign( 0.05 , self.vision_drum.result["cy_1"] )
			elif( mode_control == 2 ):
				self.vision_drum.analysis_all( self.target_drum , "left-right" , 5 )
				self.vision_drum( self.vision_drum.echo_special() )
				if( self.vision_drum.result["cx_2"] > 0.5 ):
					value_y = -0.05
				elif( self.vision_drum.result["cx_2"] < 0.0 ):
					value_y = 0.05
				else:
					value_y = 0
				if( abs( self.vision_drum.result["cy_2"] ) < 0.5 ):
					value_x = 0
				else:
					value_x = math.copysign( 0.05 , self.vision_drum.result[ "cy_2" ] )	
			self.echo( self.name + " Mode " + str( mode_control ) , 
					"We command velocity x : y --> " + str( value_x ) + " : " + str( value_y ) )
			self.velocity( {'x' : value_x , 'y' : value_y } )
			self.sleep( 0.1 )
			self.vision_drum.analysis_all( "golf" , "sevinar" , 5 )
			self.echo_vision( self.vision_drum.echo_data() )
			# This part will discus about data from vision_drum
			if( self.vision_drum.have_object() ):
				count_found_golf = 0
				self.echo( self.name , "Find golf move to center of golf")
				while( self.ok_state() and abs(count_found_golf) < 15 ):
					self.sleep(0.1)
					self.vision_drum.analysis_all( "golf" , "sevinar" , 5 )
					if( self.vision_drum.have_object() ):
						count_found_golf += 1
						self.velocity( { 'x' : math.copysign( 0.1 , self.vision_drum.center_y())
								, 'y' : math.copysign( 0.1 , -1 * self.vision_drum.center_x() ) } )
					else:
						count_found_golf -= 0
					self.echo( self.name , "Found golf is  " + str( count_found_golf ) )
				if( count_found_golf == 15 ):
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

	def pick_ball( self ):
		self.free_xy( True )
		self.fix_z( -1 )
		value_x = 0
		value_y = 0
		count_unfound = 0
		while( self.ok_state() ):
			self.sleep( 0.05 )
			self.vision_drum.analysis_all( "golf" , "sevinar" , 5 )
			if( self.vision_drum.have_object() ):
				count_unfound = 0
				if( abs( self.vision_drum.center_x() ) < 0.1 ): value_y = 0
				else: value_y = math.copysign( 0.2 , self.vision_drum.center_x() * -1 )
				if( abs( self.vision_drum.center_y() ) < 0.1 ): value_x = 0
				else: value_x = math.copysign( 0.2 , self.vision_drum.center_y() )
				if( value_x == 0 and value_y == 0 ):
					if( self.check_position( "z" , 0.1 ) ):
						self.echo( "BALL" , "Ball center and depth is ok")
						break
					else:
						self.echo( "BALL" , "Wait depth is ok")
				self.velocity( { 'x' : value_x , 'y' : value_y } )
			else:
				count_unfound += 1
				self.echo("BALL" ,  "Unfound" )
				if( count_unfound == 5 ):
					break
		start_time = time.time()
		self.velocity_z( -0.1 )
		while( self.ok_state() ):
			self.sleep( 0.05 )
			if( self.vision_drum.have_object() ):
				if( self.vision_drum.center_y()  < 0.15 ): value_x = 0.3
				elif( self.vision_drum.center_y() > 0.4 ): value_x = -0.3
				else: value_x = 0
				if( self.vision_drum.center_x()  > -0.4 ): value_y = 0.3
				elif( self.vision_drum.center_x() < -0.5  ): value_y = -0.3
				else: value_y = 0
				self.velocity( {'x' : value_x , 'y' : value_y } )
			else:
				self.velocity( {'x' : 0.05 , 'y' : 0.05 } )
	
	def play_pick( self ):
		self.fix_yaw( self.start_yaw )
		self.fix_z( -0.5 )
		self.wait_state( "yaw" , 0.1 , 4 )
		self.velocity_xy( -0.2 , 0 )
		self.echo( "PICK" , "I will move out from mat" )
		play_ball = False
		while( self.ok_state() ):
			self.sleep( 0.05 )
			self.vision_drum.analysis_all( "mat" , "sevinar" , 5 )
			self.echo_vision( self.vision_drum.echo_data() )
			if( not self.vision.have_object() ):
				break
		self.reset_velocity( "xy" )
		self.relative_xy( -0.5 , 0 )
		self.echo("PICK" , "Move backward")
		self.sleep( 1 )
		self.echo("PICK" , "Move forward")
		self.velocity_xy( 0.2 , 0 )
		while( self.ok_state() ):
			self.sleep( 0.05 )
			self.vision_drum.analysis_all( self.target_drum , "center" , 5 )
			self.echo_vision( self.vision_drum.echo_special() )
			if( self.vision_drum.have_object() ):
				self.move_to_center()
				self.hold_golf()	
				play_ball = True
				break
		self.reset_velocity( "xy" )
		self.echo( "PICK BALL" )
		if( play_ball ):
			if( self.step_01() ):
				return self.pick_ball()
		else: return False

	def play_drum( self ):
		self.velocity_z( -0.1 )
		start_time = time.time()
		limit_time = 6
		value_x = 0
		value_y = 0
		self.free_xy( True )
		while( self.ok_state() ):
			self.sleep( 0.05 )
			self.vision_drum.analysis_all( self.target_state , "center" , 5 )
			if( not self.vision_drum.result["right"] ): value_y = -0.2
			elif( not self.vision_drum.result["left"] ): value_y = 0.2 
			else: value_y = 0
			if( not self.vision_drum.result["forward"] ): value_x = 0.2
			elif( not self.vision_drum.result["backward"] ): value_x = -0.2 
			else: value_x = 0
			self.velocity( { 'x' : value_x , 'y' : value_y } )
			diff_time = time.time() - start_time
			self.echo( "DRUM" , "Now go down this time is " + str( diff_time ) )
			if(diff_time > limit_time ):
				self.fire_golf();
				break
		self.reset_velocity( "z" )
		self.fix_z( -0.8)
		while( self.ok_state() ):
			self.sleep( 0.05 )
			self.vision_drum.analysis_all( self.target_state , "center" , 5 )
			if( abs( self.vision_drum.center_x() ) < 0.2 ): value_y = 0
			else: value_y = math.copysign( 0.2 , self.vision_drum.center_x() ) * -1
			if( abs( self.vision_drum.center_y() < 0.2 ): value_x = 0
			else: value_x = math.copysign( 0.2 , self.vision_drum.center_y )
			self.velocity( {'x' : value_x , 'y' : value_y } )
			if( self.check_position("z" , 0.1 ) ):
				break
		self.reset_target("xy")
		self.free_xy( False )
		return True

	def find_mat( self , direction = 0):
		self.fix_z( -0.5 )
		self.wait_state( "z" , 0.1 , 5 )
		self.velocity_xy( 0.2 , 0 )
		while( self.ok_state() ): # I will find a mat before play drum
			self.sleep( 0.05 )
			self.vision_drum.analysis_all( "mat" , "sevinar" , 5 )
			if( self.vision_drum.have_object() ):
				self.echo( "DRUM" , "I will find mat move and find a drum")
				self.fire_gripper()
				break
		self.relative_xy( 0.1 , 0)
		self.reset_velocity( "xy" )
		if( self.ok_state() ):
			return self.find_drum( self , direction )

	def find_drum( self , direction = 0 ):
		self.fix_z( -0.9 )
		self.wait_state( "z" , 0.1 , 5 )
		self.wait_state( "xy" , 0.1 , 5 )
		value_x = 0 
		value_y = 0
		ever_play_drum = False
		count_unfound = 0
		if( direction == 0 ): direction = 1 # 1 is left
		while( self.ok_state() ):
			self.vision_drum.analysis_all( self.target_drum , "center" , 5 )
			self.sleep( 0.05 )
			if( self.vision_drum.have_object() ):
				self.echo_vision( self.vision_drum.echo_special() )
				self.echo( "DRUM" , "I find a blue drum" )
				if( abs(self.vision_drum.center_x()) < 0.2 ):
					value_y = 0
				else:
					value_y = math.copysign( 0.1 , self.vision_drum.center_x() * -1 )
				if( not self.vision_drum.result['forward'] ):
					value_x = -0.1
				elif( not self.vision_drum.result['backward'] ):
					value_x = 0.1
				else:
					value_x = 0
				self.velocity( { 'x' : value_x , 'y' : value_y } )
				if( value_x == 0 and value_y == 0 ):
					self.play_drum()
					ever_play_drum = True
					break
			else:
				self.echo_vision( self.vision_drum.echo_special() )
				self.vision_drum.analysis_all( "mat" , "sevinar" , 5 )
				self.velocity( { 'y' : math.copysign( 0.1 , direction ) )
				if( not self.vision_drum.have_object() ):
					count_unfound += 1
					if( count_unfound == 5 ): break
				else:
					count_unfound = 0
		if( ever_play_drum ):
			return True
		while( self.ok_state() ):
			self.vision_drum.analysis_all( "red" , "center" , 5 )
			self.sleep( 0.05 )
			if( self.vision_drum.have_object() ):
				self.echo_vision( self.vision_drum.echo_special() )
				self.echo( "DRUM" , "I find a red drum" )	
				if( abs(self.vision_drum.center_x()) < 0.2 ):
					value_y = 0
				else:
					value_y = math.copysign( 0.1 , self.vision_drum.center_x() * -1 )
				if( not self.vision_drum.result['forward'] ):
					value_x = -0.1
				elif( not self.vision_drum.result['backward'] ):
					value_x = 0.1
				else:
					value_x = 0
				self.velocity( { 'x' : value_x , 'y' : value_y } )
				if( value_x == 0 and value_y == 0 ):
					self.target_drum = "red"
					self.play_drum()
					ever_play_drum = True
					break
			else:
				self.echo_vision( self.vision_drum.echo_special() )
				self.vision_drum.analysis_all( "mat" , "sevinar" , 5 )
				self.velocity( { 'y' : math.copysign( 0.1 , direction ) * -1 } )
				if( not self.vision_drum.have_object() ):
					count_unfound += 1
					if( count_unfound == 5 ): break
					else: count_unfound = 0
		if( ever_play_drum ): return True
		else: return False
				
	def play_gate( self , direction ):
		self.vision_gate.analysis_all( "gate" , "sevinar" , 5 )
		play_two_pier = False
		count_found = 0
		self.echo( self.name , "We decision to play gate")
		if( self.vision_gate.num_object() == 1 ):
			self.echo( self.name , "I found one pier i will try to find two pier")
			while( self.ok_state() ):
				self.sleep( 0.05 )
				self.vision_gate.analysis_all( "gate" , "sevinar" , 5 )
				if( self.vision_gate.have_object() ):
					count_unfound = 0
					if( self.vision_gate.num_object() == 2 ):
						play_two_pier = True
						break
					else:
						self.echo( self.name , "move velocity direction is " + direction )
						self.velocity( { 'x' : 0.05 , 'y' : 0.1 * direction} )
				else:
					count_unfound += 1
					self.velocity( { 'x' : 0.05 , 'y' : 0.1 * direction} )
					self.echo( self.name , "I don't found gate when play_gate : " 
							+ str( count_unfound ) )
				if( count_unfound == 5 )
					break
		if( self.vision_gate.num_object() == 2 or play_two_pier ):
			self.echo( self.name , "I found two pier i will play to pier")
			current_fix_velocity = False
			count_unfound = 0
			while( self.ok_state() ):
				self.sleep( 0.05 )
				self.vision_gate.analysis_all( "gate" , "sevinar" , 5 )
				if( self.vision_gate.num_object() == 2 ):
					count_unfound = 0
					self.echo( "GATE" , "I still find two we will play")
					if( abs( self.vision_gate.center_x() ) < 0.15 ):
						if( not current_fix_velocity ):
							current_fix_velocity = True
							self.velocity_xy( 0.2 , 0 )
						self.echo("GATE" , "I still move forward")
					else:
						if( current_fix_velocity ):
							current_fix_velocity = False
							self.reset_velocity( "xy" )
						self.echo("GATE" , "I want to move to center" )
						self.velocity( { 'y' : math.copysign( 0.1 
								, self.vision_gate.center_x() * 1 ) } )
					distance_x = self.vision_gate.distance_x()
					self.echo( "GATE" , "Distance between pier is " + distance_x )
					if( distance_x > 1 ):
						if( current_fix_velocity ):
							current_fix_velocity = False
							self.reset_velocity( "xy" )
						break
				else:
					count_unfound += 1
					if( count_unfound == 5 ):
						self.echo( "GATE" , "I don't found object send process")
						break
		return True
					
						
				
	def find_gate( self , direction ):
		self.echo( self.name , "I will find gate" )
		self.over_time( 2 , 40 , True )
		self.velocity_xy( 0.2 , 0  )
		count_found = 0
		while( self.ok_state() and self.over_time( 2 , 40 ) ):
			self.sleep( 0.05 )
			self.vision_gate.analysis_all( "gate" , "sevinar" , 5 )
			if( self.vision.have_object() ):
				self.echo( self.name , "I found_object" ) 
				if( self.vision_gate.num_object() == 2 ):
					count_found = 5 
				else:
					count_found += 1
				if( count_found == 5 ):
					break
			else:
				count_found = 0
		self.reset_velocity( "xy" )
		if( count_found == 5 ):
			return self.play_gate( direction )
		self.over_time( 5 , 300  , True )
		count_found = 0
		self.velocity_xy( 0 , 0.2 * direction )
		while( self.ok_state() and self.over_time( 5 , 60 ) ):
			self.sleep( 0.05 )
			self.vision_gate.analysis_all( "gate" , "sevinar" , 5 )
			if( self.vision.have_object() ):
				self.echo( self.name , "I found_object" )
				if( self.vision_gate.num_object() == 2 ):
					count_found =5 
				else:
					count_found += 1
				if( count_found == 5 ):
					break
			else:
				count_found = 0
		self.reset_velocity( "xy" )
		if( count_found == 5 ):
			return self.play_gate( direction )
		return False
