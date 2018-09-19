#!/usr/bin/python2

import rospy , math , tf
import numpy

class hydrodynamic_i:
	
	def __init__( self , x , y , z , k , m , n):
		self.constant_array = numpy.asarray([
								[	x,	0,	0,	0,	0,	0],
								[	0,	y,	0,	0,	0,	0],
								[	0,	0,	z,	0,	0,	0],
								[	0,	0,	0,	k,	0,	0],
								[	0,	0,	0,	0,	m,	0],
								[	0,	0,	0,	0,	0,	n]
											])
		self.constant_matrix = numpy.asmatrix( self.constant_array)
		print( " -------- check constant of hydrodynamic rb ----------\n")
		print( " -------- constant \n")
		print(self.constant_matrix)
		print( "\n")
		print( "shape of matrix is " ) 
		print( self.constant_matrix.shape )
		print( "size of matrix is ")
		print(self.constant_matrix.size)

class hydrodynamic_q:
	
	def __init__( self , x , y , z , k , m , n):
		self.constant_array = numpy.asarray([
								[	x,	0,	0,	0,	0,	0],
								[	0,	y,	0,	0,	0,	0],
								[	0,	0,	z,	0,	0,	0],
								[	0,	0,	0,	k,	0,	0],
								[	0,	0,	0,	0,	m,	0],
								[	0,	0,	0,	0,	0,	n]
											])
		self.constant_matrix = numpy.asmatrix( self.constant_array)
		print( " -------- check constant of hydrodynamic rb ----------\n")
		print( " -------- constant \n")
		print(self.constant_matrix)
		print( "\n")
		print( "shape of matrix is " ) 
		print( self.constant_matrix.shape )
		print( "size of matrix is ")
		print(self.constant_matrix.size)

	def input_value( self, u , v , w , p , q , r):
		self.dynamic_matrix = numpy.matrix.copy( self.constant_matrix )
		print( "\n\n ------------- original matrix ")

		self.dynamic_matrix[ 0 , 0 ] *= self.absolute(u)
		self.dynamic_matrix[ 1 , 1 ] *= self.absolute(v)
		self.dynamic_matrix[ 2 , 2 ] *= self.absolute(w)
		self.dynamic_matrix[ 3 , 3 ] *= self.absolute(p)
		self.dynamic_matrix[ 4 , 4 ] *= self.absolute(q)
		self.dynamic_matrix[ 5 , 5 ] *= self.absolute(r)

		print( "\n\n ------------- new matrix ")
		print( self.dynamic_matrix )

		print( "\n\n ------------- constant matrix")
		print( self.constant_matrix )

		return self.dynamic_matrix

	def absolute( self, data):
		if( data >= 0 ): 
			return data
		else:
			return -1 * data  
		

