#!/usr/bin/python2
#################################################################################################
####
####	FILE		: mission_gate.py
####	Maintain	: Supasan Komonlit
####	Create on	: 2019 , MAR 10
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

#===============>
# Code for mission gate have find_gate , play_gate
# Code for drop ball have find_drum , play_drum
# Code for pick ball have move_out_in , play_pick

class MissionAll( StandardMission ):
	
	def __init__( self , name ):
		self.name = name 

		StandardMission.__init__( self , self.name , "/mission/all" , self.callback )

		self.vision_gate = VisionCollector( "gate" )
		self.vision_flare = VisionCollector( "flare" )
		self.vision_drum = VisionCollector( "drum" )

		self.state = False

		self.echo( self.name , "ALL MISSION SETUP FINISHED")
		
		self.time_start = time.time()

	def callback( self , request ):
		
		result = False

		self.state = request.data 
		if( not self.ok_state() ):
			return False

		self.echo( self.name , "START MISSION GATE" )

		self.reset_target( "yaw" )
		self.reset_target( "xy" )
		self.reset_velocity( "xy" )

		self.target_state()
		self.start_yaw = self.temp_state[5]

		self.fix_z( -0.8 )
		self.wait_state( "z" , 0.15 , 5 )

		self.find_gate( 4 , 30 , 2 , 15 , 1 ) # d_f , t_f , d_s , t_s

		self.echo( "CONNECTION" , "Now can go pass gate" )
		self.over_move( 10 , 70 , True )
		self.fix_z( -1.2 )
		self.velocity_xy( 0.2 , 0 )
		while( self.ok_state() ):
			self.sleep( 0.1 )
			if( self.over_move( 10 , 70 ) ):
				break
		self.reset_velocity( "xy")
		self.fix_z( -0.5 )

		if( self.ok_state() ): self.find_drum() 

		self.reset_velocity( "xy" )
		self.active_control( False )
		return True

	def over_move( self , distance , time , setup = False ):
		if( setup ):
			self.time_start = time.time()
			self.collect_state()
			return True
		else:
			if( self.distance() > distance ):
				return True
