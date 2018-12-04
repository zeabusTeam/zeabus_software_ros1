#!/usr/bin/python2
"""
    File name: navigator.py
    Author: skconan
    Date created: 2018/12/01
    Python Version: 2.7
    Description: AUV Navigation 
"""

import rospy
import math
from zeabus_vision.srv import *
from zeabus_vision.msg import *
from std_msgs.msg import String, Bool, Int8
from zeabus_planner_share.auv_controller import AUVController


class Navigator:
    def __init__(self, name):
        self.name = name
        self.auv = AUVController()
        self.object_radius = 2
        self.found = 0
        self.found_expected = 10

    def forward(self, velocity):
        self.auv.velocity({'x': velocity})

    def backward(self, velocity):
        self.auv.velocity({'x': -velocity})

    def left(self, velocity):
        self.auv.velocity({'y': velocity})

    def right(self, velocity):
        self.auv.velocity({'y': -velocity})
    
    def up(self, velocity):
        self.auv.velocity({'z': velocity})
    
    def down(self, velocity):
        self.auv.velocity({'z': -velocity})
    
    def check_around_object(self):
        pass
    
    def is_found(self):
        if self.found > self.found_expected:
            return True
        else:
            return False


if __name__ == "__main__":
    nav = Navigator("Gate")
    # nav.run()
    # rospy.spin()
