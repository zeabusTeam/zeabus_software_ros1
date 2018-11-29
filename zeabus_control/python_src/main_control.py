#!/usr/bin/env python
"""
	File name			: main_control.py
	Author				: Supasan Komonlit
	Date created		: 2018 , NOV 26
	Date last Modified	: 2018 , ??? ??
	Python Version		: 2.7
	Purpose				: This is main file of control to use in zeabus AUV
"""

from __future__ import print_function

__author__		= "Supasan Komonlit"
__credits__		= ["Supasan Komonlit" , "ROS (Robot Operating System)"]

__license__		= "BSD"
__version__		= "0.0.1"
__maintainer__	= "Supasan Komonlit"
__email__		= "supasan.k@ku.th"
__status__		= "Production"

import rospy

import numpy
import time

class MainControl:

	def __init__( self ):
		print( "Init Function of Class MainControl" )

if __name__=="__main__":
	print("Start Class MainControl")
	rospy.init_node( "control_main" )
