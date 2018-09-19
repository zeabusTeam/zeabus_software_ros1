#!/use/bin/env python2

import rospkg # get path to package in ROS

from time import localtime, #gmtime #, strftime # import library for get time

class manage_log:
# init part when declare class	
	# required 3 parameter package = name of package to collect in that package
	#					   path = path to go to directory to collect file
	#					   name file = waht name do you want
	# please create directory to already to access
	def __init__( self , package , path , name_file ):

		ros_package = rospkg.RosPack() # use to init object to get path
		
		current_time = localtime() # get current time ( localtime on computer )

		#get path to file to open and write it
		file_to_open =	ros_package.get_path( package ) + "/" + path + 
						"/" + name_file + "_" + str( current_time.tm_year ) +
						"-" + str( current_time.tm_mon ) + "-" + str( current_time.tm_mday ) +
						"-" + str( current_time.tm_hour) + "-" + str( current_time.min) +
						"-" + str( current_time.tm_sec ) + ".txt"

		#open required 2 parameter first is name_file , 2 mode r==Read w==Write a=Appending 
															# r+== Read & Write
		self.file_log = open( file_to_open , "w")

	def write( self , data , heading_time = False , amont_tab = 0):
		# you want to save time to collect or not
		if( heading_time ):
			self.file_log.write( "---------------------> " + self.get_localtime +
								 " <---------------------" )
		data_write = ""
		#for get tab
		for amont in range amont_tab:
			data_write += "\t"

		#write file
		self.file_log.write( data_write + data)

	#return string of current time
	def get_local_time( self ):
		
		current_time = localtime()

		return	str( current_time.tm_year ) + "-" + str( current_time.tm_mon ) + 
				"-" + str( current_time.tm_mday ) + "-" + str( current_time.tm_hour) + 
				"-" + str( current_time.min)
				
