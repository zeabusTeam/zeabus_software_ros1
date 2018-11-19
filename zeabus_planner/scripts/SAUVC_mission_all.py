#/usr/bin/python2
#################################################################################################
####
####	FILE		: SAUVC_all_mission.py
####	Author		: Supasan Komonlit
####	Create on	: 2018 , Nov 20
####	Purpose		: Last Code for do all mission in SAUVC 2019
####
#################################################################################################

from __future__			import print_function

import rospy
import math
import time

from auv_controller		import AUVController
from vision_collector	import VisionCollector
from zeabus_library.srv	import MissionResult

from std_msgs.msg		import Bool, Int8 , String

class SAUVC2019:

	def __init__( self ):

		self.auv		= AUVController( "all_mission" , True )
	
		self.rate		= rospy.Rate( 30 )
		self.data_pub	= rospy.Publisher('mission/echo_planner' , String , queue_size = 1)	

		print( "Waiting Service Mission GATE" , end = " =============> ")
		rospy.wait_for_service( 'mission/gate')
		print( "ALREADY")
		self.mission_gate	= rospy.ServiceProxy('/mission/gate'	, MissionResult )

		print( "Waiting Service Mission FLARE" , end = " =============> ")
		rospy.wait_for_service( 'mission/flare')
		print( "ALREADY")
		self.mission_flare	= rospy.ServiceProxy('/mission/flare'	, MissionResult )

		print( "Waiting Service Mission DRUM" , end = " =============> ")
		rospy.wait_for_service( 'mission/drum')
		print( "ALREADY")
		self.mission_drum	= rospy.ServiceProxy('/mission/drum'	, MissionResult )

		print( "Wainting Service Mission GRIPPER" , end = "============> ")
		rospy.wait_for_service( 'mission/gripper')
		print( "ALREADY")
		self.mission_gripper= rospy.ServiceProxy('/mission/gripper'	, MissionResult )

		self.vision_gate = VisionCollector( "gate" )
		self.vision_flare = VisionCollector( "flare" )
		self.vision_drum = VisionCollector( "drum" )
		
	def sleep( self , second ):
		self.rate.sleep()
		self.auv.wait_time( second )
		self.rate.sleep()

	def echo( self , message ):
		self.data_pub.publish( String( message ) )

	def first_mission( self ) :
		self.echo( "<===== ALL MISSSION =====> Start to Do mission Gate")
		self.auv.set_mode( 0 )
		self.auv.absolute_z( -3.2 )
		self.start_yaw = self.auv.receive_target('yaw')[0] 

		count_ok = 0
		self.echo( "Waiting Depth are OK")
		while( not rospy.is_shutdown() ):
			self.sleep( 0.1 )
			if( self.auv.check_state( "z" , 0.05 ) ):
				count_ok += 1
			else:
				count_ok = 0
			if( count_ok == 5 ):
				break
		self.echo("Now OK DEPTH Survey to find GATE")
		self.echo( "Waiting Yaw are OK")
		count_ok = 0
		while( not rospy.is_shutdown() ):
			self.sleep( 0.1 )
			if( self.auv.check_state( "yaw" , 0.05 ) ):
				count_ok += 1
			else:
				count_ok = 0
			if( count_ok == 5 ):
				break
		self.survey_mode( self.vision_gate , "gate" , "sevinar" , 5 , 5 , 1 , 3)
		sucess = False
		while( not rospy.is_shutdown() and not sucess):
			sucess = self.mission_gate( Bool( True ) )
			if( not sucess ):
				self.echo("<===== ALL MISSION =====> GATE IS FAIL SURVEY MODE")
				self.survey_mode( self.vision_gate , "gate" , "sevinar" , 0.5 , 1.5 , 1 , 3)
		self.echo("<===== ALL MISSION =====> GATE SUCCESS")
		self.second_mission()

	def second_mission( self ):
		None
				

	def survey_mode(	self			, vision		, task		, request	, 
						first_forward	, first_survey	, forward	, survey	):
		find_object = False
		time_first_forward = True
		time_first_survey = True
		# Big Loop make survey still find object
		self.echo(	"Data for SURVEY TASK : " + task + " REQUEST : " + request + 
					" And distance are " + str( first_forward ) + " : " + str( first_survey) +
					" : " + str( forward ) + " : " + str( survey ) )
		while( not rospy.is_shutdown() and not find_object ):
			# This for go forward
			if( time_first_forward ):
				distance = first_forward
				time_first_forward = False
			else:
				distance = forward
			self.echo( "We will move forward and search distance is " + str( distace ) )
			self.auv.survey( 'x' , distance , 0.5 )
			count_ok = 0 
			while( not rospy.is_shutdown() and not find_object ):
				self.sleep( 0.1 )
				if( self.auv.check_state( 'xy' , 0.06 ) ):
					count_ok += 1 
					if( count_ok > 5 ):
						break
				else:
					count_ok = 0
				vision.analysis_all( task , request , 5 )
				self.echo( vision.echo_data() )
				find_object = vision.have_object()
			# This for survey left
			if( time_first_survey ):
				distance = first_survey
				time_first_survey = False
			else:
				distance = math.copysign( survey , find_object )
			self.echo( "We will move survey and search distance is " + str( distance ) )
			self.auv.survey( 'y' , distance , 0.6 )
			count_ok = 0
			while( not rospy.is_shutdown() and not find_object ):
				self.sleep( 0.1 )
				if( self.auv.check_state( 'xy' , 0.06) ):
					count_ok += 1
					if( count_ok > 5 ):
						break
				else:
					count_ok = 0
				vision.analysis_all( task , request , 5 )
				self.echo( vision.echo_data() )
				find_object = vision.have_object()
			# This for forward step three
			distance = forward
			self.echo( "We will move forward and search distance is " + str( distance ) )
			self.auv.survey( 'x' , distance , 0.5 )
			count_ok = 0 
			while( not rospy.is_shutdown() and not find_object ):
				self.sleep( 0.1 )
				if( self.auv.check_state( 'xy' , 0.06 ) ):
					count_ok += 1
					if( count_ok > 5 ):
						break
				else:
					count_ok = 0
				vision.analysis_all( task , request , 5 )
				self.echo( vision.echo_data() )
				find_object = vision.have_object()
			# This for survey round four
			distance = math.copysign( survey , first_survey * -1 )
			self.echo( "We will move survey and search distance is " + str( distance ) ) 
			self.auv.survey( 'y' , distance , 0.6 )
			count_ok = 0
			while( not rospy.is_shutdown() and not find_object ):
				self.sleep( 0.1 )
				if( self.auv.check_state( 'xy' , 0.06) ):
					count_ok += 1
					if( count_ok > 5 ):
						break
				else:
					count_ok = 0
				vision.analysis_all( task , request , 5 )
				self.echo( vision.echo_data() )
				find_object = vision.have_object()
