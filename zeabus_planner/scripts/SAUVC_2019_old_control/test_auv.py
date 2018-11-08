#!/usr/bin/python2

import rospy

try:
	from zeabus_extension.manage_log import log
	from zeabus_extension.control_auv import control_auv 
except:
	print("Pleas install setup.bash in zeabus_extension package")
	exit()

if __name__=='__main__':
	rospy.init_node("Mission_Planner")		
	auv = control_auv("Connection")
#	auv.fire_gripper()
	auv.pull_gripper()
