#!/usr/bin/env python

import sys
import select
import tty
import termios
import rospy
from geometry_msgs.msg import *


class SyrenaKeyboardTeleop(object):
    """docstring for SyrenaKeyboardTeleop"""
    def __init__(self, arg={}):
        # super(SyrenaKeyboardTeleop, self).__init__()
        self.arg = arg



        self.publisher = rospy.Publisher('/zeabus/cmd_vel', Twist , queue_size=10)

        self.keyboard_map = {
            'w' : (( 1, 0, 0),(0, 0, 0)),
            's' : ((-1, 0, 0),(0, 0, 0)),
            'a' : (( 0, 1, 0),(0, 0, 0)),
            'd' : (( 0,-1, 0),(0, 0, 0)),
            'q' : (( 0, 0, 0),(0, 0, 1)),
            'e' : (( 0, 0, 0),(0, 0,-1)),
            'r' : (( 0, 0, 1),(0, 0, 0)),
            'f' : (( 0, 0,-1),(0, 0, 0)),
            ' ' : (( 0, 0, 0),(0, 0, 0)),
        }

        self.stop = ((0,0,0),(0,0,0))

    #     while not rospy.is_shutdown():

    #         loop

    def hit(self,x):

        x=x.lower()

        if x not in self.keyboard_map:
            vel_t = self.stop
        else:
            vel_t = self.keyboard_map[x]


        vel = Twist(Vector3(*vel_t[0]),Vector3(*vel_t[1]))
        print vel


        self.publisher.publish(vel)
        rospy.sleep(0.025)
        self.publisher.publish(Twist())





def isData():
    return select.select([sys.stdin], [], [], 0) == ([sys.stdin], [], [])




if __name__ == '__main__':
    x = SyrenaKeyboardTeleop()
    rospy.init_node("syrena_keyboard_teleop")

    old_settings = termios.tcgetattr(sys.stdin)
    try:
        tty.setcbreak(sys.stdin.fileno())

        #i = 0
        while not rospy.is_shutdown():
            #print i
            #i += 1

            if isData():
                c = sys.stdin.read(1)

                x.hit(c)
                if c == '\x1b':         # x1b is ESC
                    break
            #else:
            #    print 0

    finally:
        termios.tcsetattr(sys.stdin, termios.TCSADRAIN, old_settings)

    rospy.spin()


