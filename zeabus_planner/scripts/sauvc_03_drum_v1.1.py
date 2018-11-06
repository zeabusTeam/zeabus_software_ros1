#!/usr/bin/python2

import rospy
import math
import time

try:
	from zeabus_extension.manage_log import log
	from zeabus_extension.control_auv import control_auv 
except:
	print("Please install setup.bash in zeabus_extension package")
	exit()

from zeabus_vision.srv import vision_srv_drum
from zeabus_vision.msg import vision_drum
from std_msgs.msg import String

class play_drum:

	def __init__( self, rate ):
		self.already_setup = False

		print("Waiting drum service")
		rospy.wait_for_service('vision_drum')
		print("Drum service is UP")
		
		self.rate = rospy.Rate( rate )

	def setup( self ):
		self.already_setup = True
		self.log_command = log( "zeabus_planner" , "log" , "03_drum_command")
		# self.log_vision = log( "zeabus_planner" , "log" , "03_drum_vision")
		self.client_drum = rospy.ServiceProxy('vision_drum' , vision_srv_drum )
		self.auv = control_auv( "play drum" )
		self.default_depth = -3.5
		self.depth = self.default_depth
		self.stopTrackingDepth = -3.9
		self.NearDepth = -4.2
		# WITH GRIPPER self.NearDepth = -5
		self.moving_veloc = 0.05
		self.foundCount = 0

	def play( self ):
		if not self.already_setup:
			self.setup()
		self.log_command.write('Finding drum', True, 0)
		if self.find_drum():
			self.log_command.write('Running to drum', True, 0)
			self.RunToDrum()
			self.log_command.write('Dropping Golf', True, 0)
			self.lock_n_drop()
		self.log_command.write('Go UP', True, 0)
		self.GoUp()
	
	def lock_n_drop( self ):
		godown_speed = -0.06
		self.log_command.write('Locking Pos + Go down',False,1)
		print("Going down with speed"+str(godown_speed))
		self.auv.absolute_depth(self.NearDepth)
		self.auv.velocity(z=godown_speed)
		logged = 0
		while not rospy.is_shutdown() and not self.isDepthOK():
			ser_data = self.client_drum(String('drum'), String('drop')).data
			if self.depth > self.stopTrackingDepth:
				self.auv.velocity(x=self.moving_veloc*ser_data.cx*-2, y=self.moving_veloc*ser_data.cy*2)
				if logged == 0:
					print("Adjusting to centre")
					self.log_command.write('Adjusting to centre',False,1)
					logged += 1
			else:
				self.auv.velocity(x=0, y=0)
				if logged == 1:
					print("STOP adjusting to centre")
					self.log_command.write('STOP adjusting to centre',False,1)
					logged += 1
			self.rate.sleep()
		print("Near bottom")
		self.log_command.write('Near pool bottom',False,0)
		# DROP COMMAND
		print("DROPPED")
		self.log_command.write('DROPPED',False,0)
	
	def GoUp( self ):
		print("Set to default depth")
		self.log_command.write('SET DEFAULT DEPTH',False,1)
		self.auv.absolute_depth(self.default_depth)
		while not rospy.is_shutdown() and not self.isDepthOK():
			self.rate.sleep()
		self.log_command.write('REACHED DEFAULT DEPTH',False,1)
		print("Reached")

	def isDepthOK( self ):
		return self.auv.ok_position('z',0.1)

	def RunToDrum( self ):
		self.auv.velocity(x=0.3)
		while not rospy.is_shutdown():
			vis_front = self.client_drum(String('drum'), String('EDIT_THIS')).data
			vis_bottom = self.client_drum(String('drum'), String('drop')).data
			self.auv.velocity(y=self.moving_veloc*vis_front.cx*-2)
			if vis_front.n_obj == 0 or vis_bottom.n_obj > 0 and vis_bottom.cx > -0.7:
				self.log_command.write('Reach drum',False,1)
				print("Reached")
				self.auv.velocity(x=0, y=0)
				break
			self.rate.sleep()
			

	def find_drum( self ):
		# Find only one side cuz flare always on the right hand side of the robot
		self.log_command.write('Set zero yaw',False,1)
		print("Set yaw to ZERO")
		self.auv.absolute_yaw(0)
		while not rospy.is_shutdown() and not self.auv.ok_position('yaw',0.1):
			self.rate.sleep()
		self.log_command.write('Finding drum on the left hand side.',False,1)
		print("Finding drum on the left hand side.")
		if self.find(y=0.2):
			self.log_command.write('Found',False,1)
			print("Found")
			return True
		self.log_command.write('Not found',False,1)
		print("Not found")
		return False

	def find( self, **arg):
		self.foundCount = 0
		start = time.time()
		self.auv.velocity(y=arg['y'],x=arg['x'])
		while not rospy.is_shutdown():
			vis_drum = self.client_drum(String('drum'), String('EDIT_THIS')).data
			done = time.time()
			if vis_drum.n_obj > 0:
				if self.foundCount < 5:
					self.log_command.write('Found but checking for sure',False,1)
					print("Found but checking for sure")
					self.foundCount += 1
				else:
					self.log_command.write('Confirmed',False,1)
					print("Confirmed")
					self.auv.velocity(x=0, y=0)
					return True
			elif (done - start)>100:
				break
			self.rate.sleep()
		self.auv.velocity(x=0, y=0)
		self.log_command.write('Not found (Timeout)',False,1)
		print("Not found (Timeout)")
		return False

if __name__=='__main__':

	rospy.init_node("Mission Drum")		

	drum = play_drum( 30 )
	drum.setup()
	drum.play()
