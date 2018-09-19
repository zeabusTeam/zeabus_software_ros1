#!/usr/bin/env python

#import python library
import math, rospy, tf, sys, roslib
import numpy as np

#import expansion_function
from expansion_function import expansion_quaternion, expansion_twist
from expansion_PID import PID
from manage_constant import PID_constant_file as PID_file

#------import python module for Integrate or Differential------
#from scipy.integrate import quad as integral
#import scipy.integrate as integrate
#------integral(formular, start, last, args=(a,b)) for one variable------

#import message
from nav_msgs.msg import Odometry
from geometry_msgs.msg import Twist, Point, Quaternion, Pose
from std_msgs.msg import Int16, Float64, Bool
from modbus_ascii_ros.msg import Switch

#----------import service----------
#from zeabus_controller.srv import drive_x

#import dynamic server
from dynamic_reconfigure.server import Server as dynamic_reconfigure_server
#try import for dynamic above
#from zeabus_controller.cfg import zeabus_controller_data as PID_dynamic
#from zeabus_controller.msg import PIDConstantConfig
#from zeabus_controller import *

#import for using Quartenion
from tf.msg import tfMessage
from tf.transformation import quaternion_from_euler

class controller:
	def __init__(self):
# set about print
		self.output_controller = False
		self.output_constant_PID = True
# set constant value
		self.buoyancy = -0.0
		self.first_time = True
# set k  pid for first time
		self.Kp_velocity = [0.7 , 0.7 , 0.7 , 0.15, 0.15, 0.1 ]
		self.Ki_velocity = [0.05, 0.05, 0.0 , 0.0 , 0.0 , 0.0 ]
		self.Kd_velocity = [0.0 , 0.0 , 0.0 , 0.1 , 0.1 , 0.1 ]
		self.Kp_position = [0.7 , 0.7 , 3.0 , 0.4 , 0.3 , 0.4 ]
		self.Ki_position = [0.1 , 0.1 , 0.1 , 0.1 , 0.5 , 0.01]
		self.Kd_position = [0.2 , 0.2 , 0.2 , 0.08, 0.2 , 0.01] 
		self.eq = expansion_quaternion()
		self.et = expansion_twist()
		self.error_position = numpy.array([0,0,0,0,0,0]) # x, y, z, roll, pitch, yaw
		self.fix_state = [True, True, True, True, True, True] # for fix x, y, z, roll, pitch, yaw
		self.PID_position = []
		self.PID_velocity = []
		self.out_calculate = []
		for i in range(0,6):
			self.PID_position.append(PID())
			self.PID_velocity.append(PID())
			self.out_calculate.append(0)
		self.set_PID()
		self.fix_orientation = Quaternion(0,0,0,1)
	 	rospy.init_node("controller")
		rate = rospy.Rate(10) # 10 hz
	
#Listening rospy.Subscrive("topic name", variable type, function)
 		rospy.Subscribe("/auv/state", Odometry , self.get_auv_state)  #get current state
		rospy.Subscribe("/zeabus/cmd_vel", Twist , self.get_velocity) # get future state or goal
		rospy.Subscribe("/cmd_fix_position", Point, self.get_fix_position) # get x y z
		rospy.Subscribe("/cmd_fix_orientation", Quaternion, self.get_fix_orientation) # get 
		rospy.Subscribe("/zeabus_controller/mode", Int16, self.get_mode)
		rospy.Subscribe("/fix/abs/depth", Float64, self.get_fix_depth)
		rospy.Subscribe("/fix/rel/yaw", Float64, self.get_real_yaw)
		rospy.Subscribe("/fix/abs/yaw", Float64, self.get_fix_yaw)
		rospy.Subscribe("/fix/rel/x", Float64, self.get_real_x)	
		rospy.Subscribe("/switch/data", Switch, self.get_switch)
		
#Use service rospy.Service("service name", service name, function)
#		self.result = rospy.Service("/fix_rel_x_srv", drive_x, self.request_xy) #return boolean get x y
	
#declare variable for publisher rospy.Publisher('topic name', variable type, size)
		self.cmd_vel = rospy.Publisher("/cmd_vel", Twist, queue_size = 10) # send to trust mapper
		self.fix_position_pubilsher = rospy.Publisher("/zeabus_controller/is_at_fix_position", Bool, queue_size = 10) # send for check True is success or fix position
		self.fix_orientation_pubilsher = rospy.Publisher("/zeabus_controller/is_at_fix_orientation", Bool, queue_size = 10) # same above but change position to orientation

#declare about dynamic config and create
#		self.server = dynamic_reconfigure_server(PID_dynamic, self.configurePID) # ("ConfigType", function to recieve value)
		self.server = dynamic_reconfigure_server(PIDConstantConfig, self.configurePID) # ("ConfigType", function to recieve value)

