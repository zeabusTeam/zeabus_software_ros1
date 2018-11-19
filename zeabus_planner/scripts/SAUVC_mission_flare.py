#!/usr/bin/python2
#################################################################################################
####
####	FILE		: SAUVC_mission_flare.py
####	Author		: Supasan Komonlit
####	Create on	: 2018 , Nov 17
####	Purpose		: Finish Code do mission flare in SAUVC 2019
####
#################################################################################################

import rospy
import math
import time 

from auv_controller		import AUVController
from vision_collector	import VisionCollector
from zeabus_library.srv	import MissionResult

from std_msgs.msg		import Bool , Int8 , String

# LINE 24 to 47 is standard code of mission part by Supasan Komonlit

class MissionFlare:
	
	def __init__( self):

		self.auv	= AUVController( "mission_flare" , True )
		 
		self.rate	= rospy.Rate( 30 )
		self.data_pub = rospy.Publisher('mission/echo_planner' , String , queue_size = 1 )

		print( "<=== MISSION GATE ===> Waitting Service of flare" )
		rospy.wait_for_service( 'vision_flare' )
		print( "<=== MISSION GATE ===> Have Service of flare OK ")

		self.mission_planner = rospy.Service('mission/flare' , MissionResult , self.main_play)

		self.vision	= VisionCollector( "flare" )

		print( "<========== FINISH SETUP MISSION FLARE ==========>")

	def sleep( self , second):
		self.rate.sleep()
		self.auv.wait_time( second )

	def echo( self , message):
		self.data_pub.publish( String( message ) )

	def main_play( self , request ):
		self.sucess_mission = False
		self.current_step = 0

		self.vision.analysis_all( "flare" , "far" , 5 )
		if( self.vision.have_object() ):
			self.current_step += 1
	
		#First step move to the flare let go and try to find in near mode 
		#	if found in near mode I will send to second step play in mear mode
		if( self.current_step == 1 ):
			self.step_01()

		# Second Step will move to flare untile flare is disappear in vision mode
		#	and will send to step three
		if( self.current_step == 2 ):
			self.step_02() 

		# Third Step is move forward and spind around 1 round
		if( self.current_step == 3 ):
			self.step_03()

		return Bool( self.sucess_mission ) , Int8( self.current_step )

	def step_01( self ):
		self.echo( "<=== MISSION FLARE ===> PLAY STEP ONE TO FIND DATA")
		count_unfound = 0
		while( not rospy.is_shutdown() ):
			self.sleep( 0.1 )
			self.vision.analysis_all( "flare" , "far" , 5 )
			self.echo( self.vision.echo_data() )
			if( self.vision.have_object() ):
				count_unfound = 0
				if( abs(self.vision.center_x()) < 0.05 ):
					self.auv.velocity( { 'x' : 0.3 })
					self.echo( "STEP ONE Move Forward" )
				elif( self.vision.center_x() < 0 ):
					self.auv.velocity( { 'y' : 0.35} )
					self.echo( "STEP ONE Move left" )
				elif( self.vision.center_x() > 0):
					self.auv.velocity( {'y' : -0.35 } )
					self.echo( "STEP ONE Move righht")
				else:
					self.echo( " ERROR IN LINE 92 =========================================")
			else:
				count_unfound += 1
				if( count_unfound == 5 ):
					break
				self.echo("<=== MISSION FLARE ===> Warning Unfound " + str( count_unfound ) )
			# this line will try to find in near mode
			self.vision.analysis_all( "flare" , "near" , 5 )
			if( self.vision.have_object() ):
				self.echo( "<=== MISSION FLARE ===> FOUND IN NEAR MODE")
				self.current_step += 1
				break

	def step_02( self ):
		self.echo( "<=== MISSION FLARE ===> PLAY STEP TWO MOVE BY NEAR MODE")
		count_unfound = 0
		start_time = time.time()
		limit_time = 8
		while( not rospy.is_shutdown() and ( time.time() - start_time ) < limit_time):
			self.sleep( 0.1 )
			self.vision.analysis_all( "flare" , "near" , 5 )
			self.echo( self.vision.echo_data() )
			if( self.vision.have_object() ):
				count_unfound = 0
				if( abs( self.vision.center_x() ) < 0.1 ):
					self.echo( "STEP TWO MOVE FORWARD")
					self.auv.velocity( { 'x' : 0.25 } )
				elif( self.vision.center_x() < 0 ):
					self.echo( "STEP TWO MOVE LEFT")
					self.auv.velocity( { 'y' : 0.35 } )
				elif( self.vision.center_x() > 0 ):
					self.echo( "STEP TWO MOVE RIGHT")
					self.auv.velocity( { 'y' : -0.35 } )
				else:
					self.echo( "ERROR IN LINE 126 ===========================================")
			else:
				count_unfound += 1
				if( count_unfound == 3 ):
					break
				self.echo("<=== MISSION FLARE ===> UNFOUND IN STEP TWO")
		self.current_step += 1

	def step_03( self ):
		self.echo( "<=== MISSION FLARE ===> PLAY STEP THREE MAKE SURE FLARE IS DESTROYED")
#		start_time = time.time()
#		limit_time = 5
#		while( not rospy.is_shutdown() ):
#			self.auv.velocity( { 'x' : 1.5 } )
#			self.sleep( 0.2 )
#			diff_time = time.time() - start_time 
#			if( diff_time > limit_time ):
#				break
#			self.echo( "NOW GO GO TIME : " + str( diff_time ))
		self.sleep( 1 )
		self.auv.absolute_z( -3.4 )
		self.auv.set_mode( 0 )
		self.auv.relative_yaw( 3.14 )
		while( not rospy.is_shutdown() and not self.auv.check_state('yaw' , 0.1 )):
			self.sleep( 0.3 )
		self.auv.relative_yaw( 3.14 )
		while( not rospy.is_shutdown() and not self.auv.check_state('yaw' , 0.1 )):
			self.sleep( 0.3 )
		self.sucess_mission = True

if __name__=="__main__":
	mission_flare = MissionFlare()
	rospy.spin()