#			elif( ( time.time() - self.time_start ) > time ):
#				return True
			else:
				return False

	def play_drum( self ):
		self.fire_gripper()
		self.echo( "DROP" , "Command to release gripper")
		self.free_xy( True )
		value_x = 0
		value_y = 0
		ok_y = True
		ok_x = False
		ever_fire_golf = False
		self.relative_z( -0.3 )
		start_time = 0
		count_unfound = 0
		while( self.ok_state() ):
			self.sleep( 0.06 )
			self.vision_drum.analysis_all( "blue" , "left-right" , 5 )
			if( not self.vision_drum.have_object() ):
				count_unfound += 1
				if( count_unfound == 5 ):
					self.free_xy( False )
					return False
				continue
			count_unfound = 0
			if( abs(self.vision_drum.result['cx_2']) < 0.1 ): 
				value_y = -0.05
				ok_y = True
			else: 
				value_y = math.copysign( 0.25 , self.vision_drum.result['cx_2'] * -1 )
				ok_y = False
			if( abs(self.vision_drum.result['cy_2']) < 0.1 ):
				value_x = 0.02
				ok_x = True
			else: 
				value_x = math.copysign( 0.2 , self.vision_drum.result['cy_2'] )
				ok_x = False
			if( ok_y and ok_x and self.check_position( "z" , 0.15 ) 
					and self.check_position("yaw" , 0.2 ) and ( not ever_fire_golf ) ):
				self.target_state()
				if( self.temp_state[2] < -1.4 ):
					self.fire_golf()
					ever_fire_golf = True
					start_time = time.time()
				else:
					self.relative_z( -0.3 )
			self.velocity( { 'x' : value_x , 'y' : value_y } )
			if( ever_fire_golf ):
				if( ( time.time() - start_time ) > 6 ):
					self.free_xy( False )
					return True

	def find_drum( self , direction ):
		self.velocity_xy( 0.2 )
		count_found = 0 
		while( self.ok_state() ):
			self.sleep( 0.06 )
			self.vision_drum.analysis_all( "mat" , "sevinar" , 5 )
			if( self.vision_drum.have_object() ):
				count_found += 1
				if( count_found == 3 ):
					break
			else:
				count_found = 0
		self.reset_velocity( "xy" )
		self.reset_target( "xy" )

		value_x = 0
		value_y = 0
		ever_change = False
		count_found = 0
		while( self.ok_state() ):
			self.sleep( 0.06 )
			self.vision_drum.analysis_all( "mat" , "sevinar" , 5 )
			if( not self.vision_drum.result['forward'] ): value_x = 0.1
			elif( not self.vision_drum.result['backward'] ): value_x = -0.1
			else: value_x = 0
			
			if( direction == 1 ): # direction == 1 is left
				if( not self.vision_drum.result['left'] and not ever_change):
					direction *= -1
					ever_change = True
				else:
					return False
			else:
				if( not self.vision_drum.result['right'] and not ever_change):
					direction *= -1
					ever_change = True
				else:
					return False
		
			self.vision_drum.analysis_all( "blue" , "center" , 5 )
			if( self.vision_drum.have_object() ):
				count_found += 1
				if( count_found == 3 ):
					return self.play_drum()
			else:
				count_found = 0
				self.velocity( { 'x' : value_x , 'y' : 0.1 * direction } )

	def find_gate( self, distance_forward, time_forward, distance_survey, time_survey, direct):
		self.velocity_xy( 0.2 )
		self.over_move( distance_forward , time_forward , True )
		finish_gate = False
		count_found = 0 
		while( self.ok_state() and not( finish_gate )):
			self.sleep( 0.06 )
			self.vision_gate.analysis_all( "gate" , "sevinar" , 5 )
			if( self.vision_gate.have_object() ):
				if( self.vision_gate.num_object() == 2 ):
					self.reset_velocity( "xy" )
					finish_gate = self.play_gate( direct )
				else:
					count_found += 1 
					if( count_found == 5 ):
						self.reset_velocity( "xy" )
						finish_gate = self.play_gate( direct )
			else:
				count_found = 0
			if( self.over_move( distance_forward , time_forward ) ):
				break
		self.reset_velocity( "xy" )
		self.wait_state( "xy" , 0.15 , 5 )
		count_found = 0
		self.over_move( distance_survey , time_survey , True )
		if( direct == 0 ):
			return True
		while( self.ok_state() and not( finish_gate ) ):
			self.sleep( 0.06 )
			self.vision_gate.analysis_all( "gate" , "sevinar" , 5 )
			if( self.vision_gate.have_object() ):
				if( self.vision_gate.num_object() == 2 ):
					self.reset_velocity( "xy" )
					finish_gate = self.play_gate( direct )
				else:
					count_found += 1 
					if( count_found == 5 ):
						self.reset_velocity( "xy" )
						finish_gate = self.play_gate( direct )
			else:
				count_found = 0
			if( self.over_move( distance_survey , distance_forward ) ):
				break 
		return finish_gate

	def play_gate( self , direction ): # direction = 1 is right an -1 is left
		self.vision_gate.analysis_all( "gate" , "sevinar" , 5 )
		current_fix_velocity = False
		count_finish = 0
		if( self.vision_gate.num_object() == 2 ):
			while( self.ok_state() ):
				self.sleep( 0.06 )
				self.vision_gate.analysis_all( "gate" , "sevinar" , 5 )
				if( self.vision_gate.num_object() == 2 ):
					count_finish = 0
					if( abs( self.vision_gate.center_x() ) < 0.2 ): 
						if( not current_fix_velocity ):
							self.velocity_xy( 0.1 , 0 )
							current_fix_velocity = True
					else:
						if( current_fix_velocity ):
							self.reset_velocity( "xy" )
							current_fix_velocity = False
					self.echo_vision( self.vision_gate.echo_special() )
					if( self.vision_gate.distance_x() > 1 ):
						if( current_fix_velocity ):
							self.reset_velocity( "xy" )
						return True
				else:
					count_finish += 1 
					if( count_finish == 3 ):
						if( current_fix_velocity ):
							self.reset_velocity( "xy" )
						return True
		else:
			while( self.ok_state() ):
				self.sleep( 0.06 )
				self.vision_gate.analysis_all( "gate" , "sevinar" , 5 )
				if( self.vision_gate.num_object() == 2 ):
					return self.play_gate()
				elif( self.vision_gate.num_object() == 1 ):
					count_finish = 0
					self.velocity( { 'y' : direction * 0.1 } )
				else:
					count_finish += 1
					if( count_finish == 4 ):
						return True
						
if __name__=="__main__":
	rospy.init_node( "mission_all" )
	MA = MissionAll( "mission_all" )
	rospy.spin()
