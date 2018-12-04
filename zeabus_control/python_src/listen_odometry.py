#!/usr/bin/env python
"""
	File name			: listen_odometry.py
	Author				: Supasan Komonlit
	Date created		: 2018 , NOV 26
	Date last Modified	: 2018 , ??? ??
	Python Version		: 2.7
	Purpose				: This is subfile of main_control for listen message nav_msgs/Odometry
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

from nav_msgs.msg import Odometry

class ListenOdometry:

	def __init__( self ):
		None	
