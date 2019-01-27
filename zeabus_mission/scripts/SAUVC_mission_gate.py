#!/usr/bin/python2
################################################################################################
##
##	FILE				: control_auv.py
##  AUTHOR				: Supasan Komonlit
##  Date Created		: 2019, JAN 27
##  Date lasr modified	: 2019, ??? ??
##	Purpose				: Connection Control Part
##
##	Maintainer			: Supasan Komonlit
##	e-mail				: supasan.k@ku.th
##	version				: 1.0.0
##	status				: Production
##
##	??????
################################################################################################

from __future__ import print_function

import rospy
import math
import time

from control_auv		import ControlAUV
from vision_collector	import VisionCollector

from std_msgs.msg		import String

class MissionGate:

	def __init__( self ):
	
		self.auv = ControlAUV( "mission_gate" , True )

		self.rate = rospy.Rate( 30 )
		self.data_pub = rospy.Publiher('mission/echo_planner' , String , queue_size = 1 )

		print( "<=== MISSION GATE ===> Waiting Service of GATE")
		rospy.wait_for_service( 'vision_gate' )

		self.vision = VisionCollector( "gate" )

		print( "<======= FINISH SETUP MISSION GATE ========>")

	def sleep( self , second ):
		self.rate.sleep()
		self.auv.wait_time( second )

	def check_area( self , new ):
		output = "Compare Area " + str( self.past_area) + " : new is " + sre( new )
		if( self.past_area / 2 > new ):
			output += " =====> BREAKING"
			self.echo( output )
			return True
		else:
			output += " =====> CONTINUE"
			self.echo( output )
			self.past_area = new 
			return False

	def echo( self , message ):
		self.data_pub.publish( String( message ) )

	def main_play( self ):
		self.sucess_mission = False
		self.vision.analysis_all( "gate" , "servinar" , 5 )
		self.auv.absolute_z( 4 )
		count_ok_depth = 0
		self.echo( "Waitin Depth are ok")
		while( not rospy.is_shutdown() and count_ok_depth < 4 ):
			self.sleep( 0.1 )
			if( self.auv.check_state('z' , 0.05 ) ):
				self.echo("Count ok depth is " + str( count_ok_depth ) )
				count_ok_depth += 1
			else:
				self.echo("reset count ok depth is " + str( count_ok_depth ) )
				count_ok_depth = 0 
		self.current_step = 0
		if( self.vision.have_object() ):
			self.current_step = 1 
			self.past_area = self.vision.area()
		else:
			print("<=== MISSION GATE ===> Don't found picture")
		
		if( self.current_step == 1 ):
			self.step_01()

		if( self.current_step == 2 ):
			self.step_02()

		# skip step check
		if( self.current_step == 3 ):
			#self.step_03()
			self.sucess_mission = True

		if( self.current_step == 4 ):
			self.sucess_mission = True

	def step_01( self ):
		self.echo( "<========= MISSION GATE =========> MISSION GATE FIRST STEP " )
		count_unfound = 0
		while ( not rospy.is_shutdown() ):
			self.sleep( 0.1 )
			self.vision.analysis_all( "gate" , "sevinar" , 5 )
			self.vision.echo_data() # for print n_obj and center of x , y axis
			if( self.vision.have_object() ):
				count_unfound = 0
				if( self.vision.result['pos'] == 0 ):
					if( abs( self.vision.center_x() ) < 0.1 ):
						self.echo("Found Pos : 0 and Move Forward")
						self.auv.velocity( { 'x' : 0.3 } )
					elif( self.vision.center_x() < 0 ):
						self.echo("Found Pos ; 0 and Move Left")
						self.auv.velocity( { 'y' : -0.4 } )
					elif( self.vision.center_x() > 0 ):
						self.echo("Found Pos ; 0 and Move right")
						self.auv.velocity( { 'y' : 0.4 } )
				elif( self.vision.result['pos'] == -1 ):
					self.echo("Found Pos : -1 and Move Left")
					self.auv.velocity( {'y' : -0.4 } )
				elif( self.vision.result['pos'] == 1 ):
					self.echo("Found Pos : 1 and Move Right")
					self.auv.velocity( {'y' : 0.4 } )
				else: 
					self.echo("Warning Don't have this condition")
				if( self.check_area( self.vision.area() ) ):
					self.echo("BREAK BECAUSE AREA DECREASS")
					self.current_step += 1
					break
			else:
				count_unfound += 1
				self.echo("UNFOUND ROUND " + str( count_unfound ))
				if( count_unfound > 5 ):
					self.current_step += 1
					self.echo("UnFound Where is gate go to dircetion")
					break

	def step_02( self ): # move to pass gate use time to estimate distance
		self.echo( "<========== MISSION GATE ==========> MISSION GATE SECOND STEP")
#		self.echo( "Move left 4 second")
#		start_time = time.time()
#		diff_time = time.time() - start_time 
#		while( not rospy.is_shutdown() and diff_time < 4 ):
#			diff_time = time.time() - start_time 
#			self.echo("Left diff time " + str( diff_time ) )
#			self.auv.velocity( { "y" : 0.2 } )
#			self.sleep( 0.1 )
		limit_time = 10
		start_time = time.time()
		while( not rospy.is_shutdown() ):
			self.auv.velocity( {'x' : 2 } )
			self.sleep( 0.1 )
			diff_time = time.time() - start_time
			if( diff_time > limit_time ):
				self.current_step += 1
				break
			self.echo("Move forward now diff_time is " + str( diff_time ) )
		self.auv.absolute_z( 4 )

	def step_03( self ): # spin and check have gate or not
		self.echo( "<========== MISSION GATE ==========> MISSION GATE THIRD STEP")
		self.auv.relative_yaw( 3.14 )
		count_ok = 0
		self.echo("Waitting yaw are ok")
		while( not rospy.is_shutdown() ):
			self.sleep( 0.1)
			if( self.auv.check_state("yaw" , 0.1 ) ):
				count_ok += 1
			else:
				count_ok = 0
			if( count_ok == 5 ): 
				break
		self.echo("Yaw now is ok start to check now play step 1 again ")	
		self.step_01()

if __name__=="__main__":
	mission_gate = MissionGate()
	mission_gate.main_play()
