#!/usr/bin/python2
#################################################################################################
####
####	FILE		: SAUVC_qualification_vision.py
####	Maintain	: Supasan Komonlit
####	Create on	: 2019 , FEB 23
####	Purpose		: For connection with control part
####
#################################################################################################
from __future__ import print_function
#===============>

################################# EXPAND MISSION SOLUTION #######################################
####	This file is mission for qualification round for SAUVC2019 
####	In case have vision for make and we can use only center in x axis
####	Don't worry about depth because we use pressure to matk that
####	And the last one we have decide to do, although vision find only single pier
#################################################################################################

import	rospy
import	math
import	time

from vision_collector		import VisionCollector

from standard_mission		import StandardMission

new_pid = 1

class MissionQualification( StandardMission ):
	
	def __init__( self , name ):
		self.name = name

		StandardMission.__init__( self , self.name , "/mission/qualification" , self.callback )

		self.vision = VisionCollector( "qualification" )
	
		self.state = False

		print("MISSION QUALIFICATION FINISHED SETUP")

	def callback( self , message ):

		result = False

		if( message.data and self.state ):
			self.echo( self.name , "Now mission qualification have running")
			return False
		elif( message.data ):
			self.state = True
		else:
			self.state = False
			return False

		# This function will call by switch we must to reset data target
		self.reset_target( "xy" )
		self.reset_target( "yaw" )
		self.reset_velocity( "xy" )
		self.fix_z( -0.5 )

		self.echo( self.name , "START MISSION QUALIFICATION")

		#	This situation we don't know what picture we have to find but we known we have start
		# direct we will go forward until find the picture

		self.type_pier = 0 # -1 : 0 : 1 is single left : double pier : single right

		self.collect_state()
		self.velocity_xy( 0.3 , 0 )
		count_have_object = 0
		start_time = time.time()
		while( self.ok_state() ):
			self.sleep( 0.05 )
			if( ( time.time() - start_time ) < 5 ):
				self.echo( self.name , "Now time is " + str( time.time() - start_time ))
				continue	
			self.vision.analysis_all( "qualification" , "sevinar" , 5 )
			self.echo_vision( self.vision.echo_data() )
			if( self.vision.have_object() ):
				count_have_object += 1
				if( count_have_object == 6 ):
					break
			else:
				count_have_object = 0
			self.echo( self.name , "distance : count_have_object are " + 
					str( self.distance() ) + str( count_have_object ) )
			if( self.distance() > 4.5 ):
				self.reset_velocity( "xy" )
				self.velocity_xy( 0.1 , 0 )
		# we want to ensure we find object 3 round and next we will move follow data
		self.reset_velocity( "xy" )
		self.fix_z( -0.3 )

		self.vision.analysis_all( "qualification" , "servinar" , 5 )

		# we assign data of run_type is 
		#	1 <== Found only single pier		# 2 <== Found double pier <All GATE>
		#	3 <== Solution of last move form found single pier

		if( self.vision.num_object() == 2 ): self.run_type = 2
		elif( self.vision.num_object() == 1 ): self.run_type = 1
		else: self.run_type = 0 

		self.echo( self.name , "We will start do it on function type : " + str( self.run_type) )

		while( self.ok_state() ):	
			if( self.run_type == 1 ): self.type_1()
			elif( self.run_type == 2 ): self.type_2()
			elif( self.run_type == 3 ): self.type_3()
			else: break

		self.echo( self.name , "Finished run callback alway response TRUE")
		return True

	def type_1( self ):
		if( self.type_pier == 0 ):
			self.vision.analysis_all( "qualification" , "sevinar" , 5 )
			self.echo_vision( self.vision.echo_specific() )
			if( self.vision.num_object() == 1 ):
				if( self.vision.center_x() < 0 ): 
					self.echo( self.name , "I decide that pier is left pier")
					self.type_pier = -1
				else:
					self.echo( self.name , "I decide that pier is right pier")
					self.type_pier = 1
			elif( self.vision.num_object() == 2 ):
				self.type_pier = 0
				self.echo( self.name , "What I found 2 pier why?")
				self.run_type = 2
				return 0
		
		count_not_single = 0
		count_not_found = 0
		while( self.ok_state() ):
			self.sleep( 0.05 )
			self.vision.analysis_all( "qualification" , "sevinar" , 5 )
			self.echo_vision( self.vision.echo_data() )
			if( self.vision.num_object() == 1 ):
				count_not_single = 0
				count_not_found = 0
				if( abs(self.vision.center_x() ) < 0.3 ):
					self.velocity( {'x' : 0.10 } )
					self.echo( self.name , "type_1 object : " + str( self.type_pier ) + 
							" We decide to move forward" )
				elif( self.vision.center_x() < 0 ):
					self.velocity( {'y' : 0.1 } )
					self.echo( self.name , "type_1 object : " + str( self.type_pier ) + 
							" We decide to move left" )
				elif( self.vision.center_x() > 0 ):
					self.velocity( {'y' : -0.1 } )
					self.echo( self.name , "type_1 object : " + str( self.type_pier ) +
							" We decide to move right" )
				else:
					self.echo( self.name , "BUG ON LINE 127" )
					self.run_type = 0
					break
				self.echo_vision( self.vision.echo_specific() )
				if( self.vision.distance_x() > 0.1 ):
					self.echo( self.name ,  "Now over distance we have to chance to last move")
					self.run_type = 3
					break
					
			elif( self.vision.num_object() == 2 ):
				self.velocity( { 'y' : math.copysign( 0.1 , self.vision.center_x() * -1 ) } )
				self.run_type = 2
				self.type_pier = 0
				break
			else:
				count_not_found += 1
				self.echo( self.name , "type_1 not_found  pier : " + str( count_not_found ) )
				if( count_not_found == 5 ):
					self.run_type = 0
					self.relative_xy( 5 , math.copysign( 1.5,  self.type_pier ) )
					self.echo( self.name , "We decide to last move side is " + 
							str(math.copysign( 1.5 , -1*self.type_pier) ) )
					break
					
		
	def type_2( self ):
		count_not_found = 0
		count_not_doulbe = 0
		current_fix_velocity = False
		while( self.ok_state() ):
			self.sleep( 0.1 )
			self.vision.analysis_all( "qualification" , "sevinar" , 5 )
			self.echo_vision( self.vision.echo_data() )
			if( self.vision.num_object() == 2 ):
				count_not_doulbe = 0
				count_not_found = 0
				if( abs( self.vision.center_x() ) < 0.10 ):
					if( not current_fix_velocity ):
						self.velocity_xy( 0.2 , 0 )
						current_fix_velocity = True
					self.echo( self.name , "type_2 we move direct by fix velocity")
				elif( self.vision.center_x() < 0 ):
					if( current_fix_velocity ):
						current_fix_velocity = False
						self.reset_velocity( "xy" )
					self.velocity( {'y' : 0.1 } )
					self.echo( self.name , "type_2 we move left")
				elif( self.vision.center_x() > 0 ):
					if( current_fix_velocity ):
						current_fix_velocity = False
						self.reset_velocity( "xy" )
					self.velocity( {'y' : -0.1})
					self.echo( self.name , "type_2 we move right")
				self.echo_vision( self.vision.echo_specific() )
				if( self.vision.distance_x() > 1.0 ):
					self.echo( self.name , "Now over distance we decide to only move forward")
					self.run_type = 0
					if( not current_fix_velocity ):
						self.velocity_xy( 0.3 , 0 )
						self.echo( self.name , "We order contant velocity x is 0.3")
						current_fix_velocity = True
					break
			elif( self.vision.num_object() == 1 ):
				if( current_fix_velocity ):
					current_fix_velocity = False
					self.reset_velocity( "xy") 
				count_not_doulbe += 1
				self.echo( self.name ,  "Type 2 we found only single pier : " + 
						str( count_not_doulbe) )
				if( count_not_doulbe == 5 ):
					self.echo( self.name , "We change to mode type 1 single pier")
					self.run_type = 1
					self.type_pier = 0
					break
			else:
				count_not_found += 1
				self.echo( self.name , "Type 2 we not found pier : " + str( count_not_found ) )
				if( count_not_found == 5 ):
					self.echo( self.name , "Last move")
					self.run_type = 0
					self.type_pier = 0
					if( not current_fix_velocity ):
						self.velocity_xy( 0.3 , 0 )
						self.echo( self.name , "I don't know what should I do FORWARD!")
						current_fix_velocity = True
					break

	def type_3( self ):	
		self.echo( self.name , "We move in last mode type_3 and type peir is " + 
				str(self.type_pier ) )
		#	this step we will move side to don't find pier and 
		# will move a little time befor move forward
		count_not_found = 0
		while( self.ok_state() ):
			self.sleep( 0.1 )
			self.vision.analysis_all( "qualification" , "sevinar" , 5 )
			self.echo( self.name , "Type 3 and move slide to don't have object : " + 
					str(count_not_found) )
			self.velocity( {'y' : math.copysign( 0.10 ,  self.type_pier )} )	
			if( self.vision.have_object() ):
				count_not_found = 0
			else:
				count_not_found += 1
			if( count_not_found == 5 ):
				break
		start_time = time.time()
		self.echo( self.name , "Move side 3 second")
		while( self.ok_state() ):
			self.sleep( 0.1 )
			diff_time = time.time() - start_time
			self.velocity( {'y' : math.copysign( 0.10 , self.type_pier )} )	
			self.echo( self.name , "Type3 now move time is " + str( diff_time ) )
			if( diff_time > 5 ): break
		self.echo( self.name , "Time out move forward" )
		self.velocity_xy( 0.3 , 0 )
		self.run_type = 0	
			
if __name__ == "__main__":
	rospy.init_node("mission_qualification")
	MQ = MissionQualification( "mission_qualification" )
	rospy.spin()	