#declare node
        while not rospy.is_shutdown():
			rospy.spin()		
			rospy.sleep()
			self.cmd_vel.publish(caculate_PID())
			if(self.output_controller): 
				print("Current Velocity : {:.2f} {:.2f} {:.2f} {:.2f} {:.2f} {:.2f}".format(self.current_velocity[0] , self.current_velocity[1] , self.current_velocity[2] , self.current_velocity[3] , self.current_velocity[4] , self.current_velocity[5]))
				print("Target Velocity  : {:.2f} {:.2f} {:.2f} {:.2f} {:.2f} {:.2f}".format(self.target_velocity[0] , self.target_velocity[1] , self.target_velocity[2] , self.target_velocity[3] , self.target_velocity[4] , self.target_velocity[5]))
				print("Error Velocity   : {:.2f} {:.2f} {:.2f} {:.2f} {:.2f} {:.2f}".format(self.error_velocity[0] , self.error_velocity[1] , self.error_velocity[2] , self.error_velocity[3] , self.error_velocity[4] , self.error_velocity[5]))
				print("Error Position   : {:.2f} {:.2f} {:.2f} {:.2f} {:.2f} {:.2f}".format(self.error_position[0] , self.error_position[1] , self.error_position[2] , self.error_position[3] , self.error_position[4] , self.error_position[5]))
				print("Fixed State      : {} {} {} {} {} {}".format(self.fix_state[0] , self.fix_state[1] , self.fix_state[2] , self.fix_state[3] , self.fix_state[4] , self.fix_state[5]))
				print("Out Calculate    : {:.2f} {:.2f} {:.2f} {:.2f} {:.2f} {:.2f}".format(self.out_calculate[0] , self.out_calculate[1] , self.out_calculate[2] , self.out_calculate[3] , self.out_calculate[4] , self.out_calculate[5]))
				print("Current State    : {:.2f} {:.2f} {:.2f} {:.2f} {:.2f} {:.2f} {:.2f}".format(self.current_position.x , self.current_position.y , self.current_position.z , self.current_orientation.x , self.current_orientation.y , self.current_orientation.z , self.current_orientation.w))
				print("Current State    : {:.2f} {:.2f} {:.2f} {:.2f} {:.2f} {:.2f} {:.2f}".format(self.fix_position.x , self.fix_position.y , self.fix_position.z , self.fix_orientation.x , self.fix_orientation.y , self.fix_orientation.z , self.fix_orientation.w))
			if(self.output_constant_PID):
				set_name =["x\t:" , "y\t:" , "z\t:" , "roll\t:" , "pitch\t:" , "yaw\t:"]
				print("\t\t Kp_position\tKi_position\tKd_position\tKp_velocity\tKi_velocity\tKd_velocity")
				for i in range(0,6):
					print("\t\t{:.4f}\t{:.4f}\t{:.4f}\t{:.4f}\t{:.4f}\t{:.4f}".format(self.Kp_position[i] , self.Ki_position[i] , self.Kd_position[i] , self.Kp_velocity[i] , self.Ki_velocitiy[i] , self.Kd_velocity[i]))
										

#-------------------------------------------------------------            
#this def copy pattern of code from old code
	def caculate_error(self): #this code is find about e(t) have call by calculate_output
 		fix_orientation_quaternion = np.array( [self.fix_orientation.x , self.fix_orientation.y , self.fix_orientation.z , self.fix_orientation.w] )
		current_orientation_quaternion = np.array( [self.current_orientation.x , self.current_orientation.y , self.current_orientation.z , self.current_orientation.w] )

		first_order = fix_orientation_quaternion * self.eq.inverse(current_orientation_quaternion)
		current_orientation_quaternion = self.eq.inverse(current_orientation_quaternion) * first_order * current_orientation_quaternion

		new_orientation = self.eq.quaternion_to_msg(current_orientation_quaternion)

		roll = self.eq.get_roll(current_orientation)
		pitch = self.eq.get_pitch(current_orientation)
		yaw = self.eq.get_yaw(current_orientation)

		self.error_position[3] = roll
		self.error_position[4] = pitch
		self.error_position[5] = yaw

		diff_position_quaternion = np.array( [self.fix_position.x - self.current_position.x , self.fix_position.y - self.current_position.y , self.fix_position.z - self.current_position.z , 0] )

		length = self.eq.get_size(diff_position_quaternion)
		if(length!=0):
 			diff_position_quaternion = self.eq.get_normalize(diff_position_quaternion)
			second_order = np.array( [-self.current_orientation.x , -self.current_orientation.y , -self.current_orientation.z , self.current_orientation.w] )
			second_order = second_order * diff_position_quaternion * self.eq.inverse(second_order)
			error_point = self.eq.quaternion_to_msg(second_order)
			self.error_position[0] = error_point.x*length
			self.error_position[1] = error_point.y*length
			self.error_position[2] = error_point.z*length
		else:
			self.error_position[0] = 0
			self.error_position[1] = 0
			self.error_position[2] = 0
	#end calculate
		for i in range(0,6):
			if i > 2:
				if(math.fabs(self.error_position[i]) > math.pi):
 					if(self.error_position[i] > 0):
 						self.error_position[i] -= 2*math.pi
					else:
						self.error_position[i] += 2*math.pi
			self.error_velocity[i] = self.target_velocity[i] - self.current_velocity[i]

