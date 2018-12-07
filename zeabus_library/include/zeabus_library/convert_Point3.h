/*
	File name			:	convert_Point3.h		
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 06
	Date last modified	:	2018 , ??? ??
	Purpose				:	For convert array or value to zeabus_library/IMUData
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.0.5
	status				:	Production

	Namespace			:	zeabus_library
*/

#include	<stdio.h>

#include	<boost/numeric/ublas/matrix.hpp>

#include	<zeabus_library/Point3.h>

#ifndef _ZEABUS_LIBRARY_CONVERT_POINT3_H__
#define _ZEABUS_LIBRARY_CONVERT_POINT3_H__

namespace zeabus_library{

	void array_to_Point3( double* set_value , zeabus_library::Point3& result , int offset = 0 );

	// can use only matrix size ( 1 , 3 )	
	void matrix_to_Point3(	boost::numeric::ublas::matrix< double > set_value 
							, zeabus_library::Point3& result );

}

#endif
