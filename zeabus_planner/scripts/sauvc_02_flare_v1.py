#!/usr/bin/python2

import rospy
import math

try:
	from zeabus_extension.manage_log import log
	from zeabus_extension.control_auv import control_auv 
except:
	print("Pleas install setup.bash in zeabus_extension package")
	exit()

from zeabus_vision.srv import vision_srv_flare
from zeabus_vision.msg import vision_flare
from std_msgs.msg import String

class play_flare:

	def __init__( self , rate ):
		self.already_setup = False

		print("Waiting flare service")
		rospy.wait_for_service('vision_flare')
		print("I found flare service")
		
		self.rate = rospy.Rate( rate)

	def set_up( self ):
		self.already_setup = True
		self.log_command = log( "zeabus_planner" , "log" , "02_flare_command")
		self.log_vision = log( "zeabus_planner" , "log" , "02_flare_vision")
		self.client_flare = rospy.ServiceProxy('vision_flare' , vision_srv_flare )
		self.data_have = False
		self.data_center_x = 0
		self.data_center_y = 0
		self.data_area = 0
		self.auv = control_auv( "play flare" )
		self.past_have = False
		self.past_center_x = 0
		self.past_center_y = 0
		self.past_area = 0
		self.near_mode = False
		self.far_mode = False
		self.past_mode = "forward"

	def play( self ):
		if( not self.already_setup ):
			self.set_up()
		
		self.log_command.write("I will start now", True , 0)
		# plan is move left forward right and forward left continue
		self.survey_left( )

#-------------------------------- For Survey in Left mode ---------------------------------------
	def survey_left( self ):	
		self.auv.collect_position()	
		self.log_command.write("I will survey left", True , 0)
		while not rospy.is_shutdown():

			self.auv.velocity( y = 0.3 )

			self.rate.sleep()	

			self.request_data( 5 , "near" )
			if( self.data_have ):
				self.log_command.write("I found in near" , False , 1)
				self.near_mode = True
				self.copy_current_to_past()
				self.auv.velocity()
				break
			else:
				self.log_command.write("I not found in near mode" , False , 1)
			
			self.request_data( 5 , "far" )
			if( self.data_have ):
				self.log_command.write("I found in far" , False , 1)
				self.far_mode = True
				self.copy_current_to_past()
				self.auv.velocity()
				break
			else:
				self.log_command.write("I not found in far mode" , False , 1)
			if( self.auv.calculate_distance() > 3 ): 
				print( "result distance is " + str( self.auv.calculate_distance()))
				break
		
		if(self.near_mode): self.find_near()
		elif(self.far_mode): self.find_far()
		elif( self.past_mode == 'forward'):
			self.past_mode = 'left'
			self.survey_forward()
	
#-------------------------------- For Survey in right mode --------------------------------------
	def survey_right( self ):	
		self.auv.collect_position()
		self.log_command.write("I will survey right", True , 0)
		print("Now survey right")
		while not rospy.is_shutdown():

			self.auv.velocity( y = -0.3 )

			self.rate.sleep()	

			self.request_data( 5 , "near" )
			if( self.data_have ):
				self.log_command.write("I found in near" , False , 1)
				self.near_mode = True
				self.copy_current_to_past()
				self.auv.velocity()
				break
			else:
				self.log_command.write("I not found in near mode" , False , 1)
			
			self.request_data( 5 , "far" )
			if( self.data_have ):
				self.log_command.write("I found in far" , False , 1)
				self.far_mode = True
				self.copy_current_to_past()
				self.auv.velocity()
				break
			else:
				self.log_command.write("I not found in far mode" , False , 1)
			if( self.auv.calculate_distance() > 3): break
		if(self.near_mode): self.find_near()
		elif(self.far_mode): self.find_far()
		elif( self.past_mode == 'forward'):
			self.past_mode = 'right'
			self.survey_forward()
#-----------------------------------End part----------------------------------------------------

#------------------------------------ Part go forward ------------------------------------------
	def survey_forward( self ):
		self.auv.collect_position()
		self.log_command.write("I will move forward" , True , 0 )
		print("Go Forward")
		while not rospy.is_shutdown():
			self.auv.velocity( x = 0.1 )
			
			self.rate.sleep()

			self.request_data( 5 , "near" )
			if( self.data_have ):
				self.log_command.write("I found in near" , False , 1)
				self.near_mode = True
				self.copy_current_to_past()
				self.auv.velocity()
				break
			else:
				self.log_command.write("I not found in near mode" , False , 1)
			
			self.request_data( 5 , "far" )
			if( self.data_have ):
				self.log_command.write("I found in far" , False , 1)
				self.far_mode = True
				self.copy_current_to_past()
				self.auv.velocity()
				break
			else:
				self.log_command.write("I not found in far mode" , False , 1)
			if( self.auv.calculate_distance() > 1): break
		if(self.near_mode): self.find_near()
		elif(self.far_mode): self.find_far()
		elif( self.past_mode == 'right'):
			self.past_mode = 'forward'
			self.survey_left()
		elif( self.past_mode == 'left'):
			self.past_mode = 'forward'
			self.survey_right()
