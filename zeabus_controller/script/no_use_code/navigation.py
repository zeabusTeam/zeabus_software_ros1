#!/usr/bin/env python
import rospy
from geometry_msgs.msg import Twist
from nav_msgs.msg import Odometry
from std_msgs.msg import Float64, Bool
import tf
import math
from zeabus_controller.srv import navigation_srv
position = [0, 0, 0, 0, 0, 0, 0]


def twopi(rad):
    if rad <= 0:
        return abs(rad)
    else:
        return 2 * math.pi - rad


def get_position(data):
    global position

    pose = data.pose.pose
    temp = (pose.orientation.x, pose.orientation.y,
            pose.orientation.z, pose.orientation.w)
    angular = tf.transformations.euler_from_quaternion(temp)
    position[0] = pose.position.x
    position[1] = pose.position.y
    position[2] = pose.position.z
    position[3] = angular[0]
    position[4] = angular[1]
    position[5] = angular[2]
    position[6] = math.degrees(position[5])


def drive(x=0, y=0, z=0, roll=0, pitch=0, yaw=0):
    pubTwist = rospy.Publisher('/zeabus/cmd_vel', Twist, queue_size=10)
    t = Twist()
    t.linear.x = x
    t.linear.y = y
    t.linear.z = z
    t.angular.x = roll
    t.angular.y = pitch
    t.angular.z = yaw
    for i in xrange(5):
        print t
        pubTwist.publish(t)
        rospy.sleep(0.05)


def turn_yaw_rel(degree):
    global position
    turn_yaw_abs(degree + position[6])
    print ('turn yaw relative: ', degree)


def turn_yaw_abs(degree):
    turnYawAbs = rospy.Publisher(
        '/fix/abs/yaw', Float64, queue_size=10)
    rad = math.radians(degree)
    rad = Float64(rad)
    for i in xrange(5):
        turnYawAbs.publish(rad)
        rospy.sleep(0.04)
    print ('turn yaw absolute: ', degree)


def go_to_xy(x=None, y=None, yaw=None):
    #x = final_x
    #y = final_y
    global position
    drive(0, 0, 0, 0, 0, 0)
    print ('goto')
    print (x, y)

    vx = 0.2

    if x is None:
        x = position[0]
    if y is None:
        y = position[1]
    if yaw is None:
        yaw = position[6]
    #               X
    #               |
    #               |
    # -y ----------------------- y
    #               |
    #               |
    #               -x
    start_x = position[0]
    start_y = position[1]
    print ('start_x: ')+ str(start_x)
    print ('start_y: ')+ str(start_y)
    while not rospy.is_shutdown():
        now_x = position[0]
        now_y = position[1]

        scale_x = abs(start_x - x)
        scale_y = abs(start_y - y)
        scale = scale_x**2 + scale_y**2

        delta_x = x - now_x
	abs_delta_x = abs(x - now_x)
        delta_y = -(y - now_y)
	abs_delta_y = abs(y - now_y)
        # print('x y')
        # print(x, y)
        # print('begin')
        # print (now_x, now_y)
        # print('deelta')
        # print(delta_x, delta_y)
        #######################################################################
        #begin_rad = math.atan2(delta_x, delta_y)
        #begin_deg = math.degrees(begin_rad)
        #if begin_deg < 0:
        #    begin_deg += 360
        #print('begin deg')
        #print begin_deg

        #rad = position[5]
        # print rad
        #deg = math.degrees(rad)
        #if deg < 0:
        #    deg += 360
        #deg += 90
        #deg %= 360
        #print 'deg'
       # print deg
       # #res_deg = [deg - begin_deg - 360,
       #   #         begin_deg - deg, deg - begin_deg + 360]
#	res_deg = deg

        #res_deg = min(res_deg, key=abs)
        # print('res degree')
        # print(res_deg)
        #turn_yaw_rel(res_deg)
        #rospy.sleep(4)
        #print 'drive'
        #######################################################################

        dis = (now_x - x)**2 + (now_y - y)**2
	scale_vy = abs_delta_y/scale_y
	if scale_vy < 0.16:
	    scale_vy = 0.16
        if now_y < y:
            vy =  scale_vy #0.3
        else:
            vy =  -scale_vy #-0.3
	print ('distance y: ') + str(abs(now_y - y))
        if abs(now_y - y) <= 0.026:
            vy = 0
#-------------------------------------------------------------
        scale_vx = abs_delta_x/scale_x
	if scale_vx < 0.16:
	    scale_vx = 0.16
	if now_x < x:
            vx =  scale_vx #0.5
        else:
            vx = -scale_vx #-0.5
	print ('distance x: ') + str(abs(now_x - x))
        if abs(now_x - x) <= 0.026:
            vx = 0
        print('vx: ') + str(vx) + ' ' + ('vy: ') + str(vy)
        #drive(vx + 0.1, vy, 0, 0, 0, 0)
	drive(vx, vy, 0, 0, 0, 0)
        rospy.sleep(0.3)
        print '---------finnal drive---------'
        print (dis)
        if dis <= (0.035**2):
            print('stop')
            drive(0, 0, 0, 0, 0, 0)
            rospy.sleep(2)
            break
        print position
    turn_yaw_abs(yaw)
    print ('finish')
    print position
    # m = Bool()
    # m.success = True
    return True


def navigation_callback(req):
    # req = msg.req.data
    print('request: ') + str(req)

    x = req.x
    y = req.y
    yaw = req.yaw
    return go_to_xy(x, y, yaw)

if __name__ == '__main__':
    rospy.init_node('navigation')
    rospy.Subscriber('/auv/state', Odometry, get_position)
    # rospy.Subscriber('/syrena/state', Odometry, get_position)

    rospy.Service('navigation', navigation_srv(), navigation_callback)
    rospy.spin()
