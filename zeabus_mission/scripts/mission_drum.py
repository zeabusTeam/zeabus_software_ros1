#!/usr/bin/python2

import rospy
import math
import time 

from vision_collector import VisionCollector
from zeabus_vision.srv import vision_srv_golf
from standard_mission import StandardMission
from geometry_msgs.msg import Point
from std_msgs.msg import String

class MissionDrum( StandardMission ):

	def __init__( self , name ):
		self.name = name
		self.x = 100
		self.y = 100
		StandardMission.__init__( self , self.name , "/mission/drum" , self.callback )
		# rospy.Subscriber("/vision/estimate_distance", Point, self.vision_callback)
		self.request_data = rospy.ServiceProxy( "/vision/golf" , vision_srv_golf )
		
	def callback(self,msg):
		None

	# def vision_callback(self, msg):
	# 	print("Vision Callback")
	# 	self.x = msg.x
	# 	self.y = msg.y
	# 	print("x y",self.x, self.y)

	def offset_position(self):
		print("Offset to center of gripper")
		self.relative_xy(0,-0.30)	
		self.get_state()
		self.collect_state()
		y_target = self.save_state[1]
		count_ok = 0
		while( not rospy.is_shutdown() ):
			self.sleep()
			self.get_state()
			y_cur = self.temp_state[1]
			print("target",y_target)
			print("current",y_cur)

			if abs(y_cur - y_target) <= 0.05:
				count_ok += 1
				if( count_ok == 5 ):
					break
			else:
				count_ok = 0
			print("xy:count_ok",count_ok)
		print("Finish to target")
		print("Finish to target")
		print("Finish to target")
		print("Finish to target")
		print("Finish to target")

	def golf_keeping(self):
		self.velocity_z(-0.1)
		rospy.sleep(20)
		print("x y",self.x,self.y)

		self.reset_velocity('z')
		print("DONE: KEEPING")

	def run( self ):
		result = 0
		count_unfound = 0
		self.fix_z(-0.4)
		count_ok = 0
		while( not rospy.is_shutdown() ):
			self.sleep()
			if( self.check_position( "z" , 0.05 ) ):
				count_ok += 1
				if( count_ok == 5 ):
					break
			else:
				count_ok = 0
			print("z:count_ok",count_ok)

		while not rospy.is_shutdown():
			data = self.request_data(String("golf"),String("golf"))
			# print(data)
			self.x = data.point.x
			self.y = data.point.y
			if self.x == 100 or self.y == 100:
				print("Vision cannot detect")
				# res = self.reset_velocity('xy')
				# print("result reset xy",res)
				# self.velocity({'x':0,'y':0})

				continue

			r = (self.x**2 + self.y**2)**0.5
			r_str = str(r)
			print("x y",self.x,self.y)
			print("Radius in range",r_str)
			if 0.1 <= self.x <= 0.2 and -0.6 <= self.y <= -0.7:
				print("Center of golf")
				res = self.reset_velocity('xy')
				print("result reset xy",res)
				rospy.sleep(2)
				self.offset_position()
				self.golf_keeping()
				break
			else:
				print("Move to center of golf")
				if self.x == 0:
					vx = 0
				else:
					# vx = min(0.075,abs(self.x)) * (self.x/abs(self.x))
					vx = min(0.1,abs(self.x)) * (self.x/abs(self.x)-0.2)
				if self.y == 0:
					vy =0 
				else:
					# vy = min(0.075,abs(self.y)) * (self.y/abs(self.y))
					vy = min(0.1,abs(self.y)) * (self.y/abs(self.y)-0.6)
				self.velocity({'x':vx,'y':vy})

if __name__ == "__main__" :
	rospy.init_node("mission_drum")
	MG = MissionDrum( "mission_drum")
	MG.run()
