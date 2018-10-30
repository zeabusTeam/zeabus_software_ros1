#!/usr/bin/python2

import rospy
import math

try:
	from zeabus_extension.manage_log import log
	from zeabus_extension.control_auv import control_auv 
except:
	print("Pleas install setup.bash in zeabus_extension package")
	exit()

from zeabus_vision.srv import vision_srv_drum
from zeabus_vision.msg import vision_drum
from std_msgs.msg import String

class play_drum:

	def __init__( self, rate ):
		self.already_setup = False

		print("Waiting drum service")
		rospy.wait_for_service('vision_drum')
		print("I found drum service")
		
		self.rate = rospy.Rate( rate )

	def setup( self ):
		self.already_setup = True
		self.log_command = log( "zeabus_planner" , "log" , "03_drum_command")
		self.log_vision = log( "zeabus_planner" , "log" , "03_drum_vision")
		self.client_drum = rospy.ServiceProxy('vision_drum' , vision_srv_drum )
		self.auv = control_auv( "play drum" )
		self.default_depth = -3.5
		self.depth = self.default_depth
		self.moving_veloc = 0.5
		self.moving = False
		self.satisfy = False
		self.z_adding = 0.1


	def play( self ):
		if not self.already_setup:
			self.setup()
		while not rospy.is_shutdown():
			ser_data = self.client_drum(String('drum'), String('blue')).data

			if not self.auv.ok_position('z', self.z_adding):
				self.rate.sleep()
				continue
			
			if self.satisfy:
				self.auv.absolute_depth(self.default_depth)
				if self.auv.ok_position('z', self.z_adding):
					print "END DRUM"
					break
				self.rate.sleep()

			x_move = 0
			y_move = 0

			if ser_data.forward and ser_data.backward and ser_data.left and ser_data.right:
				if self.moving:
					self.auv.velocity(x=0, y=0)
					self.moving = False
					self.rate.sleep()
 					continue
				log_command.write('Move Pool Bottom',False,0)
				self.auv.absolute_depth(self.default_depth-0.7)
				self.satisfy = True
			else:
				if not (ser_data.forward or ser_data.backward or ser_data.left or ser_data.right):
					if ser_data.cx<-0.2:
						ser_data.forward = True
						log_vision.write('TOO TOP',False,0)
					elif ser_data.cx>0.2:
						ser_data.backward = True
						log_vision.write('TOO BOTTOM',False,0)
					if ser_data.cy<-0.2:
						ser_data.left = True
						log_vision.write('TOO LEFT',False,0)
					elif ser_data.cy>0.2:
						ser_data.right = True
						log_vision.write('TOO RIGHT',False,0)
				if ser_data.forward:
					x_move = self.moving_veloc
					log_command.write('Move Front',False,0)
				if ser_data.backward:
					x_move = -self.moving_veloc
					log_command.write('Move Back',False,0)
				if ser_data.left:
					y_move = self.moving_veloc
					log_command.write('Move Left',False,0)
				if ser_data.right:
					y_move = -self.moving_veloc
					log_command.write('Move Right',False,0)
				if ser_data.forward or ser_data.backward or ser_data.left or ser_data.right:
					self.auv.velocity(x=x_move, y=y_move)
					log_command.write('Commit Move',False,0)
				else:
					self.depth += 0.1
					self.auv.absolute_depth(self.depth)
					self.moving = True
					log_command.write('Move ตื้น',False,0)
			self.rate.sleep()

if __name__=='__main__':

	rospy.init_node("Mission Drum")		

	drum = play_drum( 30 )
	drum.setup()
	drum.play()
