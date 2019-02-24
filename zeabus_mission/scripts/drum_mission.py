#!/usr/bin/python2.7

import rospy
import math
import time 

from vision_collector import VisionCollector

from standart_mission import StandardMission
from geometry_msgs.msg import Point

class MissionDrum( StandardMission ):

	def __init__( self , name ):
		self.name = name
		self.x = 100
		self.y = 100
		# StandardMission.__init__( self , self.name , "/mission/drum" , self.callback )
		rospy.Subscriber("/vision/estimate_distance", Point, vision_callback)

	def vision_callback(msg):
		print("Vision Callback")
		self.x = msg.x
		self.y = msg.y
		print(self.x, self.y)

	def offset_position(self):
		print("Offset to center of gripper")
		self.relative_xy(0,-0.15)		

	def golf_keeping(self):
		self.velocity_z(-0.1)
		rospy.sleep(2)
		self.reset_velocity('z')
		print("DONE: KEEPING")

	def run( self ):
		result = 0
		count_unfound = 0
		while not rospy.is_shutdown():
			if self.x == 100 or self.y == 100:
				print("Vision cannot detect")
				self.reset_velocity('x')
				self.reset_velocity('y')
				continue

			r = (self.x**2 + self.y**2)**0.5

			if r <= 0.1:
				print("Center of golf")
				self.reset_velocity('x')
				self.reset_velocity('y')
				self.offset_position()
				self.golf_keeping()
				break
			else:
				print("Move to center of golf")
				vx = min(0.3,abs(self.position.x)) * (self.position.x/abs(self.position.x))
				vy = min(0.3,abs(self.position.y)) * (self.position.y/abs(self.position.y))
				self.velocity_xy(vx,vy)


			
			
					
if __name__ == "__main__" :
	rospy.init_node("mission_drun")
	MG = MissionGate( "mission_drum")
	MG.run()