#!/usr/bin/python2
################################################################################################
####
####	FILE		: mission_qualify.py
####	Author		: Supasan Komonlit
####	Create on	: 2019 , FEB 16
####	Purpose		: For connection of Mission Planner with Control	
####
################################################################################################
from __future__ import print_function
#===============>

import rospy
import time
import math

from control_handle		import ControlAUV
from vision_collector	import VisionCollector
from zeabus_library.srv	import TwoBool

from std_msgs.msg		import String

class MissionQualification:

	def __init__( self ):

		self.ch = ControlAUV("qualification_planner")

		self.rate = rospy.Rate( 30 )
		self.data_pub = rospy.Publisher("/mission/echo_planner" , String , queue_size = 1 )

		self.vision = VisionCollector("qualification")

		self.mission_planner = rospy.Service("/mission/qualification" ,TwoBool ,self.main_play )

	def sleep( self , second):
		self.rate.sleep()
		time.sleep( second )

	def echo( self , message ):
		self.data_pub.publish( String( message ) )

	def main_play( self , request ):
		self.echo( "==========> MISSION QUALIFICATION START!")

		self.ch.fix_z( -0.3 )
		self.ch.reset_target("xy")
		self.ch.reset_target("yaw")
		
		self.ch.relative_xy( 5 , 0 )

		count_ok = 0
		self.echo( "Waiting go forward 5 meter")
		while( not rospy.is_shutdown() ):
			self.sleep( 0.1 )
			if( self.ch.check_posisition( "xy" , 0.2) ):
				count_ok += 1
			else:
				count_ok = 0
			if( count_ok == 5 ):
				break
			self.echo( "Wait xy count is " + str( count_ok ) )
	
		self.echo("Now play")
		fix_velocity = False
		found_item = 0
		while( not rospy.is_shutdown() ):
			self.sleep( 0.1 )
			self.vision.analysis_all( "qualification" , "servinar" , 5 )
			self.vision.echo_data()
			self.echo( self.vision.echo() )
			if( self.vision.result['n_obj'] == 0 ):
				if( found_item == 0 ):
					if( not fix_velocity ):
						self.ch.velocity_xy( 1 , 0 )
					else:
						self.echo( "Now Forward for search")
				elif( found_item == 2 ):
					self.ch.relative_xy(5 , 0 )	
			elif( self.vision.result['n_obj'] == 2 ):
				found_item = 2
				if( abs(self.vision.center_x() ) < 0.3  ):
					if( not fix_velocity ):
						self.ch.velocity_xy( 1 , 0 )
					else:
						self.echo( "Now Forward by continue velocity")
				elif( self.vision.center_x() > 0.3 ):
					if( fix_velocity ):
						self.ch.reset_velocity( "xy" )
					self.ch.velocity( {'y' : 0.2 } )
					self.echo( "Move left" )
				else:
					if( fix_velocity ):
						self.ch.reset_velocity( "xy" )
					self.ch.velocity( {'y' : -0.2 } )
					self.eco( "Move Right")
			else: # if found_item = 3  is left and found_item = 4 is right
				if( not ( found_item == 3 or found_item == 4 ) ):
					if( self.vision.center_x() < -0.3 ):
						found_item = 3
					else:
						found_item = 4
		return True

if __name__=="__main__":
	rospy.init_node("mission_qualification" , anonymous=False)
	mission_qualify = MissionQualification()
	rospy.spin()
						
				 	
