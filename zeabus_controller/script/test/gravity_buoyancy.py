import rospy, math, tf

import numpy

class gravitational_buoyancy_force:
	
	def __init__(self,  w , b , x_cm , x_cb , y_cm , y_cb , z_cm , z_cb):

		self.w_b = 1.0 * ( w-b )
		self.x_cm_x_cb = 1.0 * ( x_cm*w - x_cb*b)
		self.y_cm_y_cb = 1.0 * ( y_cm*w - y_cb*b)
		self.z_cm_z_cb = 1.0 * ( z_cm*w - z_cb*b) 

		print( " constant value for gravitational and buoyancy force")
		print( self.w_b)
		print( self.x_cm_x_cb)
		print( self.y_cm_y_cb)
		print( self.z_cm_z_cb)

	def input_value(self, roll , pitch ,yaw):
		return numpy.asmatrix([
					[ 1.0 * self.w_b * math.sin( pitch) ],
					[ -1.0 * self.w_b * math.sin( yaw ) * math.cos( pitch)],
					[ -1.0 * self.w_b * math.cos( yaw ) * math.cos (pitch)],
					[ 
						(( -1.0 * self.y_cm_y_cb * math.cos(pitch) * math.cos(roll) ))
						+ ( 1.0 *self.z_cm_z_cb *math.cos(yaw) )
					],
					[ 
						( 1.0 * self.z_cm_z_cb * math.sin(pitch) )
						+ ( 1.0 * self.x_cm_x_cb * math.cos(pitch) * math.cos(roll) )
					],
					[
						( -1.0 * self.x_cm_x_cb * math.cos(pitch) * math.sin(roll))
						- self.y_cm_y_cb * math.sin(pitch)
					] 
							  ])

if __name__=='__main__':
	test = gravitational_buoyancy_force( -10 , 10 , 1, 2, 3, 4, 5, 6)
	print( test.input_value( 0 , 0 , 0))	