#--------------------------------

	def caculate_PID(self):#this code will find PID
# calculate by order of piority is position velocity
		for i in range(0,6):
			if(self.fix_state[i]):
				self.out_calculate[i] = self.PID_position[i].calculate(self.error_position[i])
			else:
				self.out_calculate[i] = self.PID_velocity[i].calculate(self.error_velocity[i])
			if(math.abs(out[i])>1):
				self.out_calculate[i]/=math.abs(out[i])
		
		self.pwm = self.et.twist_to_msg(self.out_calculate) 
# add offset force
		buoyancy_force = np.array( [0, 0, self.buoyancy, 0] )
		length = self.eq.get_size(buoyancy_force)
		if(length!=0):
 			buoyancy_force = self.eq.get_normalize(buoyancy_force)
			first_order = np.array( [-self.current_orientation.x , -self.current_orientation.y , -self.current_orientation.z , self.current_orientation.w] )
			first_order = first_order * diff_position_quaternion * self.eq.inverse(first_order)
			error_point = self.eq.quaternion_to_msg(second_order)
			self.pwm.linear.x += error_point.x*length
			self.pwm.linear.y += error_point.y*length
			self.pwm.linear.z += error_point.z*length
# end offset force
		return self.pwm
			

#-----------------------------------------------------------------
#reference is http://wiki.ros.org/tf2/Tutorials/Quaternions        
	def get_auv_state(self, auv_state): #send type Odometry
 		if(self.first_time):
			self.fix_position = auv_state.pose.pose.position
			self.fix_pose_covariance = auv_state.pose.covariance
#			self.fix_orientation = auv_state.pose.orientation # this is get Quartenion
			self.fix_twist = auv_state.twist.twist
 			self.fix_twist_covariance = auv_state.twist.covariance
			init = self.eq.msg_to_quaternion(auv_state.pose.pose.orientation)
			roll = self.eq.get_roll(init)
			pitch = self.eq.get_pitch(init)
			yaw = self.eq.get_yaw(init)
			fix_y_orientation = quaternion_from_euler(0 , 0 , yaw)
			self.fix_orientation = self.eq.quaternion_to_msg(fix_y_orientation)
			self.first_time = False 
			
		self.current_position = auv_state.pose.pose.position
		self.current_orientation = auv_state.pose.pose.orientation
		self.current_velocity = self.et.msg_to_twist(self.current_twist)
			
# End of get auv state
 		
	def get_velocity(self, target_velocity):
		self.target_velocity = self.et.msg_to_twist(target_velocity)

	def get_fix_position(self, fix_position):
		self.fix_position = fix_position
		
	def get_fix_orientation(self, fix_orientation):
		self.fix_orientation = fix_orientation

	def get_mode(self, mode):
		self.mode = mode

	def get_fix_depth(self, fix_depth):
		self.fix_depth = fix_depth

	def get_real_yaw(self, real_yaw):
		self.real_yaw = real_yaw
	
	def get_fix_yaw(self, fix_yaw):
		self.fix_yaw = fix_yaw

	def get_real_x(self, real_x):
		self.real_x = real_x

	def get_swithc(self, all_switch):
		self.all_switch = all_switch

#	def request_xy(self, request):
# 		self.x = request.x
# 		self.y = request.y 
#--------        return drive_xResponse( True ) # ???? I don't know why	
#		return True


#KPP is Kp for position and KPV is Kp for velocity
 	def configurePID(self, config, level):
# [x , y , z , roll , pitch , yaw]
		self.Kp_position = [config.KPPx, config.KPPy, config.KPPz, config.KPProll, config.KPPpitch, config.KPPyaw]
		self.Ki_position = [config.KIPx, config.KIPy, config.KIPz, config.KIProll, config.KIPpitch, config.KIPyaw]
		self.Kd_position = [config.KDPx, config.KDPy, config.KDPz, config.KDProll, config.KDPpitch, config.KDPyaw]
		self.Kp_velocity = [config.KPVx, config.KPVy, config.KPVz, config.KPVroll, config.KPVpitch, config.KPVyaw]
		self.Ki_velocity = [config.KIVx, config.KIVy, config.KIVz, config.KIVroll, config.KIVpitch, config.KIVyaw]
		self.Kd_velocity = [config.KDVx, config.KDVy, config.KDVz, config.KDVroll, config.KDVpitch, config.KDVyaw]
		self.set_PID()
		return config

	def set_PID(self): # x , y , z , roll , pitch , yaw
		for count in range(0,6):
			self.PID_position[count].set_PID(self.Kp_position[count] , self.Ki_position[count], self.Kd_position[count])
			self.PID_velocity[count].set_PID(self.Kp_velocity[count] , self.Ki_velocity[count], self.kd_position[count])
			
if __name__ == "__main__":
	zeabus_control = controller()
