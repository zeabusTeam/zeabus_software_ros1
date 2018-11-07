#!/usr/bin/env python

import rospy
from zeabus_control.msg import Type2 # message for receive data to show plot

import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec
import numpy as np

class plot_state:

	def __init__( self ):
	
		rospy.init_node("plot_state")
#		self.title_name = rospy.get_param("/title_name")
#		self.sub_topic = rospy.get_param("/sub_topic")
#		self.max_data = rospy.get_param("/max_data")
		# set up default setup
		self.title = "CONTROL ROBOT ERROR"
		self.sub_topic = "/control/robot_error"
		self.max_data = 20;
		self.run_data = [];
		for run in range ( 0 , self.max_data ):
			self.run_data.append( run+1 )

		self.x_label = [ 'axis_X' , 'axis_Y' , 'axis_Z' , 'ROLL' , 'PITCH' , 'YAW']

		self.my_figure	= plt.figure( tight_layout=True)
		self.my_grids	= gridspec.GridSpec( 4 , 2 )
		self.my_graph	= []
		self.my_graph.append( self.my_figure.add_subplot( self.my_grids[1 , 0] ))
		self.my_graph.append( self.my_figure.add_subplot( self.my_grids[1 , 1] ))
		self.my_graph.append( self.my_figure.add_subplot( self.my_grids[0 , :] ))
		self.my_graph.append( self.my_figure.add_subplot( self.my_grids[3 , 0] ))
		self.my_graph.append( self.my_figure.add_subplot( self.my_grids[3 , 1] ))
		self.my_graph.append( self.my_figure.add_subplot( self.my_grids[2 , :] ))


		self.all_data = [ [0]*self.max_data , [0]*self.max_data , [0]*self.max_data 
						, [0]*self.max_data , [0]*self.max_data , [0]*self.max_data ]
		self.style_data = [ 'b' , 'g' , 'c' , 'm' , 'y' , 'k'];

		self.count = 0

		rospy.Subscriber( self.sub_topic , Type2 , self.sub_state , queue_size = 1)
		
		plt.show()
		
	def sub_state( self , message ):
		self.all_data[0].append( message.linear.x )
		self.all_data[1].append( message.linear.y )
		self.all_data[2].append( message.linear.z )
		self.all_data[3].append( message.angular.x )
		self.all_data[4].append( message.angular.y )
		self.all_data[5].append( message.angular.z )

		
		for run in range ( 0 , 6 ):
			self.all_data[ run ] = self.all_data[ run ][-1*self.max_data:] 
		

		for run in range(0 , 6):
			self.my_graph[run].set_xlabel( self.x_label[run] )
			self.my_graph[run].plot(self.all_data[run] , self.style_data[run])	
		plt.pause(0.000000000000000000001)
		for run in range(0 , 6):
			self.my_graph[run].clear()
		
	
if __name__=="__main__":
	plot_graph = plot_state()	
	print("before spin")
	rospy.spin()
	print("after spin")

