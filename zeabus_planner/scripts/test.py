#!/usr/bin/python2

import rospy

from manage_log import log
from control_auv import control_auv 

if __name__=='__main__':

	print("I will init auv ")

	test_auv = control_auv( True , "Test_AUV")

	test_auv.set_name("test_auv")

	print(" Try service depth ")
	test_auv.absolute_depth( -3 )

	print(" Try service absolute yaw ")
	test_auv.absolute_yaw(2)

	print("Try service relative yaw")
	test_auv.relative_yaw(-1)

	print("Last test publish velocity")

	rate = rospy.Rate( 10 )

	while not rospy.is_shutdown():	
		test_auv.velocity( y = 0.6 )
		rate.sleep()

