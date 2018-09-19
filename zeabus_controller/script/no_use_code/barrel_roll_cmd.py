#!/usr/bin/env python

import rospy
import Queue as Queue
import tf as tf
from nav_msgs.msg import Odometry
from geometry_msgs.msg import Twist
from geometry_msgs.msg import Quaternion

q = Queue.Queue();
got_state = False
start_orientation = 0

rotate_45 = [0.3826834,0,0,0.9238795]


def state_callback(message):
    global got_state
    global start_orientation
    start_orientation = message.pose.pose.orientation
    got_state = True

def roll(time):
    global start_orientation
    global got_state
    rospy.init_node('barrel_roll_cmd')
    rospy.Subscriber('/auv/state',Odometry,state_callback)
    cmd_vel_publisher = rospy.Publisher('/cmd_vel',Twist,queue_size=10)
    fix_orientation_publisher = rospy.Publisher('/cmd_fix_orientation',Quaternion,queue_size=10)
    # rospy.spin()

    # wait for auv state before start
    r = rospy.Rate(2)
    while not got_state and not rospy.is_shutdown():
        print('waiting state sleeping for 0.5 sec')
        r.sleep()
    
    print "got state!"
    # calculate trajectory point and put to queue
    start_euler = tf.transformations.euler_from_quaternion([start_orientation.x,
                                                            start_orientation.y,
                                                            start_orientation.z,
                                                            start_orientation.w])
    start_orientation = tf.transformations.quaternion_from_euler(0,0,start_euler[2]);
    x = start_orientation

    l = []
    for i in range(0,8):
        x = tf.transformations.quaternion_multiply(x,rotate_45)
        l.append(x)
    for i in range(0,time):
        for quat in l:
            q.put(quat)
    
    q.put(start_orientation)
    
    twist = Twist()
    twist.linear.x = 1;
    cmd = 0;
    last_cmd = q.qsize();
    print("START ROLLING")
    while not q.empty() and not rospy.is_shutdown():
        quat = q.get();
        cmd_vel_publisher.publish(twist)
        fix_orientation_publisher.publish(*quat)
        cmd = cmd + 1
        print cmd,"/",last_cmd;
        r.sleep()
    print("END OF ROLLING")
    twist.linear.x = 0;
    cmd_vel_publisher.publish(twist)


if __name__ == '__main__':
    roll(2)
