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
# Code for pick ball have move_out_in , find_drum , play_drum
# Code for flare find_flare , play_flare

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
		self.fix_z( -0.7 )

		play_pick = False
		if( self.ok_state() ): play_pick = self.find_drum( 0 ) 
		if( self.ok_state() and play_pick ): self.move_out_in()

		self.fix_yaw( self.start_yaw - 1.57)
		self.relative_xy( 5 , -12 )
		self.wait_state( "xy" , 0.2 , 5 )
		self.find_flare( 6 , 50 , -1 )		

		self.reset_velocity( "xy" )
		self.hold_gripper()
		self.active_control( False )
		return True

	def over_move( self , distance , time_limit , setup = False ):
		if( setup ):
			self.time_start = time.time()
			self.collect_state()
			return True
		else:
			if( self.distance() > distance ):
				return True
#			elif( ( time.time() - self.time_start ) > time_limit ):
#				return True
			else:
				return False

	def play_flare( self ):
		self.hold_gripper()
		self.free_xy( True )
		self.fix_z( -1.6 )
		count_unfound = 0
		while( self.ok_state() ):
			self.sleep( 0.1 )
			self.vision_flare.analysis_all( "flare" , "near" , 5 )
			if( not self.vision_flare.have_object() ):
				count_unfound += 1
				if( count_unfound == 5 ):
					break
				continue
			count_unfound = 0
			if( abs( self.vision_flare.center_x() ) < 0.1 ):
				self.echo( "play flare" , "Now I move forward because is center")
				self.velocity( { 'x' : 0.2 } )
			else:
				self.velocity( { 'y' : math.copysign( 0.25 , self.vision_flare.center_x()*-1 ) })
				self.echo( "play flare" , "Now I move survey because isn't center")

		self.fix_z( -1.4 )	
		self.echo( "NEAR" , "wait depth" )
		while( self.ok_state() ):
			self.sleep( 0.1 )
			self.velocity({ 'x':0.01 } )
			if( self.check_position( "z" , 0.15 ) ): break
		self.free_xy( False )
		time_start = time.time()
		diff_time = 0
		self.velocity_xy( 0.2 , 0)
		while( self.ok_state() and diff_time < 6 ):
			self.sleep( 0.1 )
			diff_time = time.time() - time_start
			self.echo( "play flare" , "Last forward is diff_time is " + str(diff_time) )
		return True


	def find_flare( self , distance_survey , time_survey , direction ):
		self.echo( "find flare" , "Start survey")
		mode = 1 # 1 forward 2 survey
		setup_mode = True
		direction_survey = direction
		current_fix_velocity = False
		finish_flare = False
		self.fix_z( -1.4 )
		while( self.ok_state() ):
			self.sleep( 0.1 )
			self.vision_flare.analysis_all( "flare" , "near" , 5 )
			if( self.vision_flare.have_object() ):
				finish_flare = self.play_flare()
				if( finish_flare ):
					self.reset_velocity( "xy" )
					break
			self.vision_flare.analysis_all( "flare" , "far" , 5 )
			if( self.vision_flare.have_object() ):
				if( current_fix_velocity ):
					self.reset_velocity( "xy" )
					current_fix_velocity = False
				setup_mode = True
				if( abs( self.vision_flare.center_x() ) < 0.15 ):
					self.velocity( { 'x' : 0.1 } ) 
				else: self.velocity( {'y' : math.copysign( 0.12, self.vision_flare.center_x())} )
				continue
			if( setup_mode ):
				if( mode == 1 ):
					self.over_move( 1 , 15 , True )
				else:
					self.over_move( distance_survey ,time_survey , True )
				setup_mode = False
			if( not current_fix_velocity ):
				if( mode == 1 ):
					self.velocity_xy( 0.2 , 0 )
				else:
					self.velocity_xy( 0 , 0.2 * direction_survey )
				current_fix_velocity = True
			if( mode == 1 ):
				if( self.over_move( 1 , 15 ) ):
					self.echo( "FLARE" , "Change to mode survey" )
					mode = 2
					self.reset_velocity("xy")
					current_fix_velocity = False
					setup_mode = True
			else:
				if( self.over_move( distance_survey , time_survey ) ):
					self.echo( "FLARE" , "Change to mode forward" )
					distance_survey *= -1
					self.reset_velocity("xy")
					current_fix_velocity = False
					setup_mode = True
					mode = 1
			
	def move_out_in( self ):
		self.velocity_xy( -0.15 , 0 )
		count_unfound = 0
		while( self.ok_state() ):
			self.sleep( 0.06 )
			self.vision_drum.analysis_all( "mat" , "sevinar" , 5 )
			if( not self.vision_drum.have_object() ):
				count_unfound += 1
				if( count_unfound == 5 ):
					break
			else:
				count_unfound = 0
		self.reset_velocity( "xy" )
		return self.find_drum( 0 , True )

	def play_drum( self , pick_ball ):
		if( pick_ball ): self.echo( "PLAYDRUM" , "For pick ball")
		else: self.echo( "PLAYDRUM" , "For drop ball")
		self.fire_gripper()
		self.echo( "DROP" , "Command to release gripper")
		self.free_xy( True )
		value_x = 0
		value_y = 0
		ok_y = True
		ok_x = False
		ever_fire_golf = False
		wait_z = False
		self.relative_z( -0.3 )
		start_time = 0
		count_unfound = 0
		if( pick_ball ): self.hold_golf()
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
			self.vision_drum.result['cx_2'] -= 0.15
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
				self.echo( "DRUM" , "Now depth is " + str( self.temp_state[2] ) )
				if( self.temp_state[2] < -1.4 ):
					if( not pick_ball ): self.fire_golf()
					else: self.velocity_z( -0.15 )
					ever_fire_golf = True
					start_time = time.time()
				else:
					self.relative_z( -0.3 )
			self.velocity( { 'x' : value_x , 'y' : value_y } )
			if( wait_z ):
				if(self.check_position( "z" , 0.15  ) ):
					self.free_xy( False )
					return True
				else: continue
			if( ever_fire_golf ):
				if( ( time.time() - start_time ) > 8 ):
					self.reset_velocity( "z" )
					self.fix_z( -0.6 )
					wait_z = True

	def find_drum( self , direction , pick_ball = False ):
		if( direction == 0 ): direction = 1
		self.echo( "FIND DRUM" , "Survey for find drum")
		self.velocity_xy( 0.2 , 0 )
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
		self.echo("FIND DRUM" , "Continue to find drum")
		self.reset_velocity( "xy" )
		self.reset_target( "xy" )
		value_x = 0
		value_y = 0
		ever_change = False
		count_found = 0
		can_aborted = False
		while( self.ok_state() ):
			self.sleep( 0.06 )
			self.vision_drum.analysis_all( "mat" , "sevinar" , 5 )
			if( not self.vision_drum.result['forward'] ): value_x = 0.1
			elif( not self.vision_drum.result['backward'] ): value_x = -0.1
			else: value_x = 0
			
			if( direction == 1 and not self.vision_drum.have_object() ): # direction == 1 is left
				if( not ever_change):
					self.echo( "FIND DRUM" , "Change mode to find drum")
					direction *= -1
					ever_change = True
					can_aborted = False
				else:
					return False
			elif(direction == 2 and not self.vision_drum.have_object() ):
				if( not ever_change ):
					self.echo( "FIND DRUM" , "Change mode to find drum")
					direction *= -1
					ever_change = True
				else:
					return False
			else: can_aborted = True

			self.echo( "FIND DRUM" , "Check drum")	
			self.vision_drum.analysis_all( "blue" , "left-right" , 5 )
			if( self.vision_drum.have_object() ):
				count_found += 1
				if( count_found == 3 ):
					return self.play_drum( pick_ball )
			else:
				count_found = 0
				self.velocity( { 'x' : value_x , 'y' : 0.1 * direction } )

	def find_gate( self, distance_forward, time_forward, distance_survey, time_survey, direct):
		self.velocity_xy( 0.2 , 0)
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
