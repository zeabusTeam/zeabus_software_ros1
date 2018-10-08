#!/use/bin/env python2

import rospkg # get path to package in ROS

from time import localtime #, gmtime #, strftime # import library for get time

class log:
# init part when declare class	
	# required 3 parameter package = name of package to collect in that package
	#					   path = path to go to directory to collect file
	#					   name file = waht name do you want
	# please create directory to already to access
	def __init__( self , package , path , name_file ):

		ros_package = rospkg.RosPack() # use to init object to get path
		
		current_time = localtime() # get current time ( localtime on computer )

		#get path to file to open and write it
		file_to_open =	str(ros_package.get_path( package )) + "/" + str( path )  
		file_to_open += "/" + name_file + "_" + str( current_time.tm_year ) 
		file_to_open += "-" + str( current_time.tm_mon ) + "-" + str( current_time.tm_mday ) 
		file_to_open += "-" + str( current_time.tm_hour) + "-" + str( current_time.tm_min) 
		file_to_open += "-" + str( current_time.tm_sec ) + ".txt"

		#open required 2 parameter first is name_file , 2 mode r==Read w==Write a=Appending 
															# r+== Read & Write
		self.file_log = open( file_to_open , "w")

	def write( self , data , heading_time = False , amont_tab = 0):
		# you want to save time to collect or not
		if( heading_time ):
			time = str(self.get_local_time())
			self.file_log.write( "---------------------> "  + str(time) +
								 " <---------------------\n" )
		data_write = ""
		#for get tab
		for amont in range ( 0 , amont_tab ):
			data_write += "\t"

		#write file
		self.file_log.write( data_write + data + "\n")

	#return string of current time
	def get_local_time( self ):
		
		current_time = localtime()

		answer =	str( current_time.tm_year ) + "-" + str( current_time.tm_mon ) 
		answer +=	"-" + str( current_time.tm_mday ) + "-" + str( current_time.tm_hour) 
		answer +=	"-" + str( current_time.tm_min)
		return str(answer)

	def close( self ):
		self.file_log.close()	