#------------------------------------ End part ------------------------------------------------
		
#---------------------------- Part for ever found in near -------------------------------------	
	def find_near( self ):
		self.log_command.write("I play on near mode", True , 0)
		print( "Now play on near mode ")
		count_not_found = 0
		while not rospy.is_shutdown():
			self.request_data( 6 , "near")
			self.rate.sleep()

			if( self.data_have ):
				count_not_found = 0
				if( abs(self.data_center_x) < 0.1 ):
					self.log_command.write("I move forward")
					print("now move forward")
					self.rate.sleep()
					self.auv.velocity( x = 0.15 )
				elif( self.data_center_x < 0 ):
					print("now move left")
					self.rate.sleep()
					self.auv.velocity( y = 0.15 )
				else:
					self.rate.sleep()
					print("now move right")
					self.auv.velocity( y = -0.2 )
			elif( count_not_found < 6):
				count_not_found += 1
				print( "Not found in near mode" )
			elif(count_not_found == 6):
				self.log_command.write("Finish this part" , True)
				print("I think it finish")
				self.log_vision.write("I can't found in near mode. That finish?")
				self.auv.absolute_depth(0)
				break
			else:
				print("What is this")
#----------------------------------------------------------------------------------------------

#----------------------------- Part for ever found in far ------------------------------------
	def find_far(self ):
		self.log_command.write("I play on far mode" , True)
		print( "Now play on far mode")
		while not rospy.is_shutdown():
			self.request_data( 6 , "far" )
			self.rate.sleep()

			if( self.data_have ):
				think_to_break = 0
				if( abs(self.data_center_x) < 0.3 ):
					self.log_command.write("I move forward")
					print("now move forward")
					self.auv.velocity( x = 0.2 )
					self.rate.sleep()
					self.auv.velocity( x = 0.2 )
					self.rate.sleep()
				elif( self.data_center_x < 0 ):
					print("move left in far mode")
					self.log_command.write("I move left")
					self.auv.velocity( y = 0.2)
				else:
					print("move right in far mode")
					self.log_command.write("I move right")
					self.auv.velocity( y = -0.2 )
				continue
			else:
				self.log_vision.write("I can't found in far mode. Why?")
				think_to_break += 1
			
			self.request_data( 5 , "near")
			if( self.data_have ):
				self.log_command.write("I found near Wow")
				print("Found near")
				break
			else:
				self.log_command.write("Don't found by use near")
				print("------------------------------->don't found by near mode in far mode")
				if( think_to_break == 1): 
					break
					print("Will survey again")
		if( self.data_have ):
			self.find_near()
		elif( think_to_break == 1):
			if( self.past_center_x < 0 ): self.survey_left()
			else: self.survey_right()
#---------------------------------------- End part --------------------------------------------	

	def request_data( self , amont , type_request):
		round_found = 0
		round_unfound = 0
		message = "Start request data and type is " + type_request
		self.log_vision.write( message , True)
		first_order = String()
		first_order.data = "flare"
		second_order = String()
		second_order.data = type_request
		while( round_found < amont and round_unfound < amont):
			receive_data = self.client_flare( first_order , second_order ).data 
#			print("Watch Type")
#			print( receive_data )
#			print( type(receive_data) )
#			print( receive_data.data )
			if( receive_data.n_obj >= 1 ):
				message = "Found data and result is cx:" + str(receive_data.cx)
				message += "\tcy:" + str(receive_data.cy)
				message += "\tarea:" + str(receive_data.area)
				self.log_vision.write(message, False, 2)
				round_found += 1
				self.data_center_x += receive_data.cx
				self.data_center_y += receive_data.cy
				self.data_area += receive_data.area	
			else:
				round_unfound += 1
				self.log_vision.write("Not Found", False , 3)
		if( round_unfound == amont):
			self.log_vision.write("Last result is not found" , False , 1)
			print("The last result is False")
			self.data_have = False
		else:
			self.data_have = True
			self.data_center_x = self.data_center_x / amont
			self.data_center_y = self.data_center_y / amont
			self.data_area = self.data_area / amont
			message = "Last result is Found data and data is cx:" + str(self.data_center_x)
			message += "\tcy:" + str(self.data_center_y) 
			message += "\tarea:" +str(self.data_area)
			print( message )
			self.log_vision.write( message )
		
	def copy_current_to_past( self ):
		self.past_have = self.data_have
		self.past_center_x = self.data_center_x
		self.past_center_y = self.data_center_y
		self.past_area = self.data_area	

	def distance( x_1 = 0 , y_1 = 0 , x_2 = 0 , y_2 = 0):
		print( "on distance function")
		print(x_1)
		x_1 = float(x_1)
		x_2 = float(x_2)
		y_1 = float(y_1)
		y_2 = float(y_2)
		answer = math.sqrt( (x_1-x_2)**2 + (y_1-y_2)**2 )
		print( answer )
		return answer
#		return math.sqrt( math.pow( x_1 - x_2 , 2 ) + math.pow( y_1 - y_2 , 2))

if __name__=='__main__':

	rospy.init_node("Mission Flare")		

	flare = play_flare( 30 )
	flare.set_up()
	flare.play()
