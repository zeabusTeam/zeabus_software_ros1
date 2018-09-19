#!/usr/bin/python2

import rospy, math, tf
import numpy 

class coriolis_rb:

	def __init__( self, m , i_xx , i_yy , i_zz ):
		self.constant_array = numpy.asarray([
								[ 	0, 	0, 	0, 	0, 		m,		-m		],
								[ 	0, 	0, 	0, 	-m,		0,		m		],
								[	0,	0,	0,	m,		-m,		0		],
								[	0,	m,	-m,	0,		i_zz,	-i_yy	],
								[	-m,	0,	m,	-i_zz,	0,		i_xx	],
								[	m,	-m,	0,	i_yy,	-i_xx,	0		]
											])
		self.constant_matrix = numpy.asmatrix( self.constant_array)
		print( " -------- check constant of coriolis rb ----------\n")
		print( " -------- constant \n")
		print(self.constant_matrix)
		print( "\n")
		print( "shape of matrix is " ) 
		print( self.constant_matrix.shape )
		print( "size of matrix is ")
		print(self.constant_matrix.size)

	def input_value(self , v_x , v_y , v_z ):
		self.dynamic_matrix = numpy.matrix.copy( self.constant_matrix )
		print( "\n\n ------------- original matrix ")
		print( self.dynamic_matrix )
		
		self.dynamic_matrix[ 0 , 4 ] *= v_z  
		self.dynamic_matrix[ 0 , 5 ] *= v_y
  
		self.dynamic_matrix[ 1 , 3 ] *= v_z  
		self.dynamic_matrix[ 1 , 5 ] *= v_x  

		self.dynamic_matrix[ 2 , 3 ] *= v_y  
		self.dynamic_matrix[ 2 , 4 ] *= v_x  

		self.dynamic_matrix[ 3 , 1 ] *= v_z  
		self.dynamic_matrix[ 3 , 2 ] *= v_y 
		self.dynamic_matrix[ 3 , 4 ] *= v_z  
		self.dynamic_matrix[ 3 , 5 ] *= v_y 
 
		self.dynamic_matrix[ 4 , 0 ] *= v_z  
		self.dynamic_matrix[ 4 , 2 ] *= v_x 
		self.dynamic_matrix[ 4 , 3 ] *= v_z  
		self.dynamic_matrix[ 4 , 5 ] *= v_x 

		self.dynamic_matrix[ 5 , 0 ] *= v_y  
		self.dynamic_matrix[ 5 , 1 ] *= v_x 
		self.dynamic_matrix[ 5 , 3 ] *= v_y  
		self.dynamic_matrix[ 5 , 4 ] *= v_x 

		print( "\n\n ------------- new matrix ")
		print( self.dynamic_matrix )

		print( "\n\n ------------- constant matrix")
		print( self.constant_matrix )

		return self.dynamic_matrix

class coriolis_a:

	def __init__( self, a_1 , a_2 , a_3 , b_1 , b_2 , b_3 ):
		self.constant_array = numpy.asarray([
								[ 	0, 	0, 	0, 	0, 		-1,		1		],
								[ 	0, 	0, 	0, 	1,		0,		-1		],
								[	0,	0,	0,	-1,		1,		0		],
								[	0,	-1,	1,	0,		-1,		1		],
								[	1,	0,	-1,	1,		0,		-1		],
								[	-1,	1,	0,	-1,		1,		0		]
											])
		self.constant_matrix = numpy.asmatrix( self.constant_array)
		print( " -------- check constant of coriolis a ----------\n")
		print( " -------- constant \n")
		print(self.constant_matrix)
		print( "\n")
		print( "shape of matrix is " ) 
		print( self.constant_matrix.shape )
		print( "size of matrix is ")
		print(self.constant_matrix.size)

		self.constant_a_1 = a_1
		self.constant_a_2 = a_2
		self.constant_a_3 = a_3
		self.constant_b_1 = b_1
		self.constant_b_2 = b_2
		self.constant_b_3 = b_3	

		print("-------- all constant --------")
		print( self.constant_a_1 )
		print( self.constant_a_2 )
		print( self.constant_a_3 )
		print( self.constant_b_1 )
		print( self.constant_b_2 )
		print( self.constant_b_3 )

