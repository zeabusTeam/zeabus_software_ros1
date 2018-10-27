<<<<<<< HEAD
#!/usr/bin/python2

import rospy
from sauvc_01_gate_v2 import play_gate
from sauvc_02_flare_v1 import play_flare

=======
import rospy
from sauvc_01_gate_v2 import play_gate
from sauvc_02_flare_v1 import play_flare
>>>>>>> 83fa1328fa7e18f2b4f4f93a255325df65afe6ad
try:
	from zeabus_extension.manage_log import log
	from zeabus_extension.control_auv import control_auv 
except:
	print("Pleas install setup.bash in zeabus_extension package")
	exit()

if __name__=="__main__":
<<<<<<< HEAD
	rospy.init_node("Mission_Planner")		
	auv = control_auv("Connection")
	gate = play_gate( 30 )
	gate.setup( 2 , 4 , 1 , 2)
=======
	

	rospy.init_node("Mission Planner")		
	auv = control_auv("Connection")
	gate = play_gate( 30 )
	gate.setup( 4 , 4 , 1 , 2)
>>>>>>> 83fa1328fa7e18f2b4f4f93a255325df65afe6ad
	gate.play()

	print("Finish play gate")
	auv.relative_yaw( -1.57 )
	rate = rospy.Rate(30)
	while( not rospy.is_shutdown() and not auv.ok_position("yaw" , 0.1)):
		rate.sleep()
	auv.collect_position()
<<<<<<< HEAD
	print("Connection Forward")
	while( not rospy.is_shutdown() and auv.calculate_distance() < 1):
		print( auv.calculate_distance() )
		auv.velocity( x = 0.3)
		rate.sleep()
	print("Connection wait yaw")
	for test in range ( 0 , 10 ):
		rate.sleep()
	while( not rospy.is_shutdown() and not auv.ok_position("yaw" , 0.1)):
		rate.sleep()
	print("Connection Left")
	auv.collect_position()
	while( not rospy.is_shutdown() and auv.calculate_distance() < 1.5):
		print( auv.calculate_distance() )
=======
	print("Connecttion Forward")
	while( not rospy.is_shutdown() and auv.calculate_distance() < 3):
		auv.velocity( x = 0.3)
		rate.sleep()
	print("Connecttion wait yaw")
	while( not rospy.is_shutdown() and not auv.ok_position("yaw" , 0.1)):
		rate.sleep()
	print("Connecttion Left")
	while( not rospy.is_shutdown() and auv.calculate_distance() < 3):
>>>>>>> 83fa1328fa7e18f2b4f4f93a255325df65afe6ad
		auv.velocity( y = 0.5)
		rate.sleep()
	print("Now play flare")

	flare = play_flare( 30 )
	flare.set_up()
	flare.play()