# u , v , w , p , q , r == x , y ,  z , r , p , y

	def input_value( self , u , v , w , p , q , r):

		print( " In input value A")
		
		self.dynamic_matrix = numpy.matrix.copy( self.constant_matrix)
		print( "\n\n ------------- original matrix ")
		print( self.dynamic_matrix )

		self.dynamic_matrix[ 0 , 4 ] *= self.a_3(u , w , q)  
		self.dynamic_matrix[ 0 , 5 ] *= self.a_2(v , p , r)
  
		self.dynamic_matrix[ 1 , 3 ] *= self.a_3(u , w , q)  
		self.dynamic_matrix[ 1 , 5 ] *= self.a_1(u , w , q)  

		self.dynamic_matrix[ 2 , 3 ] *= self.a_2(v , p , r)  
		self.dynamic_matrix[ 2 , 4 ] *= self.a_1(u , w , q)  

		self.dynamic_matrix[ 3 , 1 ] *= self.a_3(u , w , q)  
		self.dynamic_matrix[ 3 , 2 ] *= self.a_2(v , p , r) 
		self.dynamic_matrix[ 3 , 4 ] *= self.b_3(v , p , r)  
		self.dynamic_matrix[ 3 , 5 ] *= self.b_2(u , w , q) 
 
		self.dynamic_matrix[ 4 , 0 ] *= self.a_3(u , w , q)  
		self.dynamic_matrix[ 4 , 2 ] *= self.a_1(u , w , q) 
		self.dynamic_matrix[ 4 , 3 ] *= self.b_3(v , p , r)  
		self.dynamic_matrix[ 4 , 5 ] *= self.b_1(v , p , r) 

		self.dynamic_matrix[ 5 , 0 ] *= self.a_2(v , p , r)  
		self.dynamic_matrix[ 5 , 1 ] *= self.a_1(u , w , q) 
		self.dynamic_matrix[ 5 , 3 ] *= self.b_2(u , w , q)  
		self.dynamic_matrix[ 5 , 4 ] *= self.b_1(v , p , r) 

		print( "\n\n ------------- new matrix ")
		print( self.dynamic_matrix )

		print( "\n\n ------------- constant matrix")
		print( self.constant_matrix )

		return self.dynamic_matrix

	def a_1( self , n_1 , n_2 , n_3 ):
		return self.constant_a_1[0]*n_1 + self.constant_a_1[1]*n_2 + self.constant_a_1[2]*n_3

	def a_2( self , n_1 , n_2, n_3):
		return self.constant_a_2[0]*n_1 + self.constant_a_2[1]*n_2 + self.constant_a_2[2]*n_3

	def a_3( self , n_1 , n_2, n_3):
		return self.constant_a_3[0]*n_1 + self.constant_a_3[1]*n_2 + self.constant_b_3[2]*n_3

	def b_1( self , n_1 , n_2 , n_3 ):
		return self.constant_b_1[0]*n_1 + self.constant_b_1[1]*n_2 + self.constant_b_1[2]*n_3

	def b_2( self , n_1 , n_2, n_3):
		return self.constant_b_2[0]*n_1 + self.constant_b_2[1]*n_2 + self.constant_b_2[2]*n_3

	def b_3( self , n_1 , n_2, n_3):
		return self.constant_b_3[0]*n_1 + self.constant_b_3[1]*n_2 + self.constant_b_3[2]*n_3

if __name__=='__main__':
	test_c_rb = coriolis_rb( 1 , 0.2 , 0.3 , 0.4)
	test_c_rb.input_value(1 , 2 , 3)

	test_c_a = coriolis_a( 	[1 , 2 , 3 ] , [ 4 , 5 , 6 ] , [ 7 , 8 , 9] ,
							[9 , 8 , 7 ] , [ 6 , 5 , 4 ] , [ 3 , 2 , 1])
